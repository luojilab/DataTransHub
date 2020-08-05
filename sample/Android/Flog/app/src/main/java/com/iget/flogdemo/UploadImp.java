package com.iget.flogdemo;

import com.iget.datatranshub.callback.IUploadImp;

import java.util.Random;

public class UploadImp implements IUploadImp {

    public UploadImp(){
    }

    @Override
    public void upload(String s) {
        //use ok http or other network lib to implement upload file

        //simulate network upload success and failed
        int min = 0;
        int max = 10;
        Random random = new Random();
        int num = random.nextInt(max) % (max - min + 1) + min;
        if (num != 5) {
            FlogInstance.getInstance().notifySucces(s);
        }else{
            FlogInstance.getInstance().notifyFailed(s);
        }
    }
}
