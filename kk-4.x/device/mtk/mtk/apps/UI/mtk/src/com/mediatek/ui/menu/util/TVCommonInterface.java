package com.mediatek.ui.menu.util;

import android.content.Context;

import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVInputManager;
import com.mediatek.tvcm.TVOutput;
import com.mediatek.tvcm.TVStorage;
import com.mediatek.tvcommon.TVConfigurer;
import com.mediatek.tvcommon.TVOutputCommon.AdjustListener;
import com.mediatek.ui.util.MtkLog;

/**
 * this class defined some TV common interface except channel related. Methods
 * 
 * @author MTK40462
 * 
 */
public class TVCommonInterface {

	private static final String TAG = "TVCommonInterface";
	private Context mContext;
	private static TVCommonInterface tvCommon;

	private TVInputManager mInputManager;
	private TVContent tv;
	private TVOutput mOutput;
	private TVStorage mStorage;
	TVConfigurer mtkCfg;

	public static final int AUTO_ADJUST = TVOutput.AUTO_ADJUST;
	public static final int AUTO_PHASE = TVOutput.AUTO_PHASE;
	public static final int AUTO_COLOR = TVOutput.AUTO_COLOR;

	TVCommonInterface(Context context) {
		this.mContext = context;
		tv = TVContent.getInstance(context);
		mInputManager = tv.getInputManager();
		mStorage = tv.getStorage();
		mtkCfg = tv.getConfigurer();
	}

	public static synchronized TVCommonInterface getInstance(Context context) {
		if (tvCommon == null) {
			tvCommon = new TVCommonInterface(context);
		}
		return tvCommon;
	}

	/**
	 * auto adjust VGA. when input source is set to VGA, this config will be
	 * visible in Video
	 * 
	 * @param listener
	 * @deprecated
	 */
	public void autoAdjustVGA(AdjustListener listener) {
		mOutput = mInputManager.getOutput("main");
		if(mOutput != null){
			mOutput.adjust(listener);
		}	
	}

	public void autoAdjust(int item, AdjustListener listener) {
		mOutput = mInputManager.getOutput("main");
		if(mOutput != null){
			mOutput.adjust(listener, item);
		}	
	}

	/**
	 * get model name of version info
	 * 
	 * @return model name
	 */
	public String getVerModelName() {
		MtkLog.i(TAG, " Model name: " + mtkCfg.getModelname());
		return mtkCfg.getModelname();
	}

	/**
	 * get version info
	 * 
	 * @return version
	 */
	public String getVer() {
		MtkLog.i(TAG, " Version: " + mtkCfg.getVersion());
		return mtkCfg.getVersion();
	}

	/**
	 * get serial number
	 * 
	 * @return serial number
	 */
	public String getVerSerialNum() {
		MtkLog.i(TAG, " Serial Number: " + mtkCfg.getSerialNum());
		return mtkCfg.getSerialNum();
	}

	/**
	 * select power on capture logo
	 * 
	 * @param value
	 *            capture logo save position
	 */
	public void selectCaptureLogo(int value) {
		int location = 0;
		if (value < 0 || value > 3) {
			value = 0;
		}
		switch (value) {
		case 0:
			location = CAPTURE_LOCATION.LOCATION_DEF;
			break;
		case 1:
			location = CAPTURE_LOCATION.LOCATION_0;
			break;
		case 2:
			location = CAPTURE_LOCATION.LOCATION_1;
			break;
		case 3:
			location = CAPTURE_LOCATION.LOCATION_NONE;
			break;
		}
		mStorage.setBootLogo(TVStorage.CAP_LOGO_TV, location);
		MtkLog.i(TAG, "Capture Logo Select[" + value + "] " + location);

	}

	class CAPTURE_LOCATION {
		static final int LOCATION_DEF = 0x100;
		static final int LOCATION_0 = 0;
		static final int LOCATION_1 = 1;
		static final int LOCATION_NONE = 0xff;

	}

	public void resetKeyDelayTime(String itemId) {
	/*	int delay = wmImpl.getKeyDelay();
		if (itemId != null && itemId.equals(MenuConfigManager.TV_MTS_MODE)) {
			wmImpl.setKeyDelay(250);
			MtkLog.d(TAG, " set key delay --- 250");
		} else {
			if (delay != 50) {
				wmImpl.setKeyDelay(50);
				MtkLog.d(TAG, " set key delay --- 50");
			}
		}*/
	}

}
