package com.iget.flog;

import android.util.Log;

import com.iget.datatranshub.callback.ISoload;
import com.iget.datatranshub.callback.IUploadCallback;
import com.iget.datatranshub.callback.IUploadImp;

public class Flog implements com.iget.flog.Log.LogImp {
    private long mNativeFlog = 0;
    private static ISoload mSoloadImp;

    private static boolean sIsLoad = false;
    private boolean mIsRelease = false;

    private static void loadLibary() {
        synchronized (Flog.class) {
            if (!sIsLoad) {
                boolean isLoadSuccess = false;
                if (mSoloadImp != null) {
                    try {
                        mSoloadImp.loadLibrary("c++_shared");
                        mSoloadImp.loadLibrary("data-trans-hub");
                        isLoadSuccess = true;
                    } catch (Throwable e) {
                        Log.d("soload:", e.getMessage());
                    }
                }

                if (!isLoadSuccess) {
                    try {
                        System.loadLibrary("c++_shared");
                        System.loadLibrary("data-trans-hub");
                        Log.d("soload:", "data-trans-hub load success!");
                    } catch (Throwable e) {
                        Log.d("soload:", e.getMessage());
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

    public synchronized static Flog makeFlog(String cacheDir, String dataDir, String encryptKey) {
        return new Flog(cacheDir, dataDir, encryptKey);
    }

    public synchronized static void releaseDataTransHub(Flog flog) {
        flog.release();
    }

    private Flog(String cacheDir, String dataDir, String encryptKey) {
        loadLibary();
        mNativeFlog = makeNativeFlog(cacheDir, dataDir, encryptKey);
    }

    public synchronized void release() {
        mIsRelease = true;
        releaseNativeFlog(mNativeFlog);
    }

    public synchronized void setUploadImp(IUploadImp uploadImp) {
        if(mIsRelease){
            return;
        }
        setUploadImp(mNativeFlog, uploadImp);
    }

    public synchronized void setFileMaxSize(long fileMaxSize) {
        if(mIsRelease){
            return;
        }
        setFileMaxSize(mNativeFlog, fileMaxSize);
    }

    public synchronized void setBufferSize(long bufferSize) {
        if(mIsRelease){
            return;
        }
        setBufferSize(mNativeFlog, bufferSize);
    }

    public synchronized void setExpiredTime(long expiredTime) {
        if(mIsRelease){
            return;
        }
        setExpiredTime(mNativeFlog, expiredTime);
    }

    public synchronized void setReportingInterval(long reportingInterval) {
        if(mIsRelease){
            return;
        }
        setReportingInterval(mNativeFlog, reportingInterval);
    }

    public synchronized void setRetryInterval(long retryInterval) {
        if(mIsRelease){
            return;
        }
        setRetryInterval(mNativeFlog, retryInterval);
    }

    public synchronized void setDataFilePrefix(String prefix) {
        if(mIsRelease){
            return;
        }
        setDataFilePrefix(mNativeFlog, prefix);
    }

    public synchronized void setUploadTriggerWayWay(int uploadTriggerWay) {
        if(mIsRelease){
            return;
        }
        setUploadTriggerWayWay(mNativeFlog, uploadTriggerWay);
    }

    public synchronized void setWriteDiskPeriod(long period) {
        if(mIsRelease){
            return;
        }
        setWriteDiskPeriod(mNativeFlog, period);
    }

    public synchronized void start() {
        if(mIsRelease){
            return;
        }
        start(mNativeFlog);
    }

    public synchronized void reaWaken() {
        if(mIsRelease){
            return;
        }
        reaWaken(mNativeFlog);
    }

    public synchronized void manualTriggerUpload(IUploadCallback callback) {
        if(mIsRelease){
            return;
        }
        manualTriggerUpload(mNativeFlog, callback);
    }

    public synchronized void write(LogInfo logInfo, byte[] data) {
        if(mIsRelease){
            return;
        }
        write(mNativeFlog, logInfo, data);
    }

    public synchronized void notifyUploadSuccess(String filePath) {
        if(mIsRelease){
            return;
        }
        notifyUploadSuccess(mNativeFlog, filePath);
    }

    public synchronized void notifyUploadFailed(String filePath) {
        if(mIsRelease){
            return;
        }
        notifyUploadFailed(mNativeFlog, filePath);
    }

    private static native int getVersion();

    private static native long makeNativeFlog(String cacheDir, String dataDir, String encryptKey);

    private static native void releaseNativeFlog(long nativeFlog);

    private static native void setUploadImp(long nativeFlog, IUploadImp uploadImp);

    private static native void setFileMaxSize(long nativeDataTransHub, long fileMaxSize);

    private static native void setBufferSize(long nativeDataTransHub, long bufferSize);

    private static native void setExpiredTime(long nativeDataTransHub, long expiredTime);

    private static native void setReportingInterval(long nativeFlog, long reportingInterval);

    private static native void setRetryInterval(long nativeFlog, long retryInterval);

    private static native void setDataFilePrefix(long nativeFlog, String prefix);

    private static native void setUploadTriggerWayWay(long nativeFlog, int uploadTriggerWay);

    private static native void setWriteDiskPeriod(long nativeFlog, long period);

    private static native void start(long nativeFlog);

    private static native void reaWaken(long nativeFlog);

    private static native void manualTriggerUpload(long nativeDataTransHub, IUploadCallback finish);

    private static native void write(long nativeFlog, LogInfo logInfo, byte[] data);

    private static native void notifyUploadSuccess(long nativeFlog, String filePath);

    private static native void notifyUploadFailed(long nativeFlog, String filePath);


    @Override
    public void logV(String tag, String filename, String funcname, int line, int pid, long tid, long maintid, String log) {
        LogInfo logInfo = new LogInfo();
        logInfo.mLevel = LogInfo.LEVEL_VERBOSE;
        logInfo.mTag = tag;
        logInfo.mFilename = filename;
        logInfo.mFuncname = funcname;
        logInfo.mLine = line;
        logInfo.mPid = pid;
        logInfo.mTid = tid;
        logInfo.mMaintid = maintid;
        write(logInfo, log.getBytes());
    }

    @Override
    public void logI(String tag, String filename, String funcname, int line, int pid, long tid, long maintid, String log) {
        LogInfo logInfo = new LogInfo();
        logInfo.mLevel = LogInfo.LEVEL_INFO;
        logInfo.mTag = tag;
        logInfo.mFilename = filename;
        logInfo.mFuncname = funcname;
        logInfo.mLine = line;
        logInfo.mPid = pid;
        logInfo.mTid = tid;
        logInfo.mMaintid = maintid;
        write(logInfo, log.getBytes());
    }

    @Override
    public void logD(String tag, String filename, String funcname, int line, int pid, long tid, long maintid, String log) {
        LogInfo logInfo = new LogInfo();
        logInfo.mLevel = LogInfo.LEVEL_DEBUG;
        logInfo.mTag = tag;
        logInfo.mFilename = filename;
        logInfo.mFuncname = funcname;
        logInfo.mLine = line;
        logInfo.mPid = pid;
        logInfo.mTid = tid;
        logInfo.mMaintid = maintid;
        write(logInfo, log.getBytes());
    }

    @Override
    public void logW(String tag, String filename, String funcname, int line, int pid, long tid, long maintid, String log) {
        LogInfo logInfo = new LogInfo();
        logInfo.mLevel = LogInfo.LEVEL_WARNING;
        logInfo.mTag = tag;
        logInfo.mFilename = filename;
        logInfo.mFuncname = funcname;
        logInfo.mLine = line;
        logInfo.mPid = pid;
        logInfo.mTid = tid;
        logInfo.mMaintid = maintid;
        write(logInfo, log.getBytes());
    }

    @Override
    public void logE(String tag, String filename, String funcname, int line, int pid, long tid, long maintid, String log) {
        LogInfo logInfo = new LogInfo();
        logInfo.mLevel = LogInfo.LEVEL_ERROR;
        logInfo.mTag = tag;
        logInfo.mFilename = filename;
        logInfo.mFuncname = funcname;
        logInfo.mLine = line;
        logInfo.mPid = pid;
        logInfo.mTid = tid;
        logInfo.mMaintid = maintid;
        write(logInfo, log.getBytes());
    }

    @Override
    public void logF(String tag, String filename, String funcname, int line, int pid, long tid, long maintid, String log) {
        LogInfo logInfo = new LogInfo();
        logInfo.mLevel = LogInfo.LEVEL_FATAL;
        logInfo.mTag = tag;
        logInfo.mFilename = filename;
        logInfo.mFuncname = funcname;
        logInfo.mLine = line;
        logInfo.mPid = pid;
        logInfo.mTid = tid;
        logInfo.mMaintid = maintid;
        write(logInfo, log.getBytes());
    }
}
