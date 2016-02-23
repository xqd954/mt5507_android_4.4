package com.mediatek.ui.menu.commonview;

import java.util.List;

import android.app.Dialog;
import android.content.Context;
import android.os.Handler;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.mediatek.tvcm.TVChannelManager;
import com.mediatek.tvcm.TVChannelSelector;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVScanner;
import com.mediatek.tvcommon.TVScanner.ScannerListener;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.setup.Loading;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

public class TVScan extends Dialog {

	private static final String TAG = "TVScan";
	private Context mContext;
	private Handler mHandler;
	public Loading loading;
	private LinearLayout lineLay;
	private MenuConfigManager mcg;
	private TVScanner sc;
	// scan status
	public TextView mStateTextView;
	public TextView mNumberChannel;
	// scan progress
	public ProgressBar mScanprogressbar;
	public TextView mFinishpercentage;
	private View view;
	// By Ese key, scan the number of units Dialog was dimiss
	public int count = 1;
	public boolean onScanning = false;
	private ScannerListener tvScanListener;
	private TVContent tvc;
	private TVChannelManager tm;

	private NavIntegration mNavIntegration;
	public static DataItem mDataItem;
	ScannerListener[] listeners;
	ScannerListener listener;

	private Handler handler = new Handler();
	private RelativeLayout mLeftLayout;
	private TextView mEnterTextView;
	private TextView mSelectTextView;
	private TextView mExitTextView;

	private List<TVChannel> mChannelList;
	private static int CURRENT_CHANNEL_TYPE = 0;


	private String mItemId;
	private int mLastChannelNum = 0;
	private TVChannelSelector selector;

	public void setHandler(Handler mHandler) {
		this.mHandler = mHandler;
	}

