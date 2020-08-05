package com.iget.datatranshubdemo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findViewById(R.id.id_push).setOnClickListener(this);
        findViewById(R.id.id_reweaken).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.id_push:
                push();
                break;
            case R.id.id_reweaken:
                reWeaken();
                break;
        }
    }

    private void push() {
        String data = "ev=s_paid_paid_impression&uid=12005419&scr=1080*2214&t=1547627349367082203&seid=dd86a82b76722c24427b9db1fb462a4d&net=wifi&mac=c6abbef9f4bea0a0&sid=dd86a82b76722c24427b9db1fb462a4d";
        DataTransHubEbook.getInstance().push(data.getBytes());
    }

    private void reWeaken() {
        //when app form background switch to frontground neet to call this to ensure the data can upload timely
        DataTransHubEbook.getInstance().reWeaken();
    }
}