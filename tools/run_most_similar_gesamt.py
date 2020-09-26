import sys
import os
import subprocess
import time

SOURCE_DIR = '/mnt/PDBe_clone/ok'
NTHREADS = 30

queries_file = sys.argv[1]
all_ids_file = sys.argv[2]
gesamt_archive_dir = sys.argv[3]
output_dir = sys.argv[4]

ids = []

with open(queries_file) as f:
    for line in f:
        ids.append(line.strip())


for object_id in ids:
    pdb_id, chain_id = object_id.split(':')
    filename = os.path.join(SOURCE_DIR, f'{pdb_id.lower()}.cif')
    output = os.path.join(output_dir, f'{object_id}.hits')

    args = ['gesamt', filename, '-s', chain_id, '-archive', gesamt_archive_dir, '-incl-list', all_ids_file, f'-nthreads={NTHREADS}', '-o', output, '-v0']
    print(' '.join(args))

    time_start = time.time()
    f_stdout = open(os.path.join(output_dir, f'{object_id}.stdout'), 'w')
    f_stderr = open(os.path.join(output_dir, f'{object_id}.stderr'), 'w')
    calculation = subprocess.run(args, stdout=f_stdout, stderr=f_stderr)
    time_end = time.time()

    f_stdout.close()
    f_stderr.close()

    if calculation.returncode:
        print('Calculation failed')
    else:
        print(f'Calculation finished at {time_end - time_start:.1f} s')

