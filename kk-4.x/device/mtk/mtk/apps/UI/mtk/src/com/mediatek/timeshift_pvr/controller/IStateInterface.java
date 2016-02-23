/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.controller;

/**
 * 
 */
public interface IStateInterface {

	boolean onKeyDown(int keycode);

	void onResume();
	
	void onPause();

	void onStop();

	void onRelease();
}
