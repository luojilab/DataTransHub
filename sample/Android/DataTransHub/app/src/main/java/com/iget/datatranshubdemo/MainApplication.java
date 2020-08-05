package com.iget.datatranshubdemo;

import android.app.Application;

public class MainApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        DataTransHubEbook.getInstance().init(this.getApplicationContext());
    }

    @Override
    public void onTerminate() {
        DataTransHubEbook.getInstance().release();
        super.onTerminate();
    }
}
