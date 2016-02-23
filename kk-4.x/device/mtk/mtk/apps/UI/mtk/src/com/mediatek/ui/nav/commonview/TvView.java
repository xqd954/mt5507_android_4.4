package com.mediatek.ui.nav.commonview;

import com.mediatek.ui.R;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;
import java.lang.reflect.Field;
import android.content.Context;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;
import android.widget.ImageView;
import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import android.content.Intent;
import android.graphics.Rect;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.os.Handler;
import android.os.Message;

public class TvView extends ImageView {
	static final String TAG = "TvView";

	private int mScreenWidth;
	private int mScreenHeight;
	private LayoutParams wmParams;
	private WindowManager windowManager;
	private static float viewX,viewY,viewWidth,viewHeight;
	private static final String PhoneBarChange = "com.mediatek.phonestatus.bar.change";
	private boolean statusBarVisible = false;
	private Context mContext;
	private int lastScreenTypeValue = 0;
	private int messagelastScreenValue = 0;
	private static final int CHANGE_TV_VIEW_POSITION = 11;
	
	private Handler mHandler = new Handler(){
		public void handleMessage(Message msg){
			switch(msg.what){
			case CHANGE_TV_VIEW_POSITION:
				setTvViewPosition(viewX,viewY,viewWidth,viewHeight);
				break;
			default:
				break;
			}
		};
	};
	
	private Runnable mRunnable = new Runnable(){
		public void run(){
			Rect rect = new Rect();
			TvView.this.getWindowVisibleDisplayFrame(rect);
			if(rect.top != lastScreenTypeValue){
				if(rect.top == 0){
					statusBarVisible = false;
				}else{
					statusBarVisible = true;
				}
				lastScreenTypeValue = rect.top;
				mHandler.removeMessages(CHANGE_TV_VIEW_POSITION);
				mHandler.sendEmptyMessage(CHANGE_TV_VIEW_POSITION);
			}
//			int messageFullScreenValue = 0;
//			try{
//				messageFullScreenValue = Settings.System.getInt(mContext.getContentResolver(),"fullScreenOrNot");
//			} catch(SettingNotFoundException e){
//				e.printStackTrace();
//			}
//			if(messageFullScreenValue == 0 && lastScreenTypeValue != 0){
//				statusBarVisible = true;
//				mHandler.removeMessages(CHANGE_TV_VIEW_POSITION);
//				mHandler.sendEmptyMessage(CHANGE_TV_VIEW_POSITION);
//				lastScreenTypeValue = messageFullScreenValue;
//			}else if(messageFullScreenValue == 1 && lastScreenTypeValue != 1){
//				statusBarVisible = false;
//				mHandler.removeMessages(CHANGE_TV_VIEW_POSITION);
//				mHandler.sendEmptyMessage(CHANGE_TV_VIEW_POSITION);
//				lastScreenTypeValue = messageFullScreenValue;
//			}
			
			mHandler.postDelayed(this,200);
		}
	};
	
	/*BroadcastReceiver mBroadcastRecevier = new BroadcastReceiver(){
		public void onReceive(Context context, Intent intent){
			if(intent.getAction() == PhoneBarChange){
				if(intent.getBooleanExtra("barvisible",true)){
					statusBarVisible = true;
				}else {
					statusBarVisible = false;
				}
			}
			setTvViewPosition(viewX,viewY,viewWidth,viewHeight);
		}
	};*/

	public TvView(Context context) {
		super(context);
		mContext = context;
		windowManager = (WindowManager) context.getApplicationContext()
				.getSystemService(context.WINDOW_SERVICE);
		mScreenWidth = ScreenConstant.SCREEN_WIDTH;
		mScreenHeight = ScreenConstant.SCREEN_HEIGHT;
		wmParams = new LayoutParams();
		wmParams.type = LayoutParams.TYPE_PHONE;
		wmParams.flags |= LayoutParams.FLAG_NOT_FOCUSABLE;
		wmParams.gravity = Gravity.LEFT | Gravity.TOP;
		wmParams.format = android.graphics.PixelFormat.PIXEL_VIDEO_HOLE;
		windowManager.addView(this, wmParams);
		setBackgroundResource(R.drawable.translucent_background);
		
	}

	public void setTvViewPosition(float x, float y, float width, float height) {
		viewX = x;
		viewY = y;
		viewWidth = width;
		viewHeight = height;
		wmParams.width = (int) (width * mScreenWidth - x * mScreenWidth);
		wmParams.height = (int) (height * mScreenHeight - y * mScreenHeight);
		wmParams.x = (int) (x * mScreenWidth);
		//if (mScreenWidth == 1280 && mScreenHeight == 720) {
			//wmParams.y = (int) (y * mScreenHeight);
		//} else {
		if(statusBarVisible){
			wmParams.y = (int) (y * mScreenHeight) - getSystemBarheight();
		}else{
			wmParams.y = (int) (y * mScreenHeight);
		}
		//}
		MtkLog.i("OSD", "~~~~~~~~mScrrenWidth:" + mScreenWidth
				+ "~~mScreenHeight:" + mScreenHeight);
		MtkLog.i("OSD", "~~wmParams.width: " + wmParams.width
				+ "~~wmParams.height: " + wmParams.height + "~~wmParams.x: "
				+ wmParams.x + "~~wmParams.y:" + wmParams.y + "~~");
		windowManager.updateViewLayout(this, wmParams);
	}

	public void show(Context context) {
		//IntentFilter filter = new IntentFilter(PhoneBarChange);
		//mContext.registerReceiver(mBroadcastRecevier,filter);
		//}
		setVisibility(View.VISIBLE);	
	}
	
	public void startCheckPosition(){
		Rect mRect = new Rect();
		TvView.this.getWindowVisibleDisplayFrame(mRect);
		lastScreenTypeValue = mRect.top;
//		try{
//			lastScreenTypeValue = Settings.System.getInt(mContext.getContentResolver(),"fullScreenOrNot");
//		} catch(SettingNotFoundException e){
//			e.printStackTrace();
//		}
		mHandler.post(mRunnable);
	}

	public void hide() {
		/*try{
			mContext.unregisterReceiver(mBroadcastRecevier);
		}
		catch(Exception e){
		
		}*/
		setVisibility(View.INVISIBLE);
		mHandler.removeCallbacks(mRunnable);
	}
	
	public int getSystemBarheight() {
		Class<?> c = null;

		Object obj = null;

		Field field = null;

		int x = 0, sbar = 0;

		try {

			c = Class.forName("com.android.internal.R$dimen");

			obj = c.newInstance();

			field = c.getField("status_bar_height");

			x = Integer.parseInt(field.get(obj).toString());

			sbar = getResources().getDimensionPixelSize(x);
			return sbar;

		} catch (Exception e1) {

			MtkLog.e("tag", "get status bar height fail");

			e1.printStackTrace();
			return 0;

		}

	}

}
