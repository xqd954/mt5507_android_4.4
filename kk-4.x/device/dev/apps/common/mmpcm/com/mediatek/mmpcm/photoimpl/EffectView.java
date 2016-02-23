package com.mediatek.mmpcm.photoimpl;

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
import android.graphics.drawable.AnimationDrawable;
import android.graphics.drawable.BitmapDrawable;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Display;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.TranslateAnimation;
import android.widget.ImageView;
/**
 * 
 * This class represents picture effect .
 *
 */
public class EffectView extends ImageView implements Runnable {

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
	private int displayW;
	private int displayH;
	
	Bitmap bmp;
	Bitmap bmpPre;
	
	
	Bitmap[] bmpGif;
	
	int[] mDelayTime;

	Paint paint = new Paint();
	DissolveData dissolveData = null;

	int DSLV_WIDTH_UNIT = 160;
	int DSLV_HEIGHT_UNIT = 120;
	int total_frame = 10;
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
	//add by shuming for Fix CR DTV00401969
	private  int EffectDigreeNum = 0;
	private  int preEffectDigreeNum = 0;

	private TranslateAnimation mAnimationTranslate;
	private Animation mAnimationScale;
	
	/**
	 * Simple constructor to use when creating a effect view from code.
	 * 
	 * @param context The Context the view is running in, through which it can
     *        access the current theme, resources, etc.
	 */
	public EffectView(Context context) {
		super(context);

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
	/**
	 * Use the WindowManager interface to create a Display object.
     * Display gives you access to some information about a particular display
     * connected to the device.
	 * @param display
	 */
	public void setWindow(int width,int height) {
		this.displayW = width;
		this.displayH = height;
	}

	int multiple = 1;
	/**
	 * Set a base value to use zoom out or zoom in.
	 * @param i
	 */
	public void setMultiple(int i) {
		multiple = i;
	}
	//add by shuming for fix CR:DTV00415287
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
	//lei add temply.
/*	public void setRorateDg(int dg){
		mRotateDg += dg;
		if (mRotateDg >= 360){
			mRotateDg = 0;
		}
	}*/

	boolean flag = true;
	/*lei add for think scale*/
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
//for some special photo file(when difference between the photo's width and height is much larger,like 1*10000.If scale the bitmap,some specail photo will show wrong)
		float specialphoto = w/h;
		//add by shuming for some specail photo file
		if(specialphoto>=SPCEIL_FILTER||specialphoto<= (float)1/SPCEIL_FILTER){
			//scale = 1.0f;
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
			//scale = getScale(bmp);
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
			//scale = getScale(bmp);	
			bmp_x = (bmpW < displayW) ? (int) Math
					.ceil((displayW - bmpW * scale) / 2.0) : 0;
			bmp_y = (bmpH < displayH) ? (int) Math
					.ceil((displayH - bmpH * scale) / 2.0) : 0;
			MmpTool.LOG_INFO("bmp_x = " + bmp_x);
			MmpTool.LOG_INFO("bmp_y = " + bmp_y);
		}

		if (bmpPreW != 0) {		    
			//scale = getScale(bmpPre);	        
			bmpPre_x = (bmpPreW < displayW) ? (int) Math
					.ceil((displayW - bmpPreW * scale) / 2.0) : 0;
			bmpPre_y = (bmpPreH < displayH) ? (int) Math
					.ceil((displayH - bmpPreH * scale) / 2.0) : 0;
		}
	}
	
