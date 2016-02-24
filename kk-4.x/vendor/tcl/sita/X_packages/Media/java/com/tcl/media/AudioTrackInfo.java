/* 
 * Copyright (C) 2006 The Android Open Source Project 
 *  
 * ©2010-2013 TCL CORPORATION All Rights Reserved. 
 */
package com.tcl.media;

import android.media.Metadata;
import android.util.Log;
import java.net.IDN;

/**
   Class to hold the audio's information.  AudioTrackInfo are used
   for human consumption and can be embedded in the media (e.g
   shoutcast) or available from an external source. The source can be
   local (e.g thumbnail stored in the DB) or remote (e.g caption
   server).

   NOTE:Users are only allowed to use the getXXX function to get the information they need.
   Can't ues this class alone.Make sure that you have called getAudioTrackInfo function in
   mediaplayer.java before use these interfaces.
 */
public class AudioTrackInfo {
    private final static String TAG = "AudioTrackInfo";

    private int mBiteRate;
    private int mTotalPlayTime;
    private int mCurrentPlayTime;
    private int mSampleRate;
    private int mCodecID;
    private int mYear;
    private String mTitle;
    private String mAlbum;
    private String mArtist;
    private String mLanguageType;
	
	private int MAX_AUDIO_TRACK = 20;

	private int mAudioTrackNum;
	private int[][] mAudioTrackInfo;
	

    public AudioTrackInfo() {
				mAudioTrackNum = 0;
    };
    
     /**
       {@hide}
     */
    protected void initAudioTrackInfo(TMediaPlayer mp,int[] data)
    {
    	mAudioTrackNum = data[1];
			////int[] data_1;
	    //    if(mAudioTrackNum > MAX_AUDIO_TRACK)
			//	mAudioTrackNum = mAudioTrackNum;
			////int i;
			////for(i=0;i<mAudioTrackNum;i++)
			{
				////data_1 = mp.getAudioTrackInfo(i+1);
				////mAudioTrackInfo[i] = data_1;
			}
    }
     /**      
     * get All AudioTrack Count.
     * @author   TCL TVOS Team 
     * @param    null
     * @return   Audio Track Num.
     */
	public int getAllAudioTrackCount(){

		return mAudioTrackNum;
	}

    /**
      * Gets the audio encoding format's id.
      * Note:It is not the track number.
      */
    private int getCodecID() {
        return mCodecID;
    }

    /**
      * Gets the total play time.
      * Only vedio type can get this value.
      */
    private int getTotalPlayTime() {
        if (-1 == mTotalPlayTime) {
            Log.d(TAG, "Only vedio type can get TotalPlayTime.");
        }
        return mTotalPlayTime;
    }

    /**
      * Gets the current play time.
      * Only vedio type can get this value.
      */
    private int getCurrentPlayTime() {
        if (-1 == mCurrentPlayTime) {
            Log.d(TAG, "Only vedio type can get CurrentPlayTime.");
        }
        return mCurrentPlayTime;
    }

    /**
      * Gets the bite rate.
      * Only audio type can get this value.
      */
    private int getBiteRate() {
        if (-1 == mBiteRate) {
            Log.d(TAG, "Only audio type can get BiteRate and local audio couldn't get infomation with the function.");
        }
        return mBiteRate;
    }

    /**
      * Gets the release year.
      * Only audio type can get this value.
      */
    private int getYear() {
        if (0 == mYear) {
            Log.d(TAG, "Only audio type can get Year and local audio couldn't get infomation with the function.");
        }
        return mYear;
    }

    /**
      * Gets the sample rate.
      * Only audio type can get this value.
      */
    private int getSampleRate() {
        if (-1 == mSampleRate) {
            Log.d(TAG, "Only audio type can get SampleRate and local audio couldn't get infomation with the function.");
        }
        return mSampleRate;
    }

    /**
      * Gets the audio's title.
      * Only audio type can get this value.
      */
    private String getTitle() {
        if (null == mTitle) {
            Log.d(TAG, "Only audio type can get Title and local audio couldn't get infomation with the function.");
        }
        return mTitle;
    }

    /**
      * Gets the audio's album.
      * Only audio type can get this value.
      */
    private String getAlbum() {
        if (null == mAlbum) {
            Log.d(TAG, "Only audio type can get Album and local audio couldn't get infomation with the function.");
        }
        return mAlbum;
    }

    /**
      * Gets the audio's artist.
      * Only audio type can get this value.
      */
    private String getArtist() {
        if (null == mArtist) {
            Log.d(TAG, "Only audio type can get Artist and local auido couldn't get infomation with the function.");
        }
        return mArtist;
    }

    /**
      * Gets the audio's language type.
      */
    private String getAudioLanguageType() {
        return mLanguageType;
    }
     /**
     * get Audio Language Type.
     * @author   TCL TVOS Team 
     * @param    info AudioLanguageType
     * @return   null.
     */
    public void getAudioLanguageType(String[] info){
    	
    	 for (int i = 0; i < mAudioTrackNum; i++) {
            info[i] = " ";
       }
    }
    
    
}
