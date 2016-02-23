package com.mediatek.ui.mmp.util;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;

import com.mediatek.mmpcm.device.DevManager;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.mmp.model.MultiFilesManager;
import com.mediatek.ui.mmp.multimedia.MtkFilesBaseListActivity;
import com.mediatek.ui.nav.EPGActivity;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.util.DestroyApp;
import com.mediatek.ui.util.MtkLog;
import android.os.SystemProperties;
import com.mediatek.ui.mmp.util.BitmapCache;
import android.util.Log;


public class Util {
	public static String TAG = "Util";
	public static boolean PHOTO_4K2K_ON = false;
	private static Activity mActivity;
	private static boolean isMMP;
	public static final String MEDIASETTINGS = "mediasettings";
	
		public static final int	NONE = 0;
		public static final int REPEATE_ONE = 1;
		public static final int REPEATE_ALL = 2;
		
		public static final int PHOTO =0;
		public static final int AUDIO  = 1;
		public static final int VIDEO = 2;
		public static final int TXT  = 3;
	
	public static void setMediaRepeatMode(Context context,int type,int value){
		MtkLog.i("MediaPlayActivity", "setMediaRepeatMode: type:"+type+" value:"+value);
		SharedPreferences sp = context.getSharedPreferences(MEDIASETTINGS,0);
		if(sp!=null){
			SharedPreferences.Editor et = sp.edit();
			switch(type){
			case PHOTO:
				et.putInt("PHOTO", value);
				et.commit();
				break;
			case AUDIO:
				et.putInt("AUDIO", value);
				et.commit();
				break;
			case VIDEO:
				et.putInt("VIDEO", value);
				et.commit();
				break;
			case TXT:
				et.putInt("TXT", value);
				et.commit();
				break;
			default:
				break;
			}
		}

		
	}
	public static int getMediaRepeatMode(Context context,int type){
		SharedPreferences sp = context.getSharedPreferences(MEDIASETTINGS,0);
		int value = 0;
		if(sp!=null){
			switch(type){
			case PHOTO:
				value = sp.getInt("PHOTO", 0);
				break;
			case AUDIO:
				value = sp.getInt("AUDIO", 0);
				break;
			case VIDEO:
				value = sp.getInt("VIDEO", 0);
				break;
			case TXT:
				value = sp.getInt("TXT", 0);
				break;
			default:
				break;
			}
		}
		MtkLog.i("MediaPlayActivity", "getMediaRepeatMode: type:"+type+" value:"+value);
		return value;
		
	}

	static{
		Log.i("MMPUtil"," ro.mtk.4k2k.photo = "+SystemProperties.getInt("ro.mtk.4k2k.photo",0));
		if(0!=SystemProperties.getInt("ro.mtk.4k2k.photo",0) ){

			PHOTO_4K2K_ON = true;
		}
		

	}
	public static void startEPGActivity(Activity actvity){
		mActivity = actvity;
		NavIntegration mNavIntegration = NavIntegration.getInstance(mActivity);
		if (mNavIntegration.isCurrentSourceTv()) {

			if (mNavIntegration.isDTVHasChannels()) {

				if (!mNavIntegration.isCurrentSourceDTV()) {
					mNavIntegration.changeDTVSource();
				}
				if (mNavIntegration.iGetChannelLength() > 0
						&& !mNavIntegration.isCurrentSourceBlocking()) {
					
					LogicManager.getInstance(mActivity).restoreVideoResource();
					LogicManager.getInstance(mActivity).finishAudioService();
					MultiFilesManager.getInstance(mActivity).destroy();
					((DestroyApp) (mActivity).getApplication()).finishAll();
					MtkFilesBaseListActivity.reSetModel();
					((Activity) mActivity).startActivityForResult(new Intent(
							mActivity, EPGActivity.class),
							0);
					
				}
			}
		}
	}
	
	
	public static void exitMmpActivity(Context context){
		Log.i(TAG,"exitMmpActivity ~~");
		LogicManager.getInstance(context).finishAudioService();
		LogicManager.getInstance(context).finishVideo();
		DevManager.getInstance().destroy();
		MultiFilesManager.getInstance(context).destroy();
		DestroyApp destroyApp = (DestroyApp)(context).getApplicationContext();
		BitmapCache.createCache(true);
		com.mediatek.ui.mmp.gamekit.util.BitmapCache.createCache(true);
		destroyApp.finishAll();
	}
	public static void setMMPFlag(boolean flag) {
		isMMP = flag;
	}
	public static boolean getMMPFlag() {
		return isMMP;
	}
	
	public static void reset3D(Context context){
		Log.i("UTIL","reset3d");
		MenuConfigManager.getInstance(context).setValue(MenuConfigManager.VIDEO_3D_MODE,0);
	}
}
