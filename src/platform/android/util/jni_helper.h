//
// Created by bingjian on 2017/9/20.
//

#ifndef ENGINE_JNIHELP_H
#define ENGINE_JNIHELP_H

#include <jni.h>

#ifndef NELEM
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

extern "C" int registerNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *gMethods,int numMethods);

#endif //ENGINE_JNIHELP_H
