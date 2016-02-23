package com.mediatek.mmpcm;

public interface ICommonSet {
    /**
     * get current volume
     * 
     * @param mAudioManager
     *            current mAudioManager
     * @param null
     *                       
     * @return volume value
     */
    public int getVolume();
    /**
     * get max volume
     * 
     * @param mAudioManager
     *            current mAudioManager
     * @param null
     *                       
     * @return max volume value
     */
    public int getMaxVolume();
    
    public int getMinVolume();
    /**
     * set current volume
     * 
     * @param mAudioManager
     *            current mAudioManager
     * @param volume
     *            the volume want to set           
     * @return void
     */
    public void setVolume(int volume);
    /**
     * set mute
     * 
     * @param mAudioManager
     *            current mAudioManager
     * @param null
     *                     
     * @return void
     */
    public void setMute();
    /**
     * get the mute or not
     * 
     * @param null
     *            
     * @param null
     *                      
     * @return boolean,true mean mute;false mean unmute
     */
    public boolean isMute();
    
    public int getPictureModeMin();
    public int getPictureModeMax();
    /**
     * get current picture mode
     * 
     * @param void
     *           
     * @param void
     *                      
     * @return int
     */
    public int getCurPictureMode();

    /**
     * change picture mode
     * 
     * @param type
     *          the picture type user want to set 
     * @param void
     *                      
     * @return void
     */ 
    public void setPictureMode(int type);
    
    public int getAudioEffectMin();
    public int getAudioEffectMax();
    /**
     * get current audio mode
     * 
     * @param void
     *           
     * @param void
     *                      
     * @return int
     */
    public int getCurAudioEffect();

    /**
     * change audio mode
     * 
     * @param type
     *          the audio type user want to set 
     * @param void
     *                      
     * @return void
     */ 
    public void setAudioEffect(int type);
    
    public int getScreenModeMin();
    public int getScreenModeMax();
    
    public int[] getAvailableScreenMode();
    
    /**
     * get current screen mode
     * 
     * @param void
     *           
     * @param void
     *                      
     * @return int
     */
    public int getCurScreenMode();

    /**
     * change screen mode
     * 
     * @param type
     *          the screen type user want to set 
     * @param void
     *                      
     * @return void
     */     
    public void setScreenMode(int type);
    
    public void setAudOnly(boolean on);
    
    public boolean getAudOnly();
    
    public void mmpFreeVideoResource();
    
    public void mmpRestoreVideoResource();
}
