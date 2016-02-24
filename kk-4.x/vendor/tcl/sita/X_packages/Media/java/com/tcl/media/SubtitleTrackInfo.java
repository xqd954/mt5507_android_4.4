/* 
 * Copyright (C) 2006 The Android Open Source Project 
 *  
 * ©2010-2013 TCL CORPORATION All Rights Reserved. 
 */
package com.tcl.media;

import android.media.Metadata;
import android.media.MediaPlayer;


import android.os.Parcel;
import android.util.Log;

/**
   Class to hold the subtitle's information.  SubtitleTrackInfo are used
   for human consumption and can be embedded in the media (e.g
   shoutcast) or available from an external source. The source can be
   local (e.g thumbnail stored in the DB) or remote (e.g caption
   server).

   NOTE:Users are only allowed to use the getXXX function to get the information they need.
   Can't ues this class alone.Make sure that you have called getSubtitleTrackInfo/getAllSubtitleTrackInfo
   function in mediaplayer.java before use these interfaces.
   eg:
   SubtitleTrackInfo info = getAllSubtitleTrackInfo();
   int totalInfoNum = info.getAllSubtitleCount();
   int allInternalSubtitleCount = info.getAllInternalSubtitleCount();
   int allImageSubtitleCount = info.getAllImageSubtitleCount();

   int subtitleType[] = new int[totalInfoNum];
   info.getSubtitleType(subtitleType);

   int subtitleCodeType[] = new int[totalInfoNum];
   info.getSubtitleCodeType(subtitleCodeType);

   String subtitleLanguageType[] = new String[totalInfoNum];
   info.getSubtitleLanguageType(subtitleLanguageType,false);

 */

   /*
     * getSubtitleInfo
     * @param streamNum : 1-based. -1 means current audio stream.
     * @return long array:
     *    0 : responseSize;
     *    1 : numStreams - total number of spu streams. 0 means no spu.
     *    2 : currentStreamNum - current playback spu stream number. (1-based) , -1 means spu stream is not set yet.
     *    3 : langCode  - ISO-639 language code
     *    4 : spuFrom - 
     *           1 : external spu stream 		          
     *           2 : embedded spu stream
     *           3 : embedded ts stream
     *           4 : embedded ts tt stream
     */

public class SubtitleTrackInfo {
    private final static String TAG = "SubtitleTrackInfo";
    private final int MAX_SUBTITLE_TRACK = 20;   
    private final int INFO_NUM = 3;

    private enum SUBTITLE_ENCODING_TYPE
    {
	     E_SUBTITLE_PARSER_ENCODING_GBK,
	     E_SUBTITLE_PARSER_ENCODING_BIG5,
	     E_SUBTITLE_PARSER_ENCODING_WESTERN,
	     E_SUBTITLE_PARSER_ENCODING_TURKISH,
	     E_SUBTITLE_PARSER_ENCODING_CENTRAL_EUROPEAN,
	     E_SUBTITLE_PARSER_ENCODING_GREEK,    
	     E_SUBTITLE_PARSER_ENCODING_CYRILLIC, 
	     E_SUBTITLE_PARSER_ENCODING_HEBREW,   
	     E_SUBTITLE_PARSER_ENCODING_SOUTH_EASTERN_EUROPEAN,
	     E_SUBTITLE_PARSER_ENCODING_JAPANESE,
	     E_SUBTITLE_PARSER_ENCODING_KOREAN,
	     E_SUBTITLE_PARSER_ENCODING_THAI,
	     E_SUBTITLE_PARSER_ENCODING_ARABIC,
	     E_SUBTITLE_PARSER_ENCODING_AUTO_DETEC,
    };

    private SUBTITLE_ENCODING_TYPE mSubtitleEncodingType;
    private int mSubtitleNum;

		private int[][] mSubtitleTrackInfo;

    public SubtitleTrackInfo() {
    	mSubtitleNum = 0;
    } 
    
    /**
     *  @hide
     */
    protected void initSubtitleTrackInfo(TMediaPlayer mp, int[] data){
    	   mSubtitleNum = data[1];
				//int[] data_1;
				//if(mSubtitleNum > MAX_SUBTITLE_TRACK)
				//	mSubtitleNum = MAX_SUBTITLE_TRACK;
				////int i;
				////for(i=0;i<mSubtitleNum;i++)
				{
					//data_1 = mp.getSubtitleInfo(i+1);
					//mSubtitleTrackInfo[i] = data_1;
				}
    }

    private SubtitleTrackInfo(Parcel reply, int subtitleNum) {
		Log.d(TAG,"SubtitleTrackInfo(Parcel reply, int subtitleNum)!");
    };

    private int getSubtitleType() {
        int mSubtitleType;
        mSubtitleType = -2;
        return mSubtitleType;
    }

    private int getSubtitleCodeType() {
        int mSubtitleCodeType;
        mSubtitleCodeType = -2;

        return mSubtitleCodeType;
    }

    private String getSubtitleLanguageType(boolean isChinese) {
       
        String subtitleLanguageType;
        subtitleLanguageType = "";
 
        return subtitleLanguageType;
    }

    private void getSubtitleType(int[] info) {

    }

    private void getSubtitleCodeType(int[] info) {
        for (int i = 0; i < mSubtitleNum; i++) {
               //? info[i] = mSubtitleTrackInfo[i];
        }
    }

    private void getSubtitleLanguageType(String[] info, boolean isChinese) {
        for (int i = 0; i < mSubtitleNum; i++) {
            info[i] = getSubtitleLanguageType(isChinese);
        }
    }
     /**      
     * get All Subtit leLanguage Type.
     * @author   TCL TVOS Team 
     * @param    info Subtit leLanguage Type.
     * @return   null.
     */    
    public void getSubtitleLanguageType(String[] info) {
        for (int i = 0; i < mSubtitleNum; i++) {
            info[i] = " ";
        }
    }
     /**      
     * get All Subtitle Count.
     * @author   TCL TVOS Team 
     * @param    null
     * @return   Subtitle Num.
     */
    public int getAllSubtitleCount() {
        return mSubtitleNum;
    }
     /**      
     * get All Internal Subtitle Count.
     * @author   TCL TVOS Team 
     * @param    null
     * @return  Internal Subtitle Num.
     */
    public int getAllInternalSubtitleCount() {
				int count = 0;
        for (int i = 0; i < mSubtitleNum; i++)
        {
        	if(mSubtitleTrackInfo[i][4] == 1)
						count++;
        }
        return (mSubtitleNum - count);
    }

    private int getAllImageSubtitleCount() {

        return 0;
    }

}
