package com.opengl.jni;



import android.graphics.Bitmap;
import android.os.SystemProperties;
import android.util.Log;

public class GLJniLib {

    static {
    	//System.loadLibrary("png");
    	if(SystemProperties.getInt("mtk.vss.demo", 0) == 1){
    		System.loadLibrary("vsstexch");
    	}
    }
    
   /**
    * @param width the current view width
    * @param height the current view height
    */
    //public static native void init(int width, int height);
    //public static native void step();
   // public static native void setBackColor(float r,float g,float b);
    
    public static native void nativeInit();
    public static native void nativeUnInit();
    public static native int nativeGetHeight();
    public static native int nativeGetWidth();
    public static native void nativeResize(int w, int h);
    public static native void nativeRender();
    public static native void nativeTimingChange();
    public static native void nativeDone();
    public static native int add(int a,int b);
    public static native void nativeUpdateTexture(int texid);
}
