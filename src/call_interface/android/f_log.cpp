#include <jni.h>
#include <string>
#include "jni_helper.h"
#include "android_util.h"
#include "f_log.h"

static int GetVersion(JNIEnv *env, jobject obj) {
#ifdef VERSION_CODE
    return VERSION_CODE;
#else
    return 0;
#endif
}

static jlong
MakeNativeFlog(JNIEnv *env, jobject obj, jstring cacheDir, jstring dataDir,
               jstring encryptKey) {
    std::string cacheDirCStr = AndroidUtil::fromJavaString(env, cacheDir);
    std::string dataDirCstr = AndroidUtil::fromJavaString(env, dataDir);
    std::string encryptKeyCstr = AndroidUtil::fromJavaString(env, encryptKey);

    future::F_Log *flog = future::F_Log::MakeLog(cacheDirCStr,
                                                 dataDirCstr,
                                                 encryptKeyCstr);
    return (jlong) flog;
}

static void ReleaseNativeFlog(JNIEnv *env, jobject obj, jlong nativeF_log) {
    future::F_Log *flog = reinterpret_cast<future::F_Log *>(nativeF_log);
    if (flog == NULL) {
        return;
    }
    future::F_Log::Destroy(flog);
}

static void SetUploadImp(JNIEnv *env, jobject obj, jlong nativeF_Log, jobject uploadImp) {
    future::F_Log *flog = reinterpret_cast<future::F_Log *>(nativeF_Log);
    if (flog == NULL) {
        return;
    }

    std::shared_ptr<JObject> jCallback(new JObject(uploadImp));
    flog->SetUploadImp([jCallback](
            const std::string &filePath) {
        jstring filePathJstr = AndroidUtil::createJavaString(
                AndroidUtil::getEnv(),
                filePath);
        AndroidUtil::Method_DataTransHub_uploadImp->call(
                jCallback->GetObj(),
                filePathJstr);
    });
}

static void SetFileMaxSize(JNIEnv *env, jobject obj, jlong nativeF_Log, jlong fileMaxSize) {
    future::F_Log *flog = reinterpret_cast<future::F_Log *>(nativeF_Log);
    if (flog == NULL) {
        return;
    }
    flog->SetFileMaxSize(fileMaxSize);
}

static void SetBufferSize(JNIEnv *env, jobject obj, jlong nativeF_Log, jlong buferSize) {
    future::F_Log *flog = reinterpret_cast<future::F_Log *>(nativeF_Log);
    if (flog == NULL) {
        return;
    }
    flog->SetBufferSize(buferSize);
}

static void SetExpiredTime(JNIEnv *env, jobject obj, jlong nativeF_Log, jlong expiredTime) {
    future::F_Log *flog = reinterpret_cast<future::F_Log *>(nativeF_Log);
    if (flog == NULL) {
        return;
    }
    flog->SetExpiredTime(expiredTime);
}

static void
SetReportingInterval(JNIEnv *env, jobject obj, jlong nativeF_Log, jlong reportingInterval) {
    future::F_Log *flog = reinterpret_cast<future::F_Log *>(nativeF_Log);
    if (flog == NULL) {
        return;
    }
    flog->SetReportingInterval(reportingInterval);
}

static void
SetRetryInterval(JNIEnv *env, jobject obj, jlong nativeF_Log, jlong retryInterval) {
    future::F_Log *flog = reinterpret_cast<future::F_Log *>(nativeF_Log);
    if (flog == NULL) {
        return;
    }
    flog->SetRetryInterval(retryInterval);
}

static void
SetDataFilePrefix(JNIEnv *env, jobject obj, jlong nativeF_Log, jstring filePrefix) {
    future::F_Log *flog = reinterpret_cast<future::F_Log *>(nativeF_Log);
    if (flog == NULL) {
        return;
    }
    std::string filePrefixCstr = AndroidUtil::fromJavaString(env, filePrefix);
    flog->SetDataFilePrefix(filePrefixCstr);
}

