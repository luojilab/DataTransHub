package com.iget.datatranshub;

import android.util.Log;

import com.iget.datatranshub.callback.ISoload;
import com.iget.datatranshub.callback.IUploadCallback;
import com.iget.datatranshub.callback.IUploadImp;

public class DataTransHub {
    public static int UploadTriggerWayWayAuto = 0;
    public static int UploadTriggerWayWayManual = 1;

    private long mNativeDataTransHub = 0;
    private static ISoload mSoloadImp;
    private boolean mIsRelease = false;

    private static boolean sIsLoad = false;

    private static void loadLibary() {
        synchronized (DataTransHub.class) {
            if (!sIsLoad) {
                boolean isLoadSuccess = false;
                if (mSoloadImp != null) {
                    try {
                        mSoloadImp.loadLibrary("c++_shared");
                        mSoloadImp.loadLibrary("data-trans-hub");
                        Log.d("DataTranshub", "soload:data-trans-hub load success!");
                        isLoadSuccess = true;
                    } catch (Throwable e) {
                        Log.d("DataTranshub", "soload" + e.getMessage());
                    }
                }

                if (!isLoadSuccess) {
                    try {
                        System.loadLibrary("c++_shared");
                        System.loadLibrary("data-trans-hub");
                        Log.d("DataTranshub", "soload:data-trans-hub load success!");
                        isLoadSuccess = true;
                    } catch (Throwable e) {
                        Log.d("DataTranshub", "soload:" + e.getMessage());
                    }
                }

                if (isLoadSuccess) {
                    sIsLoad = true;
                }
            }
        }
    }

    public static void setSoLoadImp(ISoload soLoadImp) {
        mSoloadImp = soLoadImp;
    }

    public static int getVersionCode() {
        loadLibary();
        return getVersion();
    }

    public synchronized static DataTransHub makeDataTransHub(String cacheDir, String dataDir, String encryptKey) {
        return new DataTransHub(cacheDir, dataDir, encryptKey);
    }

    public synchronized static void releaseDataTransHub(DataTransHub dataTransHub) {
        dataTransHub.release();
    }

    private DataTransHub(String cacheDir, String dataDir, String encryptKey) {
        loadLibary();
        mNativeDataTransHub = makeNativeDataTransHub(cacheDir, dataDir, encryptKey);
    }

    private DataTransHub() {

    }

    public synchronized void release() {
        mIsRelease = true;
        releaseNativeDataTransHub(mNativeDataTransHub);
    }

    public synchronized void setUploadImp(IUploadImp uploadImp) {
        if (mIsRelease) {
            return;
        }
        setUploadImp(mNativeDataTransHub, uploadImp);
    }

    public synchronized void setFileMaxSize(long fileMaxSize) {
        if (mIsRelease) {
            return;
        }
        setFileMaxSize(mNativeDataTransHub, fileMaxSize);
    }

    public synchronized void setBufferSize(long bufferSize) {
        if (mIsRelease) {
            return;
        }
        setBufferSize(mNativeDataTransHub, bufferSize);
    }

    public synchronized void setExpiredTime(long expiredTime) {
        if (mIsRelease) {
            return;
        }
        setExpiredTime(mNativeDataTransHub, expiredTime);
    }

    public synchronized void setReportingInterval(long reportingInterval) {
        if (mIsRelease) {
            return;
        }
        setReportingInterval(mNativeDataTransHub, reportingInterval);
    }

    public synchronized void setRetryInterval(long retryInterval) {
        if (mIsRelease) {
            return;
        }
        setRetryInterval(mNativeDataTransHub, retryInterval);
    }

    public synchronized void setDataFilePrefix(String prefix) {
        if (mIsRelease) {
            return;
        }
        setDataFilePrefix(mNativeDataTransHub, prefix);
    }

    public synchronized void setUploadTriggerWayWay(int uploadTriggerWay) {
        if (mIsRelease) {
            return;
        }
        setUploadTriggerWayWay(mNativeDataTransHub, uploadTriggerWay);
    }

    public synchronized void setWriteDiskPeriod(long period) {
        if (mIsRelease) {
            return;
        }
        setWriteDiskPeriod(mNativeDataTransHub, period);
    }

    public synchronized void start() {
        if (mIsRelease) {
            return;
        }
        start(mNativeDataTransHub);
    }

    public synchronized void reaWaken() {
        if (mIsRelease) {
            return;
        }
        reaWaken(mNativeDataTransHub);
    }

    public synchronized void manualTriggerUpload(IUploadCallback callback) {
        if (mIsRelease) {
            return;
        }
        manualTriggerUpload(mNativeDataTransHub, callback);
    }

    public synchronized void push(byte[] data) {
        if (mIsRelease) {
            return;
        }
        push(mNativeDataTransHub, data);
    }

    public synchronized void notifyUploadSuccess(String filePath) {
        if (mIsRelease) {
            return;
        }
        notifyUploadSuccess(mNativeDataTransHub, filePath);
    }

    public synchronized void notifyUploadFailed(String filePath) {
        if (mIsRelease) {
            return;
        }
        notifyUploadFailed(mNativeDataTransHub, filePath);
    }

    private static native int getVersion();

    private static native long makeNativeDataTransHub(String cacheDir, String dataDir, String encryptKey);

    private static native void releaseNativeDataTransHub(long nativeDataTransHub);

    private static native void setUploadImp(long nativeDataTransHub, IUploadImp uploadImp);

    private static native void setFileMaxSize(long nativeDataTransHub, long fileMaxSize);

    private static native void setBufferSize(long nativeDataTransHub, long bufferSize);

    private static native void setExpiredTime(long nativeDataTransHub, long expiredTime);

    private static native void setReportingInterval(long nativeDataTransHub, long reportingInterval);

    private static native void setRetryInterval(long nativeDataTransHub, long retryInterval);

    private static native void setDataFilePrefix(long nativeDataTransHub, String prefix);

    private static native void setUploadTriggerWayWay(long nativeDataTransHub, int uploadTriggerWay);

    private static native void setWriteDiskPeriod(long nativeDataTransHub, long period);

    private static native void start(long nativeDataTransHub);

    private static native void reaWaken(long nativeDataTransHub);

    private static native void manualTriggerUpload(long nativeDataTransHub, IUploadCallback finish);

    private static native void push(long nativeDataTransHub, byte[] data);

    private static native void notifyUploadSuccess(long nativeDataTransHub, String filePath);

    private static native void notifyUploadFailed(long nativeDataTransHub, String filePath);


}
