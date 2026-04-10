#!/usr/bin/env python3
"""
download_harwell_boeing_fixed.py
---------------------------------
Baixa matrizes do grupo Harwell–Boeing (HB) da SuiteSparse Matrix Collection.

Como a SuiteSparse usa links dinâmicos nas páginas individuais (HB/<nome>),
este script visita cada página da matriz e segue o link de "Download"
(Matrix Market / Rutherford Boeing / MATLAB).

Uso:
  python3 download_harwell_boeing_fixed.py
  python3 download_harwell_boeing_fixed.py --pattern '^bcsstk' --extract
  python3 download_harwell_boeing_fixed.py --names bcsstk01 can___96 --format rb
"""

import argparse
import concurrent.futures as cf
import html
import os
import re
import sys
import tarfile
import threading
import time
import urllib.request
import urllib.parse

BASE_INDEX = "https://sparse.tamu.edu/HB?per_page=All"
MATRIX_PAGE = "https://sparse.tamu.edu/HB/{name}"
DEFAULT_DEST = "HB_all"

FORMAT_TO_ANCHOR = {
    "mm":   re.compile(r"\bMatrix\s*Market\b", re.I),
    "rb":   re.compile(r"\bRutherford\s*Boeing\b", re.I),
    "mat":  re.compile(r"\bMATLAB\b", re.I),
}

UA = {"User-Agent": "hb-downloader/2.0 (+academic use)"}
lock = threading.Lock()

def http_get(url, timeout=30):
    req = urllib.request.Request(url, headers=UA, method="GET")
    with urllib.request.urlopen(req, timeout=timeout) as resp:
        return resp.read()

def http_get_stream(url, timeout=60):
    req = urllib.request.Request(url, headers=UA, method="GET")
    return urllib.request.urlopen(req, timeout=timeout)  # caller must close

def parse_matrix_names_from_index(index_html):
    text = index_html.decode("utf-8", errors="ignore")
    # href="/HB/bcsstk01" ou href="HB/bcsstk01"
    pat = re.compile(r'href=[\'"](?:/)?HB/([A-Za-z0-9_.+-]+)[\'"]')
    names = set(pat.findall(text))
    return sorted(names)

def extract_links(html_text):
    links = []
    for m in re.finditer(r'<a[^>]*href=[\'"]([^\'"]+)[\'"][^>]*>(.*?)</a>', html_text, flags=re.I|re.S):
        href = html.unescape(m.group(1))
        inner = re.sub(r'<[^>]+>', '', m.group(2))
        inner = html.unescape(inner).strip()
        links.append((href, inner))
    return links

def find_download_url_for_format(page_html, preferred):
    anchor_re = FORMAT_TO_ANCHOR[preferred]
    for href, inner in extract_links(page_html):
        if anchor_re.search(inner):
            return urllib.parse.urljoin("https://sparse.tamu.edu/HB/", href)
    return None

def safe_filename(s):
    return re.sub(r"[^A-Za-z0-9_. -]+", "_", s)

def ensure_dir(path):
    os.makedirs(path, exist_ok=True)

def build_download_plan(all_names, names_filter=None, regex=None):
    sel = list(all_names)
    if names_filter:
        wanted = set(names_filter)
        sel = [n for n in sel if n in wanted]
    if regex:
        r = re.compile(regex)
        sel = [n for n in sel if r.search(n)]
    return sorted(sel)

def guess_filename_from_headers(resp, fallback):
    cd = resp.headers.get("Content-Disposition") or resp.headers.get("content-disposition")
    if cd:
        m = re.search(r'filename\*=UTF-8\'\'([^;]+)', cd)
        if m:
            try:
                return safe_filename(urllib.parse.unquote(m.group(1)))
            except Exception:
                pass
        m = re.search(r'filename="?([^";]+)"?', cd)
        if m:
            return safe_filename(m.group(1))
    try:
        parsed = urllib.parse.urlparse(resp.geturl())
        base = os.path.basename(parsed.path) or fallback
        return safe_filename(base)
    except Exception:
        return safe_filename(fallback)

