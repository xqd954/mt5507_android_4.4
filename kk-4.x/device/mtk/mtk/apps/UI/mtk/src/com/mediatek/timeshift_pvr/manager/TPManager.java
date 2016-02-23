package com.mediatek.timeshift_pvr.manager;

import android.content.Context;
import android.os.Handler;
import android.os.Message;

/**
 * The Base Logic Manager
 * @author HaoSun
 *
 */
public class TPManager {
    
    private static TPManager mTPManager;
    private TPManagerHandler mTPManagerHandler;
    
    public static TPManager getInstance(Context context){
        if (mTPManager==null) {
            synchronized (mTPManager) {
                mTPManager = new TPManager(context);
            }
        }
        return mTPManager;
    }
    
    public static TPManager getInstance(){
        if (mTPManager!=null) {
            return mTPManager;
        }else {
            return null;
        }
    }
    
    private TPManager(Context context){
        mTPManagerHandler = new TPManagerHandler(context);
        
    }
    
    
    public class TPManagerHandler extends Handler{
        
        private TPManagerHandler(){
            
        }
        public TPManagerHandler(Context context){
            
        }
        
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            
        }
    }
    
    public Handler getTPManagerHandler(){
        return mTPManagerHandler;
    }

}
