#!/usr/bin/env python3
import os, re, time, sys
from urllib.parse import urljoin, urlparse
import requests
from bs4 import BeautifulSoup

BASE = "https://networkrepository.com/"
CAT  = urljoin(BASE, "dimacs.php")
DST  = "NR_DIMACS"

os.makedirs(DST, exist_ok=True)
sess = requests.Session()
sess.headers.update({
    "User-Agent": "mateus-downloader/1.0 (+academic use)"
})

def get(url, **kw):
    for i in range(5):
        try:
            r = sess.get(url, timeout=30, **kw)
            if r.status_code in (429, 503):  # rate limit
                time.sleep(5*(i+1)); continue
            r.raise_for_status()
            return r
        except Exception as e:
            if i == 4: raise
            time.sleep(2*(i+1))

def safe_name(s):
    return re.sub(r"[^A-Za-z0-9_.-]+", "_", s)

print(f"Lendo categoria: {CAT}")
cat = get(CAT)
soup = BeautifulSoup(cat.text, "html.parser")

# 1) links para páginas de datasets (*.php) dentro do domínio
dataset_pages = []
for a in soup.select("a[href$='.php']"):
    href = a.get("href")
    if not href: continue
    url = urljoin(BASE, href)
    if url.startswith(BASE) and url.endswith(".php") and "dimacs.php" not in url:
        dataset_pages.append(url)

dataset_pages = sorted(set(dataset_pages))
print(f"Encontradas {len(dataset_pages)} páginas de dataset.")

download_links = []
for i, durl in enumerate(dataset_pages, 1):
    try:
        html = get(durl).text
    except Exception as e:
        print(f"[WARN] Falha abrindo {durl}: {e}", file=sys.stderr)
        continue
    ds = BeautifulSoup(html, "html.parser")
    # 2) capturar .zip e .7z
    for a in ds.select("a[href]"):
        h = a.get("href")
        if not h: continue
        full = urljoin(durl, h)
        if re.search(r"\.(zip|7z)$", full, flags=re.I):
            download_links.append((durl, full))

download_links = sorted(set(download_links))
print(f"Coletados {len(download_links)} links de download (.zip/.7z).")

# 3) baixar cada link criando subpasta por dataset (slug da página)
for origin, link in download_links:
    slug = safe_name(os.path.splitext(os.path.basename(urlparse(origin).path))[0])
    subd = os.path.join(DST, slug)
    os.makedirs(subd, exist_ok=True)
    fname = safe_name(os.path.basename(urlparse(link).path))
    out = os.path.join(subd, fname)

    if os.path.exists(out):
        print(f"[SKIP] Já existe: {out}")
        continue

    print(f"[{slug}] Baixando {link}")
    try:
        with sess.get(link, stream=True, timeout=60) as r:
            r.raise_for_status()
            tmp = out + ".part"
            with open(tmp, "wb") as f:
                for chunk in r.iter_content(1024*256):
                    if chunk: f.write(chunk)
            os.replace(tmp, out)
    except Exception as e:
        print(f"[WARN] Erro baixando {link}: {e}", file=sys.stderr)
        if os.path.exists(out+".part"): os.remove(out+".part")
        continue

    time.sleep(0.5)  # gentileza para o servidor

print("Concluído.")