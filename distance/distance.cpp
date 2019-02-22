//
// Created by krab1k on 21.2.19.
//
#include <stdio.h>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <unordered_map>

#include "gesamtlib/gsmt_aligner.h"

#define MAX_STRUCTURES 10
#define LIST "/home/krab1k/Projects/ClusteringProteinChains/gesamt_modified/ids.txt"
#define DIRECTORY "/mnt/data/ClusteringProteinChains/RawPDBs/"


std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}


void load_single_structure(const std::string &directory, const std::string &id,
                           std::unordered_map<std::string, std::unique_ptr<gsmt::Structure>> &structures) {

    namespace fs = std::filesystem;

    auto pos = id.find(':');
    auto pdbid = id.substr(0, pos);
    auto chain = id.substr(pos + 1);

    auto s = std::make_unique<gsmt::Structure>();

    std::stringstream ss;
    ss << "pdb" << to_lower(pdbid) << ".ent";

    auto filepath = fs::path(directory) / ss.str();

    auto status = s->getStructure(filepath.c_str(), chain.c_str(), -1, false);

    if (status) {
        std::cout << "Could not load " << filepath << ": " << chain << std::endl;
        throw std::runtime_error("Loading of structure failed");
    } else {
        std::cout << "Loaded: " << id << std::endl;
    }

    structures.insert({id, std::move(s)});
}


std::unordered_map<std::string, std::unique_ptr<gsmt::Structure>> preload_structures() {

    std::vector<std::string> ids;
    try {
        std::fstream file(LIST);

        std::string line;
        int c = 0;
        while (std::getline(file, line)) {
            ids.push_back(line);
            c++;
            if (c == MAX_STRUCTURES) {
                break;
            }
        }
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    std::unordered_map<std::string, std::unique_ptr<gsmt::Structure>> structures;
    for (const auto &id: ids) {
        load_single_structure(DIRECTORY, id, structures);
    }

    return structures;
}


float get_distance(const std::string &id1, const std::string &id2) {

    static auto structures = preload_structures();

    std::cout << "Computing distance between " << id1 << " and " << id2 << std::endl;

    if (structures.find(id1) == structures.end()) {
        load_single_structure(DIRECTORY, id1, structures);
    }

    if (structures.find(id2) == structures.end()) {
        load_single_structure(DIRECTORY, id2, structures);
    }

    auto Aligner = new gsmt::Aligner();
    Aligner->setPerformanceLevel(gsmt::PERFORMANCE_CODE::PERFORMANCE_Efficient);
    Aligner->setSimilarityThresholds(0.0, 0.0);
    Aligner->setQR0(QR0_default);
    Aligner->setSigma(sigma_default);

    gsmt::PSuperposition SD;
    int matchNo;

    auto s1 = structures[id1].get();
    auto s2 = structures[id2].get();

    Aligner->Align(s1, s2, false);
    Aligner->getBestMatch(SD, matchNo);

    float distance;
    if (SD) {
        distance = 1 - static_cast<float>(SD->Q);
    } else {
        distance = 1;
    }

    delete Aligner;

    return distance;
}
