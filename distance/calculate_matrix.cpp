//
// Created by krab1k on 4.7.19.
//

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>
#include <omp.h>
#include <iomanip>
#include <vector>

#include "gesamtlib/gsmt_structure.h"
#include "gesamtlib/gsmt_aligner.h"

std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
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

        std::string file_path = line.substr(0, 4);
        std::string chain_id = line.substr(5);
        data.emplace_back(file_path, chain_id);
    }

    auto n = data.size();
    std::vector<float> results(n * n, -3);

#pragma omp parallel for
    for (size_t i = 0; i < n; i++) {
        auto file_path1 = std::get<0>(data[i]);
        auto chain_id1 = std::get<1>(data[i]);

        auto path1 = fs::path(directory) / (to_lower(file_path1) + std::string("_updated.cif"));
        gsmt::Structure s1;
        auto info1 = s1.getStructure(path1.c_str(), chain_id1.c_str(), -1, false);

        for (size_t j = 0; j < n; j++) {
            auto file_path2 = std::get<0>(data[j]);
            auto chain_id2 = std::get<1>(data[j]);

            auto path2 = fs::path(directory) / (to_lower(file_path2) + std::string("_updated.cif"));
            gsmt::Structure s2;
            auto info2 = s2.getStructure(path2.c_str(), chain_id2.c_str(), -1, false);

            if (info1 or info2) {
                results[i * n + j] = -2;
                continue;
            }

            auto Aligner = new gsmt::Aligner();
            Aligner->setPerformanceLevel(gsmt::PERFORMANCE_CODE::PERFORMANCE_Efficient);
            Aligner->setSimilarityThresholds(0.0, 0.0);
            Aligner->setQR0(QR0_default);
            Aligner->setSigma(sigma_default);

            gsmt::PSuperposition SD;
            int matchNo;

            Aligner->Align(&s1, &s2, false);
            Aligner->getBestMatch(SD, matchNo);

            results[i * n + j] = SD ? SD->Q : -1;
            delete Aligner;
        }
    }

    std::cout << std::fixed << std::setprecision(3);

    for (auto i = 0; i < n; i++) {
        for (auto j = 0; j < n; j++) {
            std::cout << results[i * n + j] << ", ";
        }
        std::cout << std::endl;
    }

    return 0;
}