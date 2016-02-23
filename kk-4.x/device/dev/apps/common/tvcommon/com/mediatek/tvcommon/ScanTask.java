/**
 * 
 */
package com.mediatek.tvcommon;

import java.util.List;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.SystemClock;
import com.mediatek.tv.common.ChannelCommon;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.model.ChannelInfo;
import com.mediatek.tv.model.ScanListener;
import com.mediatek.tv.service.BroadcastService;
import com.mediatek.tv.service.ChannelService;
import com.mediatek.tv.service.ComponentService;
import com.mediatek.tv.service.ScanService;
import com.mediatek.tv.service.ChannelService.ChannelOperator;
import com.mediatek.tvcommon.TVScanner.ScannerListener;

public abstract class ScanTask {

	protected ScanService scanService;
	protected ChannelService channelService;
	protected TVConfigurer cfg;
	protected Context mContext;
	protected BroadcastService brdSrv;
	protected ComponentService componentService;
	protected TVCommonManager tvCommonManager;

	/* my thread with handler, we could NOT do much in lower lay thread (like mw scan) */
	protected static HandlerThread mThread;
	
	/* my handler */
	protected static Handler mHandler;

	private final Object mActionDoneSync = new Object();
	private boolean mActionDone = false;

	// maximum time we wait for scan complete broadcast before going on
	private static final int MAX_UPDATE_TIME = 2000;

	protected int state = TVScanner.STATE_COMPLETED;
	protected int type = 0;

	public static final int TYPE_ATV = 0;
	public static final int TYPE_DTV = 1;

