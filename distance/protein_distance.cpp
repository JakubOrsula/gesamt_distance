//
// Created by krab1k on 04.07.20.
//

#include <future>
#include <string>
#include <chrono>
#include <vector>
#include <memory>
#include <fstream>
#include <functional>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cassert>

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
            std::string chain_id = "/0/" + chain;
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
        std::stringstream ss;
        ss << "INIT:Preload: " << e.what();
        throw std::runtime_error(ss.str());
    }
}


float get_distance(const std::string &id1, const std::string &id2, float time_threshold) {
    auto SD = std::make_unique<gsmt::Superposition>();
    auto status = run_computation(id1, id2, time_threshold, SD);
    switch (status) {
        case RESULT_OK:
            return 1 - static_cast<float>(SD->Q);
        case RESULT_DISSIMILAR:
            return 2;
        case RESULT_TIMEOUT:
            return 3;
        default:
            throw std::runtime_error("Internal error.");
    }
}


enum status run_computation(const std::string &id1, const std::string &id2, float time_threshold,
                            std::unique_ptr<gsmt::Superposition> &SD) {

    if (not cache) {
        throw std::runtime_error("LRU cache not initialized. You must run init_library first!");
    }

    /* Handle keeps structure from removing from LRU cache */
    auto handle1 = (*cache)[id1];
    auto handle2 = (*cache)[id2];

    gsmt::Structure *s1 = handle1.value().get();
    gsmt::Structure *s2 = handle2.value().get();

    auto Aligner = std::make_unique<gsmt::Aligner>();
    Aligner->setPerformanceLevel(gsmt::PERFORMANCE_CODE::PERFORMANCE_Efficient);
    Aligner->setSimilarityThresholds(threshold, threshold);
    Aligner->setQR0(QR0_default);
    Aligner->setSigma(sigma_default);

    int matchNo;

    gsmt::PSuperposition SD_raw;
    enum status ret;
    if (time_threshold < 0) {
        Aligner->Align(s1, s2, false);
        Aligner->getBestMatch(SD_raw, matchNo);
        ret = SD_raw ? RESULT_OK : RESULT_DISSIMILAR;
    } else {
        std::future<void> future = std::async(std::launch::async, [&] {
            Aligner->Align(s1, s2, false);
        });

        auto timeout = static_cast<long>(time_threshold * 1000);
        std::future_status status = future.wait_for(std::chrono::milliseconds(timeout));

        if (status == std::future_status::ready) {
            Aligner->getBestMatch(SD_raw, matchNo);
            ret = SD_raw ? RESULT_OK : RESULT_DISSIMILAR;
        } else {
            Aligner->stop = true;
            ret = RESULT_TIMEOUT;
        }
        future.wait();
    }
    if (ret == RESULT_OK) {
        SD->CopyFrom(SD_raw);
    }
    return ret;
}


void close_library() {
    delete cache;
}
