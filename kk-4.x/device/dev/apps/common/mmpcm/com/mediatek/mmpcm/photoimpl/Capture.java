package com.mediatek.mmpcm.photoimpl;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PaintFlagsDrawFilter;
import android.util.Log;
import android.view.View;

import com.mediatek.mmpcm.MmpTool;
import com.mediatek.mmpcm.photo.ICapture;
/**
 * This class represents capture a photo from view.
 * 
 *
 */
public class Capture implements ICapture {

	private static final String TAG = "Capture";
	private Bitmap bmpCap;
	private int mNativeBitmap;
	private int mWidth;
	private int mHeight;
	private int mPitch;
	private int mMode;
	/**
	 * Capture a photo from view.
	 * @param v, view that user want cpature
	 */
	public void captureImage(View v) {

		View vw = v.getRootView();
		vw.setDrawingCacheEnabled(true);
		bmpCap = vw.getDrawingCache();
		if(null==bmpCap)
		{
			try {
				bmpCap=getBitmapFromView(vw,vw.getWidth(),vw.getHeight());
			} catch (Exception e) {
				Log.e(TAG, "captureImage:"+e.getMessage());
			}
		}

		if (null != bmpCap) {
			mWidth = bmpCap.getWidth();
			mHeight = bmpCap.getHeight();
			
			//bmpCap = makeBitmap(bmpCap, mWidth, mHeight);
			mNativeBitmap = bmpCap.getNativeBitmap();
			mPitch = bmpCap.getRowBytes();
			mMode = ConfigToInt(bmpCap.getConfig());
			
			MmpTool.LOG_INFO("mMode = "+mMode);
		}
		
	}
	
	private Bitmap getBitmapFromView(View view, int width, int height)  
    {  
                int widthSpec = View.MeasureSpec.makeMeasureSpec(width,  
                    View.MeasureSpec.EXACTLY);  
                int heightSpec = View.MeasureSpec.makeMeasureSpec(height,  
                    View.MeasureSpec.EXACTLY);  
                view.measure(widthSpec, heightSpec);  
                view.layout(0, 0, width, height);  
        Bitmap bitmap = Bitmap.createBitmap(width, height,  
            Bitmap.Config.ARGB_8888);  
        Canvas canvas = new Canvas(bitmap);  
        canvas.drawColor(Color.TRANSPARENT);  
        canvas.setDrawFilter(new PaintFlagsDrawFilter(0, Paint.ANTI_ALIAS_FLAG  
            | Paint.FILTER_BITMAP_FLAG));  
        view.draw(canvas);  
  
        return bitmap;  
	}

/*	private Bitmap makeBitmap(Bitmap bitmap,int width,int height) {
		
		Bitmap bmp = Bitmap.createBitmap(bitmap, 0, 0, width, height, null, false);
		Bitmap bitmap1 = Bitmap.createBitmap(width, height, Config.ARGB_8888);
		Canvas canvas = new Canvas(bitmap1);
		canvas.drawBitmap(bmp, 0, 0, null);
		if (bmp != bitmap) {
			bitmap.recycle();
		}
		return bitmap1;

	}*/

	private int ConfigToInt(Bitmap.Config config) {

		switch (config) {
		case ALPHA_8:
			return 2;
		case RGB_565:
			return 4;
		case ARGB_4444:
			return 5;
		case ARGB_8888:
			return 6;
		default:
			return 6;
		}
	}
	/**
	 * Get native bitmap pointer.
	 * @return pointer.
	 */
	public int getNativeBitmap() {
		return mNativeBitmap;
	}
	/**
	 * Get capture photo width.
	 * @return
	 */
	public int getWidth() {
		return mWidth;
	}
	/**
	 * Get capture photo height.
	 * @return
	 */
	public int getHeight() {
		return mHeight;
	}
	/**
	 * Get the number of single-digit percentage.
	 * @return
	 */
	public int getPitch() {
		return mPitch;
	}
	/**
	 * Get capture color mode.
	 * @return
	 */
	public int getColorMode() {
		return mMode;
	}

}
