//
// Created by krab1k on 4.7.19.
//

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>
#include <omp.h>
#include <map>
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

    float threshold = 0.0f;
    if (argc == 5) {
        threshold = atof(argv[4]);
    }

    auto data1 = load_names(file1);
    auto data2 = load_names(file2);

    auto all_data = data1;
    all_data.insert(all_data.end(), data2.begin(), data2.end());

    auto n = data1.size();
    auto m = data2.size();

    std::map<std::tuple<std::string, std::string>, gsmt::Structure *> structures;

    for (const auto &[file_path, chain_id]: all_data) {
        structures[std::make_tuple(file_path, chain_id)] = nullptr;
    }

#pragma omp parallel for default(none) shared(all_data, structures, directory)
    for (size_t i = 0; i < all_data.size(); i++) {
        const auto &[file_path, chain_id] = all_data[i];
        auto it = structures.find(std::make_tuple(file_path, chain_id));

        if (it != structures.end() and it->second != nullptr) {
            continue;
        }

        auto path = fs::path(directory) / (to_lower(file_path) + std::string("_updated.cif"));
        auto s = new gsmt::Structure();
        s->getStructure(path.c_str(), chain_id.c_str(), -1, false);
        s->prepareStructure(0.0);
        structures[std::make_tuple(file_path, chain_id)] = s;
    }

    std::vector<float> results(n * m, -3);
    std::vector<float> times(n * m, -1);

#pragma omp parallel for default(none) shared(structures, data1, data2, n, m, results, times, threshold)
    for (size_t i = 0; i < n; i++) {
        auto s1 = structures[data1[i]];
        for (size_t j = 0; j < m; j++) {
            auto s2 = structures[data2[j]];

            auto Aligner = new gsmt::Aligner();
            Aligner->setPerformanceLevel(gsmt::PERFORMANCE_CODE::PERFORMANCE_Efficient);
            Aligner->setSimilarityThresholds(threshold, threshold);
            Aligner->setQR0(QR0_default);
            Aligner->setSigma(sigma_default);

            gsmt::PSuperposition SD;
            int matchNo;

            float time;
            Aligner->Align(s1, s2, true, &time);
            Aligner->getBestMatch(SD, matchNo);

            results[i * m + j] = SD ? SD->Q : -1;
            times[i * m + j]  = time;
            delete Aligner;
        }
    }

    std::cout << std::fixed << std::setprecision(3);

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < m; j++) {
            std::cout << results[i * m + j] << ", ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < m; j++) {
            std::cout << times[i * m + j] << ", ";
        }
        std::cout << std::endl;
    }

    return 0;
}