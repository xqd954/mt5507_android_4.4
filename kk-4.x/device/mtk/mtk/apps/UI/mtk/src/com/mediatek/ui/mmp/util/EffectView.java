package com.mediatek.ui.mmp.util;

import java.util.Random;

import com.mediatek.mmpcm.MmpTool;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PorterDuff.Mode;
import android.graphics.Rect;
import android.graphics.Region.Op;
import android.graphics.drawable.BitmapDrawable;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import com.mediatek.mmpcm.photoimpl.ConstPhoto;
import com.mediatek.mmpcm.photoimpl.PhotoUtil;
import com.mediatek.mmpcm.photoimpl.DissolveData;
import com.mediatek.jni.picjni;
import android.os.Looper;
import android.os.HandlerThread;
import android.os.Process;


/**
 * 
 * This class represents picture effect .
 *
 */
public class EffectView  implements Runnable {

	private int top;
	private int left;
	private int bottom;
	private int right;
	private int bmpW;
	private int bmpH;
	private int bmpPreW;
	private int bmpPreH;
	private int bmp_x;
	private int bmp_y;
	private int bmpPre_x;
	private int bmpPre_y;
	private int displayW = 3840;
	private int displayH = 2160;
	
	Bitmap bmp;
	Bitmap bmpPre;

	int gifIndex;
	
	
	Bitmap[] bmpGif;
	
	int[] mDelayTime;

	Paint paint = new Paint();
	DissolveData dissolveData = null;

	int DSLV_WIDTH_UNIT = 160;
	int DSLV_HEIGHT_UNIT = 120;
	int total_frame = 4;
	int frame_index = 0;
	int DSLV_NUMBER = 160 * 120;// 160 * 120
	int index;
	int unit_end;
	int unit_width;
	int unit_height;
	int x_offset;
	int y_offset;
	int _x;
	int _y;
	int region_width;
	int region_height;
	private Canvas myCanvas;
	private Bitmap drawBmp;
	private HandlerThread mThread;


	private static int EffectDigreeNum = 0;

	private ImagePlay playLisenter;




	public interface ImagePlay{

		public void playDone();
		public void playError();
	}

	public void setPlayLisenter(ImagePlay play){
		this.playLisenter = play;


	}

	public void recycleLastBitmap(){
		if (bmpPre != null && bmpPre != bmp) {
			bmpPre.recycle();
			bmpPre = null;
		}

	}
	
	/**
	 * Simple constructor to use when creating a effect view from code.
	 * 
	 * @param context The Context the view is running in, through which it can
     *        access the current theme, resources, etc.
	 */
	public EffectView() {
	
		
		drawBmp = Bitmap.createBitmap(displayW,displayH,Bitmap.Config.ARGB_8888);
		myCanvas = new Canvas(drawBmp);
		mThread = new HandlerThread("DrawThread",Process.THREAD_PRIORITY_DEFAULT);
		mThread.start();
		eHandler = new MyHandler(mThread.getLooper());

	}

	public void clearScreen(){
		myCanvas.clipRect(0, 0, displayW, displayH, Op.REPLACE);
		myCanvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
		playBitmap(drawBmp);
		playLisenter.playDone();
		//int[] pixels=new int[displayW*displayH];
		//picjni.doMTPHOTO4KShow(2, pixels, displayW*displayH*4, 0, 0, displayW,  displayH);
		//pixels=null;
	}

	private void recycleBitmap(Bitmap curBmp){
		if(curBmp != null ){

			curBmp.recycle();
			curBmp = null;
		}


	}


	private void recycleBitmapArray(Bitmap[] curBmps){
		if(curBmps != null  && curBmps.length > 0){

			for(Bitmap bmp: curBmps){
				recycleBitmap(bmp);

			}
		}


	}
	
	/**
	 * Set reource bitmap to use implements effect.
	 * @param bitmap
	 */
	public void setRes(PhotoUtil bitmap) {
		if(null != bitmap && null != bitmap.getmBitmaps()) {
			bmpGif = bitmap.getmBitmaps();
			mDelayTime = bitmap.getmDelayTime();
			MmpTool.LOG_INFO("setRes = " + (mDelayTime!=null?(mDelayTime.length+""):"delaytiem array is null"));
			bmp = bitmap.getmBitmaps()[0];
			if(bitmap.getmBitmaps().length > 1)
				bmp = null;
		}else{
			bmpGif = null;
			bmp = null;
		}
		if (null != bmp) {

			bmpW = (int) getBitmapWidth(bmp);
			bmpH = (int) getBitmapHeight(bmp);
					
			MmpTool.LOG_INFO("bmpW = " + bmpW);
			MmpTool.LOG_INFO("bmpH = " + bmpH);
		}
	}
	

	int multiple = 1;
	/**
	 * Set a base value to use zoom out or zoom in.
	 * @param i
	 */
	public void setMultiple(int i) {
		multiple = i;
	}

	int preMultiple = 1;
	/**
	 * Set a base value to use play effect.
	 * @param i
	 */
	public void setPreMultiple(int i) {
		preMultiple = i;
	}
	
	/**
	 * Get a base value to use zoom out or zoom in.
	 * @param i
	 */
	public int getMultiple() {
		return multiple;
	}
	/**
	 * Get a base value to use play effect.
	 * @param i
	 */
	public int getPreMultiple() {
		return preMultiple;
	}


	boolean flag = true;
	
	private float getBitmapWidth(Bitmap b){
		float w = 0;
		if (b != null){
			w = (b.getWidth() * getScale(b));
		}
		return w < 1.0f ? 1.0f : w;
	}
	/*lei add for think scale*/
	private float getBitmapHeight(Bitmap b){
		float h = 0;
		if (b != null){
			h = (b.getHeight()* getScale(b));
		}
		return h < 1.0f ? 1.0f : h;
	}
	/**
	 * Set reource bitmap to use implements effect.
	 * @param bitmap
	 */
	public void setEffectRes(Bitmap bmpEffect) {
		bmpGif = null;
		if (null != bmpEffect) {

			if (flag) {
				MmpTool.LOG_INFO("1111111");
				bmp = bmpEffect;
				bmpPre = null;
				flag = false;
				if (bmp != null) {
					bmpPreW = (int) getBitmapWidth(bmp);
					bmpPreH = (int) getBitmapHeight(bmp);
				}
			} else {
				MmpTool.LOG_INFO("2222222");
				bmpPre = bmp;
				bmp = bmpEffect;
				if (bmpPre != null) {
					bmpPreW = (int) getBitmapWidth(bmpPre);
					bmpPreH = (int) getBitmapHeight(bmpPre);
				}

				MmpTool.LOG_INFO("bmpPreW = " + bmpPreW);
				MmpTool.LOG_INFO("bmpPreH = " + bmpPreH);
			}
			if (bmp != null) {
				bmpW = (int) getBitmapWidth(bmp);
				bmpH = (int) getBitmapHeight(bmp);
			}

			MmpTool.LOG_INFO("bmpW = " + bmpW);
			MmpTool.LOG_INFO("bmpH = " + bmpH);
		}
	}