static void
SetUploadTriggerWayWay(JNIEnv *env, jobject obj, jlong nativeF_Log,
                       jint uploadTriggerWayWay) {
    future::F_Log *flog = reinterpret_cast<future::F_Log *>(nativeF_Log);
    if (flog == NULL) {
        return;
    }
    future::DataTransHub::UploadTriggerWay uploadTriggerWay;
    switch (uploadTriggerWayWay) {
        case 0:
            uploadTriggerWay = future::DataTransHub::UploadTriggerWay::Auto;
            break;
        case 1:
            uploadTriggerWay = future::DataTransHub::UploadTriggerWay::Manual;
            break;
    }
    flog->SetUploadTriggerWayWay(uploadTriggerWay);
}

static void
SetWriteDiskPeriod(JNIEnv *env, jobject obj, jlong nativeF_Log, jlong period) {
    future::F_Log *flog = reinterpret_cast<future::F_Log *>(nativeF_Log);
    if (flog == NULL) {
        return;
    }

    flog->SetWriteDiskPeriod(period);
}

static void Start(JNIEnv *env, jobject obj, jlong nativeF_Log) {
    future::F_Log *flog = reinterpret_cast<future::F_Log *>(nativeF_Log);
    if (flog == NULL) {
        return;
    }
    flog->Start();
}

static void ReaWaken(JNIEnv *env, jobject obj, jlong nativeF_Log) {
    future::F_Log *flog = reinterpret_cast<future::F_Log *>(nativeF_Log);
    if (flog == NULL) {
        return;
    }
    flog->ReaWaken();
}

static void
ManualTriggerUpload(JNIEnv *env, jobject obj, jlong nativeF_Log, jobject callback) {
    future::F_Log *flog = reinterpret_cast<future::F_Log *>(nativeF_Log);
    if (flog == NULL) {
        return;
    }

    std::shared_ptr<JObject> jCallback(new JObject(callback));
    flog->ManualTriggerUpload([jCallback]() {
        if (AndroidUtil::getEnv()->IsSameObject(jCallback->GetObj(), nullptr)) {
            return;
        }
        AndroidUtil::Method_DataTransHub_uploadCallback->call(jCallback->GetObj());
    });
}

static void Write(JNIEnv *env, jobject obj, jlong nativeF_Log, jobject logInfo, jbyteArray data) {
    future::F_Log *flog = reinterpret_cast<future::F_Log *>(nativeF_Log);
    if (flog == NULL) {
        return;
    }

    future::F_Log::Info info;
    jfieldID mLevel = env->GetFieldID(AndroidUtil::CLASS_java_LogInfo.j(), "mLevel", "I");
    jfieldID mTag = env->GetFieldID(AndroidUtil::CLASS_java_LogInfo.j(), "mTag",
                                    "Ljava/lang/String;");
    jfieldID mFilename = env->GetFieldID(AndroidUtil::CLASS_java_LogInfo.j(), "mFilename",
                                         "Ljava/lang/String;");
    jfieldID mFuncname = env->GetFieldID(AndroidUtil::CLASS_java_LogInfo.j(), "mFuncname",
                                         "Ljava/lang/String;");
    jfieldID mLine = env->GetFieldID(AndroidUtil::CLASS_java_LogInfo.j(), "mLine", "I");
    jfieldID mPid = env->GetFieldID(AndroidUtil::CLASS_java_LogInfo.j(), "mPid", "J");
    jfieldID mTid = env->GetFieldID(AndroidUtil::CLASS_java_LogInfo.j(), "mTid", "J");
    jfieldID mMaintid = env->GetFieldID(AndroidUtil::CLASS_java_LogInfo.j(), "mMaintid", "J");

    jint level = env->GetIntField(logInfo, mLevel);
    jstring tag = (jstring) env->GetObjectField(logInfo, mTag);
    jstring filename = (jstring) env->GetObjectField(logInfo, mFilename);
    jstring funcname = (jstring) env->GetObjectField(logInfo, mFuncname);
    jint line = env->GetIntField(logInfo, mLine);
    jlong pid = env->GetLongField(logInfo, mPid);
    jlong tid = env->GetLongField(logInfo, mTid);
    jlong maintid = env->GetLongField(logInfo, mMaintid);

    switch (level) {
        case 0:
            info.level = future::F_Log::Priority::VERBOSE;
            break;
        case 1:
            info.level = future::F_Log::Priority::DEBUG;
            break;
        case 2:
            info.level = future::F_Log::Priority::INFO;
            break;
        case 3:
            info.level = future::F_Log::Priority::WARNING;
            break;
        case 4:
            info.level = future::F_Log::Priority::ERROR;
            break;
        case 5:
            info.level = future::F_Log::Priority::FATAL;
            break;
        case 6:
            info.level = future::F_Log::Priority::NONE;
            break;
    }
    info.line = line;
    info.funcname = AndroidUtil::fromJavaString(env, funcname);
    info.filename = AndroidUtil::fromJavaString(env, filename);
    info.tag = AndroidUtil::fromJavaString(env, tag);
    info.pid = pid;
    info.tid = tid;
    info.maintid = maintid;

    std::size_t dataLen = env->GetArrayLength(data);
    std::vector<unsigned char> cData(dataLen);

    jbyte *dataJava = env->GetByteArrayElements(data, NULL);
    for (int i = 0; i < dataLen; i++) {
        cData[i] = dataJava[i];
    }
    env->ReleaseByteArrayElements(data, dataJava, JNI_ABORT);
    flog->Write(info, cData);
}

