//
// Created by krab1k on 24.09.19.
//
#include <cstdio>
#include "gesamtlib/gsmt_structure.h"


int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Insufficient number of arguments\n");
        return 1;
    }

    gsmt::Structure s;
    auto ret = s.getStructure(argv[1], argv[2], -1, false);
    if (ret) {
        fprintf(stderr, "Cannot open file: %s\n", argv[1]);
        return 2;
    }
    mmdb::io::File file;
    file.assign(argv[3]);
    file.rewrite();
    s.write(file);
    file.shut();
}