	private BroadcastReceiver br = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			actionDone();
		}
	};

	private void actionDone() {
		synchronized (mActionDoneSync) {
			mActionDone = true;
			mActionDoneSync.notifyAll();
		}
	}

	void onComplete() {
		flush();
		finish();
		state = TVScanner.STATE_COMPLETED;
	}

	void onCancel() {
		flush();
		finish();
		state = TVScanner.STATE_COMPLETED;
	}

	int getState() {
		return state;
	}


	// update status for compService
	private void updateCompStatus (String status) {
		if (null == tvCommonManager || null == componentService) {
			TVCommonNative.TVLog(status + " return reason: tvCommonManager " + tvCommonManager + ", componentService " + componentService);
			return;
		}
		
		int ret = -100;
		int mOutputMode = tvCommonManager.getCurrentOutputMode();
		if (ITVCommon.OUTPUT_MODE_NORMAL == mOutputMode) {
			try {
				ret = componentService.updateSysStatus(status);
				TVCommonNative.TVLog(status + " ret: " + ret);
			} catch (TVMException e) {
				e.printStackTrace();
			}
		} else {
			TVCommonNative.TVLog("don't update " + status + " because compSrv don't work in: " 
					+ (ITVCommon.OUTPUT_MODE_PIP == mOutputMode ? "PIP(" : (ITVCommon.OUTPUT_MODE_POP == mOutputMode ? "POP(" : 
						(ITVCommon.OUTPUT_MODE_MMP == mOutputMode ? "MMP(" : (ITVCommon.OUTPUT_MODE_APP_PIP == mOutputMode ? "APP_PIP(" : "???("))))
					+ mOutputMode + ") mode");
		}
	}
	

	private void finish() {
		synchronized (mActionDoneSync) {
			mActionDone = false;
		}
		
		// send broadcast to notify server to update new scanned channels.
		Intent intent = new Intent(TVCommonNative.SCAN_COMPLETE_ACTION);
		intent.putExtra("scan_type", type);
		mContext.sendOrderedBroadcast(intent, null, br, null, 0, null, null);
		final long endTime = SystemClock.elapsedRealtime() + MAX_UPDATE_TIME;
		synchronized (mActionDoneSync) {
			while (!mActionDone) {
				long delay = endTime - SystemClock.elapsedRealtime();
				if (delay <= 0) {
					/* delay == 0 means wait for ever... */
					TVCommonNative.TVLog("Time out.");
					break;
				}
				try {
					TVCommonNative.TVLog("delay: " + delay);
					mActionDoneSync.wait(delay);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}

		updateCompStatus(ComponentService.COMP_TTX_SYS_SCAN_FINISHED);
	}

	private void flush() {
		try {
			if (CrossRoad.isOnlyCableAtv) {
				TVCommonNative.TVLog("flush 2 DBs");
				channelService.fsStoreChannelList(ChannelCommon.DB_AIR);
				channelService.fsStoreChannelList(ChannelCommon.DB_CABEL);
			} else {
				channelService.fsStoreChannelList(TVCommonNative.getNativeDBName());
			}
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}

	/**
	 * Cancel current scan task.
	 */
	public void cancel() {
		scanService.cancelScan(getRawScanMode());
	}


	void prepareScan() {
		TVOption<Integer> blueOpt = (TVOption<Integer>) cfg.getOption(ConfigType.CFG_BLUE_SCREEN);
		if (blueOpt != null && blueOpt.get() == ConfigType.COMMON_ON) {
			int winIdTvIn = TVCommonService.getWinIdTvIn();
			switch(winIdTvIn) {
				case TVCommonService.TV_WIN_ID_MAIN:
					CrossRoad.isForceBlackMuteMain = true;
					break;
				case TVCommonService.TV_WIN_ID_SUB:
					CrossRoad.isForceBlackMuteSub  = true;
					break;
				default:
					CrossRoad.isForceBlackMuteMain = true;
					CrossRoad.isForceBlackMuteSub  = true;
					break;
			}
			int ret = TVMethods.setVideoMuteById(winIdTvIn, true, 0, 0, 0);
			TVCommonNative.TVLog("setVideoMuteById(" + winIdTvIn + ", true, 0, 0, 0) ret " + ret);
		}
		
		try {
			brdSrv.syncStopService();
		} catch (TVMException e1) {
			e1.printStackTrace();
		}
		
		state = TVScanner.STATE_SCANNING;

		updateCompStatus(ComponentService.COMP_TTX_SYS_CHANNEL_SCANNING);
	}


	/**
	 * Start current scan task.
	 */
	public abstract boolean scan(ScannerListener listener);

	abstract String getRawScanMode();

	protected String getNativeDBName() {
		@SuppressWarnings("unchecked")
		TVOption<Integer> option = (TVOption<Integer>) cfg.getOption(IntegerOption.CFG_BS_SRC);

		if (option == null) {
			throw new ExceptionInInitializerError("The cfg parameters IntegerOption.CFG_BS_SRC not found in the collection of parameters");
		}

		return option.get() == ConfigType.BS_SRC_AIR ? ChannelCommon.DB_AIR : ChannelCommon.DB_CABEL;
	}


	synchronized void endPreSet() {
		flush();
		finish();
	}
	

	class ScanListenerDelegater implements ScanListener {
		/* UI operation must be done in MainThread, or Fatal Exception occurs */
		private Handler mainThreadHandler = new Handler(Looper.getMainLooper());

		private ScannerListener listener;
		private int channels;

		ScanListenerDelegater(ScannerListener listener) {
			this.listener = listener;
			
			if (null == mainThreadHandler) {
				throw new IllegalAccessError("Fatal Error in init ScanListenerDelegater:  mainThreadHandler: " + mainThreadHandler);
			}
		}
		

		public void setScanCanceled(String arg0) {
			/**
			 *	must turn to my thread to do thing, especially time-consuming.
			 *	or it will block lower lay thread, such as mw scan.
			 *  Fortunately, my thead is so fast that don't make side effect so much
			 */
			mHandler.post(new Runnable() {
				public void run() {

					/* better to do onCannel and then onCompleted to AP */
					onCancel();

					/**
					 *  Ap do many thing in this nfy (to-do??? they turn to their thread? hard to do now)
					 *	Besides, these thing must be done in Main Thread. 
					 *  So... line up for Main thread...
					 */
					mainThreadHandler.post(new Runnable() {
						public void run() {
							listener.onCompleted(ScannerListener.COMPLETE_CANCEL);
						}
					});
					
				}
			});
		}


		/* details follow setScanCanceled */
		public void setScanCompleted(String arg0) {
			mHandler.post(new Runnable() {
				public void run() {

					onComplete();
					mainThreadHandler.post(new Runnable() {
						public void run() {
							listener.onProgress(100, channels);
							listener.onCompleted(ScannerListener.COMPLETE_OK);
						}
					});
					
				}
			});
		}


		/* details follow setScanCanceled */
		public void setScanError(String scanMode, int error) {
			mHandler.post(new Runnable() {
				public void run() {
					
					onComplete();
					mainThreadHandler.post(new Runnable() {
						public void run() {
							listener.onCompleted(ScannerListener.COMPLETE_ERROR);
						}
					});
					
				}
			});
		}


		/* nfy AP in Main Thread directly, because no time-consuming thing to be done here */
		public void setScanFrequence(String scanMode, final int freq) {
			mainThreadHandler.post(new Runnable() {
				public void run() {
					listener.onFrequence(freq);
				}
			});
		}


		/* nfy AP in Main Thread directly, because no time-consuming thing to be done here */
		public void setScanProgress(String scanMode, final int progress, final int channels) {
			if (channels > 0) {
				TVCommonNative.TVLog("find_channel " + channels);
			}
				
			this.channels = channels;
			mainThreadHandler.post(new Runnable() {
				public void run() {
					listener.onProgress(progress, channels);
				}
			});
		}

	}


}

