#include <iostream>
#include <filesystem>
#include <algorithm>
#include <set>
#include <fstream>
#include <omp.h>

#include "common.h"
#include "gesamtlib/gsmt_structure.h"
#include "gesamtlib/gsmt_defs.h"


namespace fs = std::filesystem;


int main(int argc, char **argv) {

    if (argc < 4) {
        std::cerr << "Not enough arguments" << std::endl;
        return 1;
    }

    const fs::path raw_pdb_dir = argv[1];
    const fs::path output_binary_dir = argv[2];
    const fs::path logfile = argv[3];

    std::ifstream log_read;
    log_read.open(logfile);

    std::set<std::string> processed;

    std::string line;
    while (std::getline(log_read, line)) {
        auto found = line.find("Done reading: ");
        if (found != std::string::npos) {
            auto filename = line.substr(14);
            processed.insert(filename);
        }
    }
    log_read.close();

    std::ofstream log(logfile, std::fstream::app | std::fstream::out);

    std::vector<fs::directory_entry> files;
    std::set<std::string> subdirs;
    for (const auto &file_entry: fs::directory_iterator(raw_pdb_dir)) {
        if (not file_entry.is_regular_file()) {
            continue;
        }

        if (processed.find(std::string(file_entry.path())) != processed.end()) {
            continue;
        }

        files.emplace_back(file_entry);
        subdirs.insert(std::string(file_entry.path().filename()).substr(0, 2));
    }

    for (const auto &subdir: subdirs) {
        fs::create_directory(output_binary_dir / subdir);
    }


#pragma omp parallel for default(none) shared(files, output_binary_dir, log, processed)
    for (size_t i = 0; i < files.size(); i++) {
        const auto &file_entry = files[i];

        const std::string pdb_id = to_upper(std::string(file_entry.path().filename()).substr(0, 4));
        const std::string subdir = std::string(file_entry.path().filename()).substr(0, 2);

        std::stringstream ss;
        ss << "Started reading: " << std::string(file_entry.path()) << std::endl;
        log << ss.str();

        int chain_no = 0;
        int rc;
        gsmt::Structure structure;
        while (true) {
            rc = structure.getStructure(file_entry.path().c_str(), nullptr, chain_no, true);
            if (rc && chain_no) {
                std::stringstream ss1;
                ss1 << "Done reading: " << std::string(file_entry.path()) << std::endl;
                log << ss1.str();
                break;
            }
            if (rc) {
                std::stringstream ss1;
                ss1 << "Cannot read from file: " << std::string(file_entry.path()) << std::endl;
                log << ss1.str();
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
                log << ss1.str();
                ss2 << std::string(output_binary_dir) << "/" << subdir << "/" << pdb_id << ":" << chain_id << ".bin";
                file.assign(ss2.str().c_str());
                file.rewrite();
                structure.write(file);
                file.shut();
            } else if (n_atoms > 0) {
                std::stringstream ss1;
                ss1 << "Chain too short: " << pdb_id << ":" << atom[0]->GetChainID() << std::endl;
                log << ss1.str();
            }
            chain_no++;
        }
    }

    log.close();
    return 0;
}
