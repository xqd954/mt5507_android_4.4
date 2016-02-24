/* 
 * Copyright (C) 2006 The Android Open Source Project 
 *  
 * ?010-2013 TCL CORPORATION All Rights Reserved. 
 */
package com.tcl.media;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import com.tcl.media.SubtitleTrackInfo;
//import com.tcl.media.VideoCodecInfo;

//import com.tcl.media.TTvSoundManager;
//import com.tcl.media.TAtvPlayer;

import android.graphics.Bitmap;
import android.media.MediaPlayer;
import android.media.Metadata;
import android.os.Parcel;
import android.os.ParcelFileDescriptor;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;

//import com.tcl.tvapi.atv.util.*;
//import com.tcl.tvapi.atv.*;
public class TMediaPlayer extends MediaPlayer {
    static {
        System.loadLibrary("tmedia_jni");
        native_init();
    }
    
    private final static String TAG = "TMediaPlayer";
    private int mNaticeSubtitleSurfaceTexture;

    private SurfaceHolder mSubtitleSurfaceHolder;

    private static final int KEY_PARAMETER_DEMUX_RESET = 2000;
    private static final int KEY_PARAMETER_SET_SEAMLESS_MODE = 2001;
    private static final int KEY_PARAMETER_DATASOURCE_SWITCH = 2002;
    private static final int KEY_PARAMETER_PAYLOAD_SHOT = 2003;
    private static final int KEY_PARAMETER_SWITCH_TO_PUSH_DATA_MODE = 2004;
    private static final int KEY_PARAMETER_CHANGE_PROGRAM = 2005;
    private static final int KEY_PARAMETER_MEDIA_CREATE_THUMBNAIL_MODE = 2006;

    private int mDataSourcePlayerType=0;
    private int mDataSourceContentType=0;
    private int mDataSourceVideoCodec=0;
    private int mDataSourceAudioCodec=0;
    private int mDataSourceMediaFormat=0;
    private int mDataSourceAppType=0;

    public static final int TMEDIA_ERROR_NO_AUDIO = 5000;
    public static final int TMEDIA_ERROR_FILE_DESTROY = 5001;
    public static final int TMEDIA_ERROR_CODEC_NOT_SUPPORT = 5002;
    public static final int TAUDIO_NOT_SUPPORT = 5003;
    public static final int TVIDEO_NOT_SUPPORT = 5004;
    public static final int TAUDIO_VIDEO_NOT_SUPPORT = 5005;


    //kangh@tcl 2013.05.13
		private int mStreamNum = 1;
		private int mEnable = 0;
		private int mTextEncoding = 1000;
		private int mTextColor = 0;
		private int mFontSize = 16;

    /** A new set of subtitle is available.
     * @see android.media.MediaPlayer.OnInfoListener
     */
    private static final int MEDIA_INFO_SUBTITLE_UPDATA = 1000;
    
    //TTvSoundManager:
    public  final static int SOURCE_PREMIXER_KTV_MIC_IN = 0;  
	  public  final static int SOURCE_PREMIXER_KTV_MP3_IN = 1;  
	  public  final static int INTPUT_SOURCE_KTV = 3;
	  
	 // private TTvSoundManager myTTvSoundManager = null;
	//  private TAtvPlayer myTAtvPlayer = null;

    private int[] AiduoTrackIndex;
    private int[] SubTitleTrackIndex;

    private int mSubTotalNum = 0; 
    private int mAudTotalNum = 0;
          
    public TMediaPlayer() {
    	super();
        Log.i(TAG,"TMediaPlayer::TMediaPlayer()");
        AiduoTrackIndex = new int[20]; //max 10
	SubTitleTrackIndex = new int[20];//max 10
    	//myTTvSoundManager = new TTvSoundManager();
    	//myTAtvPlayer = new TAtvPlayer();
        Log.i(TAG,"setPlayerType 6 -- cmpb");
	super.setPlayerType(6);
    }

