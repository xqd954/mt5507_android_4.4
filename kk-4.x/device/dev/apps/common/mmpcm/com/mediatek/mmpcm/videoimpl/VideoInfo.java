package com.mediatek.mmpcm.videoimpl;


import com.mediatek.mmp.*;
import com.mediatek.mmp.util.*;
import com.mediatek.mmp.MtkMediaPlayer.*;
import com.mediatek.mmpcm.UIMediaPlayer;

import android.media.MediaPlayer.DataSourceType;
import android.media.MediaPlayer.DivxDrmInfoType;
import android.media.MediaPlayer.PlayerSpeed;
import android.media.MediaPlayer.ABRpeatType;

import com.mediatek.mmpcm.Info;
import com.mediatek.mmpcm.MetaData;
import com.mediatek.mmpcm.MmpTool;
import com.mediatek.mmpcm.fileimpl.FileConst;

public class VideoInfo extends Info {

    private UIMediaPlayer mtkPlayer;
    private static VideoInfo vInfo = null;
    private boolean metaLoadStart = false;

    private VideoInfo(int srcType) {
		mSrcType = srcType;
        mtkPlayer = new UIMediaPlayer(srcType);
    }

    private VideoInfo() {
		mSrcType = FileConst.SRC_USB;
        mtkPlayer = new UIMediaPlayer(mSrcType);
    }

    /**
     * get static FileInfo instance
     * 
     * @return
     */
    public static synchronized VideoInfo getInstance(int srcType) {

        if (vInfo == null) {
            vInfo = new VideoInfo(srcType);
        }
        return vInfo;
    }


    public static synchronized VideoInfo getInstance() {

        if (vInfo == null) {
            vInfo = new VideoInfo();
        }
        return vInfo;
    }

    /**
     * Get MetaDate object assign
     * 
     * @param path
     *            ,srcType
     */
    public MetaData getMetaDataInfo(String path, int srcType)
            throws IllegalArgumentException {
        MmpTool.LOG_DBG("path = " + path);
        

        if (path == null) {
            throw new IllegalArgumentException("empty path!");
        }

		if(srcType != mSrcType){
			mSrcType = srcType;
			mtkPlayer.release();
			mtkPlayer = new UIMediaPlayer(srcType);
		}


		if (srcType == FileConst.SRC_USB) {
	        
	        return  getMediaInfo(path);
		}

		
        MetaData mMetaInfo = new MetaData();
        if (mtkPlayer != null) {

            mFilePath = path;

            try {
                mtkPlayer.reset();
                
                mtkPlayer.setDataSource(mFilePath);
            
                mtkPlayer.setPlayerRole(MtkMediaPlayer.PlayerRole.ROLE_GET_METADATA);
                
                mtkPlayer.prepare();
                
                } catch (Exception e) {
                MmpTool.LOG_INFO(" getMetaDataInfo()  :" + e.toString());
                mMetaInfo.setMetaData(null, null, null, null, null, null, null,
                        0, 0);
                try {
                    if (null != mtkPlayer) {
                        //mtkPlayer.stop();
                    }
                    metaLoadStart = false;
                } catch (IllegalStateException e1) {
                    e1.printStackTrace();
                }
                mtkPlayer.closeStream();
                return mMetaInfo;
            }

            metaLoadStart = true;

            MetaDataInfo metaDataInfo = mtkPlayer.getMetaDataInfo();

            if (metaDataInfo != null) {

                String mtitle = metaDataInfo.getTitle();
                String mdirector = metaDataInfo.getDirector();
                String mcopyright = metaDataInfo.getCopyright();

                String myear = metaDataInfo.getYear();
                String mgenre = metaDataInfo.getGenre();

                String martist = metaDataInfo.getArtist();

                String malbum = metaDataInfo.getAlbum();

                int mbitrate = metaDataInfo.getBiteRate();

                mMetaInfo.setMetaData(mtitle, mdirector, mcopyright, myear,
                        mgenre, martist, malbum, mtkPlayer.getDuration(),
                        mbitrate);
            }else{
                mMetaInfo.setMetaData(null, null, null, null, null, null, null,
                        mtkPlayer.getDuration(), 0);
            }
//          putMetaData(path, mMetaInfo);
            try {
                //mtkPlayer.stop();
                metaLoadStart = false;
            } catch (IllegalStateException e) {
                e.printStackTrace();
            }
            mtkPlayer.closeStream();
        }

        return mMetaInfo;
    }

    /**
     * stop meta data
     */
    public void stopMetaData() {
        if (mtkPlayer != null && metaLoadStart == true) {
            try {
                //mtkPlayer.stop();
                metaLoadStart = false;
            } catch (IllegalStateException e) {
                e.printStackTrace();
                metaLoadStart = false;
                return;
            }
        }
    }

}
