import sys
import os
import subprocess
import multiprocessing
import tqdm

chain2bin = sys.argv[1]
id_file = sys.argv[2]
source_dir = sys.argv[3]
dest_dir = sys.argv[4]


def convert(obj_id: str):
    pdb_id, chain_id = obj_id.split(':')
    prefix = pdb_id[:2].lower()

    input_file = os.path.join(source_dir, prefix, f'{pdb_id.lower()}_updated.cif')
    output_file = os.path.join(dest_dir, prefix, f'{obj_id}.bin')
    if os.path.isfile(output_file):
        return
    args = [chain2bin, input_file, chain_id, output_file]
    subprocess.call(args)


ids = []
with open(id_file) as f:
    for line in f:
        ids.append(line.strip())

for directory in {obj_id[:2].lower() for obj_id in ids}:
    try:
        os.mkdir(os.path.join(dest_dir, directory))
    except FileExistsError:
        pass

with multiprocessing.Pool() as p:
    for _ in tqdm.tqdm(p.imap_unordered(convert, ids), total=len(ids)):
        pass
