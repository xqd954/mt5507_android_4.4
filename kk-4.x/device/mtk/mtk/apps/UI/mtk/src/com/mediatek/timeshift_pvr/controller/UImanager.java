/**   
* @Description: TODO()
*/
package com.mediatek.timeshift_pvr.controller;

import android.app.Activity;

import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.timeshift_pvr.ui.BaseInfoBar;
import com.mediatek.timeshift_pvr.ui.CommonInfoBar;

/**
 *
 */
public class UImanager{

	private Activity mActivity;
	private BaseInfoBar mPopup;
	public static boolean showing = false;
	
	public UImanager(Activity mActivity) {
		super();
		this.mActivity = mActivity;
	}

	/*
	 * 
	 */
	public void onPause() {
		// TODO Auto-generated method stub
		
	}

	/*
	 * 
	 */
	public void onResume() {
		// TODO Auto-generated method stub
		
	}

	/*
	 * 
	 */
	public void hiddenAllViews() {
		// TODO Auto-generated method stub
		mPopup.dismiss();
	}

	public void showInfoBar(String info,Long duration){
		if(mPopup!=null){
			mPopup.dismiss();
			mPopup=null;
		}
		
		mPopup = new CommonInfoBar(mActivity);
		mPopup.setDuration(duration);
		Util.Hlog("2--------");
		mPopup.show();
	}
	
	public void showInfoBar(String info){
		if(mPopup!=null){
			mPopup.dismiss();
			mPopup=null;
		}
		
		mPopup = new CommonInfoBar(mActivity,info);
		Util.Hlog("3---------");
		mPopup.show();
		showing = true;
    }

    public void dissmiss() {
        // TODO Auto-generated method stub
        if (null!=mPopup) {
            mPopup.dismiss();
            showing = false; 
        }
    }
	
}