	/*lei add for think scale*/
	private float getScale(Bitmap bitmap) {
		float scale = 1.0f;
		if (bitmap == null) {
			return scale;
		}
		float w = bitmap.getWidth();
		float h = bitmap.getHeight();
		float specialphoto = w/h;

		if(specialphoto>=SPCEIL_FILTER||specialphoto<= (float)1/SPCEIL_FILTER){

		}else if (w > 0 && h > 0) {
			float widthScale = Math.min(displayW / w, 1.0f);
			float heightScale = Math.min(displayH / h, 1.0f);
			scale = Math.min(widthScale, heightScale);
		}
		if (scale <= 0.0f ){
			scale = 1.0f;
		}
		return scale;
	}
	private void InitCoordinate1() {
		float scale = 1.0f;
		if (bmpW != 0) {

			bmp_x = (bmpW < displayW) ? (int) Math
					.ceil((displayW - bmpW * scale) / 2.0) : 0;
			bmp_y = (bmpH < displayH) ? (int) Math
					.ceil((displayH - bmpH * scale) / 2.0) : 0;
			MmpTool.LOG_INFO("bmp_x = " + bmp_x);
			MmpTool.LOG_INFO("bmp_y = " + bmp_y);
		}
	}

	private void InitCoordinate2() {
		float scale = 1.0f;
		
		if (bmpW != 0) {

			bmp_x = (bmpW < displayW) ? (int) Math
					.ceil((displayW - bmpW * scale) / 2.0) : 0;
			bmp_y = (bmpH < displayH) ? (int) Math
					.ceil((displayH - bmpH * scale) / 2.0) : 0;
			MmpTool.LOG_INFO("bmp_x = " + bmp_x);
			MmpTool.LOG_INFO("bmp_y = " + bmp_y);
		}

		if (bmpPreW != 0) {		    
       
			bmpPre_x = (bmpPreW < displayW) ? (int) Math
					.ceil((displayW - bmpPreW * scale) / 2.0) : 0;
			bmpPre_y = (bmpPreH < displayH) ? (int) Math
					.ceil((displayH - bmpPreH * scale) / 2.0) : 0;
		}
	}

	private void SideToMid(Canvas canvas) {
		MmpTool.LOG_INFO("(EffectView onDraw())    side_to_mid>>>>>>>>>>");
		
		if (left2 >= right2) {

			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {

			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(left, top, right, bottom);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			     canvas.clipRect(left2, top, right2, bottom, Op.REPLACE);

				canvas.drawColor(Color.BLACK, Mode.CLEAR);
				
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");

				canvas.clipRect(left, top, right, bottom);			
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);

				MmpTool.LOG_INFO("bmp_x = " + bmp_x);
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);				
					canvas.clipRect(left2, top, right2, bottom, Op.REPLACE);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
					canvas.clipRect(left2, top, right2, bottom, Op.REPLACE);
				}