def download_one(name, dest_dir, fmt="mm", retries=3, pause=1.5):
    page_url = MATRIX_PAGE.format(name=name)
    try:
        page_html = http_get(page_url).decode("utf-8", errors="ignore")
    except Exception as e:
        with lock:
            print(f"[WARN] {name}: falha ao abrir {page_url}: {e}")
        return None, False

    dlink = find_download_url_for_format(page_html, fmt)
    if not dlink:
        with lock:
            print(f"[WARN] {name}: sem link para formato '{fmt}' em {page_url}")
        return None, False

    for attempt in range(1, retries + 1):
        try:
            resp = http_get_stream(dlink)
            try:
                fname = guess_filename_from_headers(resp, fallback=f"{name}.{fmt}")
                out_path = os.path.join(dest_dir, fname)
                tmp = out_path + ".part"
                with open(tmp, "wb") as f:
                    while True:
                        chunk = resp.read(1024 * 256)
                        if not chunk: break
                        f.write(chunk)
                os.replace(tmp, out_path)
                with lock:
                    print(f"[OK]  {name}: {out_path}")
                return out_path, True
            finally:
                resp.close()
        except Exception as e:
            with lock:
                print(f"[WARN] {name}: falha no download (tentativa {attempt}/{retries}): {e}")
            time.sleep(pause * attempt)

    with lock:
        print(f"[FAIL] {name}: não foi possível baixar.")
    return None, False

def maybe_extract(path, dest_dir):
    if not path or not path.endswith(".tar.gz"):
        return False
    try:
        with tarfile.open(path, "r:gz") as tar:
            tar.extractall(path=dest_dir)
        with lock:
            print(f"[EXTRACT] {os.path.basename(path)} -> {dest_dir}")
        return True
    except Exception as e:
        with lock:
            print(f"[WARN] extração falhou '{path}': {e}")
        return False

def main():
    ap = argparse.ArgumentParser(description="Baixa matrizes HB (SuiteSparse, links dinâmicos).")
    ap.add_argument("--dest", default=DEFAULT_DEST, help=f"Pasta de destino (default: {DEFAULT_DEST})")
    ap.add_argument("--names", nargs="*", help="Nomes exatos (ex.: bcsstk01 can___96 ...)")
    ap.add_argument("--pattern", help="Regex de seleção (ex.: '^bcsstk')")
    ap.add_argument("--format", choices=["mm","rb","mat"], default="mm", help="mm=Matrix Market, rb=Rutherford Boeing, mat=MATLAB")
    ap.add_argument("--extract", action="store_true", help="Extrair .tar.gz após baixar (quando aplicável)")
    ap.add_argument("--jobs", type=int, default=4, help="Downloads paralelos (default: 4)")
    args = ap.parse_args()

    ensure_dir(args.dest)
    print(f"[INFO] Lendo índice HB: {BASE_INDEX}")
    try:
        idx_html = http_get(BASE_INDEX)
    except Exception as e:
        print(f"[ERRO] Não foi possível acessar {BASE_INDEX}: {e}")
        sys.exit(1)

    names = parse_matrix_names_from_index(idx_html)
    if not names:
        print("[ERRO] Nenhum nome encontrado no índice. O HTML pode ter mudado.")
        sys.exit(2)

    selected = build_download_plan(names, args.names, args.pattern)
    if not selected:
        print("[INFO] Nada selecionado após filtros.")
        sys.exit(0)

    print(f"[INFO] Baixando {len(selected)} matrizes (formato: {args.format})...")
    results = []
    with cf.ThreadPoolExecutor(max_workers=max(1, args.jobs)) as ex:
        futs = [ex.submit(download_one, n, args.dest, args.format) for n in selected]
        for fut in cf.as_completed(futs):
            results.append(fut.result())

    if args.extract:
        print("[INFO] Extraindo .tar.gz quando disponível...")
        for path, ok in results:
            if ok:
                maybe_extract(path, args.dest)

    okc = sum(1 for _, ok in results if ok)
    print(f"[INFO] Concluído: {okc}/{len(selected)} downloads OK.")

if __name__ == "__main__":
    main()