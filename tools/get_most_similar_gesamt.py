import glob
import os
import sys


log_dir = sys.argv[1]
output_dir = sys.argv[2]

for filename in glob.glob(os.path.join(log_dir, '*.hits')):
    object_id, _  = os.path.splitext(os.path.basename(filename))
    print(object_id)
    hits = 0
    with open(filename) as f, open(os.path.join(output_dir, f'{object_id}.results'), 'w') as f_out:
        for line in f:
            if line.startswith('#'):
                continue

            _, pdb_id, chain_id, qscore, *_ = line.split()
            qscore = float(qscore)

            if qscore < 0.5:
                break

            hits += 1
            if hits > 1000:
                break
        
            print(f'{pdb_id}:{chain_id:<2s}: {1 - qscore:.4f}', file=f_out)