				mDisplayPreMatrix.postRotate(EffectDigreeNum, displayW / 2,
						displayH / 2);
				canvas.drawColor(Color.BLACK, Mode.CLEAR);
				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);
			}

		}
	}

	private void SideToMide_Out(Canvas canvas) {
		
		if (left2 >= right2) {
		
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
		} else {
			canvas.clipRect(left, top, right, bottom);
			canvas.clipRect(left2, top, right2, bottom, Op.REPLACE);
			
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		}
	}
	
	private void MidToSize(Canvas canvas) {
		MmpTool.LOG_INFO("(EffectView onDraw())    mid_to_side>>>>>>>>>>");
		if ((left2 <= 0) || (right2 > displayW)) {
	
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {

			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(left2, top, right2, bottom, Op.REPLACE);

				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			} else {
					MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
					canvas.clipRect(0, 0, displayW, displayH, Op.REPLACE);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);

					canvas.clipRect(bmpPre_x - 2, bmpPre_y, bmpPre_x + bmpPreW,
							bmpPre_y + bmpPreH + 1, Op.REPLACE);
				}

				mDisplayPreMatrix.postRotate(EffectDigreeNum, displayW / 2,
						displayH / 2);

				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);

			
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top, right2, bottom, Op.REPLACE);
		
				canvas.drawColor(Color.BLACK, Mode.CLEAR);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);

				MmpTool.LOG_INFO("bmp_x = " + bmp_x);		
				
			
			}

		}
	}

	private void MidToSize_Out(Canvas canvas) {
		MmpTool.LOG_INFO("(EffectView onDraw())    mid_to_side_out>>>>>>>>>>");
		if ((left2 <= 0) || (right2 > displayW)) {
	
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
	
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {

			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			canvas.clipRect(left, top, right, bottom);
			canvas.clipRect(left2, top, right2, bottom, Op.DIFFERENCE);

			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		}
	}

	@SuppressWarnings("unused")
	private void BoxIn_back(Canvas canvas) {
		MmpTool.LOG_INFO("box_in>>>>>>>>>>");
		if (left2 >= right2 || top2 >= bottom2) {

			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {

			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top2, right2, bottom2, Op.DIFFERENCE);
	
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top2, right2, bottom2, Op.DIFFERENCE);

				canvas.drawBitmap(bmp, mDisplayMatrix, paint);

				MmpTool.LOG_INFO("bmp_x = " + bmp_x);
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
					canvas.clipRect(0, 0, displayW, displayH, Op.XOR);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
				canvas.clipRect(bmpPre_x-2, bmpPre_y, bmpPre_x + bmpPreW,
							bmpPre_y + bmpPreH + 1, Op.XOR);
				}
									
				mDisplayPreMatrix.postRotate(EffectDigreeNum, displayW/2, displayH/2);
				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);
			}
		}
	}

	@SuppressWarnings("unused")
	private void BoxOut_back(Canvas canvas) {
		MmpTool.LOG_INFO("box_out>>>>>>>>>>");
		if (left2 <= 0 || right2 >= displayW) {
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {

			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top2, right2, bottom2, Op.REPLACE);

				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top2, right2, bottom2, Op.REPLACE);

				canvas.drawBitmap(bmp, mDisplayMatrix, paint);

				MmpTool.LOG_INFO("bmp_x = " + bmp_x);
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
					canvas.clipRect(0, 0, displayW, displayH, Op.XOR);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
				canvas.clipRect(bmpPre_x-2, bmpPre_y, bmpPre_x + bmpPreW,
							bmpPre_y + bmpPreH + 1, Op.XOR);
				}					
				mDisplayPreMatrix.postRotate(EffectDigreeNum, displayW/2, displayH/2);
				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);
			}
		}
	}
	private void BoxIn(Canvas canvas) {
		MmpTool.LOG_INFO("box_in>>>>>>>>>>");
		if (left2 >= right2 || top2 >= bottom2) {
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {

			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
				canvas.clipRect(left2, top2, right2, bottom2, Op.REPLACE);

				canvas.drawColor(Color.BLACK, Mode.CLEAR);
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");

				canvas.clipRect(0, 0, right, bottom);			
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);

				MmpTool.LOG_INFO("bmp_x = " + bmp_x);
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);				
					canvas.clipRect(left2, top2, right2, bottom2, Op.REPLACE);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
					canvas.clipRect(left2, top2, right2, bottom2, Op.REPLACE);
				}
				mDisplayPreMatrix.postRotate(EffectDigreeNum, displayW / 2,
						displayH / 2);
				canvas.drawColor(Color.BLACK, Mode.CLEAR);
				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);

			}
		}
	}

	private void BoxOut(Canvas canvas) {
		MmpTool.LOG_INFO("box_out>>>>>>>>>>");
		if (left2 <= 0 || right2 >= displayW) {
			canvas.clipRect(0, 0, right, bottom);
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top2, right2, bottom2, Op.REPLACE);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
					canvas.clipRect(0, 0, displayW, displayH, Op.REPLACE);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
			
					canvas.clipRect(bmpPre_x - 2, bmpPre_y, bmpPre_x + bmpPreW,
							bmpPre_y + bmpPreH + 1, Op.REPLACE);
				}

				mDisplayPreMatrix.postRotate(EffectDigreeNum, displayW / 2,
						displayH / 2);

				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);

				
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top2, right2, bottom2, Op.REPLACE);
				canvas.drawColor(Color.BLACK, Mode.CLEAR);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);

				MmpTool.LOG_INFO("bmp_x = " + bmp_x);
			}
		}
	}


	
	

	private void WipeRight(Canvas canvas) {
		MmpTool.LOG_INFO("wipe_right>>>>>>>>>> right2 =" + right2);

		if (right2 >= displayW) {
			
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {
			
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left, top, right2, bottom, Op.REPLACE);
				
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left, top, right2, bottom, Op.REPLACE);
				
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);

				MmpTool.LOG_INFO("bmp_x = " + bmp_x);
				
				
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
					canvas.clipRect(right2, 0, displayW, displayH, Op.REPLACE);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
					
					canvas.clipRect(right2, bmpPre_y, bmpPre_x + bmpPreW,
							bmpPre_y + bmpPreH + 1, Op.REPLACE);
				}
				MmpTool.LOG_INFO("-fuck--displayW= "+displayW+"displayH= "+displayH);				
				mDisplayPreMatrix.postRotate(EffectDigreeNum, displayW / 2,
							displayH / 2);

				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);
			}
		}
	}
	
	private void WipeLeft(Canvas canvas) {
		MmpTool.LOG_INFO("wipe_left>>>>>>>>>>");
		if (left2 <= 0) {
		
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {
			
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top, right, bottom, Op.REPLACE);
				
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top, right, bottom, Op.REPLACE);
				
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
				MmpTool.LOG_INFO("bmp_x = " + bmp_x);
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
					canvas.clipRect(0, 0, left2, displayH, Op.REPLACE);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
				
					canvas.clipRect(bmpPre_x - 2, bmpPre_y, left2, bmpPre_y
							+ bmpPreH + 1, Op.REPLACE);
				}
				
				mDisplayPreMatrix.postRotate(EffectDigreeNum, displayW/2, displayH/2);
				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);
			}
		}
	}
	
	private void WipeTop(Canvas canvas) {
		MmpTool.LOG_INFO("wipe_top>>>>>>>>>>");
		if (top2 <= 0) {
			
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {
			
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left, top2, right, bottom, Op.REPLACE);
				
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left, top2, right, bottom, Op.REPLACE);
				
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
				
				MmpTool.LOG_INFO("bmp_x = " + bmp_x);
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
					canvas.clipRect(0, 0, displayW,top2, Op.REPLACE);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
					canvas.clipRect(bmpPre_x - 2, bmpPre_y, bmpPre_x + bmpPreW,
							top2/* bmpPre_y + bmpPreH + 1 */, Op.REPLACE);
				}

				mDisplayPreMatrix.postRotate(EffectDigreeNum, displayW/2, displayH/2);
				
				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);
				MmpTool.LOG_INFO("bmpPreW = " + bmpPreW);
			}
		}
	}

	private void WipeBottom(Canvas canvas) {
		MmpTool.LOG_INFO("wipe_bottom>>>>>>>>>>");
		if (bottom2 >= displayH) {
			
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {
		
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left, top, right, bottom2, Op.REPLACE);
				
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left, top, right, bottom2, Op.REPLACE);
			
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);

				MmpTool.LOG_INFO("bmp_x = " + bmp_x);
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
					canvas.clipRect(0, bottom2,displayW, displayH, Op.REPLACE);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
					canvas.clipRect(bmpPre_x - 2, /* bmpPre_y */bottom2, bmpPre_x
							+ bmpPreW, bmpPre_y + bmpPreH + 1, Op.REPLACE);
				}
														
				mDisplayPreMatrix.postRotate(EffectDigreeNum, displayW/2, displayH/2);

				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);
			}
		}
	}
	
	private void Dissolve(Canvas canvas) {
		MmpTool.LOG_INFO("bmpPre if null, excute 1  frame_index"+frame_index);

		if (null == dissolveData) {
			dissolveData = new DissolveData();
		}

		unit_end = DSLV_NUMBER / total_frame;
		index = unit_end * frame_index;
		unit_end = unit_end + index;

		if (bmpPre == null) {
			if (frame_index < total_frame ) {

				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.save();
				for (index = 0; index < unit_end; index += 2) {

					int num = dissolveData.getNum(index);
					_x = num % DSLV_WIDTH_UNIT;
					_y = num / DSLV_WIDTH_UNIT;

					_x *= unit_width;
					_y *= unit_height;

					_x += x_offset;
					_y += y_offset;

					region_width = unit_width;
					region_height = unit_height;

					if (_x < 0) {
						region_width += _x;
					} else {
						region_width = (_x + region_width < displayW) ? region_width
								: displayW - _x;
					}

					if (_y < 0) {
						region_height += _y;
					} else {
						region_height = (_y + region_height < displayH) ? region_height
								: displayH - _y;
					}
					canvas.clipRect(_x, _y, _x + region_width, _y
							+ region_height, Op.REPLACE);

				
					canvas.drawBitmap(bmp, mDisplayMatrix, paint);
					
					if (region_width <= 0 || region_height <= 0) {
						continue;
					}
				}
				canvas.restore();
			} else {

				
				canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
				MmpTool.LOG_INFO("dissolve frame number= " + frame_index);
			
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			}
			frame_index++;
			
		} else {
			if (frame_index < total_frame) {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
				}								

				mDisplayPreMatrix.postRotate(EffectDigreeNum, displayW/2, displayH/2);												
				
				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				canvas.save();

				for (index = 0; index < unit_end; index += 2) {

					int num = dissolveData.getNum(index);
					_x = num % DSLV_WIDTH_UNIT;
					_y = num / DSLV_WIDTH_UNIT;

					_x *= unit_width;
					_y *= unit_height;

					_x += x_offset;
					_y += y_offset;

					region_width = unit_width;
					region_height = unit_height;

					if (_x < 0) {
						region_width += _x;
					} else {
						region_width = (_x + region_width < displayW) ? region_width
								: displayW - _x;
					}

					if (_y < 0) {
						region_height += _y;
					} else {
						region_height = (_y + region_height < displayH) ? region_height
								: displayH - _y;
					}

					canvas.clipRect(_x, _y, _x + region_width, _y
							+ region_height, Op.REPLACE);

					
					canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
					
					canvas.drawBitmap(bmp, mDisplayMatrix, null);

					if (region_width <= 0 || region_height <= 0) {
						continue;
					}

				}

				canvas.restore();

			} else {
				
				canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
				MmpTool.LOG_INFO("dissolve frame number= " + frame_index);
				
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			}
			frame_index++;
			
		}
	}
	
	private void ZoomIn(Canvas canvas) {
		MmpTool.LOG_INFO("Zoom Out == "+multiple);
		MmpTool.LOG_INFO("ZoomIn");
		getProperZoomMatrix(bmp, mDisplayMatrix, (float)multiple);
		canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		

	}
	
	private void ZoomOut(Canvas canvas) {
		
		MmpTool.LOG_INFO("Zoom Out == "+multiple);
		getProperZoomMatrix(bmp, mDisplayMatrix, (float)multiple);
		mDisplayMatrix.postRotate(EffectDigreeNum, displayW/2, displayH/2);	
		canvas.drawBitmap(bmp, mDisplayMatrix, paint);

	}
	
	private void Default(Canvas canvas) {
		MmpTool.LOG_INFO("Default+++++++++++++++++++");
		
		if( null!= bmpGif && bmpGif.length>1 ) {

			MmpTool.LOG_INFO("Default draw gif+++++++++++++++ gif index ="+gifIndex +"  bmpGif.length ="+bmpGif.length);
			if(gifIndex < bmpGif.length){
				canvas.drawBitmap(bmpGif[gifIndex], mDisplayMatrix, paint);

			}
		} else if (null != bmp) {
					
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);			
			
		} else {
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			
			//clearScreen();
			System.out.println(" Image decode failure! ");
		}

		
	}

	private void DrawBlack(Canvas canvas) {
		//clearScreen();
		canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
	}
    /**
     * Implement this to do your drawing.
     *
     * @param canvas the canvas on which the background will be drawn
     * 	Modified by Lei for add Scale Matrix when Draw bitmap
	 * CR TV00399654
     */
	public void onDraw(Canvas canvas) {
		MmpTool.LOG_INFO("onDraw ");
		canvas.clipRect(0, 0, displayW, displayH, Op.REPLACE);
		canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
	//	isrotate=false;
		if (bmp != null) {
			getProperBaseMatrix(bmp, mDisplayMatrix);
		} else if(bmpGif != null && bmpGif[0] != null) {
			getProperBaseMatrix(bmpGif[0], mDisplayMatrix);
		}else{	
			return;
		}

		switch (type) {

		case ConstPhoto.side_to_mid:
			SideToMid(canvas);
			break;
		case ConstPhoto.mid_to_side:
			MidToSize(canvas);
			break;
		case ConstPhoto.side_to_mid_out:
			SideToMide_Out(canvas);
			break;
		case ConstPhoto.mid_to_side_out:
			MidToSize_Out(canvas);
			break;
		case ConstPhoto.box_in:
			BoxIn(canvas);
			break;
		case ConstPhoto.box_out:
			BoxOut(canvas);
			break;
		case ConstPhoto.wipe_right:
			WipeRight(canvas);
			break;
		case ConstPhoto.wipe_left:
			WipeLeft(canvas);
			break;
		case ConstPhoto.wipe_top:
			WipeTop(canvas);
			break;
		case ConstPhoto.wipe_bottom:
			WipeBottom(canvas);
			break;
		case ConstPhoto.dissolve:
			Dissolve(canvas);
			break;

			case ConstPhoto.ZOOMOUT:
		case ConstPhoto.ROTATE_R:
			ZoomOut(canvas);
			break;
				
		case ConstPhoto.ROTATE_PHOTO:
			RotatePhoto(canvas);
			break;
			
		case ConstPhoto.ZOOMIN:
			ZoomIn(canvas);
			break;

	
		case ConstPhoto.DEFAULT:
			Default(canvas);
			break;
		case ConstPhoto.DRAWBLACK:
			DrawBlack(canvas);
			break;
		default:
			break;	
		}
	   MmpTool.LOG_INFO("playBitmap time begin = "+System.currentTimeMillis());
		
		playBitmap(drawBmp);
		MmpTool.LOG_INFO("playBitmap time  end = "+System.currentTimeMillis());
		
	}

	private void playBitmap(Bitmap bitmap){

		
		//int[] pixels = new int[displayW*displayH];
		//bitmap.getPixels(pixels, 0, displayW, 0, 0, displayW,displayH);
		int nativeBitmap = bitmap.getNativeBitmap();
		if(!picjni.doMTPHOTO4KShow(1, nativeBitmap, displayW*displayH*4, 0, 0, displayW,  displayH)){
			playLisenter.playError();

		}
		//pixels=null;

	}




	private static int mTempType;
	private static int type;
	private int right2;
	private int left2;
	private int top2;
	private int bottom2;
	private boolean mIsSwitchingEffect;
	
	private void InitSideToMid() {
		left = 0;
		top = 0;
		right = displayW;
		bottom = displayH;

		left2 = 0;
		right2 = displayW;

		InitCoordinate2();

	}

	private void InitMidToSide() {
		if (displayW % 2 == 0) {
			left = displayW / 2;
			top = 0;
			right = displayW / 2;
			bottom = displayH;
		} else {
			left = (displayW - 1) / 2;
			top = 0;
			right = (displayW - 1) / 2;
			bottom = displayH;
		}
		if (displayW % 2 == 0) {
			left2 = displayW / 2;
			right2 = displayW / 2;

		} else {
			left2 = (displayW - 1) / 2;
			right2 = (displayW - 1) / 2;
		}

		InitCoordinate2();
	}

	private void InitSideToMid_Out() {
		left = 0;
		top = 0;
		right = displayW;
		bottom = displayH;

		left2 = 0;
		right2 = displayW;

		InitCoordinate1();
	}

	private void InitMidToSide_Out() {

		left = 0;
		top = 0;
		right = displayW;
		bottom = displayH;

		if (displayW % 2 == 0) {
			left2 = displayW / 2;
			right2 = displayW / 2;

		} else {
			left2 = (displayW - 1) / 2;
			right2 = (displayW - 1) / 2;
		}
		InitCoordinate1();
	}

	private void InitBoxIn() {
		left = 0;
		top = 0;
		right = displayW;
		bottom = displayH;

		left2 = 0;
		top2 = 0;
		right2 = displayW;
		bottom2 = displayH;

		InitCoordinate2();
	}

	private void InitBoxOut() {
		left = 0;
		top = 0;
		right = displayW;
		bottom = displayH;

		left2 = (int) Math.ceil(displayW / 2);
		top2 = (int) Math.ceil(displayH / 2);
		right2 = (int) Math.ceil(displayW / 2);
		bottom2 = (int) Math.ceil(displayH / 2);

		InitCoordinate2();
	}

	private void InitWipeRight() {
		left = 0;
		top = 0;
		right = displayW;
		bottom = displayH;
		right2 = 0;

		InitCoordinate2();
	}

	private void InitWipeLeft() {
		left = 0;
		top = 0;
		right = displayW;
		bottom = displayH;
		left2 = displayW;

		InitCoordinate2();
	}

	private void InitWipeTop() {
		left = 0;
		top = 0;
		right = displayW;
		bottom = displayH;
		top2 = displayH;

		InitCoordinate2();
	}

	private void InitWipeBottom() {
		left = 0;
		top = 0;
		right = displayW;
		bottom = displayH;
		bottom2 = 0;

		InitCoordinate2();
	}

	private void InitDissolve() {

		frame_index = 0;

		unit_width = (displayW + DSLV_WIDTH_UNIT - 1) / DSLV_WIDTH_UNIT;
		unit_height = (displayH + DSLV_HEIGHT_UNIT - 1) / DSLV_HEIGHT_UNIT;

		MmpTool.LOG_DBG( "unit_width = " + unit_width);
		MmpTool.LOG_DBG( "unit_height = " + unit_height);

		x_offset = (displayW - unit_width * DSLV_WIDTH_UNIT) / 2;
		y_offset = (displayH - unit_height * DSLV_HEIGHT_UNIT) / 2;

		MmpTool.LOG_DBG( "x_offset = " + x_offset);
		MmpTool.LOG_DBG( "y_offset = " + y_offset);

		if (x_offset < 0) {
			x_offset = 0;
		}

		if (y_offset < 0) {
			y_offset = 0;
		}

		InitCoordinate2();
	}

	private void InitAnimation() {
		InitCoordinate2();
	}

	private void InitDefault() {
		left = 0;
		top = 0;
		right = displayW;
		bottom = displayH;

		InitCoordinate1();
	}

	static boolean randomEffectValue;
	/**
	 * Set effect type to use playback photo.
	 * @param i
	 */
	public void setType(int i) {

		if (i == ConstPhoto.ROTATE_R || i == ConstPhoto.ZOOMOUT

				|| i == ConstPhoto.DRAWBLACK||i == ConstPhoto.ROTATE_PHOTO) {
			
			mIsSwitchingEffect = false;
			type = i;
			MmpTool.LOG_INFO("set type = " + type);
			return;
		}

		mIsSwitchingEffect = true;
		mTempType = i;
		MmpTool.LOG_INFO("before get random: type = " + mTempType);

		randomEffectValue = false;
		if (getEffectValue() == ConstPhoto.RADNOM) {
			mTempType = getRandomNum();

			MmpTool.LOG_INFO("get random number after type = " + mTempType);

			randomEffectValue = true;
		}

	}

	private int disX;
	private int disY;

	public int getDisX() {

		switch (type) {
		case ConstPhoto.wipe_right:
		case ConstPhoto.wipe_left:
			disX = (int) Math.ceil(displayW / 8.0);
			return disX;

		case ConstPhoto.mid_to_side:
		case ConstPhoto.side_to_mid:
		case ConstPhoto.side_to_mid_out:
		case ConstPhoto.mid_to_side_out:
		case ConstPhoto.box_in:
		case ConstPhoto.box_out:
			disX = (int) Math
					.ceil(displayW / 8.0);
			return disX;
		default:
			return 0;
		}
	}

	public int getDisY() {
		switch (type) {
		case ConstPhoto.wipe_top:
		case ConstPhoto.wipe_bottom:
			disY = (int) Math.ceil(displayH /8.0);
			return disY;
		case ConstPhoto.box_in:
		case ConstPhoto.box_out:
			disY = (int) Math
					.ceil(displayH / 8.0);
			return disY;

		default:
			return 0;
		}
	}
	/**
	 * execute to effect for photo.
	 * 
	 */
	public void run() {

	
		MmpTool.LOG_INFO("(EffectView run()) run is doing");

		removeMessage();
		Message message = new Message();
		
		if (mIsSwitchingEffect) {
			type = mTempType;
		} else {
			mIsSwitchingEffect = true;
		}
	
		if(null != bmpGif && bmpGif.length > 1) {
			InitDefault();
			type = ConstPhoto.DEFAULT;
			message.what = 100;
			MmpTool
					.LOG_INFO("(EffectView run())  send message GIF_ANIM>>>>>>>>>>>>>");
			eHandler.sendMessage(message);
			return;
		}

		MmpTool.LOG_INFO("(EffectView run()) type = "+type);

		
		switch (type) {
		case ConstPhoto.side_to_mid:
			InitSideToMid();
			message.what = ConstPhoto.side_to_mid;
			MmpTool
					.LOG_INFO("(EffectView run())  send message side_to_mid>>>>>>>>>>>>>");
			eHandler.sendMessage(message);
			break;
		case ConstPhoto.mid_to_side:
			InitMidToSide();
			message.what = ConstPhoto.mid_to_side;
			MmpTool
					.LOG_INFO("(EffectView run())  send message mid_to_side>>>>>>>>>>>>>");
			eHandler.sendMessage(message);
			break;
		case ConstPhoto.side_to_mid_out:
			InitSideToMid_Out();
			message.what = ConstPhoto.side_to_mid_out;
			MmpTool
					.LOG_INFO("(EffectView run())  send message side_to_mid>>>>>>>>>>>>>");
			eHandler.sendMessage(message);
			break;
		case ConstPhoto.mid_to_side_out:
			InitMidToSide_Out();
			message.what = ConstPhoto.mid_to_side_out;
			MmpTool
					.LOG_INFO("(EffectView run())  send message mid_to_side>>>>>>>>>>>>>");
			eHandler.sendMessage(message);
			break;
		case ConstPhoto.box_in:
			InitBoxIn();
			message.what = ConstPhoto.box_in;
			MmpTool
					.LOG_INFO("(EffectView run())  send message box_in>>>>>>>>>>>>>");
			eHandler.sendMessage(message);
			break;
		case ConstPhoto.box_out:
			InitBoxOut();
			message.what = ConstPhoto.box_out;
			MmpTool
					.LOG_INFO("(EffectView run())  send message box_out>>>>>>>>>>>>>");
			eHandler.sendMessage(message);
			break;
		case ConstPhoto.wipe_right:
			InitWipeRight();
			message.what = ConstPhoto.wipe_right;
			MmpTool
					.LOG_INFO("(EffectView run())  send message wipe_right>>>>>>>>>>>>>");
			eHandler.sendMessage(message);
			break;
		case ConstPhoto.wipe_left:
			InitWipeLeft();
			message.what = ConstPhoto.wipe_left;
			MmpTool
					.LOG_INFO("(EffectView run())  send message wipe_left>>>>>>>>>>>>>");
			eHandler.sendMessage(message);
			break;
		case ConstPhoto.wipe_top:
			InitWipeTop();
			message.what = ConstPhoto.wipe_top;
			MmpTool
					.LOG_INFO("(EffectView run())  send message wipe_top>>>>>>>>>>>>>");
			eHandler.sendMessage(message);
			break;
		case ConstPhoto.wipe_bottom:
			InitWipeBottom();
			message.what = ConstPhoto.wipe_bottom;
			MmpTool
					.LOG_INFO("(EffectView run())  send message wipe_bottom>>>>>>>>>>>>>");
			eHandler.sendMessage(message);
			break;
		case ConstPhoto.dissolve:
			InitDissolve();
			message.what = ConstPhoto.dissolve;
			MmpTool
					.LOG_INFO("(EffectView run())  send message dissolve>>>>>>>>>>>>>");
			eHandler.sendMessage(message);
			break;
		case ConstPhoto.DEFAULT:
			InitDefault();
			eHandler.sendEmptyMessage(ConstPhoto.DEFAULT);
			break;

		case ConstPhoto.ZOOMOUT:
		case ConstPhoto.ROTATE_R:
		case ConstPhoto.ZOOMIN:
		case ConstPhoto.DRAWBLACK:
		case ConstPhoto.ROTATE_PHOTO:

			onDraw(myCanvas);
			break;

		

		}


		
	}

	MyHandler eHandler;

	class MyHandler extends Handler {

		public MyHandler(){
			super();
		}

		public MyHandler(Looper looper){
			super(looper);



		}

		public void handleMessage(Message msg) {
			switch (msg.what) {
			case ConstPhoto.side_to_mid:
				MmpTool
						.LOG_INFO("(EffectView eHandler()) case side_to_mid~~~~~~~~~~");

				if (left2 < right2) {

					left2 += getDisX();
					right2 -= getDisX();
					onDraw(myCanvas);
					//eHandler.sendEmptyMessageDelayed(ConstPhoto.side_to_mid, 30);
					eHandler.sendEmptyMessage(ConstPhoto.side_to_mid);
				} else {
					eHandler.removeMessages(ConstPhoto.side_to_mid);
					EffectDigreeNum = 0;
					preMultiple = 1;
					if(playLisenter!=null){
						playLisenter.playDone();

					}
				}
				break;
			case ConstPhoto.mid_to_side:
				MmpTool
						.LOG_INFO("(EffectView eHandler()) case mid_to_side~~~~~~~~~~");

				if ((left2 >= 0) || (right2 < displayW)) {
					left2 -= getDisX();
					right2 += getDisX();
					onDraw(myCanvas);
					//eHandler.sendEmptyMessageDelayed(ConstPhoto.mid_to_side,30);
					eHandler.sendEmptyMessage(ConstPhoto.mid_to_side);
				} else {
					eHandler.removeMessages(ConstPhoto.mid_to_side);
					EffectDigreeNum = 0;
					preMultiple = 1;	
					
					if(playLisenter!=null){
						playLisenter.playDone();

					}
				}
				break;
			case ConstPhoto.side_to_mid_out:
				MmpTool
						.LOG_INFO("(EffectView eHandler()) case side_to_mid_out~~~~~~~~~~");

				if (left2 < right2) {

					left2 += getDisX();
					right2 -= getDisX();
					onDraw(myCanvas);
					//eHandler.sendEmptyMessageDelayed(ConstPhoto.side_to_mid_out,30);
					eHandler.sendEmptyMessage(ConstPhoto.side_to_mid_out);
				} else {
					removeMessages(ConstPhoto.side_to_mid_out);
					EffectDigreeNum = 0;
					preMultiple = 1;	
					
					if(playLisenter!=null){
						playLisenter.playDone();

					}
				}
				break;
			case ConstPhoto.mid_to_side_out:
				MmpTool
						.LOG_INFO("(EffectView eHandler()) case mid_to_side_out~~~~~~~~~~");

				if ((left2 > 0) || (right2 < displayW)) {
					left2 -= getDisX();
					right2 += getDisX();
					onDraw(myCanvas);
					//eHandler.sendEmptyMessageDelayed(ConstPhoto.mid_to_side_out,30);
					eHandler.sendEmptyMessage(ConstPhoto.mid_to_side_out);
				} else {
					eHandler.removeMessages(ConstPhoto.mid_to_side_out);
					EffectDigreeNum = 0;
					preMultiple = 1;	
					
					if(playLisenter!=null){
						playLisenter.playDone();

					}
				}
				break;
			case ConstPhoto.box_in:
				MmpTool.LOG_INFO("eHandler() case box_in~~~~~~~~~~");
				if ((left2 < right2) || (top2 < bottom2)) {

					left2 += getDisX();
					top2 += getDisY();
					right2 -= getDisX();
					bottom2 -= getDisY();
					onDraw(myCanvas);
					//eHandler.sendEmptyMessageDelayed(ConstPhoto.box_in,30);
					eHandler.sendEmptyMessage(ConstPhoto.box_in);
				} else {
					eHandler.removeMessages(ConstPhoto.box_in);
					EffectDigreeNum = 0;
					preMultiple = 1;	
					
					if(playLisenter!=null){
						playLisenter.playDone();

					}
					
				}
				break;
			case ConstPhoto.box_out:
				MmpTool.LOG_INFO("eHandler() case box_out~~~~~~~~~~");
				if ((left2 >= 0) || (right2 < displayW)) {
					left2 -= getDisX();
					top2 -= getDisY();
					right2 += getDisX();
					bottom2 += getDisY();
					onDraw(myCanvas);
					//eHandler.sendEmptyMessageDelayed(ConstPhoto.box_out,30);
					eHandler.sendEmptyMessage(ConstPhoto.box_out);
				} else {

					eHandler.removeMessages(ConstPhoto.box_out);
					EffectDigreeNum = 0;
					preMultiple = 1;	
					
					if(playLisenter!=null){
						playLisenter.playDone();

					}
					
				}
				break;
			case ConstPhoto.wipe_right:
				MmpTool.LOG_INFO("case wipe_right~~~~~~~~~~");
				if (right2 <= displayW) {
					right2 += getDisX();
					onDraw(myCanvas);
					//eHandler.sendEmptyMessageDelayed(ConstPhoto.wipe_right,30);
					eHandler.sendEmptyMessage(ConstPhoto.wipe_right);
				} else {
					eHandler.removeMessages(ConstPhoto.wipe_right);					
					EffectDigreeNum = 0;
					preMultiple = 1;		
					
					if(playLisenter!=null){
						playLisenter.playDone();

					}
				}
				break;
			case ConstPhoto.wipe_left:
				MmpTool.LOG_INFO("case wipe_left~~~~~~~~~~");
				if (left2 >= 0) {
					left2 -= getDisX();
					onDraw(myCanvas);
					//eHandler.sendEmptyMessageDelayed(ConstPhoto.wipe_left,30);
					eHandler.sendEmptyMessage(ConstPhoto.wipe_left);
				} else {
					eHandler.removeMessages(ConstPhoto.wipe_left);
					EffectDigreeNum = 0;
					preMultiple = 1;	
					
					if(playLisenter!=null){
						playLisenter.playDone();

					}
					
				}
				break;
			case ConstPhoto.wipe_top:
				MmpTool.LOG_INFO("eHandler() case wipe_top~~~~~~~~~~");
				if (top2 > 0) {
					top2 -= getDisY();
					onDraw(myCanvas);
					//eHandler.sendEmptyMessageDelayed(ConstPhoto.wipe_top,30);
					eHandler.sendEmptyMessage(ConstPhoto.wipe_top);
				} else {

					eHandler.removeMessages(ConstPhoto.wipe_top);
					EffectDigreeNum = 0;
					preMultiple = 1;	
					
					if(playLisenter!=null){
						playLisenter.playDone();

					}
					
				}
				break;
			case ConstPhoto.wipe_bottom:
				MmpTool.LOG_INFO("eHandler() case wipe_bottom~~~~~~~~~~");
				if (bottom2 <= displayH) {
					bottom2 += getDisY();
					onDraw(myCanvas);
					//eHandler.sendEmptyMessageDelayed(ConstPhoto.wipe_bottom,30);
					eHandler.sendEmptyMessage(ConstPhoto.wipe_bottom);
				} else {
					eHandler.removeMessages(ConstPhoto.wipe_bottom);
					EffectDigreeNum = 0;
					preMultiple = 1;	
					
					if(playLisenter!=null){
						playLisenter.playDone();

					}
					
				}
				break;
			case ConstPhoto.dissolve:
				MmpTool.LOG_INFO("eHandler() case dissolve~~~~~~~~~~~~");
				if (frame_index <= total_frame) {
					onDraw(myCanvas);
					eHandler.sendEmptyMessage(ConstPhoto.dissolve);
				} else {

					MmpTool.LOG_INFO("remove \"dissolve\" message");

					eHandler.removeMessages(ConstPhoto.dissolve);
					EffectDigreeNum = 0;
					preMultiple = 1;	
					
					if(playLisenter!=null){
						playLisenter.playDone();

					}
					
				}
				break;
			case ConstPhoto.DEFAULT:
				MmpTool.LOG_INFO("eHandler() case DEFAULT~~~~~~~~~~~");
				onDraw(myCanvas);
				eHandler.removeMessages(ConstPhoto.DEFAULT);
				EffectDigreeNum = 0;
				preMultiple = 1;	
				
				if(playLisenter!=null){
					playLisenter.playDone();
				
				}
				break;
			case 100:
				MmpTool.LOG_INFO("eHandler() case GIF_ANIM~~~~~~~~~~~");
				if( gifIndex < bmpGif.length){
					onDraw(myCanvas);
					
					if(gifIndex < mDelayTime.length){
						eHandler.sendEmptyMessageDelayed(100,mDelayTime[gifIndex]);
					}else{
						eHandler.sendEmptyMessageDelayed(100,10);
					}
					gifIndex++;

				}else{

					MmpTool.LOG_INFO("eHandler() case GIF_ANIM end~~~~~~~~~~~");
					eHandler.removeMessages(100);
					EffectDigreeNum = 0;
					preMultiple = 1;	
					gifIndex = 0;
					
					if(playLisenter!=null){
						playLisenter.playDone();

					}


				}
				
			default:
				break;
			}
		}

	};


	private void playGif(){
		//int 



	}

	
	/**
	 * Get current plays effect value for photo
	 * @return for examples
	 */
	public int getEffectValue() {
		if (randomEffectValue == true) {
			return ConstPhoto.RADNOM;
		}

		return mTempType;
	}
	

	int temp = -1;
	/**
	 * Returns a new pseudo-random int value which is uniformly distributed between 
	 * 0 (inclusively) and the value of n (exclusively).
	 * @return
	 */
	public int getRandomNum() {
		Random random = new Random();
		int a;

		do {


			a = random.nextInt(ConstPhoto.EFFECT_END)
					% (ConstPhoto.EFFECT_END - ConstPhoto.EFFECT_START + 1)
					+ ConstPhoto.EFFECT_START;

		} while (a == temp);

		temp = a;

		MmpTool.LOG_INFO("randomNumber = " + a);

		return a;
	}
	
  
    // This is the final matrix which is computed as the concatentation
    // of the base matrix and the supplementary matrix.
    private final Matrix mDisplayMatrix = new Matrix();
    
    private final Matrix mDisplayPreMatrix = new Matrix();

    // Setup the base matrix so that the image is centered and scaled properly.
    private void getProperBaseMatrix(Bitmap bitmap, Matrix matrix) {
    	
        float viewWidth = displayW;//getWidth();
        float viewHeight = displayH;//getHeight();

        float w = bitmap.getWidth();
        float h = bitmap.getHeight();
        matrix.reset();

    
        float widthScale = Math.min(viewWidth / w, 1.0f);
        float heightScale = Math.min(viewHeight / h, 1.0f);
        float scale = Math.min(widthScale, heightScale);

        float specialScale =w/h;
 
        
        
		if (specialScale >= SPCEIL_FILTER || specialScale <= (float)1 / SPCEIL_FILTER) {

		} else if (scale > 0.0f) {
			matrix.postScale(scale, scale);
		} else {
			scale = 1.0f;
		}
        matrix.postTranslate(
                (viewWidth  - w * scale) / 2F,
                (viewHeight - h * scale) / 2F);
    }
    private int mRotateDg = 0;
    private final float SCALE_RATE = 0.6f;
    private final float SPCEIL_FILTER = 800f;

    private void getProperZoomMatrix(Bitmap bitmap, Matrix matrix, float s) {
        float viewWidth = displayW;//getWidth();
        float viewHeight = displayH;//getHeight();

        float w = bitmap.getWidth();
        float h = bitmap.getHeight();
        matrix.reset();

        float specialphoto = w/h;

		float scale = s;
		
		if ((int)s == 1) {
			float widthScale = Math.min(viewWidth / w, s);
			float heightScale = Math.min(viewHeight / h, s);
			scale = Math.min(widthScale, heightScale);
			if(specialphoto>=SPCEIL_FILTER||specialphoto<= (float)1/SPCEIL_FILTER){
				scale = 1.0f;
			}
		} else if (s > 1.0f){
			scale *= SCALE_RATE;
		}

        if (scale >0.0f){
        	matrix.postScale(scale, scale);
        } else {
        	scale = 1.0f;
        }
        matrix.postTranslate(
                (viewWidth  - w * scale) / 2F,
                (viewHeight - h * scale) / 2F);
    }
    

    
    public int getRotate(){
    	return EffectDigreeNum;
    }
    
    
    public void setRotate(int digree){
    	
    	EffectDigreeNum = digree;
    	
    }
    
   public void  RotatePhoto(Canvas canvas){
	   MmpTool.LOG_INFO("Rotate Photo use Matrix method!");
		getProperZoomMatrix(bmp, mDisplayMatrix, (float)multiple);			
		mDisplayMatrix.postRotate(EffectDigreeNum, displayW/2, displayH/2);	
		canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		//isrotate=true;
    }
   

	//private boolean isrotate;

	public void bitmapRecycle() {
	//	if (!isrotate) {
		recycleBitmap(bmp);
		recycleBitmap(bmpPre);
		recycleBitmap(drawBmp);
		recycleBitmapArray(bmpGif);
	
	//	} 

		
	} 
	
	/**
	 * Remove handler message
	 */
	public void removeMessage() {
		
		eHandler.removeMessages(ConstPhoto.side_to_mid);
		eHandler.removeMessages(ConstPhoto.mid_to_side);
		eHandler.removeMessages(ConstPhoto.side_to_mid_out);
		eHandler.removeMessages(ConstPhoto.mid_to_side_out);
		eHandler.removeMessages(ConstPhoto.dissolve);
		eHandler.removeMessages(ConstPhoto.wipe_right);
		eHandler.removeMessages(ConstPhoto.wipe_left);
		eHandler.removeMessages(ConstPhoto.wipe_top);
		eHandler.removeMessages(ConstPhoto.wipe_bottom);
		eHandler.removeMessages(ConstPhoto.box_in);
		eHandler.removeMessages(ConstPhoto.box_out);
		eHandler.removeMessages(ConstPhoto.side_to_mid);

		
	}

    
}
