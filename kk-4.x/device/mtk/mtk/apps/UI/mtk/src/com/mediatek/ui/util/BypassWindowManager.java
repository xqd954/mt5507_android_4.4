package com.mediatek.ui.util;

import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVInputManager;
import com.mediatek.ui.nav.commonview.SnowTextView;

import android.R.string;
import android.content.Context;
import android.graphics.Rect;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class BypassWindowManager {
	public final static int WINDOWLOCK_TIME = 1;
	private static BypassWindowManager instance = null;
	private static Context mContext = null;
	private static TVContent mTvContent = null;
	private static TVInputManager mTvInputManager = null;
	static String TAG = "BypassWindowManager";

	private static int[] windowIdArray = { -1, -1, -1, -1, -1 };

	private BypassWindowManager(Context context) {
		mContext = context;
		mTvContent = TVContent.getInstance(context);
		mTvInputManager = mTvContent.getInputManager();
	}

	public static synchronized BypassWindowManager getInstance(Context context) {
		if (instance == null) {
			instance = new BypassWindowManager(context);
		}
		return instance;
	}

	/*
	 * windowId should be got it from method getAvailableWindowId()
	 */
	public void setBypassWindow(boolean enable, int windowId, Rect r) {
		if (windowId > 4 || windowId < 0) {
			return;
		}
		if (enable) {
			// TODO set bypass window
			lockWindowId(windowId);
			mTvInputManager.setMjcBypassWindow(windowId, r);
			Log.e(TAG, "lock windowId :" + windowId
					+ " ---->windowIdArray[windowId]: "
					+ windowIdArray[windowId]);

		} else {
			// TODO set bypass window
			if (windowIdArray[windowId] > 4 || windowIdArray[windowId] < 0) {
				return;
			}
			unlockWindowId(windowId);
			mTvInputManager.setMjcBypassWindow(windowId, new Rect(0, 0, 0, 0));
			Log.e(TAG, "unlock windowId :" + windowId
					+ " ---->windowIdArray[windowId]: "
					+ windowIdArray[windowId]);
			
		}
	}

	public int getAvailableWindowId() {
		for (int i = 0; i < 5; i++) {
			if (windowIdArray[i] == -1) {
				Log.e(TAG, "get_available_windowId : i  " + i
						+ " ---->windowIdArray[i] :" + windowIdArray[i]);
				return i;
			}
		}

		return -1;
	}

	private void lockWindowId(int windowId) {
		windowIdArray[windowId] = windowId;
	}

	private void unlockWindowId(int windowId) {
		windowIdArray[windowId] = -1;
	}


}
