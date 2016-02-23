package com.mediatek.mmpcm;

import android.util.Log;

public class MmpTool {
    private static final String TAG = "MMP"; 
    
    public static void LOG_ERROR(String s) {
        if(Log.isLoggable(TAG, Log.ERROR)){
            StackTraceElement[] elog = new Exception().getStackTrace();
            Log.e(TAG,
                    "Class:" + elog[1].getClassName() + 
                    "." + elog[1].getMethodName() +
                    " (Line:" + new Integer(elog[1].getLineNumber()).toString() + ") :" + s);
            
        }
    }
    
    public static void LOG_INFO(String s) {
        //if(Log.isLoggable(TAG, Log.INFO)){
            StackTraceElement[] elog = new Exception().getStackTrace();
            Log.i(TAG,
                    "Class:" + elog[1].getClassName() + 
                    "." + elog[1].getMethodName() +
                    " (Line:" + new Integer(elog[1].getLineNumber()).toString() + ") :" + s);
            
       // }
    }
    
    public static void LOG_DBG(String s) {
        if(Log.isLoggable(TAG, Log.DEBUG)){
            StackTraceElement[] elog = new Exception().getStackTrace();
            Log.d(TAG,
                    "Class:" + elog[1].getClassName() + 
                    "." + elog[1].getMethodName() +
                    " (Line:" + new Integer(elog[1].getLineNumber()).toString() + ") :" + s);
            
        }
    }
    
    public static void LOG_WARN(String s) {
        if(Log.isLoggable(TAG, Log.WARN)){
            StackTraceElement[] elog = new Exception().getStackTrace();
            Log.w(TAG,
                    "Class:" + elog[1].getClassName() + 
                    "." + elog[1].getMethodName() +
                    " (Line:" + new Integer(elog[1].getLineNumber()).toString() + ") :" + s);
            
        }
    }
}
