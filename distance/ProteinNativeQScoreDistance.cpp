//
// Created by krab1k on 2020-01-08.
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

#include "ProteinNativeQScoreDistance.h"
#include "gesamtlib/gsmt_aligner.h"


#define LRU_CACHE_SIZE_BONUS 1000

static double threshold;

typedef std::function<std::shared_ptr<gsmt::Structure>(const std::string &)> load_ft;
typedef tbb::concurrent_lru_cache<std::string, std::shared_ptr<gsmt::Structure>, load_ft> cache_t;

static cache_t *cache;


std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}


std::shared_ptr<gsmt::Structure>
load_single_structure(const std::string &id, const std::string &directory, bool binary) {

    mmdb::io::File file;

    auto s = std::make_shared<gsmt::Structure>();

    std::stringstream ss;
    if (binary) {
        ss << directory << "/" << to_lower(id.substr(0, 2)) << "/" << id << ".bin";
        file.assign(ss.str().c_str());
        if (not file.exists()) {
            std::stringstream ss2;
            ss2 << "Cannot open file " << file.FileName();
            throw std::runtime_error(ss2.str());
        }
        file.reset();
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
    std::cout << "Loaded: " << id << " from: " << ss.str() << std::endl;
#endif
    s->prepareStructure(7.0);

    return s;
}


void init_library(const std::string &archive_directory, const std::string &pivot_list, bool binary_archive,
                  double approximation_threshold) {
    threshold = approximation_threshold;

    /* Populate pivots */
    std::vector<std::string> pivots;
    try {
        std::fstream file(pivot_list);

        std::string line;
        while (std::getline(file, line)) {
            pivots.push_back(line);
        }
    }
    catch (std::exception &e) {
        std::stringstream ss;
        ss << "INIT: Cannot read preload list from file: " << pivot_list;
        throw std::runtime_error(ss.str());
    }

    /* Fix arguments to avoid global variables */
    auto load = [=](auto && id) { return load_single_structure(id, archive_directory, binary_archive); };

    cache = new cache_t(load, pivots.size() + LRU_CACHE_SIZE_BONUS);

    try {
        /* Preload structures */
        for (const auto &id: pivots) {
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


JNIEXPORT void JNICALL Java_messif_distance_impl_ProteinNativeQScoreDistance_init(JNIEnv *env, jclass,
                                                                                  jstring j_directory, jstring j_list,
                                                                                  jboolean j_binary,
                                                                                  jdouble j_threshold) {

    if (j_directory == nullptr) {
        jclass Exception = env->FindClass("java/lang/NullPointerException");
        env->ThrowNew(Exception, "No archive directory specified");
        return;
    }

    if (j_list == nullptr) {
        jclass Exception = env->FindClass("java/lang/NullPointerException");
        env->ThrowNew(Exception, "No preload list specified");
        return;
    }

    const char *c_directory = env->GetStringUTFChars(j_directory, nullptr);
    const char *c_list = env->GetStringUTFChars(j_list, nullptr);

#ifndef NDEBUG
    std::cout << "JNI: Initializing the GESAMT library" << std::endl;
    std::cout << "JNI: Parameters: archive_dir = " << c_directory << " preload_list = " << c_list << " binary = " <<
                 static_cast<bool>(j_binary) << " threshold = " << j_threshold << std::endl;
#endif

    try {
        init_library(std::string(c_directory), std::string(c_list), static_cast<bool>(j_binary), j_threshold);
    }
    catch (std::exception &e) {
        jclass Exception = env->FindClass("java/lang/Exception");
        env->ThrowNew(Exception, e.what());
    }

    env->ReleaseStringChars(j_directory, nullptr);
    env->ReleaseStringChars(j_list, nullptr);
}


JNIEXPORT jfloat JNICALL
Java_messif_distance_impl_ProteinNativeQScoreDistance_getNativeDistance(JNIEnv *env, jobject, jstring o1id,
                                                                        jstring o2id, jfloat timeThresholdInSeconds) {

    if (o1id == nullptr) {
        jclass Exception = env->FindClass("java/lang/NullPointerException");
        env->ThrowNew(Exception, "First object not specified");
        return -1;
    }

    if (o2id == nullptr) {
        jclass Exception = env->FindClass("java/lang/NullPointerException");
        env->ThrowNew(Exception, "Second object not specified");
        return -1;
    }

    const char *o1s = env->GetStringUTFChars(o1id, nullptr);
    const char *o2s = env->GetStringUTFChars(o2id, nullptr);

    std::string id1 = std::string(o1s);
    std::string id2 = std::string(o2s);

    env->ReleaseStringChars(o1id, nullptr);
    env->ReleaseStringChars(o1id, nullptr);

#ifndef NDEBUG
    std::cout << "JNI: Computing distance between " << id1 << " and " << id2 << std::endl;
#endif
    try {
        return get_distance(id1, id2, timeThresholdInSeconds);
    }
    catch (std::exception &e) {
        jclass Exception = env->FindClass("java/lang/Exception");
        env->ThrowNew(Exception, e.what());
        return -1;
    }
}
