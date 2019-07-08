//
// Created by krab1k on 4.7.19.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <omp.h>
#include <vector>

#include "gesamtlib/gsmt_structure.h"
#include "gesamtlib/gsmt_aligner.h"

std::string to_upper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

int main(int argc, char **argv) {

    namespace fs = std::filesystem;

    if (argc < 3) {
        std::cerr << "Not enough arguments" << std::endl;
        return 1;
    }

    std::string directory(argv[1]);
    std::ifstream f(argv[2]);

    std::string line;
    std::vector<std::tuple<std::string, std::string>> data;

    while (std::getline(f, line)) {
        std::string file_path;
        std::string chain_id;

        std::stringstream ss(line);
        ss >> file_path >> chain_id;
        data.emplace_back(file_path, chain_id);
    }


#pragma omp parallel for
    for (size_t i = 0; i < data.size(); i++) {
        auto file_path = std::get<0>(data[i]);
        auto chain_id = std::get<1>(data[i]);

        auto path = fs::path(directory) / file_path;
        gsmt::Structure s;
        auto info = s.getStructure(path.c_str(), chain_id.c_str(), -1, false);

        if (!info) {
            auto Aligner = new gsmt::Aligner();
            Aligner->setPerformanceLevel(gsmt::PERFORMANCE_CODE::PERFORMANCE_Efficient);
            Aligner->setSimilarityThresholds(0.0, 0.0);
            Aligner->setQR0(QR0_default);
            Aligner->setSigma(sigma_default);

            gsmt::PSuperposition SD;
            int matchNo;

            Aligner->Align(&s, &s, false);
            Aligner->getBestMatch(SD, matchNo);

#pragma omp critical
            {
                std::cout << to_upper(file_path.substr(0, 4)) << ":" << chain_id << " ";
                if (SD) {
                    std::cout << "OK " << s.getNCalphas() << " " << SD->Q << std::endl;
                } else {
                    std::cout << "OK " << s.getNCalphas() << " -1" << std::endl;
                }

            }
            delete Aligner;
        }
    }

    return 0;
}
