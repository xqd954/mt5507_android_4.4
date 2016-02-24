package com.tcl.snapshot;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileOutputStream;
import java.io.OutputStream;

import android.R.integer;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.Bitmap.Config;
import android.graphics.Matrix;
import android.util.Log;

import android.os.SystemProperties;

//mstar901
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.ByteBuffer;

import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.PictureCallback;
import android.os.Handler;
//mstar901


public class CaptureScreen {

	private final static String TAG = "CaptureScreen";
	/**
	 * 
	 * @param mode
	 *   MODE_OSD = 0 
	 *   MODE_VIDEO  = 1 
	 *   MODE_OSD_AND_VIDEO = 2
	 *   MODE_TV = 3
	 *   MODE_NOT_TV = 4;
	 */
	public final static int MODE_OSD = 0;
	public final static int MODE_VIDEO = 1;
	public final static int MODE_OSD_AND_VIDEO = 2;
	public final static int MODE_TV = 3;
	public final static int MODE_NOT_TV = 4;

	public boolean CapturePicture(int mode, Rect rect, String path) {
          
		File file = new File(path);
		return CapturePicture(mode, rect, file);
	}

	public boolean CapturePicture(int mode, Rect rect, File file) {

		boolean ret = false;
		try {
			VideoCamera(mode, rect, file);
		} catch (Exception e) {
			// TODO: handle exception
			e.printStackTrace();
		}
		return ret;
	}

	public boolean CapturePicture(int mode, Rect rect, FileDescriptor fd) {
		
                boolean ret = false;
		return ret;
	}

	private Bitmap drawIntoBitmap(Rect rect) {
		Bitmap bm = Bitmap.createBitmap(rect.width(), rect.height(),
				Config.ARGB_8888);
		float x = bm.getWidth();
		float y = bm.getHeight();
		Canvas c = new Canvas(bm);
		Paint p = new Paint();
		p.setAntiAlias(true);
		p.setColor(0xFFFFFFFF);
		c.drawCircle(x / 2, y / 2, x / 2, p);

		p.setColor(0xFF000000);
		p.setTextSize(x / 10);
		p.setTextAlign(Paint.Align.CENTER);
		Paint.FontMetrics fm = p.getFontMetrics();
		c.drawText("TEST", x / 2, y / 2 - fm.ascent / 2, p);

		return bm;
	}

	private static class InstanceHolder {
		public final static CaptureScreen instance = new CaptureScreen();
	}

	public static CaptureScreen getInstance() {
		return InstanceHolder.instance;
	}
		
	
	//mstar901
	android.hardware.Camera mCameraDevice = null;
	Handler tempHandler = null;
	private File savefile;
	private int width;
	private int height;
	private boolean captureTv = false;
	//mstar901
	
		
	public void VideoCamera(int mode, Rect rect, File file) {
                Log.d(TAG, "VideoCamera ");
                if (mode == MODE_TV) {
	            captureTv = true;
		} else {
                    captureTv = false;
                }

		width = rect.width();
		height = rect.height();

		savefile = file;
                Log.d(TAG, "VideoCamera mode="+ mode + " width="+ width +" height="+ height + " file=" + file + " captureTv" + captureTv);
                takePicturesOnce();
	}

	private void takePicturesOnce() {
		try {
                        MyPictureCallback mMyPictureCallback = new MyPictureCallback();
			mCameraDevice.startPreview();
			mCameraDevice.takePicture(null, null, mMyPictureCallback);
			/*try {
				Thread.currentThread().sleep(4000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
                        mCameraDevice.stopPreview();
			mCameraDevice.release();*/

		} catch (Exception e) {
			// TODO: handle exception
			e.printStackTrace();
		}
	}

	
	private final class MyPictureCallback implements PictureCallback {

		MyPictureCallback() {
                        Log.d(TAG, "CameraDevice in-----------------------");
			mCameraDevice = android.hardware.Camera.open(4);
			Parameters mParameters;
			// vecapture api application
			// 1. get parameter
                        //Log.d(TAG, "CameraDevice open-----------------------");
			mParameters = mCameraDevice.getParameters();
                        //Log.d(TAG, "CameraDevice getParameter-----------------------");
			// 2. set parameters
			mParameters.setPictureSize(640, 360);
                        //Log.d(TAG, "CameraDevice set Size-----------------------");
			if(captureTv){
                                //Log.d(TAG, "Parameter 2--------------------");
				mParameters.set("dwincapture-mode", 2);  // capture only tv screen
			}else{
                                //Log.d(TAG, "Parameter 1--------------------");
				mParameters.set("traveling-mode",1);  // capture all screen 
			}
			
			mCameraDevice.setParameters(mParameters);
                        //Log.d(TAG, "CameraDevice set Parameter-----------------------");
		}

		public void onPictureTaken(byte[] data, Camera arg1) {
			try {
				// handle data here
				Log.d(TAG, "catch picture data -----------------------");
				Bitmap mBitmap = Bitmap.createBitmap(width, height,
						Bitmap.Config.ARGB_8888);
				ByteBuffer buffer = ByteBuffer.wrap(data);
                                //mCameraDevice.stopPreview();
			        //mCameraDevice.release();
				mBitmap.copyPixelsFromBuffer(buffer);
				buffer.clear();
				saveMyBitmap(mBitmap);
				Log.d(TAG, "save end picture data -----------------------");
                                mCameraDevice.stopPreview();
			        mCameraDevice.release();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	/*private void saveMyBitmap(Bitmap mBitmap) {

		try {
			File f = new File(PATH);
			if (f.exists()) {
				f.delete();
			}
			f.createNewFile();
			FileOutputStream fOut = null;
			fOut = new FileOutputStream(f);
			mBitmap.compress(Bitmap.CompressFormat.JPEG, 100, fOut);
			fOut.flush();
			fOut.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}*/

	private void saveMyBitmap(Bitmap mBitmap) throws IOException {

		try {
                        FileOutputStream fos = null;
		        savefile.createNewFile();
			fos = new FileOutputStream(savefile);
                        mBitmap.compress(Bitmap.CompressFormat.JPEG, 100, fos);
                        fos.flush();
                        fos.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
}