static void NotifyUploadSuccess(JNIEnv *env, jobject obj, jlong nativeF_Log, jstring filePath) {
    future::F_Log *flog = reinterpret_cast<future::F_Log *>(nativeF_Log);
    if (flog == NULL) {
        return;
    }
    std::string filePathCstr = AndroidUtil::fromJavaString(env, filePath);
    flog->NotifyUploadSuccess(filePathCstr);
}

static void NotifyUploadFailed(JNIEnv *env, jobject obj, jlong nativeF_Log, jstring filePath) {
    future::F_Log *flog = reinterpret_cast<future::F_Log *>(nativeF_Log);
    if (flog == NULL) {
        return;
    }
    std::string filePathCstr = AndroidUtil::fromJavaString(env, filePath);
    flog->NotifyUploadFailed(filePathCstr);
}

static JNINativeMethod gJavaFlogMethods[] = {
        {"getVersion",             "()I",                                                       (void *) GetVersion},
        {"makeNativeFlog",         "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J", (void *) MakeNativeFlog},
        {"releaseNativeFlog",      "(J)V",                                                      (void *) ReleaseNativeFlog},
        {"setUploadImp",           "(JLcom/iget/datatranshub/callback/IUploadImp;)V",           (void *) SetUploadImp},
        {"setFileMaxSize",         "(JJ)V",                                                     (void *) SetFileMaxSize},
        {"setBufferSize",          "(JJ)V",                                                     (void *) SetBufferSize},
        {"setExpiredTime",         "(JJ)V",                                                     (void *) SetExpiredTime},
        {"setReportingInterval",   "(JJ)V",                                                     (void *) SetReportingInterval},
        {"setRetryInterval",       "(JJ)V",                                                     (void *) SetRetryInterval},
        {"setDataFilePrefix",      "(JLjava/lang/String;)V",                                    (void *) SetDataFilePrefix},
        {"setUploadTriggerWayWay", "(JI)V",                                                     (void *) SetUploadTriggerWayWay},
        {"setWriteDiskPeriod",     "(JJ)V",                                                     (void *) SetWriteDiskPeriod},
        {"start",                  "(J)V",                                                      (void *) Start},
        {"reaWaken",               "(J)V",                                                      (void *) ReaWaken},
        {"manualTriggerUpload",    "(JLcom/iget/datatranshub/callback/IUploadCallback;)V",      (void *) ManualTriggerUpload},
        {"write",                  "(JLcom/iget/flog/LogInfo;[B)V",                             (void *) Write},
        {"notifyUploadSuccess",    "(JLjava/lang/String;)V",                                    (void *) NotifyUploadSuccess},
        {"notifyUploadFailed",     "(JLjava/lang/String;)V",                                    (void *) NotifyUploadFailed},

};

int registerFlog(JNIEnv *env) {
    const char *className = "com/iget/flog/Flog";
    return registerNativeMethods(env, className,
                                 gJavaFlogMethods, NELEM(gJavaFlogMethods));
}
