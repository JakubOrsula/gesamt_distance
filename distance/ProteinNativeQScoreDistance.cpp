//
// Created by krab1k on 2020-01-08.
//

#include <string>
#include <iostream>
#include <sstream>

#include "protein_distance.h"
#include "ProteinNativeQScoreDistance.h"


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
        jclass Exception = env->FindClass("java/lang/Exception");
        std::stringstream ss;
        ss << "Approximative threshold of " << j_threshold << " has to be between 0 and 1";
        const std::string message = ss.str();
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

    if (timeThresholdInSeconds > 3600) {
        jclass Exception = env->FindClass("java/lang/Exception");
        std::stringstream ss;
        ss << "Time threshold of " << timeThresholdInSeconds << " is higher than allowed (3600 s)";
        const std::string message = ss.str();
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
              << timeThresholdInSeconds << std::endl;
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
