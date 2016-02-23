package com.mediatek.tvcm;

import java.io.IOException;
import com.mediatek.tvcommon.TVCAMManager;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVConfigurer;
import com.mediatek.tvcommon.TVEventManager;
import com.mediatek.tvcommon.TVMethods;
import com.mediatek.tvcommon.TVOutputCommon;
import com.mediatek.tvcommon.TVScanner;
import com.mediatek.tvcommon.TVCommonNative;

import android.content.Context;
import android.content.Intent;
import android.os.RemoteException;

public class TVContent extends TVComponent {

	static private TVContent instance;
	private TVMethods mtvm;
	private TVOutputCommon mTVOutputCommon;

	protected TVContent(Context context) {
		super(context);
		init();
	}

	static public TVContent getInstance(Context context) {
		if (instance == null) {
			instance = new TVContent(context);
		}

		return instance;
	}

	protected void init() {
		mtvm = TVMethods.getInstance(context);
		mTVOutputCommon = TVOutputCommon.getInstance(context);
	}

	public TVConfigurer getConfigurer() {
		return TVConfigurer.getInstance(context);
	}

	public TVChannelManager getChannelManager() {
		return TVChannelManager.getInstance(context);
	}

	public TVChannelSelector getChannelSelector() {
		return TVChannelSelector.getInstance(context);
	}

	public TVCAMManager getCAMManager() {
		return TVCAMManager.getInstance(context);
	}

	public TVInputManager getInputManager() {
		return TVInputManager.getInstance(context);
	}

	public TVScanner getScanner() {
		return TVScanner.getInstance(context);
	}

	public TVStorage getStorage() {
		return TVStorage.getInstance(context);
	}

	public TVEventManager getEventManager() {
		return TVEventManager.getInstance(context);
	}

	public TVTeleTextManager getTeleTextManager(){
		return TVTeleTextManager.getInstance(context);
	}
	
	public boolean isMute() {
		return mtvm.isMute();
	}

	public void setAudioMute(boolean mute) {
		mtvm.setAudioMute(mute);
	}

	public void enterTV() {
		mtvm.enterTV();
	}

	public void leaveTV() {
		mtvm.leaveTV();
	}

	public boolean setColorKey(boolean b_enabled, int color) {
		return mtvm.setColorKey(b_enabled, color);
	}

	public boolean setOpacity(int opacity) {
		return mtvm.setOpacity(opacity);
	}

	public void manualStop() {
		mtvm.manualStop();
	}

	public void flushMedia() {
		try {
			Runtime.getRuntime().exec("sync");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void setScreenPosition(String output, float x, float y, float w,
			float h) {
		mTVOutputCommon.setScreenPosition(output, x, y, w, h);
	}

	public void setVideoBlueMute(boolean isMute) {
		TVCommonNative.TVLog("setVideoBlueMute(" + isMute + ")");
		mtvm.setVideoBlueMute(isMute);
	}

	public TVTimerManager getTimerManager() {
		return TVTimerManager.getInstance(context);
	}

	public TVChannel getCurrentChannel() {
		try {
			return getITVCommon().getCurrentChannel();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return null;
	}

	public void sendUpdate() {
		try {
			if(null != itv){
			itv.updateCurrentOutput();
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	public void unblock(String outputName) {
		try {
			if(null != itv){
			itv.tempUnblockInputSource(itv.getCurrentInputSource());
			TVChannel ch = itv.getCurrentChannel();
			if (ch != null) {
				ch.tempUnblocked();
				}
			}
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void sendPowerOff() {
		getHandler().post(new Runnable() {

			public void run() {
				// TODO Auto-generated method stub
				Intent i = new Intent(
						"android.intent.action.ACTION_REQUEST_SHUTDOWN");
				i.putExtra("android.intent.extra.KEY_CONFIRM", false);
				i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				context.startActivity(i);

			}
		});
		// We cannot use this hide action in Eclipse
	}

	public String getSubtitleLang() {
		return mTVOutputCommon.getSubtitleLang();
	}

	public String getCurSubtitleLang() {
		return mTVOutputCommon.getCurSubtitleLang();
	}

	public void setSubtitleLang(String lang) {
		mTVOutputCommon.setSubtitleLang(lang);
	}

	public void syncStopSubtitleStream() {
		mTVOutputCommon.syncStopSubtitleStream();
	}

}