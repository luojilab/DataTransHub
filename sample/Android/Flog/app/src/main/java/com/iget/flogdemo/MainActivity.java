package com.iget.flogdemo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;

import com.iget.flog.Log;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findViewById(R.id.id_log).setOnClickListener(this);
        findViewById(R.id.id_reweaken).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.id_log:
                testLog();
                break;
            case R.id.id_reweaken:
                reWeaken();
                break;
        }
    }

    private void testLog() {
        Log.d("test", "testLog");
        StackTraceElement[] stackTrace = Thread.currentThread().getStackTrace();
        Log.d("test", stackTrace[2].getFileName(), stackTrace[2].getMethodName(), stackTrace[2].getLineNumber(), "testLog");
    }

    private void reWeaken() {
        //when app form background switch to frontground neet to call this to ensure the log can upload timely
        FlogInstance.getInstance().reWeaken();
    }
}