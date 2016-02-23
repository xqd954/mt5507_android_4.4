package com.mediatek.mmpcm;

import com.mediatek.tv.TVManager;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.service.ConfigService;
import com.mediatek.tvcommon.TVConfigurer;
import com.mediatek.tvcommon.TVMethods;
import com.mediatek.tvcommon.TVOptionRange;
import android.util.Log;

import android.content.Context;

public class CommonSet implements ICommonSet {
	public static final String TAG = "[CommonSet] ";

    public static final int VID_SCREEN_MODE_AUTO = 0;
    public static final int VID_SCREEN_MODE_NORMAL = 1;
    public static final int VID_SCREEN_MODE_LETTER_BOX = 2;
    public static final int VID_SCREEN_MODE_PAN_SCAN = 3;
    public static final int VID_SCREEN_MODE_NON_LINEAR_ZOOM = 4;
    public static final int VID_SCREEN_MODE_DOT_BY_DOT = 5;

    private int volumeMax;
    private int volumeMin;
    private static CommonSet mmpCom = null;
    private TVMethods mTV;
    private TVOptionRange<Integer> volumeOption;
    private TVOptionRange<Integer> picModeOption;
    private TVOptionRange<Integer> audModeOption;
    private TVOptionRange<Integer> screenModeOption;
    private TVOptionRange<Integer> audOnlyOption;
    private TVOptionRange<Integer> blueMuteOption;
    private TVConfigurer tvcfg;
    private ConfigService mmpCfg;
    private int blackmute;

    private CommonSet(Context context) {
        mTV = TVMethods.getInstance(context); //add by lei 12-13

        TVManager tvManager = TVManager.getInstance(null);
        mmpCfg = (ConfigService) tvManager
                .getService(ConfigService.ConfigServiceName);

        tvcfg = TVConfigurer.getInstance(context);
        volumeOption = (TVOptionRange<Integer>) tvcfg
                .getOption(ConfigType.CFG_VOLUME);
        picModeOption = (TVOptionRange<Integer>) tvcfg
                .getOption(ConfigType.CFG_PICTURE_MODE);
        audModeOption = (TVOptionRange<Integer>) tvcfg
                .getOption(ConfigType.CFG_AUDIO_MODE);
        screenModeOption = (TVOptionRange<Integer>) tvcfg
                .getOption(ConfigType.CFG_SCREEN_MODE);
        audOnlyOption = (TVOptionRange<Integer>) tvcfg
                .getOption(ConfigType.CFG_AUDIO_ONLY);
        blueMuteOption = (TVOptionRange<Integer>) tvcfg
        .getOption(ConfigType.CFG_BLUE_SCREEN);

		if(volumeOption != null){
        		this.volumeMax = volumeOption.getMax();
        		this.volumeMin = volumeOption.getMin();
			}
    }

