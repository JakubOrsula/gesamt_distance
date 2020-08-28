//
// Created by krab1k on 4.7.19.
//

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>


#include "protein_distance.h"


std::vector<std::string> load_ids(const std::string &filename) {
    std::vector<std::string> data;

    std::ifstream f(filename);
    std::string line;
    while (std::getline(f, line)) {
        data.emplace_back(line);
    }

    return data;
}

int main(int argc, char **argv) {

    if (argc < 2) {
        std::cerr << "Not enough arguments" << std::endl;
        return 1;
    }

    std::string directory(argv[1]);
    auto file = argv[2];
    auto all_data = load_ids(file);

    for (const auto &id: all_data) {
        auto s = load_single_structure(id, directory, true);
        std::cout << id << ": " << s->getNCalphas() << std::endl;
    }

    return 0;
}