    /**
    * To support extended features in the STAOS,two API are added.
    * Gets audio track information using the Audio TrackInfo structure.
    *
    * @return the information .
    */
    // private native void native_getAudioTrackInfo(boolean type, Parcel audioTrackInfo);

    /**
     * To get the string for audiotrack
     * @param infoType 1:title;2:album;3:artist;
     */
    // private native String getAudioTrackStringData(int infoType);

     
     /**      
     * Gets audio track information using the Audio TrackInfo structure.
     * @author   TCL TVOS Team 
     * @param    null
     * @return   AudioTrackInfo. null if an error occured.
     */
    public AudioTrackInfo getAudioTrackInfo() {
       	//int[]data = getAudioTrackInfo(-1);
       	int[] data = new int[4];        
        data[1] = getAllAudioTrackCount();
        AudioTrackInfo info = new AudioTrackInfo();
        info.initAudioTrackInfo(this,data);
        return info;
    }

    public int getAllAudioTrackCount() {        
        Log.i(TAG, "getAllAudioTrackCount()");        
                        
        android.media.AudioTrackInfo[] info = super.getAllAudioTrackInfo(true);
        int totalCount = info.length;
        
        return totalCount;   
    }

    public int[] getAudioTrackInfo(int idx) {
       	//int[]data = getAudioTrackInfo(-1);
       	int[] data = new int[4];        
        data[1] = getAllAudioTrackCount(); 
        return data;
    }

    /**
     * Gets subtitletrack information.
     * @param trackIndex the track id which you want to get information
     * @param subtitleTrackInfo holds the information returned
     * @return the information .
     */
   // private native void native_getSubtitleTrackInfo(final int trackIndex, Parcel subtitleTrackInfo);

    /**
     * @param trackIndex the track id which you want to get information
     * @return The metadata, possibly empty. null if an error occured.
     */
    //kangh@tcl 2013.05.14
    /*
    	private SubtitleTrackInfo getSubtitleTrackInfo(final int trackIndex) {
       	int[]data = getSubtitleInfo(trackIndex);
        	SubtitleTrackInfo info = new SubtitleTrackInfo(data);
        	return info;
    	}
	*/

    /**
     * Gets all of the subtitletracks' information.
     * @param subtitleTrackInfo holds the information returned
     * @return the subtitle number
     */
    //private native int native_getAllSubtitleTrackInfo(Parcel subtitleTrackInfo);

    //kangh@tcl 2013.05.15    
    /**      
	   * Gets all of the subtitletracks' information.
	   * @author   TCL TVOS Team 
	   * @param    null
	   * @return   SubtitleTrackInfo. null if an error occured.
	   */
    public SubtitleTrackInfo getAllSubtitleTrackInfo() {
		
        int[] data = new int[4];        
        data[1] = getAllSubtitleTrackCount();
        SubtitleTrackInfo info = new SubtitleTrackInfo();
        info.initSubtitleTrackInfo(this,data);
        return info;
    }

      public int getAllSubtitleTrackCount() {        
        Log.i(TAG, "getAllSubtitleTrackCount()");        
                        
        android.media.SubtitleTrackInfo[] info = super.getAllSubtitleTrackInfoEx();
        int totalCount = info.length;
        
       return totalCount;   
    }
    /**      
	   * Set audio track with track number that you want to play.
	   * @author   TCL TVOS Team 
	   * @param    track track number that you want to play.
	   * @return   null
	   */
   public void setAudioTrack(int track){
        int currenttrack = track -1;
        Log.i(TAG, "setAudioTrack()"+currenttrack);      
        super.setAudioTrackEx(currenttrack);
    
    }
    //kangh@tcl 2013.05.14
    /**      
	   * Set subtitle track with track number that you want to play.
	   * @author   TCL TVOS Team 
	   * @param    track track number that you want to play.
	   * @return   null
	   */ 
    public void setSubtitleTrack(int track){
        int currenttrack = track -1;
        Log.i(TAG, "setSubtitleTrack()"+currenttrack);      
        super.setSubtitleTrackEx(currenttrack);
	
    }

