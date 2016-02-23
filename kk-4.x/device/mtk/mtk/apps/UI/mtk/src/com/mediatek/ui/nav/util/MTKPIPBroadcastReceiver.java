package com.mediatek.ui.nav.util;

import com.mediatek.tvcommon.ITVCommon;
import com.mediatek.tvcommon.TVCommonNative;
import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import com.mediatek.capture.CaptureScreen;
import com.mediatek.ui.util.DestroyApp;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.graphics.Rect;
import android.os.Environment;
import android.os.RemoteException;

public class MTKPIPBroadcastReceiver extends BroadcastReceiver {
	private NewPipLogic mNewPipLogic;
	private Context mContext;
	private static final String TAG = "MTKPIPBroadcastReceiver";
	private CaptureScreen mCapture;

	public MTKPIPBroadcastReceiver() {
		super();
		mCapture = CaptureScreen.getInstance();
	}

	public MTKPIPBroadcastReceiver(Context context) {
		super();
		mCapture = CaptureScreen.getInstance();
	}

	@Override
	public void onReceive(Context context, Intent intent) {
		int mode = 0;
		ITVCommon tv = TVCommonNative.getDefault(mContext);
		if (tv == null) {
			return;
		}
		try {
			mode = tv.getCurrentOutputMode();
			if (mode == ITVCommon.OUTPUT_MODE_POP
					|| mode == ITVCommon.OUTPUT_MODE_PIP) {
				return;
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		mContext = context.getApplicationContext();
		mNewPipLogic = NewPipLogic.getInstance(mContext);

		if ("tv.policy.SYSTEM_KEY".equals(intent.getAction())) {
			int keyCode = intent.getIntExtra(Intent.EXTRA_KEY_EVENT, -1);
			switch (keyCode) {
			case KeyMap.KEYCODE_MTKIR_PIPPOP:
				MtkLog.d(TAG, "onReceive---KeyMap.KEYCODE_MTKIR_PIPPOP");
				DestroyApp.setTopTask(false);
				mNewPipLogic.switchOutputMode();
				break;

			case KeyMap.KEYCODE_MTKIR_PIPPOS:
				MtkLog.d(TAG, "onReceive---KeyMap.KEYCODE_MTKIR_PIPPOS");
				mNewPipLogic.changePipPosition();
				break;

			case KeyMap.KEYCODE_MTKIR_PIPSIZE:
				MtkLog.d(TAG, "onReceive---KeyMap.KEYCODE_MTKIR_PIPSIZE");
				mNewPipLogic.changePipSize();
				break;

			case KeyMap.KEYCODE_MTKIR_MTKIR_SWAP:
				MtkLog.d(TAG, " keyCode is KeyMap.KEYCODE_MTKIR_MTKIR_SWAP");
				if (isValid()) {
					mNewPipLogic.swap3rdappTvapp();
				}
				break;

			case KeyMap.KEYCODE_MTKIR_CHUP:
				MtkLog.d(TAG, "onReceive---KeyMap.KEYCODE_MTKIR_CHUP");
				mNewPipLogic.channelUp();
				break;

			case KeyMap.KEYCODE_MTKIR_CHDN:
				MtkLog.d(TAG, "onReceive---KeyMap.KEYCODE_MTKIR_CHDN");
				mNewPipLogic.channelDown();
				break;
				
				/*
			case KeyMap.KEYCODE_MTKIR_GREEN:
				if(mode == ITVCommon.OUTPUT_MODE_APP_PIP)
					return;
				String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss")
						.format(new Date());
				File file = new File(Environment.getExternalStorageDirectory()
						.getPath()
						+ File.separator
						+ "IMG_"
						+ timeStamp
						+ ".jpg");
				mCapture.captureJPGPictureOnce(0, new Rect(0, 0, 1280, 720), file);
				break;*/
			default:
				break;
			}
		}
	}

	private static long mLastKeyDownTime;
	private final long KEY_DURATION = 1000;

	private boolean isValid() {
		long currentTime = System.currentTimeMillis();
		if ((currentTime - mLastKeyDownTime) >= KEY_DURATION) {
			mLastKeyDownTime = currentTime;
			MtkLog.i(TAG, " ~~~~~~~~~~swap key down duration :" + "> 1000 mm");
			return true;
		} else {
			MtkLog.i(TAG, " ~~~~~~~~~~swap key down duration :"
					+ (currentTime - mLastKeyDownTime) + "< 1000 mm");
			mLastKeyDownTime = currentTime;
			return false;
		}
	}
}
