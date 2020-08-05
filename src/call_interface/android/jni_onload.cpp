//
// Created by bingjian on 2017/9/20.
//
#include <jni.h>
#include <stdlib.h>
#include "android_util.h"

#define EXPORT __attribute__((visibility("default")))

extern int registerDataTransHub(JNIEnv *);

extern int registerFlog(JNIEnv *);

struct RegistrationMethod {
    const char *name;

    int (*func)(JNIEnv *);
};

static RegistrationMethod gRegMethods[] = {
        {"DataTransHub", registerDataTransHub},
        {"Flog",         registerFlog},
};

EXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint result = -1;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }

    const RegistrationMethod *method = gRegMethods;
    const RegistrationMethod *end = method + sizeof(gRegMethods) / sizeof(RegistrationMethod);
    while (method != end) {
        if (method->func(env) < 0) {
            return result;
        }
        method++;
    }

    AndroidUtil::init(vm);
    return JNI_VERSION_1_4;
}

EXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {

}