    @Override
    public void setDataSource(String path)
            throws IOException, IllegalArgumentException, SecurityException, IllegalStateException {
        Log.i(TAG, "setDataSource = " + path);
        super.setDataSource(path);
        super.setSubtitleDataSource(path);
    }
    //kangh@tcl 2013.5.14 
    /**      
	   * Set the Content URI of the data you want to play.
	   * @author   TCL TVOS Team 
	   * @param    Uri the Content URI of the data you want to play.
	   * @return   null
	   */
//		public void setSubtitleDataSource(String Uri){
//		setExtSubtitlePath(Uri);
//		}
		
    /**      
	   * Set PlayMode.
	   * @author   TCL TVOS Team 
	   * @param    param speed the forward/backword speed which is -32, -16, -8, -4, -2, *  2, 4, 8, 16, 32.
	   * @return   True if set PlayMode is OK, else false.

	   */
//    private native boolean setPlayMode(int speed);

    /**      
	   * get PlayMode.
	   * @author   TCL TVOS Team 
	   * @param    null
	   * @return   speed information for application Layer: -32, -16, -8, -4, -2, *  2, 4, 8, 16, 32.
	   */
//    private native int getPlayMode();

    //kangh@tcl 2013.05.14
    /**      
	   * turn on the subtitle.
	   * @author   TCL TVOS Team 
	   * @param    null
	   * @return   null.
	   */
    public void onSubtitleTrack(){
         Log.i(TAG, "onSubtitleTrack");      
    	 super.onSubtitleTrackEx();
    }

    //kangh@tcl 2013.05.14
    /**      
	   * turn off the subtitle.
	   * @author   TCL TVOS Team 
	   * @param    null
	   * @return   null.
	   */
    public void offSubtitleTrack(){
        Log.i(TAG, "offSubtitleTrack");      
    	super.offSubtitleTrackEx();
    }

    /**
    * Get subtitle text data
    * @return return the subtitle text data
    */
   // private native String getSubtitleData();

    //private native void _setSubtitleSurface(Surface surface);
   
    private void updateSubtitleSurfaceScreenOn() {
        if (mSubtitleSurfaceHolder != null) {
            mSubtitleSurfaceHolder.setKeepScreenOn(false);
        }
    }
    /**
    * Sets the SurfaceHolder to use for displaying the subtitle portion of the media.
    */
    private void setSubtitleDisplay(SurfaceHolder sh) {

    }

    private static native final void native_init();
    //private native int setSubtitleSync(int timeMs);
    //private native int native_getAudioCodecType(Parcel pstAudioCodecType);
    private String getAudioCodecType() {
        Parcel reply = Parcel.obtain();
        Metadata data = new Metadata();

       // native_getAudioCodecType(reply);

        if (!data.parse(reply)) {
            Log.w(TAG, "parse error!");
            reply.recycle();
            return null;
        }

        String info = data.getString(Metadata.AUDIO_CODEC);
        reply.recycle();

        return info;
    }
    //add API for diandu app by HQS@TCL 130731---start 
    /**      
	   * set the player attribute.
	   * @author   TCL TVOS Team 
	   * @param    Seamless 
	   * @return   null.
	   */
    public void _setPlaySeamless(boolean Seamless){
        Log.i(TAG,"---_setPlaySeamless---");
//	setPlaySeamless(Seamless);
    }
    //add API for diandu app by HQS@TCL 130731---end
    
    //add API for SoundManager by kangh@tcl 2013.8.19 ---start
    /**      
	   * open TTvSoundManager.
	   * @author   TCL TVOS Team 
	   * @param    null 
	   * @return   True if openTTvSoundManager is OK, else false..
	   */
    public boolean openTTvSoundManager(){
    	//myTTvSoundManager = new TTvSoundManager();
    	//myTAtvPlayer = new TAtvPlayer();
      return false;
    }
    
