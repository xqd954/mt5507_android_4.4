package com.mediatek.jni;

public class picjni {
	
	static {
		try {
			System.loadLibrary("picapi");
			System.out.println("System.loadLibrary: successfully");
		} catch(UnsatisfiedLinkError e) {
			System.out.println("System.loadLibrary" + e);
			e.printStackTrace(System.out);
		}
	}

	public static native int add(int w, int h);

	public static native boolean nativeGet4K2KPlayerFlag();

	public static native int nativeExitFromPhotoPlay(boolean flag, int index);
	public static native boolean doMTPHOTO4KShow(int type,int data,int dataSize,int x,int y,int width,int height);
}
