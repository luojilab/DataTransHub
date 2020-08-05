package com.iget.flogdemo;

import android.content.Context;

import com.iget.datatranshub.DataTransHub;
import com.iget.flog.Flog;
import com.iget.flog.Log;

public class FlogInstance {
    private static volatile FlogInstance instance = null;
    private Flog mFlog;

    public static FlogInstance getInstance() {
        if (instance == null) {
            synchronized (FlogInstance.class) {
                if (instance == null) {
                    instance = new FlogInstance();
                }
            }
        }
        return instance;
    }

    public void init(Context context) {
        String baseDir = context.getFilesDir().getAbsolutePath();
        mFlog = Flog.makeFlog(baseDir + "/dth/cache", baseDir + "/dth/data", "572d1e2710ae5fbca54c76a382fdd44050b3a675cb2bf39feebe85ef63d947aff0fa4943f1112e8b6af34bebebbaefa1a0aae055d9259b89a1858f7cc9af9df1");
        //10s
        mFlog.setReportingInterval(1000 * 10);
        //10s
        mFlog.setRetryInterval(1000 * 10);
        //150k
        mFlog.setBufferSize(1024 * 150);
        mFlog.setDataFilePrefix("log");
        //15day
        mFlog.setExpiredTime(3600 * 24 * 15);
        //300k
        mFlog.setFileMaxSize(1024 * 300);
        //auto report
        mFlog.setUploadTriggerWayWay(DataTransHub.UploadTriggerWayWayAuto);
        //10 min
        mFlog.setWriteDiskPeriod(1000 * 60 * 10);
        mFlog.setUploadImp(new UploadImp());
        Log.setLogImp(mFlog);
        mFlog.start();
    }

    public void reWeaken() {
        mFlog.reaWaken();
    }

    public void notifySucces(final String path) {
        mFlog.notifyUploadSuccess(path);
    }

    public void notifyFailed(final String path) {
        mFlog.notifyUploadFailed(path);
    }

    private FlogInstance() {

    }

    public void release() {
        mFlog.release();
    }
}