    public static CommonSet getInstance(Context context) {
        if (mmpCom == null) {
            synchronized (CommonSet.class) {
                //if (mmpCom == null) {
                    mmpCom = new CommonSet(context);
                //}
            }
        }
        return mmpCom;
    }
    /**
     * Get volume value
     * @return
     * 
     */
    public int getVolume() {
        return volumeOption.get();
    }
    /**
     * Get max volume value
     * @return
     */
    public int getMaxVolume() {
        return this.volumeMax;
    }
    /**
     * Get min volume value
     * @return
     */
    public int getMinVolume() {
        return this.volumeOption.getMin();
    }
    /**
     * Set volume value
     * @return
     */
    public void setVolume(int volume) {
        int curVolume;

        if (volume <= this.volumeMax) {
            curVolume = volume;
        } else {
            curVolume = this.volumeMax;
        }
        volumeOption.set(curVolume);
    }
    /**
     * Set Mute or UnMute, 
     * if the state is mute, call it will unmute, else mute.
     * @return
     */
    public void setMute() {
        if (mTV.isMute() == true) {
            mTV.setAudioMute(false);
        } else {
            mTV.setAudioMute(true);
        }
    }
    /**
     * Check the mute state, 
     * Returns: true, mute, false unmute.
     * @return
     */
    public boolean isMute() {
        return mTV.isMute();
    }
    /**
     * Get picture mode min value.
     * @return
     */
    public int getPictureModeMin() {
        return picModeOption.getMin();
    }
    /**
     * Get picture mode max value.
     * @return
     */
    public int getPictureModeMax() {
        return picModeOption.getMax();
    }
    /**
     * Get current picture mode
     * @return
     */
    public int getCurPictureMode() {
        return picModeOption.get();
    }
    /**
     * Set picture mode
     * @return
     */
    public void setPictureMode(int type) {
        picModeOption.set(type);
    }
    /**
     * Get audio effect min value.
     * @return
     */
    public int getAudioEffectMin() {
        return audModeOption.getMin();
    }
    /**
     * Get audio effect max value.
     * @return
     */
    public int getAudioEffectMax() {
        return audModeOption.getMax();
    }
    /**
     * Get current audio effect value.
     * @return
     */
    public int getCurAudioEffect() {
        return audModeOption.get();
    }
    /**
     * Set current audio effect value.
     * @return
     */
    public void setAudioEffect(int type) {
        audModeOption.set(type);
    }
    /**
     * Get screen mode min value.
     * @return
     */
    public int getScreenModeMin() {
        return screenModeOption.getMin();
    }
    /**
     * Get screen mode max value.
     * @return
     */
    public int getScreenModeMax() {
        return screenModeOption.getMax();
    }
    /**
     * Get available Screen mode.
     * 
     * @return
     * VID_SCREEN_MODE_AUTO
     * VID_SCREEN_MODE_NORMAL
     * VID_SCREEN_MODE_LETTER_BOX
     * VID_SCREEN_MODE_PAN_SCAN
     * VID_SCREEN_MODE_NON_LINEAR_ZOOM
     * VID_SCREEN_MODE_DOT_BY_DOT
     */
    public int[] getAvailableScreenMode() {
        int[] scrMode = new int[6];
        scrMode[0] = VID_SCREEN_MODE_AUTO;
        scrMode[1] = VID_SCREEN_MODE_NORMAL;
        scrMode[2] = VID_SCREEN_MODE_LETTER_BOX;
        scrMode[3] = VID_SCREEN_MODE_PAN_SCAN;
        scrMode[4] = VID_SCREEN_MODE_NON_LINEAR_ZOOM;
        scrMode[5] = VID_SCREEN_MODE_DOT_BY_DOT;

        int tmp[] = new int[6];
        
        for(int i = 0; i < 6; i++){
            tmp[i] = -1;
        }

        try {
            int allMode = mmpCfg.getCfg(ConfigType.CFG_GET_ALL_SCREEN_MODE)
                    .getIntValue();

            for (int i = 0; i < 6; i++) {
                if (((allMode >> i) & 1) != 0) {
                    tmp[i] = scrMode[i];
                }                                  
            }

        } catch (TVMException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        return tmp;
    }
    /**
     * Get current Screen mode.
     * @return,VID_SCREEN_MODE_AUTO
     * VID_SCREEN_MODE_NORMAL
     * VID_SCREEN_MODE_LETTER_BOX
     * VID_SCREEN_MODE_PAN_SCAN
     * VID_SCREEN_MODE_NON_LINEAR_ZOOM
     * VID_SCREEN_MODE_DOT_BY_DOT
     */
    public int getCurScreenMode() {
        int mode = screenModeOption.get();

        switch (mode) {
        case ConfigType.SCREEN_MODE_CUSTOM_DEF_0:
            return VID_SCREEN_MODE_AUTO;

        case ConfigType.SCREEN_MODE_NORMAL:
            return VID_SCREEN_MODE_NORMAL;

        case ConfigType.SCREEN_MODE_LETTERBOX:
            return VID_SCREEN_MODE_LETTER_BOX;

        case ConfigType.SCREEN_MODE_PAN_SCAN:
            return VID_SCREEN_MODE_PAN_SCAN;

        case ConfigType.SCREEN_MODE_NON_LINEAR_ZOOM:
            return VID_SCREEN_MODE_NON_LINEAR_ZOOM;

        case ConfigType.SCREEN_MODE_DOT_BY_DOT:
            return VID_SCREEN_MODE_DOT_BY_DOT;

        default:
            return VID_SCREEN_MODE_AUTO;
        }
    }
    /**
     * Set screen mode
     * @param: the screen type user want to set .
     */
    public void setScreenMode(int type) {

        int mode = ConfigType.SCREEN_MODE_CUSTOM_DEF_0;

        switch (type) {
        case VID_SCREEN_MODE_AUTO:
            mode = ConfigType.SCREEN_MODE_CUSTOM_DEF_0;
            break;
            
        case VID_SCREEN_MODE_NORMAL:
            mode = ConfigType.SCREEN_MODE_NORMAL;
            break;

        case VID_SCREEN_MODE_LETTER_BOX:
            mode = ConfigType.SCREEN_MODE_LETTERBOX;
            break;

        case VID_SCREEN_MODE_PAN_SCAN:
            mode = ConfigType.SCREEN_MODE_PAN_SCAN;
            break;

        case VID_SCREEN_MODE_NON_LINEAR_ZOOM:
            mode = ConfigType.SCREEN_MODE_NON_LINEAR_ZOOM;
            break;

        case VID_SCREEN_MODE_DOT_BY_DOT:
            mode = ConfigType.SCREEN_MODE_DOT_BY_DOT;
            break;

        default:
            mode = ConfigType.SCREEN_MODE_CUSTOM_DEF_0;
            break;
        }

        screenModeOption.set(mode);
    }
    /**
     * Set audio only, tv power-saving state
     * @param, true, OFF, false, ON.
     * 
     */
    public void setAudOnly(boolean on) {
        if (on == true) {
            audOnlyOption.set(ConfigType.COMMON_OFF);
        } else {
            audOnlyOption.set(ConfigType.COMMON_ON);
        }
    }
    /**
     * Get current TV power-saving state
     * @return ture, OFF; false, ON.
     */
    public boolean getAudOnly() {
        int audOnly = audOnlyOption.get();

        if (audOnly == ConfigType.COMMON_OFF) {
            return true;
        } else {
            return false;
        }
    }
	/**
	 * Free all video resource.
	 */
	public void mmpFreeVideoResource() {
		System.out.println(TAG + "mmpFreeVideoResource() return directly. when tv --> mmp, tv has set black mute. mmp need do nothing ");
		return;
		// don't need do following actions, which will cause blue screen flash when exit mmp to tv
		// besides, it's dangerous to modify the blue mute cfg, which is not allowed by cfg owner
		/*
		blackmute = blueMuteOption.get();
		if(blueMuteOption.get() != 1){
			return;
		}
		MmpTool.LOG_DBG("blackmute = " + blackmute);
		blueMuteOption.set(0);
		mTV.setVideoBlueMute(false);
		*/
	}

	/**
	 * Save all video resource.
	 */
	public void mmpRestoreVideoResource() {
		System.out.println(TAG + "mmpRestoreVideoResource() return directly. when tv --> mmp, tv has set black mute. mmp need do nothing ");
		return;
		/*
		MmpTool.LOG_DBG("blackmute = " + blackmute);
		Log.d(TAG,"mmpRestoreVideoResource blackmute = " + blackmute);
		blueMuteOption.set(blackmute);
		if (blackmute == 0) {
			mTV.setVideoBlueMute(false);
		} else if (blackmute == 1) {
			mTV.setVideoMute();
		}
		*/
	}
}
