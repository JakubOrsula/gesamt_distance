#include <iostream>
#include <filesystem>
#include <algorithm>
#include <set>
#include <omp.h>

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

    std::vector<fs::directory_entry> files;
    std::set<std::string> subdirs;
    for (const auto &file_entry: fs::directory_iterator(raw_pdb_dir)) {
        if (not file_entry.is_regular_file()) {
            continue;
        }

        files.emplace_back(file_entry);
        subdirs.insert(std::string(file_entry.path().filename()).substr(0, 2));
    }

    for (const auto &subdir: subdirs) {
        fs::create_directory(output_binary_dir / subdir);
    }

#pragma omp parallel for default(none) shared(files, output_binary_dir, std::cerr, std::cout)
    for (size_t i = 0; i < files.size(); i++) {
        const auto &file_entry = files[i];

        std::string pdb_id = to_upper(std::string(file_entry.path().filename()).substr(0, 4));
        const std::string subdir = std::string(file_entry.path().filename()).substr(0, 2);

        int chain_no = 0;
        int rc;
        while (true) {
            gsmt::Structure structure;
            rc = structure.getStructure(file_entry.path().c_str(), nullptr, chain_no, true);
            if (rc && chain_no) {
                break;
            }
            if (rc) {
                std::stringstream ss;
                ss << "Cannot read from file: " << std::string(file_entry.path()) << std::endl;
                std::cerr << ss.str();
                break;
            }

            mmdb::PPAtom atom;
            int n_atoms;
            structure.getCalphas(atom, n_atoms);

            if (n_atoms >= seg_length_default) {
                std::string chain_id = atom[0]->GetChainID();
                mmdb::io::File file;
                std::stringstream ss1, ss2;

                ss1 << "Converted: " << pdb_id << ":" << chain_id << std::endl;
                std::cout << ss1.str();
                ss2 << std::string(output_binary_dir) << "/" << subdir << "/" << pdb_id << ":" << chain_id << ".bin";
                file.assign(ss2.str().c_str());
                file.rewrite();
                structure.write(file);
                file.shut();
            }
            chain_no++;
        }

    }
    return 0;
}
