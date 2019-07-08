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

std::vector<std::tuple<std::string, std::string>> load_names(const std::string &filename) {
    std::vector<std::tuple<std::string, std::string>> data;

    std::ifstream f(filename);

    std::string line;
    while (std::getline(f, line)) {

        std::string file_path = line.substr(0, 4);
        std::string chain_id = line.substr(5);
        data.emplace_back(file_path, chain_id);
    }

    return data;
}

int main(int argc, char **argv) {

    namespace fs = std::filesystem;

    if (argc < 4) {
        std::cerr << "Not enough arguments" << std::endl;
        return 1;
    }

    std::string directory(argv[1]);
    auto file1 = argv[2];
    auto file2 = argv[3];

    auto data1 = load_names(file1);
    auto data2 = load_names(file2);

    auto n = data1.size();
    auto m = data2.size();
    std::vector<float> results(n * m, -3);

#pragma omp parallel for
    for (auto i = 0; i < n; i++) {
        auto file_path1 = std::get<0>(data1[i]);
        auto chain_id1 = std::get<1>(data1[i]);

        auto path1 = fs::path(directory) / (to_lower(file_path1) + std::string("_updated.cif"));
        gsmt::Structure s1;
        auto info1 = s1.getStructure(path1.c_str(), chain_id1.c_str(), -1, false);

        for (auto j = 0; j < m; j++) {
            auto file_path2 = std::get<0>(data2[j]);
            auto chain_id2 = std::get<1>(data2[j]);

            auto path2 = fs::path(directory) / (to_lower(file_path2) + std::string("_updated.cif"));
            gsmt::Structure s2;
            auto info2 = s2.getStructure(path2.c_str(), chain_id2.c_str(), -1, false);

            if (info1 or info2) {
                results[i * m + j] = -2;
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

            results[i * m + j] = SD ? SD->Q : -1;
            delete Aligner;
        }
    }

    std::cout << std::fixed << std::setprecision(3);

    for (auto i = 0; i < n; i++) {
        for (auto j = 0; j < m; j++) {
            std::cout << results[i * m + j] << ", ";
        }
        std::cout << std::endl;
    }

    return 0;
}