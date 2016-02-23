package com.mediatek.ui.nav.commonview;

import java.util.Timer;
import java.util.TimerTask;

import android.content.Context;
import android.os.Handler;
import android.view.Gravity;
import android.view.View;
import android.os.SystemProperties;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.commonview.ToastDialog;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.util.MtkLog;

/**
 * this view is used for toast special information, such as
 * "feature not available"
 * 
 * @author MTK40462
 * 
 */
public class ToastInfoView {

	private static final String TAG = "VGANoSingalView";
	private static ToastInfoView infoView;
	private Context mContext;
	private NavIntegration mNavIntegration;
	private Timer mVGATimer;
	private Handler mHandler;
	private View mSnowTextView;
	private ToastDialog mToastDialog = null;

	public View getmSnowTextView() {
		return mSnowTextView;
	}

	public void setmSnowTextView(View mSnowTextView) {
		this.mSnowTextView = mSnowTextView;
	}

	private ToastInfoView(Context context) {
		mNavIntegration = NavIntegration.getInstance(context);
		mHandler = new Handler(context.getMainLooper());
		mContext = context;
	}

	public static synchronized ToastInfoView getInstance(Context context) {
		if (infoView == null) {
			infoView = new ToastInfoView(context);
		}
		return infoView;
	}

	/**
	 * when VGA source has no signal and DPMS config is on, show this
	 * information after 30 seconds
	 */
	public void toastVGAInfo() {
		MtkLog.d(TAG, "reset DMPS VGA Timer");
		if (mVGATimer != null) {
			mVGATimer.cancel();
		}
		mNavIntegration.cancelDpmsTimer();
    if ("1".equals(SystemProperties.get("mtk.auto_test"))){
		  return;
		}
		if (mNavIntegration.isCurrentSourceVGA()
				&& !mNavIntegration.isCurrentSourceBlocking()
				&& !mNavIntegration.iCurrentInputSourceHasSignal()
				&& (mNavIntegration.isNonPIPMode())
				&& (mNavIntegration.dpmsOption.get() == 1)) {
			mVGATimer = new Timer();
			mVGATimer.schedule(new TimerTask() {
				public void run() {
					// mNavIntegration.startDpmsTimer();
					mHandler.post(new Runnable() {

						public void run() {
							// TODO Auto-generated method stub
							if (mNavIntegration.isCurrentSourceVGA()
									&& !mNavIntegration
											.isCurrentSourceBlocking()
									&& !mNavIntegration
											.iCurrentInputSourceHasSignal()
									&& (mNavIntegration.isNonPIPMode())
									&& (mNavIntegration.dpmsOption.get() == 1)) {
								timeShowView();
								mNavIntegration.startDpmsTimer();
								mSnowTextView.setVisibility(View.GONE);
							}
						}

					});

				}
			}, 30000);
		}
	}

	public void cancelVGATimer() {
		if (mVGATimer != null) {
			mVGATimer.cancel();
			mVGATimer = null;
		}
	}

	public void cancelVGAInfo() {
		cancelVGATimer();
		mNavIntegration.cancelDpmsTimer();
		if (mToastDialog != null && mToastDialog.isShowing()) {
			mToastDialog.dismiss();
		}
	}

	/**
	 * after toasting VGA Info view, show this view 10 seconds
	 */
	public void timeShowView() {
		MtkLog.d(TAG, "-----------show text power off -------------");
		mToastDialog = new ToastDialog(mContext, true, mSnowTextView);
		mToastDialog.setText(mContext.getResources().getString(
				R.string.vga_no_sinal_info));
		mToastDialog.setBackGround(R.drawable.nav_shorttip_bg);
		mToastDialog.setPositon(-200, -100);
		mToastDialog.setTextSize(20, Gravity.CENTER);
		mToastDialog.setSize(400, 80);
		mToastDialog.run(10000);
	}

	/**
	 * show "Feature not Available " information
	 */
	public void toastCaptureInfo() {
		mToastDialog = new ToastDialog(mContext, true, null);
		mToastDialog.setText("Feature Not Available");
		mToastDialog.setTextSize(30, Gravity.CENTER);
		mToastDialog.setBackGround(R.color.capture_gray_background);
		mToastDialog.setPositon(-180, -150);
		mToastDialog.setSize(400, 50);
		mToastDialog.run(3000);
	}

}
