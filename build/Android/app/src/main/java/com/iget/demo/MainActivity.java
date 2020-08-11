package com.iget.demo;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

import com.iget.datatranshub.DataTransHub;
import com.iget.datatranshub.callback.IUploadCallback;
import com.iget.datatranshub.callback.IUploadImp;
import com.iget.datatranshub.demo.R;
import com.iget.flog.Flog;
import com.iget.flog.Log;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    private DataTransHub dataTransHub1;
    private DataTransHub dataTransHub2;
    private Flog flog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findViewById(R.id.id_start1).setOnClickListener(this);
        findViewById(R.id.id_push1).setOnClickListener(this);
        findViewById(R.id.id_reweaken1).setOnClickListener(this);
        findViewById(R.id.id_manual_trigger_upload1).setOnClickListener(this);
        findViewById(R.id.id_release1).setOnClickListener(this);
        findViewById(R.id.id_test_flog).setOnClickListener(this);

        findViewById(R.id.id_start2).setOnClickListener(this);
        findViewById(R.id.id_push2).setOnClickListener(this);
        findViewById(R.id.id_reweaken2).setOnClickListener(this);
        findViewById(R.id.id_manual_trigger_upload2).setOnClickListener(this);
        findViewById(R.id.id_release2).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.id_start1:
                start1();
                break;
            case R.id.id_reweaken1:
                testReweaken1();
                break;
            case R.id.id_manual_trigger_upload1:
                testManualTriggerUpload1();
                break;
            case R.id.id_push1:
                testPush1();
                break;
            case R.id.id_release1:
                testRelease1();
                break;
            case R.id.id_test_flog:
                testFlog();
                break;
            case R.id.id_start2:
                start2();
                break;
            case R.id.id_reweaken2:
                testReweaken2();
                break;
            case R.id.id_manual_trigger_upload2:
                testManualTriggerUpload2();
                break;
            case R.id.id_push2:
                testPush2();
                break;
            case R.id.id_release2:
                testRelease2();
                break;

        }
    }
int tt = 0;
    public void start1() {
        String dataTransHubCacheDir = this.getFilesDir().getAbsolutePath() + "/data_trans_hub_cache1";
        String dataTransHubDataDir = this.getFilesDir().getAbsolutePath() + "/data_trans_hub_data1";
        dataTransHub1 = DataTransHub.makeDataTransHub(dataTransHubCacheDir, dataTransHubDataDir, "572d1e2710ae5fbca54c76a382fdd44050b3a675cb2bf39feebe85ef63d947aff0fa4943f1112e8b6af34bebebbaefa1a0aae055d9259b89a1858f7cc9af9df1");
        dataTransHub1.setUploadTriggerWayWay(DataTransHub.UploadTriggerWayWayAuto);
        dataTransHub1.setUploadImp(new IUploadImp() {
            @Override
            public void upload(final String filePath) {
                Log.d("test","dth1 file:" + filePath);
                Handler uiHandler = new Handler(Looper.getMainLooper());
                uiHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        if(tt++ < 5){
                            dataTransHub1.notifyUploadFailed(filePath);
                        }else{
                            dataTransHub1.notifyUploadSuccess(filePath);
                        }
                        //dataTransHub1.notifyUploadSuccess(filePath);
                        //dataTransHub1.notifyUploadFailed(filePath);
                    }
                });
            }
        });
        //1 day
        dataTransHub1.setExpiredTime(60 * 60 * 24);
        //10 min
        dataTransHub1.setReportingInterval(10 * 1000 *60);
        dataTransHub1.setReportingInterval(10 * 1000);
        dataTransHub1.setRetryInterval(2 * 1000);
        dataTransHub1.setFileMaxSize(1024 * 200);
        dataTransHub1.start();
    }

    private void testPush1() {
        for (int i = 0; i < 100; i++) {
            dataTransHub1.push("ev=s_paid_paid_impression&uid=12005419&scr=1080*2214&t=1547627349367082203&seid=dd86a82b76722c24427b9db1fb462a4d&net=wifi&mac=c6abbef9f4bea0a0&sid=dd86a82b76722c24427b9db1fb462a4d".getBytes());
        }
    }

    private void testReweaken1() {
        dataTransHub1.reaWaken();
    }

    private void testManualTriggerUpload1() {
        dataTransHub1.manualTriggerUpload(new IUploadCallback() {
            @Override
            public void finish() {
                Log.d("", "");
            }
        });
    }

    private void testRelease1() {
        if(dataTransHub1 != null){
            dataTransHub1.release();
        }
    }

    public void start2() {
        String dataTransHubCacheDir = this.getFilesDir().getAbsolutePath() + "/data_trans_hub_cache2";
        String dataTransHubDataDir = this.getFilesDir().getAbsolutePath() + "/data_trans_hub_data2";
        dataTransHub2 = DataTransHub.makeDataTransHub(dataTransHubCacheDir, dataTransHubDataDir, "572d1e2710ae5fbca54c76a382fdd44050b3a675cb2bf39feebe85ef63d947aff0fa4943f1112e8b6af34bebebbaefa1a0aae055d9259b89a1858f7cc9af9df1");
        dataTransHub2.setUploadTriggerWayWay(DataTransHub.UploadTriggerWayWayAuto);
        dataTransHub2.setUploadImp(new IUploadImp() {
            @Override
            public void upload(final String filePath) {
                Log.d("test","dth2 file:" + filePath);
                Handler uiHandler = new Handler(Looper.getMainLooper());
                uiHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        dataTransHub2.notifyUploadSuccess(filePath);
                        //dataTransHub2.notifyUploadFailed(filePath);
                    }
                });
            }
        });
        //1 day
        dataTransHub2.setExpiredTime(60 * 60 * 24);
        //10 min
        dataTransHub2.setReportingInterval(1000 *60);
        dataTransHub2.setFileMaxSize(1024 * 200);
        dataTransHub2.start();
    }

    private void testPush2() {
        for (int i = 0; i < 10000; i++) {
            dataTransHub2.push("ev=s_paid_paid_impression&uid=12005419&scr=1080*2214&t=1547627349367082203&seid=dd86a82b76722c24427b9db1fb462a4d&net=wifi&mac=c6abbef9f4bea0a0&sid=dd86a82b76722c24427b9db1fb462a4d".getBytes());
        }
    }

    private void testReweaken2() {
        dataTransHub2.reaWaken();
    }

    private void testManualTriggerUpload2() {
        dataTransHub2.manualTriggerUpload(new IUploadCallback() {
            @Override
            public void finish() {
                Log.d("", "");
            }
        });
    }

    private void testRelease2() {
        if(dataTransHub2 != null){
            dataTransHub2.release();
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
