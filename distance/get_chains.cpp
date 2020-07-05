//
// Created by krab1k on 05.07.20.
//

#include <iostream>
#include <sstream>
#include "gesamtlib/gsmt_structure.h"


int main(int argc, char **argv) {
    if (argc < 3) {
        std::cerr << "Insufficient number of arguments" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_dir = argv[2];

    gsmt::Structure s;
    auto ret = s.getStructure(input_file.c_str(), "", -1, false);

    if (ret) {
        std::cerr << "Cannot open file: " << input_file << std::endl;
        return 2;
    }

    auto M = s.getMMDBManager();
    auto model = M->GetFirstDefinedModel();

    mmdb::PPChain  chain;
    int nc;

    std::string pdbid = M->GetEntryID();
    model->GetChainTable (chain,nc);

    for (int i  = 0; i < nc; i++) {
        auto chainid = chain[i]->GetChainID();
        if (chainid[0] != '\0') {
            std::cout << pdbid << ":" << chainid << std::endl;
            auto M1 = new mmdb::Manager();

            M1->Copy(M, mmdb::COPY_MASK::MMDBFCM_All);
            gsmt::Structure s2;
            s2.getStructure(M1, chainid, -1, false);
            mmdb::io::File file;
            std::stringstream ss;
            ss << output_dir << "/" << pdbid << ":" << chainid << ".bin";
            file.assign(ss.str().c_str());
            file.rewrite();
            s2.write(file);
            file.shut();
        }
    }
    return 0;
}