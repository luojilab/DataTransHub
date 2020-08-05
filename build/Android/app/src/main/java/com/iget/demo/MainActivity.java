package com.iget.demo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.View;

import com.iget.datatranshub.DataTransHub;
import com.iget.datatranshub.callback.IUploadCallback;
import com.iget.datatranshub.demo.R;
import com.iget.datatranshub.callback.IUploadImp;
import com.iget.flog.Flog;
import com.iget.flog.Log;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    private DataTransHub dataTransHub;
    private Flog flog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findViewById(R.id.id_start).setOnClickListener(this);
        findViewById(R.id.id_push).setOnClickListener(this);
        findViewById(R.id.id_reweaken).setOnClickListener(this);
        findViewById(R.id.id_manual_trigger_upload).setOnClickListener(this);
        findViewById(R.id.id_release).setOnClickListener(this);
        findViewById(R.id.id_test_flog).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.id_start:
                start();
                break;
            case R.id.id_reweaken:
                testReweaken();
                break;
            case R.id.id_manual_trigger_upload:
                testManualTriggerUpload();
                break;
            case R.id.id_push:
                testPush();
                break;
            case R.id.id_release:
                testRelease();
                break;
            case R.id.id_test_flog:
                testFlog();
                break;

        }
    }

    public void start() {
        String dataTransHubCacheDir = this.getFilesDir().getAbsolutePath() + "/data_trans_hub_cache";
        String dataTransHubDataDir = this.getFilesDir().getAbsolutePath() + "/data_trans_hub_data";
        dataTransHub = DataTransHub.makeDataTransHub(dataTransHubCacheDir, dataTransHubDataDir, "572d1e2710ae5fbca54c76a382fdd44050b3a675cb2bf39feebe85ef63d947aff0fa4943f1112e8b6af34bebebbaefa1a0aae055d9259b89a1858f7cc9af9df1");
        dataTransHub.setUploadTriggerWayWay(DataTransHub.UploadTriggerWayWayAuto);
        dataTransHub.setUploadImp(new IUploadImp() {
            @Override
            public void upload(final String filePath) {
                Log.d("test","file:" + filePath);
                Handler uiHandler = new Handler(Looper.getMainLooper());
                uiHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        dataTransHub.notifyUploadSuccess(filePath);
                    }
                });
            }
        });
        //1 day
        dataTransHub.setExpiredTime(60 * 60 * 24);
        //10 min
        dataTransHub.setReportingInterval(1000 *60);
        dataTransHub.setFileMaxSize(1024 * 200);
        dataTransHub.start();
    }

    private void testPush() {
        for (int i = 0; i < 10000; i++) {
            dataTransHub.push("ev=s_paid_paid_impression&uid=12005419&scr=1080*2214&t=1547627349367082203&seid=dd86a82b76722c24427b9db1fb462a4d&net=wifi&mac=c6abbef9f4bea0a0&sid=dd86a82b76722c24427b9db1fb462a4d".getBytes());
        }
    }

    private void testReweaken() {
        dataTransHub.reaWaken();
    }

    private void testManualTriggerUpload() {
        dataTransHub.manualTriggerUpload(new IUploadCallback() {
            @Override
            public void finish() {
                Log.d("", "");
            }
        });
    }

    private void testRelease() {
        if(dataTransHub != null){
            dataTransHub.release();
        }
    }

    private void testFlog() {
        String flogCacheDir = this.getFilesDir().getAbsolutePath() + "/flog_cache";
        String flogDataDir = this.getFilesDir().getAbsolutePath() + "/flog_data";
        flog = Flog.makeFlog(flogCacheDir, flogDataDir, "572d1e2710ae5fbca54c76a382fdd44050b3a675cb2bf39feebe85ef63d947aff0fa4943f1112e8b6af34bebebbaefa1a0aae055d9259b89a1858f7cc9af9df1");
        flog.setUploadImp(new IUploadImp() {
            @Override
            public void upload(final String filePath) {
                Handler uiHandler = new Handler(Looper.getMainLooper());
                uiHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        flog.notifyUploadSuccess(filePath);
                    }
                });
            }
        });
        flog.setExpiredTime(3600);
        flog.start();
        Log.setLogImp(flog);
        for (int i = 0; i < 1000000; i++) {
            Log.d("test", "ev=s_paid_paid_impression&uid=12005419&scr=1080*2214&t=1547627349367082203&seid=dd86a82b76722c24427b9db1fb462a4d&net=wifi&mac=c6abbef9f4bea0a0&sid=dd86a82b76722c24427b9db1fb462a4d");
        }
    }
}
