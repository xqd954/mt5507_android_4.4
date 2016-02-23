/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.ui;

import android.app.Activity;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Gravity;
import android.widget.LinearLayout.LayoutParams;
import android.widget.PopupWindow;

import com.mediatek.timeshift_pvr.controller.StateFileList;
import com.mediatek.timeshift_pvr.controller.StatePVR;
import com.mediatek.timeshift_pvr.controller.StateTimeShift;
import com.mediatek.timeshift_pvr.manager.Core;
import com.mediatek.timeshift_pvr.manager.PvrConstant;
import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.timeshift_pvr.manager.Util;

import java.lang.ref.WeakReference;
import java.util.Timer;
import java.util.TimerTask;

/**
 *
 */
public class BaseInfoBar extends PopupWindow {

	public Long mDefaultDuration = 5L * 1000;
	public Long mRefreshDuration = 1L * 1000;

	public Activity mContext;
	private static BaseInfoBar mSelf;
	private MyHandler mHandler;

	private final int mMsgWhat = 0;

	private Timer timer;
	private BaseInfoBar currentBaseInfoBar;
	
	private final String CLSNAME=this.getClass().getSimpleName();

	static class MyHandler extends Handler {
		WeakReference<Activity> mActivity;

		MyHandler(Activity activity) {
			
			mActivity = new WeakReference<Activity>(activity);
		}

		public void handleMessage(Message msg) {
			switch (msg.what) {
			default:
//				mSelf.doSomething();
				break;
			}
			super.handleMessage(msg);
		}

	}

	public BaseInfoBar(Activity context, int layoutID) {
		super(context.getLayoutInflater().inflate(layoutID, null),
				LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
		Util.Hlog("--------BaseInfoBar------1----");
		initView();
		mSelf = this;
		this.mHandler = new MyHandler(context);
		this.mContext = context;
	}

	/**
	 * @param context
	 * @param layoutID
	 * @param duration
	 * @param i
	 * @param j
	 */
	public BaseInfoBar(Activity context, int layoutID, Long duration,
			int width, int height) {
		super(context.getLayoutInflater().inflate(layoutID, null), width,
				height);
		Util.Hlog("--------BaseInfoBar------2----");
		initView();

		this.mContext = context;
		setDuration(Long.valueOf(duration));

		mSelf = this;
		this.mHandler = new MyHandler(context);
	}

	/**
	 * @param duration
	 */
	public void setDuration(Long duration) {
		this.mDefaultDuration = duration;
	}

	public void show() {

        if (isShowing()) {
            timer.cancel();
        } else {
            setLocation();
            initView();
        }
        Util.Hlog("currentBaseInfoBar:---------");
        startTimeTask(null);
    }
	
	public void show(BaseInfoBar bBar) {
	    currentBaseInfoBar = bBar;
		if (isShowing()) {
			timer.cancel();
		} else {
			setLocation();
			initView();
		}
		Util.Hlog("currentBaseInfoBar:"+currentBaseInfoBar);
		startTimeTask(bBar);
		
	}

	/**
	 * 
	 */
	protected void setLocation() {
		showAtLocation(mContext.findViewById(Core.ROOT_VIEW),
				Gravity.CENTER, 20, 20);
	}

	/**
	 * Init all views.
	 * <p>
	 * use <code> getContentView().findViewById(R.id.xxx); </code>
	 */
	public void initView() {
		// TODO Auto-generated method stub

	}

	public void doSomething() {
		// TODO Auto-generated method stub

	}

	@Override
	public void dismiss() {
		stopTimerTask();
		super.dismiss();
	}

	private void startTimeTask(final BaseInfoBar bBar) {
	    Util.Hlog("startTimeTask:"+bBar);
		timer = new Timer();
		
		TimerTask syncTask = new TimerTask() {
			public void run() {
				mHandler.sendEmptyMessage(mMsgWhat);
			}
		};
		
		TimerTask clearTask = new TimerTask() {
			public void run() {
				Util.showDLog("BaseInfoBar.this.dismiss() start...."+bBar);
				timer.cancel();
//				bBar.dismiss();
                boolean attentionShow = TimeShiftManager.getInstance().getUiManager().showing;
                if (attentionShow) {
                    if (TimeShiftManager.getInstance() != null) {
                        TimeShiftManager.getInstance().getTopHandler()
                                .sendEmptyMessage(PvrConstant.Dissmiss_Info_Bar);
                    }
                }else {
                    if (StateTimeShift.getInstance() != null&&StateTimeShift.getInstance().getStateTimeShiftHandler()!=null) {
                        StateTimeShift.getInstance().getStateTimeShiftHandler()
                                .sendEmptyMessage(PvrConstant.Dissmiss_Tshift_BigCtrlbar);
                    }
                    if (StatePVR.getInstance() != null && StatePVR.getInstance().getStatePVRHandler()!= null) {
                        StatePVR.getInstance().getStatePVRHandler()
                                .sendEmptyMessage(PvrConstant.Dissmiss_PVR_BigCtrlbar);
                    }
                    if (StateFileList.getInstance() != null && StateFileList.getInstance().getHandler()!= null) {
                        StateFileList.getInstance().getHandler()
                                .sendEmptyMessage(PvrConstant.Dissmiss_PVR_BigCtrlbar);
                    }
                }

				
			}
		};
		try {
			if (mDefaultDuration == null||mDefaultDuration<0) {
				mDefaultDuration=15L*1000;
			}
			Log.e("mDefaultDuration", "mDefaultDuration:"+mDefaultDuration);
			timer.schedule(clearTask, mDefaultDuration);
		} catch (Exception e) {
			Util.showELog("startTimeTask(),clearTask:" + e.toString());
		}
		
		try {
			if (mRefreshDuration == null||mRefreshDuration<=1) {
				mRefreshDuration=1L*1000;
			}
			Log.e("mRefreshDuration", "mRefreshDuration:"+mRefreshDuration);
			timer.schedule(syncTask, mRefreshDuration, 1L * 1000);
		} catch (Exception e) {
			Util.showELog("startTimeTask(),syncTask:" + e.toString());
		}
	}

	public void stopTimerTask() {
		try {
			timer.cancel();

			mHandler.removeMessages(mMsgWhat);
		} catch (Exception e) {
			Util.showELog("stopTimerTask():" + e.toString());
		}
	}
}
