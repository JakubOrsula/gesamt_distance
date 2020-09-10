#include <iostream>
#include <filesystem>
#include <algorithm>

#include "gesamtlib/gsmt_structure.h"
#include "gesamtlib/gsmt_defs.h"


namespace fs = std::filesystem;

std::string to_upper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}


int main(int argc, char **argv) {

    if (argc < 3) {
        std::cerr << "Not enough arguments" << std::endl;
        return 1;
    }

    const fs::path raw_pdb_dir = argv[1];
    const fs::path output_binary_dir = argv[2];

    for (const auto &file_entry: fs::directory_iterator(raw_pdb_dir)) {
        if (not file_entry.is_regular_file()) {
            continue;
        }

        const std::string subdir = std::string(file_entry.path().filename()).substr(0, 2);
        fs::create_directory(output_binary_dir / subdir);

        std::string pdb_id = to_upper(std::string(file_entry.path().filename()).substr(0, 4));

        std::cerr << "Started processing: " << pdb_id << std::endl;
        int chain_no = 0;
        int rc;
        while (true) {
            gsmt::Structure structure;
            rc = structure.getStructure(file_entry.path().c_str(), nullptr, chain_no, true);
            if (rc && chain_no) {
                std::cerr << "No other chain is present" << std::endl;
                /* No other chain is present */
                break;
            }
            if (rc) {
                std::cerr << "Cannot read from file: " << file_entry.path() << std::endl;
                break;
            }

            mmdb::PPAtom atom;
            int n_atoms;
            structure.getCalphas(atom, n_atoms);

            std::cerr << "Chain_no: " << chain_no << " # atoms: " << n_atoms << std::endl;

            if (n_atoms >= seg_length_default) {
                std::string chain_id = atom[0]->GetChainID();
                mmdb::io::File file;
                std::stringstream ss;
                std::cout << pdb_id << ":" << chain_id << std::endl;
                ss << std::string(output_binary_dir) << "/" << subdir << "/" << pdb_id << ":" << chain_id << ".bin";
                file.assign(ss.str().c_str());
                file.rewrite();
                structure.write(file);
                file.shut();
            } else {
                if (n_atoms > 0) {
                    std::cout << "Skipping: " << pdb_id << ":" << atom[0]->GetChainID() << " # atoms: " << n_atoms
                              << std::endl;
                }
            }
            chain_no++;
        }

    }
    return 0;
}
