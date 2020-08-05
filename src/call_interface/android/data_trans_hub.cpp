#include <jni.h>
#include <string>
#include "jni_helper.h"
#include "android_util.h"
#include "data_trans_hub.h"

static int GetVersion(JNIEnv *env, jobject obj) {
#ifdef VERSION_CODE
    return VERSION_CODE;
#else
    return 0;
#endif
}

static jlong
MakeNativeDataTransHub(JNIEnv *env, jobject obj, jstring cacheDir, jstring dataDir,
                       jstring encryptKey) {
    std::string cacheDirCStr = AndroidUtil::fromJavaString(env, cacheDir);
    std::string dataDirCstr = AndroidUtil::fromJavaString(env, dataDir);
    std::string encryptKeyCstr = AndroidUtil::fromJavaString(env, encryptKey);

    future::DataTransHub *dataTransHub = future::DataTransHub::MakeDataTransHub(cacheDirCStr,
                                                                                dataDirCstr,
                                                                                encryptKeyCstr);
    return (jlong) dataTransHub;
}

static void ReleaseNativeDataTransHub(JNIEnv *env, jobject obj, jlong nativeDataTransHub) {
    future::DataTransHub *dataTransHub = reinterpret_cast<future::DataTransHub *>(nativeDataTransHub);
    if (dataTransHub == NULL) {
        return;
    }
    future::DataTransHub::Destroy(dataTransHub);
}

static void SetUploadImp(JNIEnv *env, jobject obj, jlong nativeDataTransHub, jobject uploadImp) {
    future::DataTransHub *dataTransHub = reinterpret_cast<future::DataTransHub *>(nativeDataTransHub);
    if (dataTransHub == NULL) {
        return;
    }

    std::shared_ptr<JObject> jCallback(new JObject(uploadImp));
    dataTransHub->SetUploadImp([jCallback](
            const std::string &filePath) {
        jstring filePathJstr = AndroidUtil::createJavaString(
                AndroidUtil::getEnv(),
                filePath);
        AndroidUtil::Method_DataTransHub_uploadImp->call(
                jCallback->GetObj(),
                filePathJstr);
    });
}

static void SetFileMaxSize(JNIEnv *env, jobject obj, jlong nativeDataTransHub, jlong fileMaxSize) {
    future::DataTransHub *dataTransHub = reinterpret_cast<future::DataTransHub *>(nativeDataTransHub);
    if (dataTransHub == NULL) {
        return;
    }
    dataTransHub->SetFileMaxSize(fileMaxSize);
}

static void SetBufferSize(JNIEnv *env, jobject obj, jlong nativeDataTransHub, jlong buferSize) {
    future::DataTransHub *dataTransHub = reinterpret_cast<future::DataTransHub *>(nativeDataTransHub);
    if (dataTransHub == NULL) {
        return;
    }
    dataTransHub->SetBufferSize(buferSize);
}

static void SetExpiredTime(JNIEnv *env, jobject obj, jlong nativeDataTransHub, jlong expiredTime) {
    future::DataTransHub *dataTransHub = reinterpret_cast<future::DataTransHub *>(nativeDataTransHub);
    if (dataTransHub == NULL) {
        return;
    }
    dataTransHub->SetExpiredTime(expiredTime);
}

static void
SetReportingInterval(JNIEnv *env, jobject obj, jlong nativeDataTransHub, jlong reportingInterval) {
    future::DataTransHub *dataTransHub = reinterpret_cast<future::DataTransHub *>(nativeDataTransHub);
    if (dataTransHub == NULL) {
        return;
    }
    dataTransHub->SetReportingInterval(reportingInterval);
}

static void
SetRetryInterval(JNIEnv *env, jobject obj, jlong nativeDataTransHub, jlong retryInterval) {
    future::DataTransHub *dataTransHub = reinterpret_cast<future::DataTransHub *>(nativeDataTransHub);
    if (dataTransHub == NULL) {
        return;
    }
    dataTransHub->SetRetryInterval(retryInterval);
}