	public TVScan(final Context context, String itemId, View v) {
		super(context, R.style.Theme_ScanDialog);
		setContentView(R.layout.menu_scan_dialog_layout);

		setCanceledOnTouchOutside(false);
		mcg = MenuConfigManager.getInstance(context);
		sc = mcg.getScanner();
		tvc = TVContent.getInstance(mContext);
		tm = tvc.getChannelManager();
		selector = tvc.getChannelSelector();
		mNavIntegration = NavIntegration.getInstance(mContext);
		if (selector.getCurrentChannel() != null) {
			TVChannel channelCurrent = mNavIntegration.iGetCurrentChannel();
			if (null != channelCurrent) {
				mLastChannelNum = channelCurrent.getChannelNum();
			}	
		}

		this.mItemId = itemId;

		this.view = v;
		this.mContext = context;
		lineLay = (LinearLayout) findViewById(R.id.menu_scan_dialog_id);
		mStateTextView = (TextView) findViewById(R.id.state);
		mStateTextView.setText(context
				.getString(R.string.menu_setup_channel_scan));

		loading = (Loading) findViewById(R.id.setup_tv_scan_loading);
		mScanprogressbar = (ProgressBar) findViewById(R.id.scanprogressbar);

		mFinishpercentage = (TextView) findViewById(R.id.finishpercentage);
		mFinishpercentage.setText(" 0%");
		mNumberChannel = (TextView) findViewById(R.id.numberchannel);

		mNumberChannel.setText(getChannelString() + " 0");

		if (itemId.equals(MenuConfigManager.TV_UPDATE_SCAN)
				|| mDataItem.mItemID.equals(MenuConfigManager.TV_UPDATE_SCAN)) {
			listener = (TVScanner.ScannerListener) new ScannerListener() {
				public void onCompleted(int completeValue) {
					switch (completeValue) {
					case TVScanner.ScannerListener.COMPLETE_OK:
						loading.stopDraw();
						selectChannel();
						break;
					case TVScanner.ScannerListener.COMPLETE_CANCEL:
					case TVScanner.ScannerListener.COMPLETE_ERROR:
						count = 0;
						showCancelScanInfo();
						selectChannel();
						break;
					default:
						break;
					}
					System.gc();
				}

				public void onFrequence(int freq) {

				}

				public void onProgress(final int progress, final int channels) {
					mHandler.post(new Runnable() {
						public void run() {
							onScanning = true;
							mStateTextView
									.setText(context
											.getString(R.string.menu_setup_channel_scan));
							mNumberChannel.setText(getChannelString()
									+ channels);
							mScanprogressbar.setProgress(progress);
							mFinishpercentage.setText(progress + "%");

							if (progress >= 100) {
								count = 0;
								onScanning = false;
								mStateTextView
										.setText(context
												.getString(R.string.menu_setup_channel_scan_done));
								mHandler.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
								mHandler.sendEmptyMessageDelayed(
										MessageType.MENU_AUTO_EXIT_MESSAGE,
										MessageType.delayMillis2);
							}
						}
					});

				}

			};
			sc.atvUpdateScan(listener);

		} else {
			tvScanListener = new ScannerListener() {
				public void onCompleted(int completeValue) {
					switch (completeValue) {
					case TVScanner.ScannerListener.COMPLETE_OK:
						if (tm.getChannelLength() > 0) {
							List<TVChannel> channelList = tm.getChannels();
							if(null != channelList){
								TVChannel ch = channelList.get(0);
								if (ch != null) {
									tvc.getChannelSelector().select(ch);
								}
							}
						}
						count = 0;
						break;
					// cancel scan
					case TVScanner.ScannerListener.COMPLETE_CANCEL:
					case TVScanner.ScannerListener.COMPLETE_ERROR:
						count = 0;
						onScanning = false;
						MtkLog.d(TAG, " ---- scan canceled----");
						showCancelScanInfo();
						MtkLog.d(TAG, "Channel List Length: "
								+ tm.getChannelLength());
						if (tm.getChannelLength() > 0) {
							List<TVChannel> channels = tm.getChannels();
							if(null != channels){
								TVChannel ch = channels.get(0);
								if (ch != null) {
									selector.select(ch);
								}
							}
						}
						break;
					default:
						break;
					}
					System.gc();

				}

				public void onProgress(final int progress, final int channels) {
					mHandler.post(new Runnable() {
						public void run() {
							onScanning = true;
							mSelectTextView.setVisibility(View.INVISIBLE);
							MtkLog.i(TAG, "[Channels]: " + channels
									+ "  [Progress]: " + progress);
							mStateTextView
									.setText(mContext
											.getString(R.string.menu_setup_channel_scan));
							mNumberChannel.setText(getChannelString() + channels);
							mScanprogressbar.setProgress(progress);
							mFinishpercentage.setText(progress + "%");
							if(progress >= 100) {
									onScanning = false;
									mEnterTextView.setVisibility(View.INVISIBLE);
									mSelectTextView.setVisibility(View.VISIBLE);
									mExitTextView.setVisibility(View.VISIBLE);
									mSelectTextView.setText(mContext.getString(R.string.menu_scan));
									mExitTextView.setText(mContext.getString(R.string.menu_back));
									mStateTextView.setText(mContext
											.getString(R.string.menu_setup_channel_scan_done));
									loading.stopDraw();
									loading.setVisibility(View.INVISIBLE);
									mHandler.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
									mHandler.sendEmptyMessageDelayed(
											MessageType.MENU_AUTO_EXIT_MESSAGE,
											MessageType.delayMillis2);
							}
						}
					});
				}

				public void onFrequence(int freq) {
					// TODO Auto-generated method stub

				};
			};

			if(NavIntegration.getInstance(mContext).isCurrentSourceDTV()){
				sc.dtvScan(tvScanListener);
			}else{
			    sc.atvScan(tvScanListener);
			}
		}

		setPositonSize();
	}

	public void setPositonSize() {
		Window mWindow = TVScan.this.getWindow();
		WindowManager.LayoutParams lp = mWindow.getAttributes();
		WindowManager wm = (WindowManager) mContext
				.getSystemService(Context.WINDOW_SERVICE);
		LayoutParams p = mWindow.getAttributes();
		p.height = (int) (ScreenConstant.SCREEN_HEIGHT * 0.3);
		p.width = (int) (ScreenConstant.SCREEN_WIDTH * 0.4);
		int[] location = new int[2];
		view.getLocationInWindow(location);
		int dialogX = location[0] / 2;
		int dialogY = -view.getHeight() / 8;
		lp.x = dialogX;
		lp.y = dialogY;
		mWindow.setAttributes(p);
		mWindow.setAttributes(lp);
		mScanprogressbar.setLayoutParams(new LinearLayout.LayoutParams(
				p.width * 6 / 8, LinearLayout.LayoutParams.WRAP_CONTENT));
		lineLay.setLayoutParams(new FrameLayout.LayoutParams(p.width,
				p.height * 7 / 8));
	}

