package com.mediatek.tvcm;

import com.mediatek.tv.TVManager;
import com.mediatek.tvcommon.ITVCommon;
import com.mediatek.tvcommon.TVCommonNative;

import android.content.Context;
import android.os.Handler;
import android.os.Message;

public abstract class TVComponent {
	private Handler handler;
	protected Context context;
	protected ITVCommon itv;
	protected TVManager tvMgr; 

	public TVComponent(Context context) {
		this.context = context.getApplicationContext();
		itv = TVCommonNative.getDefault(context);
		tvMgr = TVManager.getInstance(context);

		handler = new Handler(this.context.getMainLooper(),
				new Handler.Callback() {

					@Override
					public boolean handleMessage(Message msg) {
						TVComponent.this.handleMessage(msg.what, msg.obj);
						return true;
					}
				});
	}

	protected void handleMessage(int msgCode, Object obj) {

	}

	protected void sendMessageOnce(int msgCode, Object obj) {
		if (handler.hasMessages(msgCode)) {
			return;
		}
		handler.sendMessage(handler.obtainMessage(msgCode, obj));
	}

	protected void sendMessageOnceDelayed(int msgCode, Object obj,
			long delayMillis) {
		if (handler.hasMessages(msgCode)) {
			return;
		}
		handler.sendMessageDelayed(handler.obtainMessage(msgCode, obj),
				delayMillis);
	}

	protected void sendMessage(int msgCode, Object obj) {
		handler.sendMessage(handler.obtainMessage(msgCode, obj));
	}

	protected void sendMessageDelayed(int msgCode, Object obj, long delayMillis) {
		handler.sendMessageDelayed(handler.obtainMessage(msgCode, obj),
				delayMillis);
	}

	protected Handler getHandler() {
		return handler;
	}

	protected TVContent getContent() {
		return TVContent.getInstance(context);
	}

	protected ITVCommon getITVCommon() {
		return itv;
	}

	private static final boolean isLog = true;
	public static void TvLog(String mString) {
		if (!isLog) {
			return;
		}
		StackTraceElement[] el = new Exception().getStackTrace();
		android.util.Log.i(
				"TVCM",
				"Class:" + el[1].getClassName() + "." + el[1].getMethodName()
						+ " (Line:"
						+ new Integer(el[1].getLineNumber()).toString()
						+ ") : " + mString);

	}
}
