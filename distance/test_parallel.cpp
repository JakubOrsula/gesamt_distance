//
// Created by krab1k on 21.01.20.
//

#include <iostream>
#include <omp.h>
#include <string>
#include <vector>
#include <fstream>
#include "protein_distance.h"


int main(int argc, char **argv) {

    if (argc != 5) {
        std::cerr << "Not enough arguments" << std::endl;
        return 1;
    }

    const std::string archive = argv[1];
    const std::string pivots = argv[2];
    int size = atoi(argv[3]);
    float timeout = atof(argv[4]);

    init_library(archive, pivots, true, 0.6);

    std::vector<std::string> data;

    try {
        std::fstream file(pivots);

        std::string line;
        while (std::getline(file, line)) {
            data.push_back(line);
        }
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

#pragma omp parallel for default(none) shared(data, timeout, size, std::cout) schedule(dynamic) collapse(2)
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < data.size(); j++) {
            std::cout << get_distance(data[i], data[j], timeout) << std::endl;
        }
    }
    return 0;
}
