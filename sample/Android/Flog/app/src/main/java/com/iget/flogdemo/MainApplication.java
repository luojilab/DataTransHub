package com.iget.flogdemo;

import android.app.Application;

public class MainApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        FlogInstance.getInstance().init(this.getApplicationContext());
    }

    @Override
    public void onTerminate() {
        FlogInstance.getInstance().release();
        super.onTerminate();
    }
}
