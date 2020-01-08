//
// Created by krab1k on 2020-01-08.
//

#include <thread>
#include <future>
#include <string>
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
std::mutex lock;

std::set<std::string> ids;
std::unordered_map<std::string, std::unique_ptr<gsmt::Structure>> structures;


std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

void load_single_structure(const std::string &id) {

    mmdb::io::File file;

    auto s = std::make_unique<gsmt::Structure>();

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
    s->prepareStructure(0);

    lock.lock();
    structures.insert({id, std::move(s)});
    lock.unlock();
}


JNIEXPORT void JNICALL Java_messif_distance_impl_ProteinNativeQScoreDistance_init(JNIEnv *env, jobject,
                                                                                  jstring j_directory, jstring j_list,
                                                                                  jboolean j_binary,
                                                                                  jdouble j_threshold) {
    const char *c_directory = env->GetStringUTFChars(j_directory, nullptr);
    const char *c_list = env->GetStringUTFChars(j_list, nullptr);
    directory = std::string(c_directory);
    preload_list = std::string(c_list);
    binary = static_cast<bool>(j_binary);
    threshold = j_threshold;
    env->ReleaseStringChars(j_directory, nullptr);
    env->ReleaseStringChars(j_list, nullptr);

    /* Populate pivots */
    try {
        std::fstream file(preload_list);

        std::string line;
        while (std::getline(file, line)) {
            ids.insert(line);
        }
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    /* Preload structures */
    for (const auto &id: ids) {
        load_single_structure(id);
    }
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

    lock.lock();
    auto res1 = structures.find(id1) == structures.end();
    lock.unlock();

    if (res1) {
        load_single_structure(id1);
    }

    lock.lock();
    auto res2 = structures.find(id2) == structures.end();
    lock.unlock();

    if (res2) {
        load_single_structure(id2);
    }

    auto Aligner = new gsmt::Aligner();
    Aligner->setPerformanceLevel(gsmt::PERFORMANCE_CODE::PERFORMANCE_Efficient);
    Aligner->setSimilarityThresholds(threshold, threshold);
    Aligner->setQR0(QR0_default);
    Aligner->setSigma(sigma_default);

    gsmt::PSuperposition SD;
    int matchNo;

    lock.lock();
    auto s1 = structures[id1].get();
    auto s2 = structures[id2].get();
    lock.unlock();

    std::future<void> future = std::async(std::launch::async, [&] {
        Aligner->Align(s1, s2, false);
    });

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

    lock.lock();
    if (ids.find(id1) == ids.end()) {
#ifndef NDEBUG
        std::cout << "Unloading " << id1 << std::endl;
#endif
        structures.erase(id1);
    }
    if (ids.find(id2) == ids.end()) {
#ifndef NDEBUG
        std::cout << "Unloading " << id2 << std::endl;
#endif
        structures.erase(id2);
    }
    lock.unlock();

    env->ReleaseStringChars(o1id, nullptr);
    env->ReleaseStringChars(o1id, nullptr);

    return distance;
}