	public void cancelScan() {
		MtkLog.d(TAG, "--- call cancel scan");
		sc.cancelScan();
		onScanning = false;
	}

	public void continueScan() {
		if (count != 2) {
			count = 2;
		}
		if (mItemId.equals(MenuConfigManager.TV_UPDATE_SCAN)
				|| mDataItem.mItemID.equals(MenuConfigManager.TV_UPDATE_SCAN)) {
			sc.atvUpdateScan(listener);
	    } else {
	    	if(NavIntegration.getInstance(mContext).isCurrentSourceDTV()){
	    		sc.dtvScan(tvScanListener);
	    	}else{
	    		sc.atvScan(tvScanListener);
	    	}
	    }

		handler.postDelayed(runnable, 1000);
	}

	public Runnable runnable = new Runnable() {
		public void run() {
			// TODO continue to scan,change the TextView
			mStateTextView.setText(mContext
					.getString(R.string.menu_setup_channel_scan));
			loading.drawLoading();
			loading.setVisibility(View.VISIBLE);
			mNumberChannel.setVisibility(View.VISIBLE);
		}
	};

	private String getChannelString() {
		if (mNavIntegration.isCurrentSourceDTV()) {
			return mContext.getString(R.string.menu_setup_channel_scan_dig);
		} else {
			return mContext.getString(R.string.menu_setup_channel_scan_ana);
		}
	}


	/**
	 * show cancel scan information
	 */
	private void showCancelScanInfo() {
		mStateTextView.setText(mContext
				.getString(R.string.menu_setup_channel_scan_cancel));
		loading.stopDraw();
		loading.setVisibility(View.INVISIBLE);
	}

	/**
	 * show scanning state
	 * 
	 * @return TRUE if scan is completed, FALSE if scan is canceled
	 */
	public boolean showScanStateInfo() {
		if (mStateTextView.getText().toString().equals(
				mContext.getString(R.string.menu_setup_channel_scan_done))) {
			loading.stopDraw();
			loading.setVisibility(View.INVISIBLE);
			dismiss();
			mEnterTextView.setVisibility(View.VISIBLE);
			mSelectTextView.setVisibility(View.VISIBLE);
			mExitTextView.setVisibility(View.VISIBLE);
			mEnterTextView.setText(mContext.getString(R.string.menu_enter));
			mSelectTextView.setText(mContext.getString(R.string.menu_select));
			mExitTextView.setText(mContext.getString(R.string.menu_back));
			return true;
		} else {
			cancelScan();
			return false;
		}
	}

	// get left layout in menu
	public void getMenuLeftLayout(RelativeLayout menuLayout) {
		mLeftLayout = menuLayout;
		mEnterTextView = (TextView) mLeftLayout
				.findViewById(R.id.menumain_left_enter);
		mSelectTextView = (TextView) mLeftLayout
				.findViewById(R.id.menumain_left_select);
		mExitTextView = (TextView) mLeftLayout
				.findViewById(R.id.menumain_left_exit);
	}

	// for volume down/up
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		if (mStateTextView.getText().toString().equals(
				mContext.getString(R.string.menu_setup_channel_scan))) {
			mHandler.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
		}
		switch (keyCode) {
		case KeyMap.KEYCODE_VOLUME_DOWN:
		case KeyMap.KEYCODE_VOLUME_UP:
			return true;

		default:
			break;
		}
		return super.onKeyDown(keyCode, event);
	}

	private void selectChannel() {
		if (tm.getChannelLength() > 0) {
			TVChannel ch = tm.findChannelByNumber(mLastChannelNum);

			if (ch == null) {
				List<TVChannel> channels = tm.getChannels();
				if(null != channels){
					ch = channels.get(0);
				}	
			}

			tvc.getChannelSelector().select(ch);
		}
	}
}
