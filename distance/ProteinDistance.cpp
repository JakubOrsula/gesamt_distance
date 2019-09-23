#include <thread>
#include <future>
#include <cstdio>
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
#include <mutex>
#include <set>

#include "ProteinDistance.h"
#include "gesamtlib/gsmt_aligner.h"
#include "config.h"


std::mutex lock;

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
    ss << to_lower(pdbid) << "_updated.cif";

    auto filepath = fs::path(directory) / ss.str();

    auto status = s->getStructure(filepath.c_str(), chain.c_str(), -1, false);
    s->prepareStructure(0);

    if (status) {
        std::cout << "Could not load " << filepath << ": " << chain << std::endl;
        throw std::runtime_error("Loading of structure failed");
    } else {
        std::cout << "Loaded: " << id << std::endl;
    }

    lock.lock();
    structures.insert({id, std::move(s)});
    lock.unlock();
}


std::unordered_map<std::string, std::unique_ptr<gsmt::Structure>> preload_structures(const std::set<std::string> &ids) {
    std::unordered_map<std::string, std::unique_ptr<gsmt::Structure>> structures;
    for (const auto &id: ids) {
        load_single_structure(DIRECTORY, id, structures);
    }
    return structures;
}

std::set<std::string> load_ids() {
    std::set<std::string> ids;
    try {
        std::fstream file(LIST);

        std::string line;
        while (std::getline(file, line)) {
            ids.insert(line);
        }
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    return ids;
}


JNIEXPORT jfloat JNICALL
Java_ProteinDistance_getDistance(JNIEnv *env, jobject, jstring o1id, jstring o2id, jfloat timeThresholdInSeconds) {

    const char *o1s = env->GetStringUTFChars(o1id, nullptr);
    const char *o2s = env->GetStringUTFChars(o2id, nullptr);

    std::string id1 = std::string(o1s);
    std::string id2 = std::string(o2s);

    static auto ids = load_ids();
    static auto structures = preload_structures(ids);

    std::cout << "Computing distance between " << id1 << " and " << id2 << std::endl;

    lock.lock();
    auto res1 = structures.find(id1) == structures.end();
    lock.unlock();

    if (res1) {
        load_single_structure(DIRECTORY, id1, structures);
    }

    lock.lock();
    auto res2 = structures.find(id2) == structures.end();
    lock.unlock();

    if (res2) {
        load_single_structure(DIRECTORY, id2, structures);
    }

    auto Aligner = new gsmt::Aligner();
    Aligner->setPerformanceLevel(gsmt::PERFORMANCE_CODE::PERFORMANCE_Efficient);
    Aligner->setSimilarityThresholds(0.0, 0.0);
    Aligner->setQR0(QR0_default);
    Aligner->setSigma(sigma_default);

    gsmt::PSuperposition SD;
    int matchNo;

    lock.lock();
    auto s1 = structures[id1].get();
    auto s2 = structures[id2].get();
    lock.unlock();

    std::future<void> future = std::async(std::launch::async,[&]{
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
        std::cout << "Unloading " << id1 << std::endl;
        structures.erase(id1);
    }
    if (ids.find(id2) == ids.end()) {
        std::cout << "Unloading " << id2 << std::endl;
        structures.erase(id2);
    }
    lock.unlock();

    env->ReleaseStringChars(o1id, nullptr);
    env->ReleaseStringChars(o1id, nullptr);

    return distance;
}
