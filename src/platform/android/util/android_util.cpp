#include <stddef.h>
#include <jni.h>
#include "android_util.h"

JavaVM *AndroidUtil::JAVA_VM = NULL;

JavaClass AndroidUtil::Class_java_lang_Object("java/lang/Object");
JavaClass AndroidUtil::CLASS_java_String("java/lang/String");
JavaClass AndroidUtil::CLASS_java_DataTransHub("com/iget/datatranshub/DataTransHub");
JavaClass AndroidUtil::CLASS_java_IUploadImp("com/iget/datatranshub/callback/IUploadImp");
JavaClass AndroidUtil::CLASS_java_IUploadCallback("com/iget/datatranshub/callback/IUploadCallback");
JavaClass AndroidUtil::CLASS_java_LogInfo("com/iget/flog/LogInfo");
std::shared_ptr<VoidMethod> AndroidUtil::Method_DataTransHub_uploadImp;
std::shared_ptr<VoidMethod> AndroidUtil::Method_DataTransHub_uploadCallback;

JNIEnv *AndroidUtil::getEnv() {
    JNIEnv *env;
    JAVA_VM->GetEnv((void **) &env, JNI_VERSION_1_2);
    return env;
}

JNIEnv *AndroidUtil::attachCurrentThread() {
    JNIEnv *env;
    JAVA_VM->AttachCurrentThread(&env, NULL);
    return env;
}

void AndroidUtil::detachCurrentThread() {
    JAVA_VM->DetachCurrentThread();
}

bool AndroidUtil::init(JavaVM *jvm) {
    JAVA_VM = jvm;
    CLASS_java_String.j();
    CLASS_java_DataTransHub.j();
    CLASS_java_IUploadImp.j();
    CLASS_java_LogInfo.j();
    CLASS_java_IUploadCallback.j();

    Method_DataTransHub_uploadImp = std::shared_ptr<VoidMethod>(
            new VoidMethod(AndroidUtil::CLASS_java_IUploadImp, "upload",
                           "(Ljava/lang/String;)"));
    Method_DataTransHub_uploadCallback = std::shared_ptr<VoidMethod>(
            new VoidMethod(AndroidUtil::CLASS_java_IUploadCallback, "finish",
                           "()"));
    return true;
}

JObject::JObject(jobject object) {
    m_Object = AndroidUtil::getEnv()->NewGlobalRef(object);
}

JObject::~JObject() {
    AndroidUtil::getEnv()->DeleteGlobalRef(m_Object);
}

JString::JString(JNIEnv *env, const std::string &str, bool emptyIsNull) : m_env(env) {
    m_J = (emptyIsNull && str.empty()) ? NULL : env->NewStringUTF(str.c_str());
}

JString::~JString() {
    if (m_J != NULL) {
        m_env->DeleteLocalRef(m_J);
    }
}

jstring AndroidUtil::createJavaString(JNIEnv *env, const std::string &str) {
    if (str.empty()) {
        return env->NewStringUTF("");
    }
    return env->NewStringUTF(str.c_str());
}

std::string AndroidUtil::convertNonUtfString(const std::string &str) {
    const int len = str.length();
    if (len == 0) {
        return str;
    }

    JNIEnv *env = getEnv();

    jchar *chars = new jchar[len];
    for (int i = 0; i < len; ++i) {
        chars[i] = (unsigned char) str[i];
    }
    jstring javaString = env->NewString(chars, len);
    const std::string result = fromJavaString(env, javaString);
    env->DeleteLocalRef(javaString);
    delete[] chars;

    return result;
}

std::string AndroidUtil::fromJavaString(JNIEnv *env, jstring from) {
    if (from == NULL) {
        return std::string("");
    }
    const char *data = env->GetStringUTFChars(from, 0);
    const std::string result(data);
    env->ReleaseStringUTFChars(from, data);
    return result;
}

jintArray AndroidUtil::createJavaIntArray(JNIEnv *env, const std::vector<jint> &data) {
    std::size_t size = data.size();
    jintArray array = env->NewIntArray(size);
    env->SetIntArrayRegion(array, 0, size, &data.front());
    return array;
}

jbyteArray AndroidUtil::createJavaByteArray(JNIEnv *env, const std::vector<jbyte> &data) {
    std::size_t size = data.size();
    jbyteArray array = env->NewByteArray(size);
    env->SetByteArrayRegion(array, 0, size, &data.front());
    return array;
}

