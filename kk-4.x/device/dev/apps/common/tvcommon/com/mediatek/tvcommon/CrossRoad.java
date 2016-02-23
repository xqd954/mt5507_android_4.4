package com.mediatek.tvcommon;

import android.os.SystemProperties;

public class CrossRoad {

	/** option for different flow */

	/**
	 * isOnlyCableAtv
	 * true:  only 1 copy of atv channel stored in cable svl (svl id 2)
	 * false: original turnkey flow. 2 copys of atv channel stored in air, cable svl separately. 
	 */
	public static boolean isOnlyCableAtv = true;
	static {
		TVCommonNative.TVLog("mtk.isOnlyCableAtv = " + SystemProperties.getBoolean("mtk.isOnlyCableAtv", true));
		if(false == SystemProperties.getBoolean("mtk.isOnlyCableAtv", true)) {
			isOnlyCableAtv = false;
		}
	}


	/**
	 * isForceBlackMute(Main/Sub)
	 * true:  set main(sub) output mute color to black. Usually used when blue mute cfg is on. 
	 * false: do not set main(sub) output mute color to black. Don't need do more actions.
	 */
	public static boolean isForceBlackMuteMain = false;
	public static boolean isForceBlackMuteSub  = false;


	/**
	 * show input/channel lock details or not
	 */
	public static boolean isShowBlockDetails = false;


	/**
	 * show dtmb need ap to set GUI Language or not
	 */
	public static boolean isGUILangNeedSetToDtmb = false;
	
}

