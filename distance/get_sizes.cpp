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

    if (argc < 3) {
        std::cerr << "Not enough arguments" << std::endl;
        return 1;
    }

    std::string directory(argv[1]);
    auto file = argv[2];
    auto all_data = load_names(file);

    auto sizes = std::vector<size_t>(all_data.size(), 0);

#pragma omp parallel for default(none) shared(all_data, directory, sizes)
    for (size_t i = 0; i < all_data.size(); i++) {
        const auto &[file_path, chain_id] = all_data[i];
        auto path = fs::path(directory) / (to_lower(file_path) + std::string("_updated.cif"));
        auto s = new gsmt::Structure();
        s->getStructure(path.c_str(), chain_id.c_str(), -1, false);
        sizes[i] = s->getNCalphas();
        delete s;
    }

    for (size_t i = 0; i < all_data.size(); i++) {
        const auto &[file_path, chain_id] = all_data[i];
        std::cout << file_path << ":" << chain_id << " " << sizes[i] << std::endl;
    }

    return 0;
}
