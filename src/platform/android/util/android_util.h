
#ifndef __ANDROIDUTIL_H__
#define __ANDROIDUTIL_H__

#include <jni.h>
#include <string>
#include <vector>
#include <mutex>
#include <memory>

#include "jni_envelope.h"

class JavaClass;

class JavaArray;

class Constructor;

class ObjectField;

class VoidMethod;

class IntMethod;

class LongMethod;

class BooleanMethod;

class StringMethod;

class ObjectMethod;

class ObjectArrayMethod;

class StaticObjectMethod;

class StaticLongMethod;

class StaticVoidMethod;

class JObject {
public:
    JObject(jobject object);

    ~JObject();

    jobject GetObj() {
        return m_Object;
    }

private:
    JObject();

    JObject(const JObject &);

    const JObject &operator=(const JObject &rhis);

private:
    jobject m_Object;
};

class JString {

public:
    JString(JNIEnv *env, const std::string &str, bool emptyIsNull = true);

    ~JString();

    jstring j();

private:
    JNIEnv *m_env;
    jstring m_J;

private:
    JString(const JString &);

    const JString &operator=(const JString &);
};

class AndroidUtil {

private:
    static JavaVM *JAVA_VM;

public:
    static JavaClass Class_java_lang_Object;
    static JavaClass CLASS_java_String;
    static JavaClass CLASS_java_DataTransHub;
    static JavaClass CLASS_java_IUploadImp;
    static JavaClass CLASS_java_IUploadCallback;
    static JavaClass CLASS_java_LogInfo;
    static std::shared_ptr<VoidMethod> Method_DataTransHub_uploadImp;
    static std::shared_ptr<VoidMethod> Method_DataTransHub_uploadCallback;


public:
    static bool init(JavaVM *jvm);

    static JNIEnv *getEnv();

    static JNIEnv *attachCurrentThread();

    static void detachCurrentThread();

    static std::string fromJavaString(JNIEnv *env, jstring from);

    static jstring createJavaString(JNIEnv *env, const std::string &str);

    static std::string convertNonUtfString(const std::string &str);

    static jintArray createJavaIntArray(JNIEnv *env, const std::vector<jint> &data);

    static jbyteArray createJavaByteArray(JNIEnv *env, const std::vector<jbyte> &data);
};

inline jstring JString::j() { return m_J; }

#endif /* __ANDROIDUTIL_H__ */
