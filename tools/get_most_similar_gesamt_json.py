import glob
import os
import sys
import json

log_dir = sys.argv[1]
output_json = sys.argv[2]

with open(output_json, 'w') as f_out:
    for filename in glob.glob(os.path.join(log_dir, '*.hits')):
        object_id, _  = os.path.splitext(os.path.basename(filename))
        print(object_id)
        hits = 0
        with open(filename) as f: 

            out = {}
            out['result'] = {}
            out['result']['query'] = {'proteinObj': {'_id': object_id}}
            out['result']['opAnswer'] = {}
            out['result']['opAnswer']['answer_distances'] = []
            out['result']['opAnswer']['answer_records'] = []
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

                out['result']['opAnswer']['answer_distances'].append(round(1 - qscore, 4))
                out['result']['opAnswer']['answer_records'].append({'_id': f'{pdb_id}:{chain_id}'})
            

            out['result']['opAnswer']['answer_count'] = hits

            out_str = json.dumps(out)
            f_out.write(f'{out_str}\n')