	// Modified by Lei for add Scale Matrix when Draw bitmap
	// CR TV00399654
	private void SideToMid(Canvas canvas) {
		MmpTool.LOG_INFO("(EffectView onDraw())    side_to_mid>>>>>>>>>>");
		
		if (left2 >= right2) {
			//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {
			// Modified by Dan for fix bug DTV00375788
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(left, top, right, bottom);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			     canvas.clipRect(left2, top, right2, bottom, Op.REPLACE);
				// canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawColor(Color.BLACK, Mode.CLEAR);
				
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");

				canvas.clipRect(left, top, right, bottom);			
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);

				MmpTool.LOG_INFO("bmp_x = " + bmp_x);
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);				
					//canvas.clipRect(left2, top, right2, bottom, Op.REPLACE);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
					//canvas.clipRect(left2, top, right2, bottom, Op.REPLACE);
				}
				canvas.clipRect(left2, top, right2, bottom, Op.REPLACE);

				// add by shuming fox fix bug
				// paint.setAntiAlias(true);
				mDisplayPreMatrix.postRotate(preEffectDigreeNum, displayW / 2,
						displayH / 2);
				canvas.drawColor(Color.BLACK, Mode.CLEAR);
				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);
			}

		}
	}
	// Modified by Lei for add Scale Matrix when Draw bitmap
	// CR TV00399654
	private void SideToMide_Out(Canvas canvas) {
		
		if (left2 >= right2) {
			// Modified by Dan for fix bug DTV00375788
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
		} else {
			canvas.clipRect(left, top, right, bottom);
			canvas.clipRect(left2, top, right2, bottom, Op.REPLACE);
			//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		}
	}
	// Modified by Lei for add Scale Matrix when Draw bitmap
	// CR TV00399654
	private void MidToSize(Canvas canvas) {
		MmpTool.LOG_INFO("(EffectView onDraw())    mid_to_side>>>>>>>>>>");
		if ((left2 <= 0) || (right2 > displayW)) {
			//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {
			// Modified by Dan for fix bug DTV00375788
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(left2, top, right2, bottom, Op.REPLACE);
				//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			} else {
					MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
					//canvas.clipRect(0, 0, displayW, displayH, Op.REPLACE);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
					/*
					 * canvas.clipRect(bmpPre_x-2, bmpPre_y, bmpPre_x + bmpPreW,
					 * bmpPre_y + bmpPreH + 1, Op.XOR);
					 */
					//canvas.clipRect(bmpPre_x - 2, bmpPre_y, bmpPre_x + bmpPreW,
					//		bmpPre_y + bmpPreH + 1, Op.REPLACE);
				}
				canvas.clipRect(0, 0, displayW, displayH, Op.REPLACE);

				
				// add by shuming for fix bug
				// paint.setAntiAlias(true);
				mDisplayPreMatrix.postRotate(preEffectDigreeNum, displayW / 2,
						displayH / 2);

				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);

			
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top, right2, bottom, Op.REPLACE);
				// canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawColor(Color.BLACK, Mode.CLEAR);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);

				MmpTool.LOG_INFO("bmp_x = " + bmp_x);		
				
			
			}

		}
	}
	// Modified by Lei for add Scale Matrix when Draw bitmap
	// CR TV00399654
	private void MidToSize_Out(Canvas canvas) {
		MmpTool.LOG_INFO("(EffectView onDraw())    mid_to_side_out>>>>>>>>>>");
		if ((left2 <= 0) || (right2 > displayW)) {
			// Modified by Dan for fix bug DTV00375788
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			// canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {
			// Modified by Dan for fix bug DTV00375788
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			canvas.clipRect(left, top, right, bottom);
			canvas.clipRect(left2, top, right2, bottom, Op.DIFFERENCE);
			//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		}
	}
	// Modified by Lei for add Scale Matrix when Draw bitmap
	// CR TV00399654
	@SuppressWarnings("unused")
	private void BoxIn_back(Canvas canvas) {
		MmpTool.LOG_INFO("box_in>>>>>>>>>>");
		if (left2 >= right2 || top2 >= bottom2) {
			//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {
			// Modified by Dan for fix bug DTV00375788
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top2, right2, bottom2, Op.DIFFERENCE);
				//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top2, right2, bottom2, Op.DIFFERENCE);
				//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);

				MmpTool.LOG_INFO("bmp_x = " + bmp_x);
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
					//canvas.clipRect(0, 0, displayW, displayH, Op.XOR);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
				//canvas.clipRect(bmpPre_x-2, bmpPre_y, bmpPre_x + bmpPreW,
				//			bmpPre_y + bmpPreH + 1, Op.XOR);
				}

				canvas.clipRect(0, 0, displayW, displayH, Op.XOR);
				//canvas.drawBitmap(bmpPre, bmpPre_x, bmpPre_y, paint);	
				
				//add by shuming fox fix bug
//				paint.setAntiAlias(true);										
				mDisplayPreMatrix.postRotate(preEffectDigreeNum, displayW/2, displayH/2);
				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);
			}
		}
	}
	// Modified by Lei for add Scale Matrix when Draw bitmap
	// CR TV00399654
	@SuppressWarnings("unused")
	private void BoxOut_back(Canvas canvas) {
		MmpTool.LOG_INFO("box_out>>>>>>>>>>");
		if (left2 <= 0 || right2 >= displayW) {
			//Modified by Lei for add Scale Matrix to Draw bitmap
			//CR TV00399654
			//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {
			// Modified by Dan for fix bug DTV00375788
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top2, right2, bottom2, Op.REPLACE);
				//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top2, right2, bottom2, Op.REPLACE);
				//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);

				MmpTool.LOG_INFO("bmp_x = " + bmp_x);
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
					//canvas.clipRect(0, 0, displayW, displayH, Op.XOR);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
				//canvas.clipRect(bmpPre_x-2, bmpPre_y, bmpPre_x + bmpPreW,
						//	bmpPre_y + bmpPreH + 1, Op.XOR);
				}
				canvas.clipRect(0, 0, displayW, displayH, Op.XOR);
				//canvas.drawBitmap(bmpPre, bmpPre_x, bmpPre_y, paint);
				
				//add by shuming for fix bug
//				paint.setAntiAlias(true);							
				mDisplayPreMatrix.postRotate(preEffectDigreeNum, displayW/2, displayH/2);
				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);
			}
		}
	}
	// ---------add by xudong---------------------------------
	private void BoxIn(Canvas canvas) {
		MmpTool.LOG_INFO("box_in>>>>>>>>>>");
		if (left2 >= right2 || top2 >= bottom2) {
			// canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {
			// Modified by Dan for fix bug DTV00375788
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
				canvas.clipRect(left2, top2, right2, bottom2, Op.REPLACE);
				// canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawColor(Color.BLACK, Mode.CLEAR);
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");

				canvas.clipRect(0, 0, right, bottom);			
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);

				MmpTool.LOG_INFO("bmp_x = " + bmp_x);
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);				
					//canvas.clipRect(left2, top2, right2, bottom2, Op.REPLACE);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
					//canvas.clipRect(left2, top2, right2, bottom2, Op.REPLACE);
				}
				canvas.clipRect(left2, top2, right2, bottom2, Op.REPLACE);

				// add by shuming fox fix bug
				// paint.setAntiAlias(true);
				mDisplayPreMatrix.postRotate(preEffectDigreeNum, displayW / 2,
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
			// Modified by Lei for add Scale Matrix to Draw bitmap
			// CR TV00399654
			// canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
			canvas.clipRect(0, 0, right, bottom);
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {
			// Modified by Dan for fix bug
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top2, right2, bottom2, Op.REPLACE);
				// canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
					//canvas.clipRect(0, 0, displayW, displayH, Op.REPLACE);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
					/*
					 * canvas.clipRect(bmpPre_x-2, bmpPre_y, bmpPre_x + bmpPreW,
					 * bmpPre_y + bmpPreH + 1, Op.XOR);
					 */
					//canvas.clipRect(bmpPre_x - 2, bmpPre_y, bmpPre_x + bmpPreW,
					//		bmpPre_y + bmpPreH + 1, Op.REPLACE);
				}
				canvas.clipRect(0, 0, displayW, displayH, Op.REPLACE);
				// add by shuming for fix bug
				// paint.setAntiAlias(true);
				mDisplayPreMatrix.postRotate(preEffectDigreeNum, displayW / 2,
						displayH / 2);

				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);

				
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top2, right2, bottom2, Op.REPLACE);
				// canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawColor(Color.BLACK, Mode.CLEAR);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);

				MmpTool.LOG_INFO("bmp_x = " + bmp_x);
			}
		}
	}

	// ------------------------------------------
	// Modified by Lei for add Scale Matrix when Draw bitmap
	// CR TV00399654
	private void WipeRight(Canvas canvas) {
		MmpTool.LOG_INFO("wipe_right>>>>>>>>>> right2 =" + right2);

		if (right2 >= displayW) {
			//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {
			// Modified by Dan for fix bug DTV00375788
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left, top, right2, bottom, Op.REPLACE);
				//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left, top, right2, bottom, Op.REPLACE);
				//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);

				MmpTool.LOG_INFO("bmp_x = " + bmp_x +"preMultiple = "+preMultiple +"preEffectDigreeNum = "+preEffectDigreeNum);
				
				//add by shuming for fix CR:DTV00415287
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
					//canvas.clipRect(right2, 0, displayW, displayH, Op.REPLACE);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
					/*
					 * canvas.clipRect(bmpPre_x-2, bmpPre_y, bmpPre_x + bmpPreW,
					 * bmpPre_y + bmpPreH + 1, Op.XOR);
					 */
					//canvas.clipRect(right2, bmpPre_y, bmpPre_x + bmpPreW,
					//		bmpPre_y + bmpPreH + 1, Op.REPLACE);
				}

				canvas.clipRect(right2, 0, displayW, displayH, Op.REPLACE);
				MmpTool.LOG_INFO("-fuck--displayW= "+displayW+"displayH= "+displayH);				
				//add by shuming for fix bug
							
