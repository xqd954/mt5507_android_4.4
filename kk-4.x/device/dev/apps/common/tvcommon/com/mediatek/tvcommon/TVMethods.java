package com.mediatek.tvcommon;

import java.io.IOException;

import android.content.Context;
import android.os.RemoteException;

import com.mediatek.tv.TVManager;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.service.BroadcastService;
import com.mediatek.tv.service.InputService;
import com.mediatek.tv.service.OSDService;
import com.mediatek.tv.service.TVRemoteService;

public class TVMethods {
	static private TVMethods instance;
	TVManager tvMngr;
	static private BroadcastService brdSrv;
	static Context mContext;

	TVMethods(Context context) {
		mContext = context.getApplicationContext();
		tvMngr = TVManager.getInstance(mContext);
		brdSrv = (BroadcastService) tvMngr.getService(BroadcastService.BrdcstServiceName);
		if (null == tvMngr || null == brdSrv) {
			throw new IllegalAccessError("Fatal Error happens when initiliaze TVMethods");
		}
	}

	public static TVMethods getInstance(Context context) {
		if (instance == null) {
			instance = new TVMethods(context);
		}

		return instance;
	}

	public boolean isMute() {
		boolean mute = false;
		BroadcastService srv = (BroadcastService) tvMngr
				.getService(BroadcastService.BrdcstServiceName);
		try {
			if(srv != null)
				mute = srv.getMute();
		} catch (TVMException e) {
			e.printStackTrace();
		}
		return mute;
	}

