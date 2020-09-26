import sys

filename = sys.argv[1]


FILE_PATTERN = '.cif'
CHAIN_PATTERN = 'packed in pack'

last_pdbid = '----'

with open(filename) as f:
    for line in f:
        file_line = False
        if FILE_PATTERN in line:
            last_pdbid = line.split(FILE_PATTERN)[0][-4:].upper()
            file_line = True
        if CHAIN_PATTERN in line:
            if file_line:
                chainid = line.split(':')[1].split()[0]
            else:
                chainid = line.split()[0]

            print(f'{last_pdbid}:{chainid}')

