//
// Created by krab1k on 2020-01-08.
//

#include <thread>
#include <future>
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <mutex>
#include <set>

#include "ProteinNativeQScoreDistance.h"
#include "gesamtlib/gsmt_aligner.h"

bool binary = false;
double threshold;
std::string directory;
std::string preload_list;

std::set<std::string> pivots;
std::unordered_map<std::string, std::shared_ptr<gsmt::Structure>> structures;

std::mutex lock;


std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}


std::shared_ptr<gsmt::Structure> load_single_structure(const std::string &id) {

    mmdb::io::File file;

    auto s = std::make_shared<gsmt::Structure>();

    std::stringstream ss;
    if (binary) {
        ss << directory << "/" << to_lower(id.substr(0, 2)) << "/" << id << ".bin";
        file.assign(ss.str().c_str());
        file.reset();
        s->read(file);
        file.shut();
    } else {
        auto pos = id.find(':');
        auto pdbid = id.substr(0, pos);
        auto chain = id.substr(pos + 1);
        ss << directory << "/" << to_lower(id.substr(0, 2)) << "/" << to_lower(pdbid) << "_updated.cif";
        s->getStructure(ss.str().c_str(), chain.c_str(), -1, false);
    }
#ifndef NDEBUG
    std::cout << "Loaded: " << id << " from: " << ss.str() << std::endl;
#endif
    s->prepareStructure(7.0);

    return s;
}


void load_pivots() {
    /* Preload structures */
    for (const auto &id: pivots) {
        auto s = load_single_structure(id);
        structures.insert({id, std::move(s)});
    }
}


void init_library(const std::string &archive_directory, const std::string &pivot_list, bool binary_archive,
                  double approximation_threshold) {
    directory = archive_directory;
    preload_list = pivot_list;
    binary = binary_archive;
    threshold = approximation_threshold;

    /* Populate pivots */
    try {
        std::fstream file(preload_list);

        std::string line;
        while (std::getline(file, line)) {
            pivots.insert(line);
        }
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    load_pivots();
}


float get_distance(const std::string& id1, const std::string &id2) {

    std::shared_ptr<gsmt::Structure> s1;
    std::shared_ptr<gsmt::Structure> s2;

    lock.lock();
    if (structures.find(id1) != structures.end()) {
        s1 = structures[id1];
    } else {
        s1 = load_single_structure(id1);
    }
    lock.unlock();

    lock.lock();
    if (structures.find(id2) != structures.end()) {
        s2 = structures[id2];
    } else {
        s2 = load_single_structure(id2);
    }
    lock.unlock();

    auto Aligner = std::make_unique<gsmt::Aligner>();
    Aligner->setPerformanceLevel(gsmt::PERFORMANCE_CODE::PERFORMANCE_Efficient);
    Aligner->setSimilarityThresholds(threshold, threshold);
    Aligner->setQR0(QR0_default);
    Aligner->setSigma(sigma_default);

    gsmt::PSuperposition SD;
    int matchNo;

    Aligner->Align(s1.get(), s2.get(), false);
    Aligner->getBestMatch(SD, matchNo);

    float distance;
    if (SD) {
        distance = 1 - static_cast<float>(SD->Q);
    } else {
        distance = 2;
    }

    return distance;
}


JNIEXPORT void JNICALL Java_messif_distance_impl_ProteinNativeQScoreDistance_init(JNIEnv *env, jclass,
                                                                                  jstring j_directory, jstring j_list,
                                                                                  jboolean j_binary,
                                                                                  jdouble j_threshold) {
    const char *c_directory = env->GetStringUTFChars(j_directory, nullptr);
    const char *c_list = env->GetStringUTFChars(j_list, nullptr);

    init_library(std::string(c_directory), std::string(c_list), static_cast<bool>(j_binary), j_threshold);

    env->ReleaseStringChars(j_directory, nullptr);
    env->ReleaseStringChars(j_list, nullptr);

}


JNIEXPORT jfloat JNICALL
Java_messif_distance_impl_ProteinNativeQScoreDistance_getNativeDistance(JNIEnv *env, jobject, jstring o1id,
                                                                        jstring o2id, jfloat timeThresholdInSeconds) {

    const char *o1s = env->GetStringUTFChars(o1id, nullptr);
    const char *o2s = env->GetStringUTFChars(o2id, nullptr);

    std::string id1 = std::string(o1s);
    std::string id2 = std::string(o2s);

#ifndef NDEBUG
    std::cout << "Distance between " << id1 << " and " << id2 << std::endl;
#endif

    std::shared_ptr<gsmt::Structure> s1;
    std::shared_ptr<gsmt::Structure> s2;

    if (structures.find(id1) != structures.end()) {
        s1 = structures[id1];
    } else {
        s1 = load_single_structure(id1);
    }

    if (structures.find(id2) != structures.end()) {
        s2 = structures[id2];
    } else {
        s2 = load_single_structure(id2);
    }

    auto Aligner = new gsmt::Aligner();
    Aligner->setPerformanceLevel(gsmt::PERFORMANCE_CODE::PERFORMANCE_Efficient);
    Aligner->setSimilarityThresholds(threshold, threshold);
    Aligner->setQR0(QR0_default);
    Aligner->setSigma(sigma_default);

    gsmt::PSuperposition SD;
    int matchNo;

    std::future<void> future = std::async(std::launch::async, [&] {
        Aligner->Align(s1.get(), s2.get(), false);
    });

    timeThresholdInSeconds = 0.6f;
    auto timeout = static_cast<long>(timeThresholdInSeconds * 1000);
    std::future_status status = future.wait_for(std::chrono::milliseconds(timeout));

    float distance;
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

    delete Aligner;

    env->ReleaseStringChars(o1id, nullptr);
    env->ReleaseStringChars(o1id, nullptr);

    return distance;
}

