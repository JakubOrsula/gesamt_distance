//
// Created by krab1k on 2020-01-08.
//

#include <string>
#include <iostream>
#include <algorithm>

#include "protein_distance.h"
#include "ProteinNativeQScoreDistance.h"


static const int LRU_CACHE_SIZE = 600;


JNIEXPORT void JNICALL Java_messif_distance_impl_ProteinNativeQScoreDistance_init(JNIEnv *env, jclass,
                                                                                  jstring j_directory,
                                                                                  jdouble j_threshold) {

    if (j_directory == nullptr) {
        jclass Exception = env->FindClass("java/lang/NullPointerException");
        env->ThrowNew(Exception, "No archive directory specified");
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

#ifndef NDEBUG
    std::cerr << "JNI: Initializing the GESAMT library" << std::endl;
    std::cerr << "JNI: Parameters: archive_dir = " << c_directory << " threshold = " << j_threshold << std::endl;
#endif

    try {
        init_library(std::string(c_directory), "/dev/null", true, j_threshold, LRU_CACHE_SIZE);
    }
    catch (std::exception &e) {
        jclass Exception = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(Exception, e.what());
    }

    env->ReleaseStringChars(j_directory, nullptr);
}


JNIEXPORT jfloatArray JNICALL
Java_messif_distance_impl_ProteinNativeQScoreDistance_getStats(JNIEnv *env, jclass, jstring o1id, jstring o2id,
                                                               jfloat min_qscore) {
    if (o1id == nullptr) {
        jclass Exception = env->FindClass("java/lang/NullPointerException");
        env->ThrowNew(Exception, "First object not specified");
        return {};
    }

    if (o2id == nullptr) {
        jclass Exception = env->FindClass("java/lang/NullPointerException");
        env->ThrowNew(Exception, "Second object not specified");
        return {};
    }

    const char *o1s = env->GetStringUTFChars(o1id, nullptr);
    const char *o2s = env->GetStringUTFChars(o2id, nullptr);

    std::string id1 = std::string(o1s);
    std::string id2 = std::string(o2s);

    env->ReleaseStringChars(o1id, nullptr);
    env->ReleaseStringChars(o1id, nullptr);

#ifndef NDEBUG
    std::cerr << "JNI: Computing distance between " << id1 << " and " << id2 << " with minimum required Q-score "
              << min_qscore << std::endl;
#endif

    auto SD = std::make_unique<gsmt::Superposition>();

    float native_result[20];
    std::fill_n(native_result, 20, -1);

    jfloatArray result = env->NewFloatArray(20);
    try {
        auto status = run_computation(id1, id2, min_qscore, SD);
        if (status == RESULT_OK) {
            native_result[0] = static_cast<float>(SD->Q);
            native_result[1] = static_cast<float>(SD->rmsd);
            native_result[2] = static_cast<float>(SD->seqId);
            native_result[3] = static_cast<float>(SD->Nalgn);
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    native_result[4 + i * 4 + j] = static_cast<float>(SD->T[i][j]);
                }
            }
        }
    } catch (std::exception &e) {
        jclass Exception = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(Exception, e.what());
        return {};
    }

    env->SetFloatArrayRegion(result, 0, 20, native_result);
    return result;
}
