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

#define TBB_PREVIEW_CONCURRENT_LRU_CACHE 1
#include "tbb/concurrent_lru_cache.h"


#include "config.h"
#include "gesamtlib/gsmt_aligner.h"

static double threshold;

typedef std::function<std::shared_ptr<gsmt::Structure>(const std::string &)> load_ft;
typedef tbb::concurrent_lru_cache<std::string, std::shared_ptr<gsmt::Structure>, load_ft> cache_t;

static cache_t *cache = nullptr;


std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}


std::shared_ptr<gsmt::Structure>
load_single_structure(const std::string &id, const std::string &directory, bool binary) {

    mmdb::io::File file;

    auto s = std::make_shared<gsmt::Structure>();

    std::stringstream ss;

/* Handle query objects */
    if (id[0] == '_') {

        auto new_id = id.substr(1);
        auto pos = new_id.find(':');
        auto pdbid = new_id.substr(0, pos);
        auto chain = new_id.substr(pos + 1);

        ss << QUERY_CHAINS_DIRECTORY << "/" << new_id << ".bin";

        file.assign(ss.str().c_str());
        if (not file.exists()) {
            std::stringstream ss2;
            ss2 << "Cannot open file: " << file.FileName();
            throw std::runtime_error(ss2.str());
        }
        if (not file.reset(true)) {
            std::stringstream ss2;
            ss2 << "Cannot read from file: " << file.FileName();
            throw std::runtime_error(ss2.str());
        }
        s->read(file);
        file.shut();

        s->prepareStructure(7.0);

#ifndef NDEBUG
        std::cerr << "Loaded query structure: " << id << " from: " << ss.str() << std::endl;
#endif
        return s;
    }

    if (binary) {
        ss << directory << "/" << to_lower(id.substr(0, 2)) << "/" << id << ".bin";
        file.assign(ss.str().c_str());
        if (not file.exists()) {
            std::stringstream ss2;
            ss2 << "Cannot open file: " << file.FileName();
            throw std::runtime_error(ss2.str());
        }
        if (not file.reset(true)) {
            std::stringstream ss2;
            ss2 << "Cannot read from file: " << file.FileName();
            throw std::runtime_error(ss2.str());
        }
        s->read(file);
        file.shut();
    } else {
        auto pos = id.find(':');
        auto pdbid = id.substr(0, pos);
        auto chain = id.substr(pos + 1);
        ss << directory << "/" << to_lower(id.substr(0, 2)) << "/" << to_lower(pdbid) << "_updated.cif";
        auto rc = s->getStructure(ss.str().c_str(), chain.c_str(), -1, false);
        if (rc) {
            std::stringstream ss2;
            ss2 << "Cannot open file " << ss.str();
            throw std::runtime_error(ss2.str());
        }
    }
#ifndef NDEBUG
    std::cerr << "Loaded: " << id << " from: " << ss.str() << std::endl;
#endif
    s->prepareStructure(7.0);

    return s;
}


void init_library(const std::string &archive_directory, const std::string &preload_list_filename, bool binary_archive,
                  double approximation_threshold) {
    threshold = approximation_threshold;

    /* Load structures' names */
    std::vector<std::string> structure_ids;
    std::ifstream file(preload_list_filename);
    if (not file.is_open()) {
        std::stringstream ss;
        ss << "INIT: Cannot read preload list from file: " << preload_list_filename;
        throw std::runtime_error(ss.str());
    }

    std::string line;
    while (std::getline(file, line)) {
        structure_ids.push_back(line);
    }

    /* Fix arguments to avoid global variables */
    auto load = [=](auto &&id) { return load_single_structure(id, archive_directory, binary_archive); };

    if (cache == nullptr) {
#ifndef NDEBUG
        std::cerr << "INIT: initializing new LRU cache of size " << LRU_CACHE_SIZE << std::endl;
#endif
        cache = new cache_t(load, LRU_CACHE_SIZE);
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

    gsmt::PSuperposition SD;
    int matchNo;

    float distance;
    if (time_threshold < 0) {
        Aligner->Align(s1, s2, false);
        Aligner->getBestMatch(SD, matchNo);
        if (SD) {
            distance = 1 - static_cast<float>(SD->Q);
        } else {
            distance = 2;
        }
    } else {
        std::future<void> future = std::async(std::launch::async, [&] {
            Aligner->Align(s1, s2, false);
        });

        auto timeout = static_cast<long>(time_threshold * 1000);
        std::future_status status = future.wait_for(std::chrono::milliseconds(timeout));

        if (status == std::future_status::ready) {
            Aligner->getBestMatch(SD, matchNo);
            if (SD) {
                distance = 1 - static_cast<float>(SD->Q);
            } else {
                distance = 2;
            }
        } else {
            Aligner->stop = true;
            distance = 3;
        }
        future.wait();
    }

    return distance;
}