	public void setAudioMute(boolean mute) {
		BroadcastService srv = (BroadcastService) tvMngr
				.getService(BroadcastService.BrdcstServiceName);
		try {
			if (srv != null)
			srv.setMute(mute);
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}

	public boolean setColorKey(boolean b_enabled, int color) {
		OSDService srv = (OSDService) tvMngr
				.getService(OSDService.OSDServiceName);
		if (srv != null)
			return srv.setColorKey(b_enabled, color);
		else
			return false;
	}

	public boolean setOpacity(int opacity) {
		OSDService srv = (OSDService) tvMngr
				.getService(OSDService.OSDServiceName);
		if (srv != null)
			return srv.setOpacity(opacity);
		else
			return false;
	}

	public void manualStop() {
		try {
			int ret = brdSrv.syncStopService();
			TVCommonNative.TVLog("syncStopService() ret " + ret);
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}

	/**
	 * stop video for TV sources.
	 */
	public void tvVideoStop() {
		try {
			int ret = brdSrv.syncStopVideoStream();
			TVCommonNative.TVLog("syncStopVideoStream() ret " + ret);
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}

	public static final String inputTypes[] = { 
			InputService.INPUT_TYPE_TV,
			InputService.INPUT_TYPE_AV, 
			InputService.INPUT_TYPE_SVIDEO,
			InputService.INPUT_TYPE_COMPONENT,
			InputService.INPUT_TYPE_COMPOSITE, 
			InputService.INPUT_TYPE_HDMI,
			InputService.INPUT_TYPE_VGA };

	protected static final int cfgGrpForType[] = { 
			TVConfigurer.CFG_GRP_ATV,
			TVConfigurer.CFG_GRP_AV, 
			TVConfigurer.CFG_GRP_AV,
			TVConfigurer.CFG_GRP_COMPONENT, 
			TVConfigurer.CFG_GRP_AV,
			TVConfigurer.CFG_GRP_HDMI, 
			TVConfigurer.CFG_GRP_VGA };

	/**
	 * These APIs help AP handle some dirty things when AP leaves or enters
	 * active mode...
	 */
	public void enterTV() {
		TVConfigurer cfger = TVConfigurer.getInstance(mContext);
		if (null == cfger) {
			TVCommonNative.TVLog("return reason: null == cfger");
			return;
		}
			
		ITVCommon itv = TVCommonNative.getDefault(mContext);
		if (null == itv) {
			TVCommonNative.TVLog("return reason: null == itv");
			return;
		}

		String currentInput = null;
		String cInputType = null;
		int cfgIndex = 0;
		try {
			currentInput = itv.getCurrentInputSource();
			if (currentInput != null) {
				cInputType = itv.getInputSourceType(currentInput);
				for (int i = 0; i < inputTypes.length; i++) {
					if (cInputType.equals(inputTypes[i])) {
						if (cInputType.equals(InputService.INPUT_TYPE_TV)) {
							cfgIndex = currentInput.equals("atv") ? TVConfigurer.CFG_GRP_ATV
									: TVConfigurer.CFG_GRP_DTV;
						} else {
							cfgIndex = cfgGrpForType[i];
						}
						cfger.setGroup(cfgIndex);
                        TVCommonNative.TVLog("Set Configure Group to " + TVConfigurer.CFG_GRP_NAMES[cfgIndex]);
						break;
					}
				}
			}
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void leaveTV() {
		TVConfigurer cfger = TVConfigurer.getInstance(mContext);
		if(cfger != null)
			cfger.setGroup(TVConfigurer.CFG_GRP_LEAVE);
        TVCommonNative.TVLog("Set Configure Group to mmp");
	}

	/**
	 * In order to improve the performance of file system, it do not add 'sync'
	 * option when mount UBIFS file system. Now UBIFS work on write-back, which
	 * means that file changes do not go to the flash media straight away, but
	 * they are cached and go to the flash later, when it is absolutely
	 * necessary. Invoke this method when you need to flash data to media
	 * immediately.
	 */
	public void flushMedia() {
		try {
			Runtime.getRuntime().exec("sync");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}


	/**
	 * Set video as blue mute
	 * 
	 * @param isMute
	 *            , set blue mute if it is true
	 */
	public void setVideoBlueMute() {
		ITVCommon itv = TVCommonNative.getDefault(mContext);
		TVConfigurer cf = TVConfigurer.getInstance(mContext);
		@SuppressWarnings("unchecked")
		TVOptionRange<Integer> blueOpt = (TVOptionRange<Integer>) cf.getOption(ConfigType.CFG_BLUE_SCREEN);

		try {
			if(itv != null){
			int ret = -100;
			TVChannel ch;
			ch = itv.getCurrentChannel();
			if (ch != null){
				if(blueOpt.get() == ConfigType.COMMON_ON){
					ret = brdSrv.setVideoBlueMute(true, ch.isBlocked());
					TVCommonNative.TVLog("setVideoBlueMute(true, " + ch.isBlocked() + ") ret " + ret);
				}else {
					brdSrv.setVideoBlueMute(false, ch.isBlocked());
					TVCommonNative.TVLog("setVideoBlueMute(false, " + ch.isBlocked() + ") ret " + ret);
				}
			}
		 }
		} catch (TVMException e) {
			e.printStackTrace();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}
	
	public int setVideoBlueMute(boolean isMute) {
		int	ret = -100;
		try {
			ret = brdSrv.setVideoBlueMute(isMute);
			//TVCommonNative.TVLog("setVideoBlueMute(" + isMute + ") ret " + ret);
		} catch (TVMException e) {
			e.printStackTrace();
		}
		return ret;
	}

	public void setVideoMute() {
		try {
			int ret = brdSrv.setVideoMute();
			//TVCommonNative.TVLog("setVideoMute() ret " + ret);
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}

	public static int setVideoMuteById(int winId, boolean b_enable, int red, int green, int blue) {
		int ret = -100;
		try {
			ret = brdSrv.setVideoMuteById(winId, b_enable, red, green, blue);
			//TVCommonNative.TVLog("setVideoMuteById(" + winId + ", " + b_enable + ", " + red + ", " + green +", " + blue + ")");
		} catch (TVMException e) {
			e.printStackTrace();
		}

		try {
			// vdp_yangyang.t: drv mute color setting is async
			Thread.sleep(20);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}

		return ret;
	}

	public int setUserDefaultVideoMuteById(int winId, boolean b_enable, int red, int green, int blue) {
		int ret = -100;
		try {
			ret = brdSrv.setUserDefaultVideoMuteById(winId, b_enable, red, green, blue);
			//TVCommonNative.TVLog("setUserDefaultVideoMuteById(" + winId + ", " + b_enable + ", " + red + ", " + green +", " + blue + ")");
		} catch (TVMException e) {
			e.printStackTrace();
		}
		return ret;
	}

	public long getBroadcastUTC() {
		// Sorry for this arugments,
		// TVManager told me that these are 'output'
		// arguments. I don't need to init them.
		int[] args = new int[2];
		long lret = 0;
		try {
			// That is in second...
			lret = brdSrv.dtGetBrdcstUtc(args) * 1000;
			TVCommonNative.TVLog("dtGetBrdcstUtc(" + args + ") * 1000 = " + lret);
			return lret;
		} catch (TVMException e) {
			e.printStackTrace();
		}

		lret = System.currentTimeMillis();
		TVCommonNative.TVLog("ret " + lret);
		return lret;
	}

	public long getBroadcastTZOffset() {
		long lret = 0;
		try {
			// That is in second...
			lret = brdSrv.dtGetTz() * 1000;
			TVCommonNative.TVLog("ret" + lret);
			return lret;
		} catch (TVMException e) {
			e.printStackTrace();
		}

		TVCommonNative.TVLog("ret 0");
		return 0;
	}

	public boolean isFreeze() {
		boolean freeze = false;
		try {
			/*
			 *ITVCommon not give the interface to get current output,
			 *so, I just use "main" as default output.
			 */
			freeze = brdSrv.isFreeze("main");
		} catch (TVMException e) {
			e.printStackTrace();
		}

		TVCommonNative.TVLog("ret " + freeze);
		return freeze;
	}

	public void setFreeze(boolean isFreeze) {
		try {
			int ret = brdSrv.setFreeze("main", isFreeze);
			TVCommonNative.TVLog("setFreeze(main, " + isFreeze + ") ret " + ret);
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}
	
	public void setFreeze(String outputName, boolean isFreeze) {
		try {
			int ret = brdSrv.setFreeze(outputName, isFreeze);
			TVCommonNative.TVLog("setFreeze(" + outputName + ", " + isFreeze + ") ret " + ret);
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}

	public void changeFreqById(int winId, int tunerMode, String rfChannel, int freq, int eMod, int symRate) {
		try {
			int ret = brdSrv.changeFreqById(winId, tunerMode, rfChannel, freq, eMod, symRate);
			TVCommonNative.TVLog("changeFreqById() ret " + ret);
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}
	
	public static int setDefaultVideoModeById(int winId, boolean b_mute) {
		int ret = -100;
		try {
			ret = brdSrv.setDefaultVideoModeById(winId, b_mute);
		} catch (TVMException e) {
			e.printStackTrace();
		}
		return ret;
	}
	
}


