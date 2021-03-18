//
// Created by krab1k on 04.07.20.
//

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <functional>
#include <algorithm>
#include <iostream>

#define TBB_PREVIEW_CONCURRENT_LRU_CACHE 1

#include "tbb/concurrent_lru_cache.h"


#include "config.h"
#include "gesamtlib/gsmt_aligner.h"
#include "protein_distance.h"

static double threshold;

typedef std::function<std::shared_ptr<gsmt::Structure>(const std::string &)> load_ft;
typedef tbb::concurrent_lru_cache<std::string, std::shared_ptr<gsmt::Structure>, load_ft> cache_t;

static cache_t *cache = nullptr;


std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}


std::shared_ptr<gsmt::Structure> get_structure(const std::string &id) {
    if (not cache) {
        throw std::runtime_error("LRU cache not initialized. You must run init_library first!");
    }
    return (*cache)[id].value();
}


std::shared_ptr<gsmt::Structure>
load_single_structure(const std::string &id, const std::string &directory, bool binary) {

    mmdb::io::File file;

    auto s = std::make_shared<gsmt::Structure>();

    std::string path;
/* Handle query objects */
    if (id[0] == '_') {
        auto new_id = id.substr(1);
        auto pos = new_id.find(':');
        auto dir = new_id.substr(0, pos);
        auto chain = new_id.substr(pos + 1);

        if (binary) {
            path = std::string(QUERIES_DIRECTORY) + "/query" + dir + "/query:" + chain + ".bin";
            file.assign(path.c_str());
            if (not file.exists()) {
                throw std::runtime_error("Cannot open binary query file: " + std::string(file.FileName()));
            }
            if (not file.reset(true)) {
                throw std::runtime_error("Cannot read from binary query file: " + std::string(file.FileName()));
            }
            s->read(file);
            file.shut();
        } else {
            path = std::string(QUERIES_DIRECTORY) + "/query" + dir + "/query";
            std::string chain_id = "/1/" + chain;
            auto rc = s->getStructure(path.c_str(), chain_id.c_str(), -1, false);
            if (rc) {
                throw std::runtime_error("Cannot open raw query file: " + path);
            }
        }
        s->prepareStructure(7.0);

#ifndef NDEBUG
        std::cerr << "Loaded query structure: " << id << " from: " << path << std::endl;
#endif
        return s;
    }

    if (binary) {
        path = directory + "/" + to_lower(id.substr(0, 2)) + "/" + id + ".bin";
        file.assign(path.c_str());
        if (not file.exists()) {
            throw std::runtime_error("Cannot open binary file: " + std::string(file.FileName()));
        }
        if (not file.reset(true)) {
            throw std::runtime_error("Cannot read from binary file: " + std::string(file.FileName()));
        }
        s->read(file);
        file.shut();
    } else {
        auto pos = id.find(':');
        auto pdbid = id.substr(0, pos);
        auto chain = id.substr(pos + 1);
        path = directory + "/" + to_lower(pdbid) + CIF_SUFFIX;
        std::string chain_id = "/1/" + chain;
        auto rc = s->getStructure(path.c_str(), chain_id.c_str(), -1, false);
        if (rc) {
            throw std::runtime_error("Cannot open raw file " + path);
        }
    }
#ifndef NDEBUG
    std::cerr << "Loaded: " << id << " from: " << path << std::endl;
#endif
    s->prepareStructure(7.0);

    return s;
}


void init_library(const std::string &archive_directory, const std::string &preload_list_filename, bool binary_archive,
                  double approximation_threshold, int cache_size) {
    threshold = approximation_threshold;

    /* Load structures' names */
    std::vector<std::string> structure_ids;
    std::ifstream file(preload_list_filename);
    if (not file.is_open()) {
        throw std::runtime_error("INIT: Cannot read preload list from file: " + preload_list_filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        structure_ids.push_back(line);
    }

    /* Fix arguments to avoid global variables */
    auto load = [=](auto &&id) { return load_single_structure(id, archive_directory, binary_archive); };

    if (cache == nullptr) {
#ifndef NDEBUG
        std::cerr << "INIT: initializing new LRU cache of size " << cache_size << std::endl;
#endif
        cache = new cache_t(load, cache_size);
    } else {
#ifndef NDEBUG
        std::cerr << "INIT: already initialized before, reusing old LRU cache" << std::endl;
#endif
    }

    try {
        /* Preload structures */
        for (const auto &id: structure_ids) {
            (*cache)[id];
        }
    } catch (std::exception &e) {
        std::string message = "INIT:Preload" + std::string(e.what());
        throw std::runtime_error(message);
    }
}


enum status run_computation(const std::string &id1, const std::string &id2, float min_qscore,
                            std::unique_ptr<gsmt::Superposition> &SD) {

    if (not cache) {
        throw std::runtime_error("LRU cache not initialized. You must run init_library first!");
    }

    /* Handle keeps structure from removing from LRU cache */
    auto handle1 = (*cache)[id1];
    auto handle2 = (*cache)[id2];

    gsmt::Structure *s1 = handle1.value().get();
    gsmt::Structure *s2 = handle2.value().get();

    if (min_qscore != 0) {
        auto size1 = s1->getNCalphas();
        auto size2 = s2->getNCalphas();

        if (size2 < min_qscore * size1 or size1 < min_qscore * size2) {
#ifndef NDEBUG
            std::cerr << "Structures " << id1 << " and " << id2 << "have sizes too different (" << size1 << ", "
                      << size2 << ") to achieve minimum required Q-score " << min_qscore << std::endl;
#endif
            return RESULT_DISSIMILAR;
        }
    }

    auto Aligner = std::make_unique<gsmt::Aligner>();
    Aligner->setPerformanceLevel(gsmt::PERFORMANCE_CODE::PERFORMANCE_Efficient);
    Aligner->setSimilarityThresholds(threshold, threshold);
    Aligner->setQR0(QR0_default);
    Aligner->setSigma(sigma_default);

    int matchNo;
    gsmt::PSuperposition SD_raw;
    Aligner->Align(s1, s2, false);
    Aligner->getBestMatch(SD_raw, matchNo);
    if (SD_raw) {
        SD->CopyFrom(SD_raw);
        return RESULT_OK;
    } else {
        return RESULT_DISSIMILAR;
    }
}


void close_library() {
    delete cache;
}
