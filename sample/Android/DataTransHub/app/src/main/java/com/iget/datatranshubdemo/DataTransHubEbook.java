package com.iget.datatranshubdemo;

import android.content.Context;
import com.iget.datatranshub.DataTransHub;

public class DataTransHubEbook {
    private static volatile DataTransHubEbook instance = null;
    private DataTransHub mDataTransHub;

    public static DataTransHubEbook getInstance() {
        if (instance == null) {
            synchronized (DataTransHubEbook.class) {
                if (instance == null) {
                    instance = new DataTransHubEbook();
                }
            }
        }
        return instance;
    }

    public void init(Context context) {
        String baseDir = context.getFilesDir().getAbsolutePath();
        mDataTransHub = DataTransHub.makeDataTransHub(baseDir + "/dth/cache", baseDir + "/dth/data", "572d1e2710ae5fbca54c76a382fdd44050b3a675cb2bf39feebe85ef63d947aff0fa4943f1112e8b6af34bebebbaefa1a0aae055d9259b89a1858f7cc9af9df1");
        //10s
        mDataTransHub.setReportingInterval(1000 * 10);
        //10s
        mDataTransHub.setRetryInterval(1000 * 10);
        //150k
        mDataTransHub.setBufferSize(1024 * 150);
        mDataTransHub.setDataFilePrefix("dm");
        //15day
        mDataTransHub.setExpiredTime(3600 * 24 * 15);
        //300k
        mDataTransHub.setFileMaxSize(1024 * 300);
        //auto report
        mDataTransHub.setUploadTriggerWayWay(DataTransHub.UploadTriggerWayWayAuto);
        //10 min
        mDataTransHub.setWriteDiskPeriod(1000 * 60 * 10);
        mDataTransHub.setUploadImp(new UploadImp());
        mDataTransHub.start();
    }

    public void push(byte[] data) {
        mDataTransHub.push(data);
    }

    public void reWeaken() {
        mDataTransHub.reaWaken();
    }

    public void notifySucces(final String path) {
        mDataTransHub.notifyUploadSuccess(path);
    }

    public void notifyFailed(final String path) {
        mDataTransHub.notifyUploadFailed(path);
    }

    private DataTransHubEbook() {

    }

    public void release() {
        mDataTransHub.release();
    }
}