static void
SetUploadTriggerWayWay(JNIEnv *env, jobject obj, jlong nativeDataTransHub,
                       jint uploadTriggerWayWay) {
    future::DataTransHub *dataTransHub = reinterpret_cast<future::DataTransHub *>(nativeDataTransHub);
    if (dataTransHub == NULL) {
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
    dataTransHub->SetUploadTriggerWayWay(uploadTriggerWay);
}

static void
SetWriteDiskPeriod(JNIEnv *env, jobject obj, jlong nativeDataTransHub, jlong period) {
    future::DataTransHub *dataTransHub = reinterpret_cast<future::DataTransHub *>(nativeDataTransHub);
    if (dataTransHub == NULL) {
        return;
    }

    dataTransHub->SetWriteDiskPeriod(period);
}

static void
SetDataFilePrefix(JNIEnv *env, jobject obj, jlong nativeDataTransHub, jstring filePrefix) {
    future::DataTransHub *dataTransHub = reinterpret_cast<future::DataTransHub *>(nativeDataTransHub);
    if (dataTransHub == NULL) {
        return;
    }
    std::string filePrefixCstr = AndroidUtil::fromJavaString(env, filePrefix);
    dataTransHub->SetDataFilePrefix(filePrefixCstr);
}

static void Start(JNIEnv *env, jobject obj, jlong nativeDataTransHub) {
    future::DataTransHub *dataTransHub = reinterpret_cast<future::DataTransHub *>(nativeDataTransHub);
    if (dataTransHub == NULL) {
        return;
    }
    dataTransHub->Start();
}

static void ReaWaken(JNIEnv *env, jobject obj, jlong nativeDataTransHub) {
    future::DataTransHub *dataTransHub = reinterpret_cast<future::DataTransHub *>(nativeDataTransHub);
    if (dataTransHub == NULL) {
        return;
    }
    dataTransHub->ReaWaken();
}

static void
ManualTriggerUpload(JNIEnv *env, jobject obj, jlong nativeDataTransHub, jobject callback) {
    future::DataTransHub *dataTransHub = reinterpret_cast<future::DataTransHub *>(nativeDataTransHub);
    if (dataTransHub == NULL) {
        return;
    }

    std::shared_ptr<JObject> jCallback(new JObject(callback));
    dataTransHub->ManualTriggerUpload([jCallback]() {
        if (AndroidUtil::getEnv()->IsSameObject(jCallback->GetObj(), nullptr)) {
            return;
        }
        AndroidUtil::Method_DataTransHub_uploadCallback->call(jCallback->GetObj());
    });
}

static void Push(JNIEnv *env, jobject obj, jlong nativeDataTransHub, jbyteArray data) {
    future::DataTransHub *dataTransHub = reinterpret_cast<future::DataTransHub *>(nativeDataTransHub);
    if (dataTransHub == NULL) {
        return;
    }

    std::size_t dataLen = env->GetArrayLength(data);
    std::vector<unsigned char> cData(dataLen);

    jbyte *dataJava = env->GetByteArrayElements(data, NULL);
    for (int i = 0; i < dataLen; i++) {
        cData[i] = dataJava[i];
    }
    env->ReleaseByteArrayElements(data, dataJava, JNI_ABORT);
    dataTransHub->Push(cData);
}

static void
NotifyUploadSuccess(JNIEnv *env, jobject obj, jlong nativeDataTransHub, jstring filePath) {
    future::DataTransHub *dataTransHub = reinterpret_cast<future::DataTransHub *>(nativeDataTransHub);
    if (dataTransHub == NULL) {
        return;
    }
    std::string filePathCstr = AndroidUtil::fromJavaString(env, filePath);
    dataTransHub->NotifyUploadSuccess(filePathCstr);
}

static void
NotifyUploadFailed(JNIEnv *env, jobject obj, jlong nativeDataTransHub, jstring filePath) {
    future::DataTransHub *dataTransHub = reinterpret_cast<future::DataTransHub *>(nativeDataTransHub);
    if (dataTransHub == NULL) {
        return;
    }
    std::string filePathCstr = AndroidUtil::fromJavaString(env, filePath);
    dataTransHub->NotifyUploadFailed(filePathCstr);
}

static JNINativeMethod gJavaDataTransHubMethods[] = {
        {"getVersion",                "()I",                                                       (void *) GetVersion},
        {"makeNativeDataTransHub",    "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J", (void *) MakeNativeDataTransHub},
        {"releaseNativeDataTransHub", "(J)V",                                                      (void *) ReleaseNativeDataTransHub},
        {"setUploadImp",              "(JLcom/iget/datatranshub/callback/IUploadImp;)V",           (void *) SetUploadImp},
        {"setFileMaxSize",            "(JJ)V",                                                     (void *) SetFileMaxSize},
        {"setBufferSize",             "(JJ)V",                                                     (void *) SetBufferSize},
        {"setExpiredTime",            "(JJ)V",                                                     (void *) SetExpiredTime},
        {"setReportingInterval",      "(JJ)V",                                                     (void *) SetReportingInterval},
        {"setRetryInterval",          "(JJ)V",                                                     (void *) SetRetryInterval},
        {"setDataFilePrefix",         "(JLjava/lang/String;)V",                                    (void *) SetDataFilePrefix},
        {"setUploadTriggerWayWay",    "(JI)V",                                                     (void *) SetUploadTriggerWayWay},
        {"setWriteDiskPeriod",        "(JJ)V",                                                     (void *) SetWriteDiskPeriod},
        {"start",                     "(J)V",                                                      (void *) Start},
        {"reaWaken",                  "(J)V",                                                      (void *) ReaWaken},
        {"manualTriggerUpload",       "(JLcom/iget/datatranshub/callback/IUploadCallback;)V",      (void *) ManualTriggerUpload},
        {"push",                      "(J[B)V",                                                    (void *) Push},
        {"notifyUploadSuccess",       "(JLjava/lang/String;)V",                                    (void *) NotifyUploadSuccess},
        {"notifyUploadFailed",        "(JLjava/lang/String;)V",                                    (void *) NotifyUploadFailed},

};

int registerDataTransHub(JNIEnv *env) {
    const char *className = "com/iget/datatranshub/DataTransHub";
    return registerNativeMethods(env, className,
                                 gJavaDataTransHubMethods, NELEM(gJavaDataTransHubMethods));
}
