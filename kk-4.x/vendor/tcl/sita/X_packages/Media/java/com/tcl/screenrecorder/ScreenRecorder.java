/*
 * ScreenRecorder.java
 * 
 *  Created on: 2013-5-21
 *      Author: zhangsh
 *      
 *      
 *     Call example:
 *     
 *     ScreenRecorder mir = new  ScreenRecorder();
 *     
 *     mir.setOutputFormat(ScreenRecorder.MPEGTS);
 *     mir.setOutputFile("/data/test.ts");
 *     mir.setVideoWithOSD(true);
 *     //mir.setVideoWithOSD(true);
 *     mir.setVideoSize(640,480);
 *     mir.setVideoFrameRate(25);
 *     
 *     mir.prepare();
 *     mir.start();
 *     ...
 *     mir.stop();
 *     mir.reset();
 *     mir.release();
 *           
 */
package com.tcl.screenrecorder;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import android.util.Log;

/**
 * @author zhangsh
 *
 */
public class ScreenRecorder {
	
	private static final String TAG = "SCREENRECORDER";
	
	public static final int MPEGTS = 0;
	public static final int RECORD_FORMAT_MPEGTS = MPEGTS;
	
	public static final int RECORD_MODE_OSD_ONLY = 0;
	public static final int RECORD_MODE_VIDEO_ONLY = 1;
	public static final int RECORD_MODE_OSD_VIDEO = 2;
	
	static 
	{
		try
		{
			Log.d(TAG, " load library screenrecorder_jni ! ");
			System.loadLibrary("screenrecorder_jni");
		}
		catch (Exception e)
		{
			Log.d(TAG, " load library screenrecorder_jni failed ! ");
		}
	}
	
	
	public ScreenRecorder()
	{
		Log.i(TAG, "ScreenRecorder init.......");
		_init();
	}
	
	public void  setOutputFile(FileDescriptor fd)
	{
		Log.i(TAG, "ScreenRecorder setOutputFile FileDescriptor ");
		_setOutputFile(fd); 
	}
	
	public void  setOutputFile(String path)
	{
		Log.i(TAG, "ScreenRecorder setOutputFile  "+ path);
		FileOutputStream fos = null;
        try {
        	fos = new FileOutputStream(path);
        	setOutputFile(fos.getFD());
        }
        catch (Exception e) {
			// TODO: handle exception
        	e.printStackTrace();
		}finally {
        	try {
        		fos.close();
			} catch (Exception e) {
				// TODO: handle exception
				e.printStackTrace();
			}
            
        }
	}
	
	public void setOutputFormat(int output_format)
	{
		Log.i(TAG, "ScreenRecorder setOutputFormat： " + output_format);
		
		//TODO: current only support mpeg2ts container format
		_setOutputFormat(this.RECORD_FORMAT_MPEGTS);
	}
	
	public void setVideoWithOSD(boolean bool)
	{
		Log.i(TAG, "ScreenRecorder setVideoWithOSD： "+bool);
		if(bool)
		{
			setRecordMode(this.RECORD_MODE_OSD_VIDEO);
		}
		else {
			setRecordMode(this.RECORD_MODE_VIDEO_ONLY);
		}
	}
	
	public void setRecordMode(int mode)
	{
		Log.i(TAG, "ScreenRecorder setRecordMode： "+mode);
		_setRecordMode(mode);
	}
	
	public void setVideoSize(int width,int height)
	{
		Log.i(TAG, "ScreenRecorder setVideoSize: "+width+"*"+height);
		_setVideoSize(width, height);
	}
	
	public void setVideoFrameRate(int rate)
	{
		Log.i(TAG, "ScreenRecorder setVideoFrameRate: "+rate);
		_setVideoFrameRate(rate);
	}
	
	
	public void prepare()
	{
		Log.i(TAG, "ScreenRecorder prepare ");
		_prepare();
	}
	
	public void start()
	{
		Log.i(TAG, "ScreenRecorder start.................");
		_start();
	}
	
	public void stop()
	{
		Log.i(TAG, "ScreenRecorder stop");
		_stop();
	}
	
	public void reset()
	{
		Log.i(TAG, "ScreenRecorder reset");
		_reset();
	}
	
	public void release()
	{
		Log.i(TAG, "ScreenRecorder release");
		_release();
	}

	
	private native void _init();
	
	private native void _setOutputFile(FileDescriptor fd);
	private native void _setOutputFormat(int output_format);
	private native void _setRecordMode(int mode);
	private native void _setVideoSize(int width,int height);
	private native void _setVideoFrameRate(int rate);
	
	private native void _prepare();
	
	private native void _start();
	
	private native void _stop();
	
	private native void _reset(); 
	
	private native void _release();

}
