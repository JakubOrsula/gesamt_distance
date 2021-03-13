import sys
import json

if len(sys.argv) < 3:
    print('Insufficient number of argument')
    sys.exit(1)


pivots = sys.argv[1]
results = sys.argv[2]

pivot_ids = set()


with open(pivots) as f:
    for line in f:
        js = json.loads(line)
        pivot_ids.add(js['_id'])

with open(results) as f:
    for line in f:
        js = json.loads(line)
        current_id = js['_id']
        if current_id in pivot_ids:
            if js['dists'][current_id] != 0.0:
                print(js['dists'][current_id])
