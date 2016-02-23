package com.mediatek.ui.nav.commonview;

import java.util.Locale;
import java.util.Timer;
import java.util.TimerTask;

import com.mediatek.ui.R;
import com.mediatek.ui.util.AnimationManager;
import com.mediatek.ui.util.GetCurrentTask;
import com.mediatek.ui.util.ScreenConstant;

import android.R.integer;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.Handler;
import android.os.Message;
import android.os.AsyncTask.Status;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.view.animation.Interpolator;
import android.view.animation.LinearInterpolator;
import android.widget.TextView;

public class SnowTextView extends TextView {

	public final static int DRAW_TEXT = 1;
	private Timer mTimer;
	private TimerTask mTimertask;
	public static boolean mForcedtoGONE = false;
	private ToastInfoView mToastInfo;
	private ShowType mShowType;

	private Paint mPaint = new Paint();
	private Canvas mCanvas;
	private int mPointX = 100;
	private int mPointY = 100;
	private int width, hight;
	private int offset = 19;
	private Context mContext;
	private Bitmap bitmap = null ;
	private ObjectAnimator translation,translationY,visToInvis,invisToInvis;
	
	private Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case DRAW_TEXT:
				SnowTextView.this.invalidate();
				break;
			default:
				break;
			}
		}
	};

	public enum ShowType {
		SPECIAL_CHANNEL_LOCKED, SPECIAL_INPUT_LOCKED, SPECIAL_NO_SIGNAL, SPECIAL_NO_SUPPORT, SPECIAL_PLEASE_SCAN, SPECIAL_RADIO_ONLY, SPECIAL_CHANNEL_SCRAMBLED;
	}

	public SnowTextView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		mContext = context;
		width = context.getApplicationContext()
				.getWallpaperDesiredMinimumHeight();
		if (width <= 0) {
			width = 1280;
		}
		hight = context.getApplicationContext()
				.getWallpaperDesiredMinimumWidth() / 2;
		if (hight <= 0) {
			hight = 720;
		}
		//initView(context);
		mToastInfo = ToastInfoView.getInstance(context);
	}

	public SnowTextView(Context context, AttributeSet attrs) {
		super(context, attrs);
		mContext = context;
		width = ScreenConstant.SCREEN_WIDTH;
		if (width <= 0) {
			width = 1280;
		}
		hight = ScreenConstant.SCREEN_HEIGHT;
		if (hight <= 0) {
			hight = 720;
		}
		//initView(context);
		mToastInfo = ToastInfoView.getInstance(context);
	}

	public SnowTextView(Context context, Handler handler) {
		super(context);
		mContext = context;
		mHandler = handler;
	}

	private void initView(Context context) {
		mTimer = new Timer();
		mPointX = width * 6/ 16;
		mTimertask = new TimerTask() {
			@Override
			public void run() {
				if (SnowTextView.this.getVisibility() == View.VISIBLE) {
					if (mPointY >= hight - hight / 3) {
						offset = -hight/16;
					}
					if (mPointY <= hight / 3) {			
						offset =  hight/16;
					}
					mPointY = mPointY + offset;
					if (mPointX == width * 5 / 16) {
						mPointX = width * 5 / 15 + hight/16;
					} else {
						mPointX = width * 5 / 16;
					}
					//purplearrow start
					if(!AnimationManager.getInstance().getIsAnimation()) {
						mHandler.sendEmptyMessage(DRAW_TEXT);
					}
					//purplearrow end
				}
			}
		};
		mTimer.schedule(mTimertask, 1000, 1000);
	}

	@Override
	protected void onDraw(Canvas canvas) {
		mCanvas = canvas;
		//Bitmap bitmap = null;
		if (canvas != null) {
			//Integer tag = (Integer) getTag();			
			//bitmap = BitmapFactory.decodeResource(getResources(), tag);
//purplearrow start
			if(AnimationManager.getInstance().getIsAnimation()){
				canvas.drawBitmap(bitmap, new Rect(0, 0, bitmap.getWidth(), bitmap.getHeight()), new Rect(0, 20, ScreenConstant.SCREEN_WIDTH, ScreenConstant.SCREEN_HEIGHT), mPaint);

			} else {
				canvas.drawBitmap(bitmap, mPointX, mPointY, mPaint);
			}
//purplearrow end
		}
//		if (bitmap != null) {
//			if (bitmap.isRecycled() == false) {
//				bitmap.recycle();
//				bitmap = null;
//			}
//		}
	}

	public void showSpecialView(ShowType showType) {
		// mSpecial_SnowTextView.setText(SpecialString);
		mPointY = 100;
		int mTag = R.drawable.nav_no_signal_en;
		mShowType = showType;
		// true represent English
		if (getLocaleLanguage()) {
			switch (showType) {
			// channel locked
			case SPECIAL_CHANNEL_LOCKED:
				mTag = R.drawable.nav_channel_locked_en;
				bitmap = BitmapFactory.decodeResource(getResources(), mTag);
				break;
			// input locked
			case SPECIAL_INPUT_LOCKED:
				mTag = R.drawable.nav_input_locked_en;
				bitmap = BitmapFactory.decodeResource(getResources(), mTag);
				break;
			// no signal
			case SPECIAL_NO_SIGNAL:
				mTag = R.drawable.nav_no_signal_en;
				bitmap = BitmapFactory.decodeResource(getResources(), mTag);
				break;
			case SPECIAL_NO_SUPPORT:			
				mTag = R.drawable.no_support;
				bitmap = BitmapFactory.decodeResource(getResources(), mTag);
				break;
			// please scan
			case SPECIAL_PLEASE_SCAN:
				mTag = R.drawable.nav_please_scan_en;
				bitmap = BitmapFactory.decodeResource(getResources(), mTag);
				break;
			// radio program
			case SPECIAL_RADIO_ONLY:
				mTag = R.drawable.nav_audio_program_en;
				bitmap = BitmapFactory.decodeResource(getResources(), mTag);
				break;
			case SPECIAL_CHANNEL_SCRAMBLED:
				mTag = R.drawable.nav_scrambled_en;
				bitmap = BitmapFactory.decodeResource(getResources(), mTag);
				break;
			default:
				break;
			}

		} else {
			switch (showType) {
			// channel locked
			case SPECIAL_CHANNEL_LOCKED:
				mTag = R.drawable.nav_channel_locked_zh;
				bitmap = BitmapFactory.decodeResource(getResources(), mTag);
				break;
			// input locked
			case SPECIAL_INPUT_LOCKED:
				mTag = R.drawable.nav_input_locked_zh;
				bitmap = BitmapFactory.decodeResource(getResources(), mTag);
				break;
			// no signal
			case SPECIAL_NO_SIGNAL:
				mTag = R.drawable.nav_no_signal_zh;
				bitmap = BitmapFactory.decodeResource(getResources(), mTag);
				break;
			// please scan
			case SPECIAL_PLEASE_SCAN:
				mTag = R.drawable.nav_please_scan_zh;
				bitmap = BitmapFactory.decodeResource(getResources(), mTag);
				break;
			// radio program
			case SPECIAL_RADIO_ONLY:
				mTag = R.drawable.nav_audio_program_zh;
				bitmap = BitmapFactory.decodeResource(getResources(), mTag);
				break;
			case SPECIAL_CHANNEL_SCRAMBLED:
				mTag = R.drawable.nav_scrambled_zh;
				bitmap = BitmapFactory.decodeResource(getResources(), mTag);
				break;
			case SPECIAL_NO_SUPPORT:	
				mTag = R.drawable.nav_input_nosupport_zh;
				bitmap = BitmapFactory.decodeResource(getResources(), mTag);
				break;
			default:
				break;
			}
		}
		//setAnimator();
		//setTag(mTag);
	}

	private boolean getLocaleLanguage() {
		boolean mCurrentLan = true;
		Locale mLocale = Locale.getDefault();
		String mLanguage = String.format("%s-%s", mLocale.getLanguage(),
				mLocale.getCountry());
		if ("zh-CN".equals(mLanguage)) {
			mCurrentLan = false;
		} else {
			mCurrentLan = true;
		}
		return mCurrentLan;
	}

	@Override
	public void setVisibility(int visibility) {
		if((visibility == View.GONE && this.getVisibility() != View.GONE) ||
			(visibility == View.VISIBLE && this.getVisibility() != View.VISIBLE)){
		// TODO Auto-generated method stub
		super.setVisibility(visibility);
		if (visibility == View.GONE) {
			mPointY = 100;
			mForcedtoGONE = true;
			if(null != mTimer){
				mTimer.cancel();
			}
			if(null != mTimertask){
				mTimertask.cancel();
			}
			this.setAlpha(1);
			this.setRotationX(0);
			this.setRotationY(0);
			this.setTranslationX(0);
			this.setTranslationY(0);
			this.setScaleX(1);
			this.setScaleY(1);
			invisToInvis.clearAllAnimations();
			visToInvis.clearAllAnimations();
			translation.clearAllAnimations();
			translationY.clearAllAnimations();
			mToastInfo.cancelVGATimer();
		} else if (visibility == View.VISIBLE) {
			initView(mContext);
				setAnimator();
			mForcedtoGONE = false;
			boolean isCurTKMain = GetCurrentTask.getInstance(mContext).isCurActivityTkuiMainActivity();
			if (mShowType == ShowType.SPECIAL_NO_SIGNAL && isCurTKMain) {
				mToastInfo.toastVGAInfo();
			}
		}
		}
	}
	private void setAnimator(){
//purplearrow start
		Interpolator accelerator = new LinearInterpolator();

		translation = ObjectAnimator.ofFloat(this, "translationX", -ScreenConstant.SCREEN_WIDTH/3f, ScreenConstant.SCREEN_WIDTH/3f);
		translationY = ObjectAnimator.ofFloat(this, "translationY", -ScreenConstant.SCREEN_HEIGHT/3f,ScreenConstant.SCREEN_HEIGHT/3f);
		visToInvis = ObjectAnimator.ofFloat(this,
				"rotationY", -60f, 60f);
		invisToInvis = ObjectAnimator.ofFloat(this,
				"rotationX", -70f, 70f);
		translation.setDuration(6000);
		translation.setRepeatMode(ObjectAnimator.REVERSE);
		translation.setRepeatCount(android.animation.ValueAnimator.INFINITE);
		
		
		translationY.setDuration(4500);
		translationY.setRepeatMode(ObjectAnimator.REVERSE);
		translationY.setRepeatCount(android.animation.ValueAnimator.INFINITE);
		
		
		visToInvis.setRepeatMode(ObjectAnimator.REVERSE);
		visToInvis.setRepeatCount(android.animation.ValueAnimator.INFINITE);
		this.setScaleX(0.3f);
		this.setScaleY(0.3f);
		invisToInvis.setRepeatMode(ObjectAnimator.REVERSE);
		invisToInvis.setRepeatCount(android.animation.ValueAnimator.INFINITE);
		invisToInvis.setDuration(8000);
		visToInvis.setDuration(5000);
		invisToInvis.setInterpolator(accelerator);
		visToInvis.setRepeatCount(android.animation.ValueAnimator.INFINITE);

		if(AnimationManager.getInstance().getIsAnimation()){
		AnimatorSet set =new AnimatorSet();
		set.playTogether(invisToInvis, visToInvis,translation,translationY);
		set.start();
		}else{
			this.setAlpha(1);
			this.setRotationX(0);
			this.setRotationY(0);
			this.setTranslationX(0);
			this.setTranslationY(0);
			this.setScaleX(1);
			this.setScaleY(1);
			invisToInvis.clearAllAnimations();
			visToInvis.clearAllAnimations();
			translation.clearAllAnimations();
			translationY.clearAllAnimations();
		}
	}
}
