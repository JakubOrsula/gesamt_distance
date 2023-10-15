import json
import csv
import sys
import python_distance
import time
import multiprocessing
import functools

N = 30


def worker(obj, query):
    start = time.time()
#    d = python_distance.get_results(query, obj, '/mnt/PDBe_clone_binary' , 0)
    d = python_distance.get_distance(query, obj, -1)
    end = time.time()
    print(f'{obj} {end - start:.3f}')
    return d


def main():
    archive_dir = sys.argv[1]
    mapping_file = sys.argv[2]
    gt_file = sys.argv[3]

    to_compute = {}
    mapping = {}
    with open(mapping_file) as f:
        reader = csv.reader(f, delimiter=';')
        for chain_id, number in reader:
            mapping[number] = chain_id

    with open(gt_file) as f:
        for line in f:
            record = json.loads(line)
            query_number = record['queryObj_id']
            similar_objects = [mapping[nr['_id']] for nr in record['answer_records'][:N]]
            to_compute[mapping[query_number]] = similar_objects

    python_distance.init_library(archive_dir, '/dev/null', True, 0.6, 100)
    with multiprocessing.Pool() as pool:
        for query, nearest in to_compute.items():
            if query != '6RWA:A':
                continue
            start = time.time()
            list(pool.map(functools.partial(worker, query=query), nearest))
            end = time.time()
            print(f'{query} {end - start:.3f}')


if __name__ == '__main__':
    main()
