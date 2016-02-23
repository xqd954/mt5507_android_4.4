package com.mediatek.ui.cec;

import android.R.string;
import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.Intent;

import com.mediatek.mmpcm.device.DevManager;
import com.mediatek.ui.mmp.model.MultiFilesManager;
import com.mediatek.ui.mmp.multimedia.MtkFilesBaseListActivity;
import com.mediatek.ui.nav.EPGActivity;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.util.DestroyApp;
import com.mediatek.ui.util.MtkLog;

import android.os.SystemProperties;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;


public class CecUtil {
	public static String TAG = "Util";
	public static boolean PHOTO_4K2K_ON = false;
	private static Activity mActivity;
	private static boolean isMMP;


	static{
		Log.i("MMPUtil"," ro.mtk.4k2k.photo = "+SystemProperties.getInt("ro.mtk.4k2k.photo",0));
		if(0!=SystemProperties.getInt("ro.mtk.4k2k.photo",0) ){

			PHOTO_4K2K_ON = true;
		}
		

	}
	
	public static void log(String str){
	    Log.e("TK_CEC", "cec_:"+str);
	}
	
	public static void exitMmpActivity(Context context){
	}
	public static void setMMPFlag(boolean flag) {
		isMMP = flag;
	}
	public static boolean getMMPFlag() {
		return isMMP;
	}
	
	
	public static void setPositon(int xxxx,int yyyy,Dialog dialog) {
        Window window = dialog.getWindow();
        WindowManager.LayoutParams lp = window.getAttributes();
        lp.x = xxxx;// * ScreenConstant.SCREEN_WIDTH/CecManager.tvWidth;
        lp.y = yyyy;//yoff*ScreenConstant.SCREEN_HEIGHT/CecManager.tvHeight;
        MtkLog.e("CEC", "cec:xxxx:"+xxxx+"_yyyy:"+yyyy);
        window.setAttributes(lp);
    }
    
}