    private void closeTTvSoundManager(){
    	//myTTvSoundManager = null;
    }
    /**      
	   * set MicrophoneMute.
	   * @author   TCL TVOS Team 
	   * @param    on true is Mute.
	   * @return   True if setMicrophoneMute is OK, else false..
	   */    
    public boolean setMicrophoneMute(boolean on) {
    	//boolean m;
    	//Log.i(TAG,"-setMicrophoneMute--myTTvSoundManager---:" +  myTTvSoundManager);
    	//if(myTTvSoundManager == null)
    	//	 myTTvSoundManager = new TTvSoundManager();
    	//m = myTTvSoundManager.setMicrophoneMute(on);
    	//Log.i(TAG,"-setMicrophoneMute--m---:" +  m);
      return false;
    }
    /**      
	   * set InputLevel.
	   * @author   TCL TVOS Team 
	   * @param    AudioInputSourceType  Audio InputSource Type.
	   * @level    level.	   
	   * @return   True if setMicrophoneMute is OK, else false.
	   */     	
    public boolean setInputLevel(int AudioInputSourceType  , int level) {
    	//boolean m;
    	//Log.i(TAG,"-setInputLevel---this:" +  this);
    	//Log.i(TAG,"-setInputLevel--in---  myTTvSoundManager:" + myTTvSoundManager+"  "+ level );
    	//if(myTTvSoundManager == null)
    	//	 myTTvSoundManager = new TTvSoundManager();
    	//m = myTTvSoundManager.setInputLevel(AudioInputSourceType, level);
    	//Log.i(TAG,"-setInputLevel--out---:" +  level);
    	return false;
    }
    /**      
	   * get InputLevel.
	   * @author   TCL TVOS Team 
	   * @param    AudioInputSourceType  Audio InputSource Type.  
	   * @return   level.
	   */      
    public int getInputLevel(int AudioInputSourceType) {
    	//if(myTTvSoundManager == null)
    	//	 myTTvSoundManager = new TTvSoundManager();
    	//return myTTvSoundManager.getInputLevel(AudioInputSourceType);
         return 0;
   	}
    /**      
	   * get AudioTrackNs.
	   * @author   TCL TVOS Team 
	   * @param    null.  
	   * @return   AudioTrackNs.
	   */    	
   	private int getAudioTrackNs() {
   		//int[]data = getAudioTrackInfo(-1);
                //return data[2];
                return 0;
   	}
    /**      
	   * set InputSource.
	   * @author   TCL TVOS Team 
	   * @param    inputSource input Source.  
	   * @return   True if setInputSource is OK, else false..
	   */       	
   	public  boolean setInputSource(int inputSource){
   		//if(INTPUT_SOURCE_KTV == inputSource)
   		//{
   		//   if(myTAtvPlayer == null)
    		// 		myTAtvPlayer = new TAtvPlayer();
    		//return myTAtvPlayer.setInputSource(EnTCLInputSource.EN_TCL_KTV);
    	//}
    	return false;
    }
   	
   	//add API for SoundManager by kangh@tcl 2013.8.19 ---end

    /**      
	   * checkCurrentStreamIsDolby().
	   * @author   TCL TVOS Team 
	   * @param    .  
	   * @return   1: is dolby stream, 0: is not dolby stream.
	   */       	
   	public  int checkCurrentStreamIsDolby(){
              int DolbyStreamFlag = 0;
              android.media.AudioTrackInfo[] info = super.getAllAudioTrackInfo(true);
              if(info!=null&&info.length!=0){
                 Log.i(TAG,"audio codec:"+info[0].getCodecID());
                 if(info[0].getCodecID()==0x0010){
                     DolbyStreamFlag = 1;
                     Log.i(TAG,"audio codec: is dolby");
                 }else if(info[0].getCodecID()==0x0800){
                     DolbyStreamFlag = 2;
                     Log.i(TAG,"audio codec: is dolby plus");
                 }
              }
    	Log.i(TAG,"audio codec DolbyStreamFlag"+ DolbyStreamFlag);
        
    	return DolbyStreamFlag;
    }

