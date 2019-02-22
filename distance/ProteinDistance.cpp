#include "ProteinDistance.h"
#include "distance.h"


JNIEXPORT jfloat JNICALL
Java_ProteinDistance_getDistance(JNIEnv *env, jobject, jstring o1id, jstring o2id, jfloat) {

    const char *o1s = env->GetStringUTFChars(o1id, nullptr);
    const char *o2s = env->GetStringUTFChars(o2id, nullptr);

    float dist = get_distance(o1s, o2s);

    env->ReleaseStringChars(o1id, nullptr);
    env->ReleaseStringChars(o1id, nullptr);

    return dist;
}
