/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.test;

import android.content.Context;

/**
 * 
 */
public interface IManager {

	/**
	 * @return
	 */
	boolean hasRemovableDisk();

	/**
	 * @return
	 */
	boolean diskIsReady();

	/**
	 * @return
	 */
	boolean timeShiftIsEnable();

	/**
	 * @return
	 */
	int getTshiftFileSize();

	/**
	 * @return
	 */
	void setTshiftFileSize(int size);

	void setDiskSpeed(float speed);

	float getDiskSpeed();

	/**
	 * when timeshift not support on this module. return false.
	 * 
	 * @return
	 */
	boolean isSupportTimeShift();

	/**
	 * @param keyCode
	 * @return
	 */
	boolean onKeyDown(int keyCode);

	/**
	 * @param e
	 */
	void registListener(Context e);

	/**
	 * @param errorCode
	 */
	void onErrorHappened(int errorCode);

	/**
	 * @return isDTV ?
	 */
	boolean isDTVSource();

	/**
	 * @return hasSignal?
	 */
	boolean hasDTVSignal();

	boolean dtvNotReady();

	/**
	 * @return isPipPopMode ? false : true;
	 */
	boolean isPipPopMode();

	/**
	 * @return isPipPopMode ? false : true;
	 */
	boolean isScanning();

	/**
	 * match with activity's lift cycle.
	 */
	void onResume();

	/**
	 * match with activity's lift cycle.
	 */
	void onPause();

	/**
	 * match with activity's lift cycle.
	 */
	void onStop();

	/**
	 * Restore the statte to default state:StateNormal
	 */
	void restoreToDefault();

	/**
	 * @param keycode
	 * @return
	 */
	boolean hasFreeSize(int keycode);
}