   	public  int checkCurrentStreamIsDTS(){
        int DTSStreamFlag = 0;
        int audioInfo[];
        
        audioInfo = getAudioTrackInfo(-1);

        if((audioInfo[3] == 0x00001004) || ((audioInfo[3] == 0x00001005)))
        {
               DTSStreamFlag = 1;
        }

    	Log.i(TAG,"audio codec:"+ audioInfo[3] + "IsDTSStream flag:" + DTSStreamFlag );
        
    	return DTSStreamFlag;
    }

    public  int[] tclGetVideoInfo(){

        int[] videoinfo = new int[9];
        int i;
        for(i=0;i<9;i++)
        {
        	videoinfo[i]=0;
        }
        int video_codec_int = -1;
            String video_codec = getVideoCodec();
                    if(video_codec == null)
                    {
                        video_codec_int = -1;
                    }else{
                      if(video_codec.equals("MPEG1_2")||video_codec.equals("MPEG1")||video_codec.equals("MPEG2"))
                      {
                        video_codec_int = 1;
                      }
                      else if(video_codec.equals("MPEG4"))
                      {
                        video_codec_int = 2;
                      }
                      else if(video_codec.equals("MJPEG"))
                      {
                        video_codec_int = 3;
                      }
                      else if(video_codec.equals("DIVX311"))//?
                      {
                        video_codec_int = 4;
                      }
                      else if(video_codec.equals("RV8") ||
                            video_codec.equals("RV9_10") )
                      {
                        video_codec_int = 5;
                      }
                      else if(video_codec.equals("H264"))//?
                      {
                        video_codec_int = 6;
                      }
                      else if(video_codec.equals("H263"))
                      {
                        video_codec_int = 7;
                      }
                      else if(video_codec.equals("VC1"))
                      {
                        video_codec_int = 8;
                      }
                      else if(video_codec.equals("VP6"))//?
                      {
                        video_codec_int = 9;
                      }
                      else if(video_codec.equals("VP8"))
                      {
                        video_codec_int = 10;
                      }
                      else if(video_codec.equals("AVS"))
                      {
                        video_codec_int = 11;
                      }
                      else if(video_codec.equals("WMV1"))
                      {
                        video_codec_int = 12;
                      }
                      else if(video_codec.equals("WMV3_EX"))
                      {
                        video_codec_int = 13;
                      }
                      else if(video_codec.equals("DV"))
                      {
                        video_codec_int = 14;
                      }
                      else if(video_codec.equals("H265"))
                      {
                        video_codec_int = 15;
                      }
                    }
                    
        
                    videoinfo[2] = video_codec_int;
                    videoinfo[5] = super.getVideoWidth();
                    videoinfo[6] = super.getVideoHeight();
                    
                 //   int bit_rate = metadata.getInt(Metadata.BIT_RATE);
                    videoinfo[8] = 0;
        return videoinfo;
    }

    private void parserTrack() {
	    int index_audio = 0; 
	    int index_subtitle = 0;
	    TrackInfo[] trackInfos = super.getTrackInfo();

	    if (trackInfos != null && trackInfos.length > 0)
	    {
		    for (int i = 0; i < trackInfos.length; i++)
		    {
			    final TrackInfo info = trackInfos[i];
			    Log.w(TAG, "TrackInfo: " + info.getTrackType() + " " + info.getLanguage());
			    if (info.getTrackType() == TrackInfo.MEDIA_TRACK_TYPE_AUDIO)
			    {
				    AiduoTrackIndex[index_audio++] = i;
			    }
			    else if (info.getTrackType() == TrackInfo.MEDIA_TRACK_TYPE_TIMEDTEXT)
			    {
				    SubTitleTrackIndex[index_subtitle++] = i;
			    }
		    }
	    }
	    mAudTotalNum = index_audio;
	    mSubTotalNum = index_subtitle;

    }



    
  }
