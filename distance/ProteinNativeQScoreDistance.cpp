//
// Created by krab1k on 2020-01-08.
//

#include <string>
#include <iostream>
#include <chrono>
#include <mysql/mysql.h>

#include "config.h"
#include "protein_distance.h"

#include "ProteinNativeQScoreDistance.h"


static const int LRU_CACHE_SIZE = 600;


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

    if (j_threshold < 0 or j_threshold > 1) {
        jclass Exception = env->FindClass("java/lang/IllegalArgumentException");
        const std::string message =
                "Approximative threshold of " + std::to_string(j_threshold) + " has to be between 0 and 1";
        const char *c_message = message.c_str();
        env->ThrowNew(Exception, c_message);
        return;
    }

    const char *c_directory = env->GetStringUTFChars(j_directory, nullptr);
    const char *c_list = env->GetStringUTFChars(j_list, nullptr);

#ifndef NDEBUG
    std::cerr << "JNI: Initializing the GESAMT library" << std::endl;
    std::cerr << "JNI: Parameters: archive_dir = " << c_directory << " preload_list = " << c_list << " binary = "
              << static_cast<bool>(j_binary) << " threshold = " << j_threshold << std::endl;
#endif

    try {
        init_library(std::string(c_directory), std::string(c_list), static_cast<bool>(j_binary), j_threshold,
                     LRU_CACHE_SIZE);
    }
    catch (std::exception &e) {
        jclass Exception = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(Exception, e.what());
    }

    env->ReleaseStringChars(j_directory, nullptr);
    env->ReleaseStringChars(j_list, nullptr);

    if (mysql_library_init(0, nullptr, nullptr)) {
        jclass Exception = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(Exception, "MYSQL init library failed");
        return;
    }
}


JNIEXPORT jfloat JNICALL
Java_messif_distance_impl_ProteinNativeQScoreDistance_getNativeDistance(JNIEnv *env, jobject, jstring o1id,
                                                                        jstring o2id, jfloat timeThresholdInSeconds,
                                                                        jboolean storeResults) {

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

    if (timeThresholdInSeconds > 3600) {
        jclass Exception = env->FindClass("java/lang/IllegalArgumentException");
        const std::string message =
                "Time threshold of " + std::to_string(timeThresholdInSeconds) + " is higher than allowed (3600 s)";
        const char *c_message = message.c_str();
        env->ThrowNew(Exception, c_message);
        return -1;
    }

    const char *o1s = env->GetStringUTFChars(o1id, nullptr);
    const char *o2s = env->GetStringUTFChars(o2id, nullptr);

    std::string id1 = std::string(o1s);
    std::string id2 = std::string(o2s);

    env->ReleaseStringChars(o1id, nullptr);
    env->ReleaseStringChars(o1id, nullptr);

#ifndef NDEBUG
    std::cerr << "JNI: Computing distance between " << id1 << " and " << id2 << " using time threshold of "
              << timeThresholdInSeconds << "; storing into DB: " << std::to_string(bool(storeResults)) << std::endl;
#endif


    auto begin = std::chrono::high_resolution_clock::now();
    auto SD = std::make_unique<gsmt::Superposition>();
    float qscore;
    try {
        auto status = run_computation(id1, id2, timeThresholdInSeconds, SD);
        switch (status) {
            case RESULT_OK:
                qscore = 1 - static_cast<float>(SD->Q);
                break;
            case RESULT_DISSIMILAR:
                qscore = 2;
                break;
            case RESULT_TIMEOUT:
                qscore = 3;
                break;
            default:
                throw std::runtime_error("Internal error.");
        }
    }
    catch (std::exception &e) {
        jclass Exception = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(Exception, e.what());
        return -1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    if (storeResults and elapsed.count() > 1000) {
        MYSQL *conn = mysql_init(nullptr);
        if (conn == nullptr) {
            jclass Exception = env->FindClass("java/lang/RuntimeException");
            env->ThrowNew(Exception, mysql_error(conn));
            return -1;
        }

        if (mysql_real_connect(conn, "localhost", DB_USER, DB_PASS, DB_NAME, 0, nullptr, 0) == nullptr) {
            jclass Exception = env->FindClass("java/lang/RuntimeException");
            env->ThrowNew(Exception, mysql_error(conn));
            mysql_close(conn);
            return -1;
        }

        std::string query = "INSERT IGNORE INTO queriesNearestNeighboursStats VALUES (" +
                            std::to_string(elapsed.count()) + ", NULL, \"" + id1 + "\", \"" + id2 + "\", " +
                            std::to_string(SD->Q) + ", " + std::to_string(SD->rmsd) + ", " + std::to_string(SD->Nalgn) +
                            ", " + std::to_string(SD->seqId) + ")";

#ifndef NDEBUG
        std::cerr << "DB query: " << query << std::endl;
#endif

        if (mysql_query(conn, query.c_str())) {
            jclass Exception = env->FindClass("java/lang/RuntimeException");
            env->ThrowNew(Exception, mysql_error(conn));
            mysql_close(conn);
            return -1;
        }
        mysql_close(conn);
    }

    return qscore;
}
