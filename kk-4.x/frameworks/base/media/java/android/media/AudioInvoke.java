package android.media;

import java.lang.reflect.Method;
import android.content.Context;
import android.util.Log;

public class AudioInvoke {
	private static final String TAG = "AudioInvoke";
    private static final boolean LOGI = true;
	
	public static void setVolume(int value,Context context) {
		try {
			Class clzSoundManager = Class.forName("com.tcl.tvmanager.TTvSoundManager");
			if(LOGI) Log.i(TAG,"Class SoundManager:" + clzSoundManager);
			
			Method mtdSoundManager$getInstance = clzSoundManager
					.getDeclaredMethod("getInstance",new Class[] { Context.class });
			if(LOGI) Log.i(TAG,"Method getInstance:" + mtdSoundManager$getInstance);
			
			Object objSoundManager = mtdSoundManager$getInstance.invoke(clzSoundManager, context);
			if(LOGI) Log.i(TAG,"Object SoundManager:" + objSoundManager);
			
			Method mtdSoundManager$setVolume = clzSoundManager
					.getDeclaredMethod("setVolume", new Class[] { int.class });
			if(LOGI) Log.i(TAG,"Method setVolume:" + mtdSoundManager$setVolume);
			
			mtdSoundManager$setVolume.invoke(objSoundManager, value);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public static int getVolume(Context context) {
		int mRet = 0;
		try {
			Class clzSoundManager = Class.forName("com.tcl.tvmanager.TTvSoundManager");
			if(LOGI) Log.i(TAG,"Class SoundManager:" + clzSoundManager);
			
			Method mtdSoundManager$getInstance = clzSoundManager
					.getDeclaredMethod("getInstance",new Class[] { Context.class });
			if(LOGI) Log.i(TAG,"Method getInstance:" + mtdSoundManager$getInstance);
			
			Object objSoundManager = mtdSoundManager$getInstance.invoke(clzSoundManager, context);
			if(LOGI) Log.i(TAG,"Object SoundManager:" + objSoundManager);
			
			Method mtdSoundManager$getVolume = clzSoundManager
					.getDeclaredMethod("getVolume");
			if(LOGI) Log.i(TAG,"Method getVolume:" + mtdSoundManager$getVolume);
			
			mRet = (Integer)mtdSoundManager$getVolume.invoke(objSoundManager);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return mRet;
	}
	
	public static void setMute(boolean mute,Context context) {
		try {
			Class clzSoundManager = Class.forName("com.tcl.tvmanager.TTvSoundManager");
			if(LOGI) Log.i(TAG,"Class SoundManager:" + clzSoundManager);
			
			Method mtdSoundManager$getInstance = clzSoundManager
					.getDeclaredMethod("getInstance",new Class[] { Context.class });
			if(LOGI) Log.i(TAG,"Method getInstance:" + mtdSoundManager$getInstance);
			
			Object  objSoundManager = mtdSoundManager$getInstance.invoke(clzSoundManager, context);
			if(LOGI) Log.i(TAG,"Object SoundManager:" + objSoundManager);
			
			Method mtdSoundManager$setAudioMuteEnabled = clzSoundManager
					.getDeclaredMethod("setAudioMuteEnabled", new Class[] { boolean.class });
			if(LOGI) Log.i(TAG,"Method setAudioMuteEnabled:" + mtdSoundManager$setAudioMuteEnabled);
			
			mtdSoundManager$setAudioMuteEnabled.invoke(objSoundManager, mute);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public static boolean isMute(Context context) {
		boolean mRet = false;
		try {
			Class clzSoundManager = Class.forName("com.tcl.tvmanager.TTvSoundManager");
			if(LOGI) Log.i(TAG,"Class SoundManager:" + clzSoundManager);
			
			Method mtdSoundManager$getInstance = clzSoundManager
					.getDeclaredMethod("getInstance",new Class[] { Context.class });
			if(LOGI) Log.i(TAG,"Method getInstance:" + mtdSoundManager$getInstance);
			
			Object  objSoundManager = mtdSoundManager$getInstance.invoke(clzSoundManager, context);
			if(LOGI) Log.i(TAG,"Object SoundManager:" + objSoundManager);
			
			Method mtdSoundManager$getAudioMuteEnabled = clzSoundManager
					.getDeclaredMethod("getAudioMuteEnabled");
			if(LOGI) Log.i(TAG,"Method getAudioMuteEnabled:" + mtdSoundManager$getAudioMuteEnabled);
			
			mRet = (Boolean)mtdSoundManager$getAudioMuteEnabled.invoke(objSoundManager);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return mRet;
	}
}
