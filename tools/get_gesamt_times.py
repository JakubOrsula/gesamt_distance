import sys
import glob
import os

log_dir = sys.argv[1]


for filename in glob.glob(os.path.join(log_dir, '*.stdout')):
    object_id, _ = os.path.splitext(os.path.basename(filename))
    with open(filename) as f:
        for line in f:
            if 'total time' in line:
                time = line.strip().split()[-1]
                hours, rest = time.split(':')
                hours = int(hours)
                minutes, seconds = rest.split('.')
                minutes = int(minutes)
                seconds = int(seconds)
                print(f'{object_id:9s} {3600 * hours + 60 * minutes + seconds:>7d}')
                break
