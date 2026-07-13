import os
import shutil

base_dir = "/Users/mateuspedrosa/Documents/l21-agbo/data/Harwell-Boeing/HB_all"

for root, dirs, files in os.walk(base_dir, topdown=False):
    # Ignora o diretório base
    if root == base_dir:
        continue

    if len(files) == 1 and not dirs:  # só um arquivo e nenhuma subpasta
        file_path = os.path.join(root, files[0])
        parent_dir = os.path.dirname(root)
        new_path = os.path.join(parent_dir, files[0])

        print(f"Movendo {file_path} → {new_path}")
        shutil.move(file_path, new_path)

        print(f"Removendo pasta {root}")
        os.rmdir(root)