//				paint.setAntiAlias(true);
				mDisplayPreMatrix.postRotate(preEffectDigreeNum, displayW / 2,
							displayH / 2);

				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);
			}
		}
	}
	// Modified by Lei for add Scale Matrix when Draw bitmap
	// CR TV00399654
	private void WipeLeft(Canvas canvas) {
		MmpTool.LOG_INFO("wipe_left>>>>>>>>>>");
		if (left2 <= 0) {
			//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {
			// Modified by Dan for fix bug DTV00375788
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top, right, bottom, Op.REPLACE);
				//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left2, top, right, bottom, Op.REPLACE);
				//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
				MmpTool.LOG_INFO("bmp_x = " + bmp_x +"preMultiple ="+preMultiple +"preEffectDigreeNum = "+preEffectDigreeNum);
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
					//canvas.clipRect(0, 0, left2, displayH, Op.REPLACE);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
					/*
					 * canvas.clipRect(bmpPre_x-2, bmpPre_y, bmpPre_x + bmpPreW,
					 * bmpPre_y + bmpPreH + 1, Op.XOR);
					 */
					//canvas.clipRect(bmpPre_x - 2, bmpPre_y, left2, bmpPre_y
						//	+ bmpPreH + 1, Op.REPLACE);
				}
				//canvas.drawBitmap(bmpPre, bmpPre_x, bmpPre_y, paint);	
				canvas.clipRect(0, 0, left2, displayH, Op.REPLACE);
				//add by shuming for fix bug							
//				paint.setAntiAlias(true);
				mDisplayPreMatrix.postRotate(preEffectDigreeNum, displayW/2, displayH/2);
				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);
			}
		}
	}
	// Modified by Lei for add Scale Matrix when Draw bitmap
	// CR TV00399654
	private void WipeTop(Canvas canvas) {
		MmpTool.LOG_INFO("wipe_top>>>>>>>>>>");
		if (top2 <= 0) {
			//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {
			// Modified by Dan for fix bug DTV00375788
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left, top2, right, bottom, Op.REPLACE);
				//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left, top2, right, bottom, Op.REPLACE);
				//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
				
				MmpTool.LOG_INFO("bmp_x = " + bmp_x +"preMultiple = "+preMultiple +"preEffectDigreeNum = "+preEffectDigreeNum);
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
					//canvas.clipRect(0, 0, displayW,top2, Op.REPLACE);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
					//canvas.clipRect(bmpPre_x - 2, bmpPre_y, bmpPre_x + bmpPreW,
					//		top2/* bmpPre_y + bmpPreH + 1 */, Op.REPLACE);
				}

				canvas.clipRect(0, 0, displayW,top2, Op.REPLACE);
//				if (multiple > 1.0f){
//					getProperZoomMatrix(bmpPre, mDisplayPreMatrix, (float)multiple);
//				}
/*				canvas.clipRect(bmpPre_x, bmpPre_y, bmpPre_x + bmpPreW,
						bmpPre_y + bmpPreH, Op.XOR);*/
				//canvas.drawBitmap(bmpPre, bmpPre_x, bmpPre_y, paint);
				
				//add by shuming for fix bug
