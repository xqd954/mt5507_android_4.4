package com.mediatek.ui.menu.util;

import java.util.Locale;

import android.app.ActivityManagerNative;
import android.app.IActivityManager;
import android.app.backup.BackupManager;
import android.content.Context;
import android.content.res.Configuration;
import android.os.RemoteException;

import com.mediatek.tv.service.EventService;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcommon.TVEventCommand;
import com.mediatek.tvcommon.ITVCommon;
import com.mediatek.tvcommon.TVCommonNative;
import com.mediatek.tvcommon.CrossRoad;

public class OSDLanguage {

    IActivityManager am;
    Configuration config;
    private TVContent mTV;
	private TVEventCommand tvEventCommand;
	private ITVCommon mITVCommon;
    public OSDLanguage(Context mContext) {
		super();
		mTV=TVContent.getInstance(mContext);
		tvEventCommand = new TVEventCommand();
		mITVCommon = TVCommonNative.getDefault(mContext);
		if (null == mITVCommon) {
			throw new IllegalAccessError("Fatal Error happens when initiliaze OSDLanguage");
		}
	}

    public void setOSDLanguage(int choose) throws RemoteException {
		int mask = 0;
		int ret = 0;
		mask |= EventService.EVENT_CMD_CFG_PREF_LANG;
        try {
            am = ActivityManagerNative.getDefault();
            config = am.getConfiguration();

            switch (choose) {
            case 0:
                config.locale = Locale.US;
                tvEventCommand.setPrefLanuage(new String[] { "eng", "", "", "" });

				/* to support dtmb multilingual channel name */
				if (CrossRoad.isGUILangNeedSetToDtmb) {
					ret = mITVCommon.setDtmbGUILang("eng");
					TVCommonNative.TVLog("set \"eng\" ret: " + ret);
				} else {
					TVCommonNative.TVLog("don't need set \"eng\"");
				}
                break;
            case 1:
                config.locale = Locale.SIMPLIFIED_CHINESE;
                tvEventCommand.setPrefLanuage(new String[] { "chn", "chi", "", "" }) ;   

				/* to support dtmb multilingual channel name */
				if (CrossRoad.isGUILangNeedSetToDtmb) {
					ret = mITVCommon.setDtmbGUILang("chi");
					TVCommonNative.TVLog("set \"chi\" ret: " + ret);
				} else {
					TVCommonNative.TVLog("don't need set \"chi\"");
				}
				break;
            case 2:
                config.locale = Locale.TRADITIONAL_CHINESE;
                tvEventCommand.setPrefLanuage(new String[] { "chn", "chi", "", "" }) ;   
                break;
            }
            mask |= EventService.EVENT_CMD_DO_RESTART;
            tvEventCommand.setDoRestart(true);
            tvEventCommand.setCommandMask(mask);
            mTV.getEventManager().setCommand(tvEventCommand);
            config.userSetLocale = true;
            am.updateConfiguration(config);
            BackupManager.dataChanged("com.android.providers.settings");
        } catch (RemoteException e) {
            throw e;
        }
    }
    public boolean getOSDLanguageIsChinese(){
    	am = ActivityManagerNative.getDefault();
        try {
			config = am.getConfiguration();
			
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		if(null != config && Locale.SIMPLIFIED_CHINESE.equals(config.locale)){
			return true;
		}else{
			return false;
		}
    	
    }
}