//				paint.setAntiAlias(true);
				mDisplayPreMatrix.postRotate(preEffectDigreeNum, displayW/2, displayH/2);
				
				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);
				MmpTool.LOG_INFO("bmpPreW = " + bmpPreW);
			}
		}
	}
	// Modified by Lei for add Scale Matrix when Draw bitmap
	// CR TV00399654
	private void WipeBottom(Canvas canvas) {
		MmpTool.LOG_INFO("wipe_bottom>>>>>>>>>>");
		if (bottom2 >= displayH) {
			//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		} else {
			// Modified by Dan for fix bug DTV00375788
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			if (bmpPre == null) {
				MmpTool.LOG_INFO("1111111~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left, top, right, bottom2, Op.REPLACE);
				//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			} else {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				canvas.clipRect(0, 0, right, bottom);
				canvas.clipRect(left, top, right, bottom2, Op.REPLACE);
				//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);

				MmpTool.LOG_INFO("bmp_x = " + bmp_x +"preMultiple = "+preMultiple +"preEffectDigreeNum = "+preEffectDigreeNum);
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
					//canvas.clipRect(0, bottom2,displayW, displayH, Op.REPLACE);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
					//canvas.clipRect(bmpPre_x - 2, /* bmpPre_y */bottom2, bmpPre_x
					//		+ bmpPreW, bmpPre_y + bmpPreH + 1, Op.REPLACE);
				}

				canvas.clipRect(0, bottom2,displayW, displayH, Op.REPLACE);
				//canvas.drawBitmap(bmpPre, bmpPre_x, bmpPre_y, paint);
				
				//add by shuming for fix bug											
				mDisplayPreMatrix.postRotate(preEffectDigreeNum, displayW/2, displayH/2);
//				paint.setAntiAlias(true);
				canvas.drawBitmap(bmpPre, mDisplayPreMatrix, paint);
				MmpTool.LOG_INFO("bmpPre_x = " + bmpPre_x);
			}
		}
	}
	// Modified by Lei for add Scale Matrix when Draw bitmap
	// CR TV00399654
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

					//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
					canvas.drawBitmap(bmp, mDisplayMatrix, paint);
					
					if (region_width <= 0 || region_height <= 0) {
						continue;
					}
				}
				canvas.restore();
			} else {

				// Modified by Dan for fix bug DTV00375788
				canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
				MmpTool.LOG_INFO("dissolve frame number= " + frame_index);
				//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			}
			frame_index++;
			
		} else {
			if (frame_index < total_frame) {
				MmpTool.LOG_INFO("22222222~~~~~~~~~~~");
				//add by shuming for fix bug
				if (preMultiple > 1.0f) {
					getProperZoomMatrix(bmpPre, mDisplayPreMatrix,
							(float) preMultiple);
				} else {
					getProperBaseMatrix(bmpPre, mDisplayPreMatrix);
				}								
//				paint.setAntiAlias(true);
				mDisplayPreMatrix.postRotate(preEffectDigreeNum, displayW/2, displayH/2);												
				//canvas.drawBitmap(bmpPre, bmpPre_x, bmpPre_y, paint);
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

					// Modified by Dan for fix bug DTV00375788
					canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
					//canvas.drawBitmap(bmp, bmp_x, bmp_y, null);
					canvas.drawBitmap(bmp, mDisplayMatrix, null);

					if (region_width <= 0 || region_height <= 0) {
						continue;
					}

				}

				canvas.restore();

			} else {
				// Modified by Dan for fix bug DTV00375788
				canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
				MmpTool.LOG_INFO("dissolve frame number= " + frame_index);
				//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
				canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			}
			frame_index++;
			
		}
	}
	// Modified by Lei for add Scale Matrix when Draw bitmap
	// CR TV00399654
	private void ZoomIn(Canvas canvas) {
		// Removed by Dan for fix bug DTV00393917
		// type = mTempType;
		MmpTool.LOG_INFO("ZoomIn");
		getProperZoomMatrix(bmp, mDisplayMatrix, (float)multiple);
		canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		
//		canvas.drawBitmap(bmp, new Rect(0, 0, 0 + bmpW, 0 + bmpH), new Rect(
//				(int) Math.ceil((displayW - bmpW * Math.sqrt(multiple)
//						/ multiple) / 2), (int) Math.ceil((displayH - bmpH
//						* Math.sqrt(multiple) / multiple) / 2),
//				(int) (Math.ceil((displayW - bmpW * Math.sqrt(multiple)
//						/ multiple) / 2) + bmpW * Math.sqrt(multiple)
//						/ multiple), (int) (Math.ceil((displayH - bmpH
//						* Math.sqrt(multiple) / multiple) / 2) + bmpH
//						* Math.sqrt(multiple) / multiple)), paint);
	}
	// Modified by Lei for add Scale Matrix when Draw bitmap
	// CR TV00399654
	private void ZoomOut(Canvas canvas) {
		// Removed by Dan for fix bug DTV00393917
		// type = mTempType;
		MmpTool.LOG_INFO("Zoom Out == "+multiple);
		getProperZoomMatrix(bmp, mDisplayMatrix, (float)multiple);
		//add by shuming for fix CR DTV00404406
		mDisplayMatrix.postRotate(EffectDigreeNum, displayW/2, displayH/2);	
		canvas.drawBitmap(bmp, mDisplayMatrix, paint);
//		canvas
//				.drawBitmap(bmp, new Rect(0, 0, 0 + bmpW, 0 + bmpH),
//						new Rect((int) Math.ceil((displayW - bmpW
//								* Math.sqrt(multiple)) / 2),
//								(int) Math.ceil((displayH - bmpH
//										* Math.sqrt(multiple)) / 2),
//								(int) (Math.ceil((displayW - bmpW
//										* Math.sqrt(multiple)) / 2) + bmpW
//										* Math.sqrt(multiple)),
//								(int) (Math.ceil((displayH - bmpH
//										* Math.sqrt(multiple)) / 2) + bmpH
//										* Math.sqrt(multiple))), paint);
	}
	// Modified by Lei for add Scale Matrix when Draw bitmap
	// CR TV00399654
	private void Default(Canvas canvas) {
		MmpTool.LOG_INFO("Default+++++++++++++++++++");

		if(null!= bmpGif&&bmpGif.length>1) {
			this.setVisibility(View.VISIBLE);
		} else if (null != bmp) {
			//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);			
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);			
			this.setImageDrawable(null);
		} else {
			// Modified by Dan for fix bug DTV00375788
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			System.out.println(" Image decode failure! ");
		}
		this.setImageDrawable(null);
	}

	private void DrawBlack(Canvas canvas) {
		// Removed by Dan for fix bug DTV00393917
		// type = mTempType;
		// MmpTool.LOG_INFO("mTempType = " + mTempType);

		// Modified by Dan for fix bug DTV00375788
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
		MmpTool.LOG_INFO("onDraw");
		super.onDraw(canvas);
		//isrotate=false;
		// Modified by Lei for add Scale Matrix when Draw bitmap
		// CR TV00399654
		if (bmp != null) {
			getProperBaseMatrix(bmp, mDisplayMatrix);
		} else if(bmpGif != null && bmpGif[0] != null) {
			getProperBaseMatrix(bmpGif[0], mDisplayMatrix);
		}else{
			// Modified by Dan for fix bug DTV00375788
			canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
			return;
		}
		if (null != bmp) {

			this.setVisibility(View.VISIBLE);
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

		case ConstPhoto.LEFT_TO_RIGHT:
		case ConstPhoto.RIGHT_TO_LEFT:
		case ConstPhoto.TOP_TO_BOTTOM:
		case ConstPhoto.BOTTOM_TO_TOP:
		case ConstPhoto.LEFTTOP_TO_RIGHTDOWN:
		case ConstPhoto.LEFTDOWN_TO_RIGHTTOP:
			super.onDraw(canvas);
			//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			break;

		case ConstPhoto.LEFT_TO_RIGHT_OUT:
		case ConstPhoto.RIGHT_TO_LEFT_OUT:
		case ConstPhoto.TOP_TO_BOTTOM_OUT:
		case ConstPhoto.BOTTOM_TO_TOP_OUT:
		case ConstPhoto.LEFTTOP_TO_RIGHTDOWN_OUT:
		case ConstPhoto.LEFTDOWN_TO_RIGHTTOP_OUT:
			super.onDraw(canvas);
			//canvas.drawBitmap(bmp, bmp_x, bmp_y, paint);
			canvas.drawBitmap(bmp, mDisplayMatrix, paint);
			break;

		case ConstPhoto.ZOOMOUT:
		case ConstPhoto.ROTATE_R:
			ZoomOut(canvas);
			break;
	//add by shuming for Fix CR DTV00401969					
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
	    }
	}

	private static int mTempType;
	private static int type;
	private int right2;
	private int left2;
	private int top2;
	private int bottom2;
	// Added by Dan for fix bug DTV00393917
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
			// mTempType = type;
			// Removed by Dan for fix bug DTV00393917
			// mTempType = getEffectValue();
			// MmpTool.LOG_INFO("mTempType = " + mTempType);
			// Added by Dan for fix bug DTV00393917
			mIsSwitchingEffect = false;
			type = i;
			MmpTool.LOG_INFO("set type = " + type);
			return;
		}

		// Modified by Dan for fix bug DTV00393917
		mIsSwitchingEffect = true;
		mTempType = i;
		MmpTool.LOG_INFO("before get random: type = " + mTempType);

		randomEffectValue = false;
		if (getEffectValue() == ConstPhoto.RADNOM) {
			// Modified by Dan for fix bug DTV00393917
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
			disX = (int) Math.ceil(displayW / (float) ConstPhoto.fragment);
			return disX;

		case ConstPhoto.mid_to_side:
		case ConstPhoto.side_to_mid:
		case ConstPhoto.side_to_mid_out:
		case ConstPhoto.mid_to_side_out:
		case ConstPhoto.box_in:
		case ConstPhoto.box_out:
			disX = (int) Math
					.ceil(displayW / (float) (2 * ConstPhoto.fragment));
			return disX;
		default:
			return 0;
		}
	}

	public int getDisY() {
		switch (type) {
		case ConstPhoto.wipe_top:
		case ConstPhoto.wipe_bottom:
			disY = (int) Math.ceil(displayH / (float) ConstPhoto.fragment);
			return disY;
		case ConstPhoto.box_in:
		case ConstPhoto.box_out:
			disY = (int) Math
					.ceil(displayH / (float) (2 * ConstPhoto.fragment));
			return disY;

		default:
			return 0;
		}
	}
	/**
	 * execute to effect for photo.
	 * for example:
	 * LEFT_TO_RIGHT, RIGHT_TO_LEFT, TOP_TO_BOTTOM
	 * BOTTOM_TO_TOP,LEFTTOP_TO_RIGHTDOWN,LEFTDOWN_TO_RIGHTTOP
	 * LEFT_TO_RIGHT_OUT,RIGHT_TO_LEFT_OUT, TOP_TO_BOTTOM_OUT,
	 * BOTTOM_TO_TOP_OUT,LEFTTOP_TO_RIGHTDOWN_OUT,LEFTDOWN_TO_RIGHTTOP_OUT
	 * 
	 */
	public void run() {
		removeMessage();
		this.setScaleType(ScaleType.CENTER);
		MmpTool.LOG_INFO("(EffectView run()) run is doing");
		
		Message message = new Message();
		
		// Added by Dan for fix bug DTV00393917
		if (mIsSwitchingEffect) {
			type = mTempType;
		} else {
			mIsSwitchingEffect = true;
		}
	
		if(null != bmpGif && bmpGif.length > 1) {
			InitDefault();
			invalidate();
			eHandler.sendEmptyMessage(ConstPhoto.DEFAULT);
			AnimationDrawable mAnimationDrawable = new AnimationDrawable();
			
			for (int i = 0; i < bmpGif.length; i++) {
				MmpTool.LOG_INFO("mAnimationDrawable:"+mAnimationDrawable+"bmpGif:"+bmpGif+"mDelayTime:"+mDelayTime);
				if(mDelayTime!=null){
					mAnimationDrawable.addFrame(new BitmapDrawable(bmpGif[i]),mDelayTime[i]);
				}else{
					mAnimationDrawable.addFrame(new BitmapDrawable(bmpGif[i]),100);
				}
			}
			this.setScaleX(multiple);
			this.setScaleY(multiple);
			this.setImageMatrix(mDisplayMatrix);
			mAnimationDrawable.setOneShot(false);
			this.setImageDrawable(mAnimationDrawable);
			mAnimationDrawable.start();
			type = ConstPhoto.DEFAULT;
		}else{
			this.setImageDrawable(null);
			this.setScaleX(1);
			this.setScaleY(1);
		}
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

		case ConstPhoto.ZOOMOUT:
		case ConstPhoto.ROTATE_R:
		case ConstPhoto.ZOOMIN:
		case ConstPhoto.DRAWBLACK:
//add by shuming for Fix CR DTV00401969			
		case ConstPhoto.ROTATE_PHOTO:
//			type = mTempType;
			invalidate();
			break;

		case ConstPhoto.LEFT_TO_RIGHT:
			// invalidate();
			InitAnimation();
			setEffect(ConstPhoto.LEFT_TO_RIGHT);
			break;
		case ConstPhoto.RIGHT_TO_LEFT:
			// invalidate();
			InitAnimation();
			setEffect(ConstPhoto.RIGHT_TO_LEFT);
			break;
		case ConstPhoto.TOP_TO_BOTTOM:
			// invalidate();
			InitAnimation();
			setEffect(ConstPhoto.TOP_TO_BOTTOM);
			break;
		case ConstPhoto.BOTTOM_TO_TOP:
			// invalidate();
			InitAnimation();
			setEffect(ConstPhoto.BOTTOM_TO_TOP);
			break;
		case ConstPhoto.LEFTTOP_TO_RIGHTDOWN:
			// invalidate();
			InitAnimation();
			setEffect(ConstPhoto.LEFTTOP_TO_RIGHTDOWN);
			break;
		case ConstPhoto.LEFTDOWN_TO_RIGHTTOP:
			// invalidate();
			InitAnimation();
			setEffect(ConstPhoto.LEFTDOWN_TO_RIGHTTOP);
			break;

		case ConstPhoto.LEFT_TO_RIGHT_OUT:
			InitAnimation();
			setEffect(ConstPhoto.LEFT_TO_RIGHT_OUT);
			break;
		case ConstPhoto.RIGHT_TO_LEFT_OUT:
			InitAnimation();
			setEffect(ConstPhoto.RIGHT_TO_LEFT_OUT);
			break;
		case ConstPhoto.TOP_TO_BOTTOM_OUT:
			InitAnimation();
			setEffect(ConstPhoto.TOP_TO_BOTTOM_OUT);
			break;
		case ConstPhoto.BOTTOM_TO_TOP_OUT:
			InitAnimation();
			setEffect(ConstPhoto.BOTTOM_TO_TOP_OUT);
			break;
		case ConstPhoto.LEFTTOP_TO_RIGHTDOWN_OUT:
			InitAnimation();
			setEffect(ConstPhoto.LEFTTOP_TO_RIGHTDOWN_OUT);
			break;
		case ConstPhoto.LEFTDOWN_TO_RIGHTTOP_OUT:
			InitAnimation();
			setEffect(ConstPhoto.LEFTDOWN_TO_RIGHTTOP_OUT);
			break;

		case ConstPhoto.DEFAULT:
			InitDefault();
			invalidate();
			eHandler.sendEmptyMessage(ConstPhoto.DEFAULT);
			break;

		}
	}

	Handler eHandler = new Handler() {

		public void handleMessage(Message msg) {
			switch (msg.what) {
			case ConstPhoto.side_to_mid:
				MmpTool
						.LOG_INFO("(EffectView eHandler()) case side_to_mid~~~~~~~~~~");

				if (left2 < right2) {

					left2 += getDisX();
					right2 -= getDisX();
					invalidate();
					eHandler.sendEmptyMessageDelayed(ConstPhoto.side_to_mid, 30);
				} else {
					eHandler.removeMessages(ConstPhoto.side_to_mid);
					preEffectDigreeNum = 0;
					preMultiple = 1;	
				}
				break;
			case ConstPhoto.mid_to_side:
				MmpTool
						.LOG_INFO("(EffectView eHandler()) case mid_to_side~~~~~~~~~~");

				if ((left2 >= 0) || (right2 < displayW)) {
					left2 -= getDisX();
					right2 += getDisX();
					invalidate();
					eHandler.sendEmptyMessageDelayed(ConstPhoto.mid_to_side,30);
				} else {
					eHandler.removeMessages(ConstPhoto.mid_to_side);
					preEffectDigreeNum = 0;
					preMultiple = 1;	
				}
				break;
			case ConstPhoto.side_to_mid_out:
				MmpTool
						.LOG_INFO("(EffectView eHandler()) case side_to_mid_out~~~~~~~~~~");

				if (left2 < right2) {

					left2 += getDisX();
					right2 -= getDisX();
					invalidate();
					eHandler.sendEmptyMessageDelayed(ConstPhoto.side_to_mid_out,30);
				} else {
					removeMessages(ConstPhoto.side_to_mid_out);
					preEffectDigreeNum = 0;
					preMultiple = 1;	
				}
				break;
			case ConstPhoto.mid_to_side_out:
				MmpTool
						.LOG_INFO("(EffectView eHandler()) case mid_to_side_out~~~~~~~~~~");

				if ((left2 > 0) || (right2 < displayW)) {
					left2 -= getDisX();
					right2 += getDisX();
					invalidate();
					eHandler.sendEmptyMessageDelayed(ConstPhoto.mid_to_side_out,30);
				} else {
					eHandler.removeMessages(ConstPhoto.mid_to_side_out);
					preEffectDigreeNum = 0;
					preMultiple = 1;	
				}
				break;
			case ConstPhoto.box_in:
				MmpTool.LOG_INFO("eHandler() case box_in~~~~~~~~~~");
				if ((left2 < right2) || (top2 < bottom2)) {

					left2 += getDisX();
					top2 += getDisY();
					right2 -= getDisX();
					bottom2 -= getDisY();
					invalidate();
					eHandler.sendEmptyMessageDelayed(ConstPhoto.box_in,30);
				} else {
					eHandler.removeMessages(ConstPhoto.box_in);
					preEffectDigreeNum = 0;
					preMultiple = 1;	
					
				}
				break;
			case ConstPhoto.box_out:
				MmpTool.LOG_INFO("eHandler() case box_out~~~~~~~~~~");
				if ((left2 >= 0) || (right2 < displayW)) {
					left2 -= getDisX();
					top2 -= getDisY();
					right2 += getDisX();
					bottom2 += getDisY();
					invalidate();
					eHandler.sendEmptyMessageDelayed(ConstPhoto.box_out,30);
				} else {

					eHandler.removeMessages(ConstPhoto.box_out);
					preEffectDigreeNum = 0;
					preMultiple = 1;	
					
				}
				break;
			case ConstPhoto.wipe_right:
				MmpTool.LOG_INFO("case wipe_right~~~~~~~~~~");
				if (right2 <= displayW) {
					right2 += getDisX();
					invalidate();
					eHandler.sendEmptyMessageDelayed(ConstPhoto.wipe_right,30);
				} else {
					eHandler.removeMessages(ConstPhoto.wipe_right);					
					preEffectDigreeNum = 0;
					preMultiple = 1;					
				}
				break;
			case ConstPhoto.wipe_left:
				MmpTool.LOG_INFO("case wipe_left~~~~~~~~~~");
				if (left2 >= 0) {
					left2 -= getDisX();
					invalidate();
					eHandler.sendEmptyMessageDelayed(ConstPhoto.wipe_left,30);
				} else {
					eHandler.removeMessages(ConstPhoto.wipe_left);
					preEffectDigreeNum = 0;
					preMultiple = 1;	
					
				}
				break;
			case ConstPhoto.wipe_top:
				MmpTool.LOG_INFO("eHandler() case wipe_top~~~~~~~~~~");
				if (top2 > 0) {
					top2 -= getDisY();
					invalidate();
					eHandler.sendEmptyMessageDelayed(ConstPhoto.wipe_top,30);
				} else {

					eHandler.removeMessages(ConstPhoto.wipe_top);
					preEffectDigreeNum = 0;
					preMultiple = 1;	
					
				}
				break;
			case ConstPhoto.wipe_bottom:
				MmpTool.LOG_INFO("eHandler() case wipe_bottom~~~~~~~~~~");
				if (bottom2 <= displayH) {
					bottom2 += getDisY();
					invalidate();
					eHandler.sendEmptyMessageDelayed(ConstPhoto.wipe_bottom,30);
				} else {
					eHandler.removeMessages(ConstPhoto.wipe_bottom);
					preEffectDigreeNum = 0;
					preMultiple = 1;	
					
				}
				break;
			case ConstPhoto.dissolve:
				if (frame_index <= total_frame) {
					invalidate();
					eHandler.sendEmptyMessage(ConstPhoto.dissolve);
				} else {

					MmpTool.LOG_INFO("remove \"dissolve\" message");

					eHandler.removeMessages(ConstPhoto.dissolve);
					preEffectDigreeNum = 0;
					preMultiple = 1;	
					
				}
				break;
			case ConstPhoto.DEFAULT:
				preEffectDigreeNum = 0;
				preMultiple = 1;	
				break;
			default:
				break;
			}
		}

	};
	/**
	 * Get current plays effect value for photo
	 * @return for example:
	 * LEFT_TO_RIGHT, RIGHT_TO_LEFT, TOP_TO_BOTTOM
	 * BOTTOM_TO_TOP,LEFTTOP_TO_RIGHTDOWN,LEFTDOWN_TO_RIGHTTOP
	 * LEFT_TO_RIGHT_OUT,RIGHT_TO_LEFT_OUT, TOP_TO_BOTTOM_OUT,
	 * BOTTOM_TO_TOP_OUT,LEFTTOP_TO_RIGHTDOWN_OUT,LEFTDOWN_TO_RIGHTTOP_OUT
	 */
	public int getEffectValue() {
		if (randomEffectValue == true) {
			return ConstPhoto.RADNOM;
		}

		// Modified by Dan for fix bug DTV00393917
		return mTempType;
	}
	/**
	 * Set plays effect type for photo
	 * @param for example:
	 * LEFT_TO_RIGHT, RIGHT_TO_LEFT, TOP_TO_BOTTOM
	 * BOTTOM_TO_TOP,LEFTTOP_TO_RIGHTDOWN,LEFTDOWN_TO_RIGHTTOP
	 * LEFT_TO_RIGHT_OUT,RIGHT_TO_LEFT_OUT, TOP_TO_BOTTOM_OUT,
	 * BOTTOM_TO_TOP_OUT,LEFTTOP_TO_RIGHTDOWN_OUT,LEFTDOWN_TO_RIGHTTOP_OUT
	 */
	public void setEffect(int type) {
		switch (type) {
		case ConstPhoto.LEFT_TO_RIGHT:
			MmpTool.LOG_INFO("effect now\\\\");
			mAnimationTranslate = new TranslateAnimation(-displayW, 0, 0, 0);
			mAnimationTranslate.setDuration(1000);
			mAnimationScale = (Animation) mAnimationTranslate;
			this.startAnimation(mAnimationScale);
			break;
		case ConstPhoto.RIGHT_TO_LEFT:
			mAnimationTranslate = new TranslateAnimation(displayW, 0, 0, 0);
			mAnimationTranslate.setDuration(1000);
			mAnimationScale = (Animation) mAnimationTranslate;
			this.startAnimation(mAnimationScale);
			break;
		case ConstPhoto.TOP_TO_BOTTOM:
			mAnimationTranslate = new TranslateAnimation(0, 0, -displayH, 0);
			mAnimationTranslate.setDuration(1000);
			mAnimationScale = (Animation) mAnimationTranslate;
			this.startAnimation(mAnimationScale);
			break;
		case ConstPhoto.BOTTOM_TO_TOP:
			mAnimationTranslate = new TranslateAnimation(0, 0, displayH, 0);
			mAnimationTranslate.setDuration(1000);
			mAnimationScale = (Animation) mAnimationTranslate;
			this.startAnimation(mAnimationScale);
			break;
		case ConstPhoto.LEFTTOP_TO_RIGHTDOWN:
			mAnimationTranslate = new TranslateAnimation(-displayW, 0,
					-displayH, 0);
			mAnimationTranslate.setDuration(1000);
			mAnimationScale = (Animation) mAnimationTranslate;
			this.startAnimation(mAnimationScale);
			break;
		case ConstPhoto.LEFTDOWN_TO_RIGHTTOP:
			mAnimationTranslate = new TranslateAnimation(-displayW, 0,
					displayH, 0);
			mAnimationTranslate.setDuration(1000);

			mAnimationScale = (Animation) mAnimationTranslate;
			this.startAnimation(mAnimationScale);
			break;

		case ConstPhoto.LEFT_TO_RIGHT_OUT:
			mAnimationTranslate = new TranslateAnimation(0, displayW, 0, 0);
			mAnimationTranslate.setDuration(1000);
			mAnimationTranslate.setFillAfter(true);
			mAnimationScale = (Animation) mAnimationTranslate;
			this.startAnimation(mAnimationScale);
			break;
		case ConstPhoto.RIGHT_TO_LEFT_OUT:
			mAnimationTranslate = new TranslateAnimation(0, -displayW, 0, 0);
			mAnimationTranslate.setDuration(1000);
			mAnimationTranslate.setFillAfter(true);
			mAnimationScale = (Animation) mAnimationTranslate;
			this.startAnimation(mAnimationScale);
			break;
		case ConstPhoto.TOP_TO_BOTTOM_OUT:
			mAnimationTranslate = new TranslateAnimation(0, 0, 0, displayH);
			mAnimationTranslate.setDuration(1000);
			mAnimationTranslate.setFillAfter(true);
			mAnimationScale = (Animation) mAnimationTranslate;
			this.startAnimation(mAnimationScale);
			break;
		case ConstPhoto.BOTTOM_TO_TOP_OUT:
			mAnimationTranslate = new TranslateAnimation(0, 0, 0, -displayH);
			mAnimationTranslate.setDuration(1000);
			mAnimationTranslate.setFillAfter(true);
			mAnimationScale = (Animation) mAnimationTranslate;
			this.startAnimation(mAnimationScale);
			break;
		case ConstPhoto.LEFTTOP_TO_RIGHTDOWN_OUT:
			mAnimationTranslate = new TranslateAnimation(0, displayW, 0,
					displayH);
			mAnimationTranslate.setDuration(1000);
			mAnimationTranslate.setFillAfter(true);
			mAnimationScale = (Animation) mAnimationTranslate;
			this.startAnimation(mAnimationScale);
			break;
		case ConstPhoto.LEFTDOWN_TO_RIGHTTOP_OUT:
			mAnimationTranslate = new TranslateAnimation(0, displayW, 0,
					-displayH);
			mAnimationTranslate.setDuration(1000);
			mAnimationTranslate.setFillAfter(true);
			mAnimationScale = (Animation) mAnimationTranslate;
			this.startAnimation(mAnimationScale);
			break;

		default:
			break;
		}
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

			//If produce the same number of continue to cycle 

			a = random.nextInt(ConstPhoto.EFFECT_END)
					% (ConstPhoto.EFFECT_END - ConstPhoto.EFFECT_START + 1)
					+ ConstPhoto.EFFECT_START;

		} while (a == temp);

		temp = a;

		MmpTool.LOG_INFO("randomNumber = " + a);

		return a;
	}
	
    // This is the base transformation which is used to show the image
    // initially.  The current computation for this shows the image in
    // it's entirety, letterboxing as needed.  One could choose to
    // show the image as cropped instead.
    //
    // This matrix is recomputed when we go from the thumbnail image to
    // the full size image.
    protected Matrix mBaseMatrix = new Matrix();
    // This is the final matrix which is computed as the concatentation
    // of the base matrix and the supplementary matrix.
    private final Matrix mDisplayMatrix = new Matrix();
    
    private final Matrix mDisplayPreMatrix = new Matrix();
    // This is the supplementary transformation which reflects what
    // the user has done in terms of zooming and panning.
    //
    // This matrix remains the same when we go from the thumbnail image
    // to the full size image.
    protected Matrix mSuppMatrix = new Matrix();
	//Modified by Lei for add Scale Matrix to Draw bitmap
	//CR TV00399654
    // Setup the base matrix so that the image is centered and scaled properly.
    private void getProperBaseMatrix(Bitmap bitmap, Matrix matrix) {
    	
        float viewWidth = displayW;//getWidth();
        float viewHeight = displayH;//getHeight();

        float w = bitmap.getWidth();
        float h = bitmap.getHeight();
        matrix.reset();

        // We limit up-scaling to 3x otherwise the result may look bad if it's
        // a small icon.
    
        float widthScale = Math.min(viewWidth / w, 1.0f);
        float heightScale = Math.min(viewHeight / h, 1.0f);
        float scale = Math.min(widthScale, heightScale);

        //add by shuming for some specail photofile.
        float specialScale =w/h;
        //matrix.postConcat(bitmap.getRotateMatrix());
        
        
		if (specialScale >= SPCEIL_FILTER || specialScale <= (float)1 / SPCEIL_FILTER) {
			//don't scale, but translate.
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
	//Modified by Lei for add Scale Matrix to Draw bitmap
	//CR TV00399654
    // Setup the base matrix so that the image is centered and scaled properly.
    private void getProperZoomMatrix(Bitmap bitmap, Matrix matrix, float s) {
        float viewWidth = displayW;//getWidth();
        float viewHeight = displayH;//getHeight();

        float w = bitmap.getWidth();
        float h = bitmap.getHeight();
        matrix.reset();
        //add by shuming for some specail photo file
        float specialphoto = w/h;

        // We limit up-scaling to 3x otherwise the result may look bad if it's
        // a small icon.
		float scale = s;
		
		if ((int)s == 1) {
			float widthScale = Math.min(viewWidth / w, s);
			float heightScale = Math.min(viewHeight / h, s);
			scale = Math.min(widthScale, heightScale);
			//add by shuming for some specail photo file
			if(specialphoto>=SPCEIL_FILTER||specialphoto<= (float)1/SPCEIL_FILTER){
				scale = 1.0f;
			}
		} else if (s > 1.0f){
			scale *= SCALE_RATE;
		}

        //matrix.postConcat(getRotateMatrix(scale));
        
        if (scale >0.0f){
        	matrix.postScale(scale, scale);
        } else {
        	scale = 1.0f;
        }
        matrix.postTranslate(
                (viewWidth  - w * scale) / 2F,
                (viewHeight - h * scale) / 2F);
    }
    
/*    private Matrix getRotateMatrix(float s) {
        // By default this is an identity matrix.
        Matrix matrix = new Matrix();
        if (mRotateDg != 0) {
            // We want to do the rotation at origin, but since the bounding
            // rectangle will be changed after rotation, so the delta values
            // are based on old & new width/height respectively.
            int w = bmp.getWidth();
            int h = bmp.getHeight();
            matrix.preTranslate(-cx, -cy);
            matrix.postRotate(mRotateDg);
            matrix.postTranslate(
                    (displayW  - h * s) / 2F,
                    (displayW - w * s) / 2F);
        }
        return matrix;
    }*/
    // Combine the base matrix and the supp matrix to make the final matrix.
/*    protected Matrix getImageViewMatrix() {
        // The final matrix is computed as the concatentation of the base matrix
        // and the supplementary matrix.
        mDisplayMatrix.set(mBaseMatrix);
        mDisplayMatrix.postConcat(mSuppMatrix);
        return mDisplayMatrix;
    }*/
    
  //add by shuming for Fix CR DTV00401969
    
    public int getRotate(){
    	return EffectDigreeNum;
    }
    
    
    public void setRotate(int digree){
    	MmpTool.LOG_INFO("setRotateGigree digree = "+digree);
    	EffectDigreeNum = digree;
    	
    }


	public int getPreRotate(){
    	return preEffectDigreeNum;
    }
    
    
    public void setPreRotate(int digree){
    	MmpTool.LOG_INFO("setPreRotateGigree digree = "+digree);
    	preEffectDigreeNum = digree;
    	
    }
    
   public void  RotatePhoto(Canvas canvas){
	   MmpTool.LOG_INFO("Rotate Photo use Matrix method!");
		getProperZoomMatrix(bmp, mDisplayMatrix, (float)multiple);			
		mDisplayMatrix.postRotate(EffectDigreeNum, displayW/2, displayH/2);	
//		paint.setAntiAlias(true);
		canvas.drawBitmap(bmp, mDisplayMatrix, paint);
		//isrotate=true;
    }
   

	//private boolean isrotate;

	public void bitmapRecycle() {
		//if (!isrotate) {
			
			recycleBitmap(bmp);
			recycleBitmap(bmpPre);
			recycleBitmapArray(bmpGif);
		//}
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
//		this.clearAnimation();
//		this.onDisplayHint();
		
	}
	//end
    
}
