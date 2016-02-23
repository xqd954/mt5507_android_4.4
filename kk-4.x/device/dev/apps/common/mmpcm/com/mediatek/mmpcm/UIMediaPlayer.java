package com.mediatek.mmpcm;

import java.io.File;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

import java.util.HashMap;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;


import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Rect;

import android.media.MediaPlayer;
import android.media.MediaPlayer.OnBufferingUpdateListener;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.media.MediaPlayer.OnInfoListener;
import android.media.MediaPlayer.OnPreparedListener;
import android.media.MediaPlayer.OnSeekCompleteListener;
import android.media.MediaPlayer.OnTimedTextListener;
import android.media.MediaPlayer.OnVideoSizeChangedListener;
import android.media.MediaPlayer.TrackInfo;
import android.media.Metadata;
import android.net.Uri;
import android.os.Parcel;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import com.mediatek.mmp.util.VideoCodecInfo;
import com.mediatek.mmp.util.ThumbnailInfo;

import com.mediatek.mmp.MtkMediaPlayer;
import com.mediatek.mmp.MtkMediaPlayer.DataSource;
import com.mediatek.mmp.MtkMediaPlayer.DataSourceMetadata;
import com.mediatek.mmp.util.MetaDataInfo;
import com.mediatek.mmp.util.PcmMediaInfo;
import com.mediatek.mmp.util.MetaDataInfo;

import com.mediatek.mmpcm.fileimpl.MtkFile;
import com.mediatek.netcm.dlna.DLNADataSource;
import com.mediatek.netcm.dlna.DLNAManager;
import com.mediatek.netcm.samba.SambaManager;

import android.media.MediaPlayer.DataSourceType;
import android.media.MediaPlayer.DivxDrmInfoType;
import android.media.MediaPlayer.PlayerSpeed;
import android.media.MediaPlayer.ABRpeatType;

import android.media.SubtitleTrackInfo;
import android.media.AudioTrackInfo;
import android.media.SubtitleAttr;
import android.media.MediaMetadataRetriever;
import com.mediatek.mmpcm.fileimpl.FileConst;

public class UIMediaPlayer implements DataSource {
    
	public static final int MODE_LOCAL = FileConst.SRC_USB;
    public static final int MODE_SAMBA = FileConst.SRC_SMB;    
    public static final int MODE_DLNA = FileConst.SRC_DLNA; 
    public static final int MODE_HTTP = FileConst.SRC_HTTP;
   public static final int IMTK_PB_ERROR_CODE_NEW_TRICK               =   -400;  ///< new trick flow, has no index table, only support speed less then 8x, ap should reset to normal play(1x)
    public static final int IMTK_PB_BUFFER_NOT_ENOUGH	                  =   -101;  ///< new trick flow, buffer not enough
    private static final String TAG = "UIMediaPlayer";
    private MtkMediaPlayer mtkPlayer;
    private MediaPlayer mPlayer;
    private int sourceType;
    private String mPath;
    
    private InputStream mInputStream;
    
    
    
    public UIMediaPlayer(int sourceType){
        this.sourceType = sourceType;
        if( sourceType ==  MODE_LOCAL){
            mPlayer = new MediaPlayer();
        }else{
            mtkPlayer = new MtkMediaPlayer();
        }
        
    }
    
    
    
    public MtkMediaPlayer getMtkPlayer(){
        return mtkPlayer;
    }
    
    public MediaPlayer getPlayer(){
        return mPlayer;
    }

    public void  setPlayType(int playType){
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            //mPlayer.setPlayType(playType);
        }
    }

    public SubtitleTrackInfo getSubtitleTrackInfo(final int trackIndex){
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            TrackInfo[] tracks = mPlayer.getTrackInfo();
            int textTrackNum = 0;
            if(tracks != null && tracks.length >0){
                for(TrackInfo info: tracks){
                    if(info.getTrackType() == TrackInfo.MEDIA_TRACK_TYPE_TIMEDTEXT){
                        if(textTrackNum == trackIndex){
                            //wait for after complete.
                            return null;
                        }else{
                            textTrackNum++;
                        }
                    }
                }
            }

        }else if(mtkPlayer != null){
            return mtkPlayer.getSubtitleTrackInfo(trackIndex);
        }
        return null;
    }
    
    public SubtitleTrackInfo[] getAllSubtitleTrackInfo(){
         Log.d(TAG,"getAllSubtitleTrackInfo~~~ sourceType = "+ sourceType);
		 SubtitleTrackInfo[] subTrackinfos = null;
		if( sourceType ==  MODE_LOCAL && mPlayer != null){
			//return mPlayer.ifMP3Media();
			TrackInfo[] tracks = mPlayer.getTrackInfo();
			int textTrackNum = 0;
            int trackIndex = 0;
			if(tracks != null && tracks.length >0){
				for(TrackInfo info: tracks){
					if(info.getTrackType() == TrackInfo.MEDIA_TRACK_TYPE_TIMEDTEXT){
                            Log.d(TAG,"MEDIA_TRACK_TYPE_TIMEDTEXT index ="+trackIndex +"textTrackNum ="+textTrackNum);
							textTrackNum++;
						
					}
                    trackIndex++;
				}
			}
			
			if(textTrackNum > 0){
				//only for ui get subtitleTrackinfo num,other wait for after complete.
				subTrackinfos = new SubtitleTrackInfo[textTrackNum];
			}
			
			//return subTrackinfos;


            
		}else if(sourceType ==  MODE_DLNA){
			Log.d(TAG,"getAllSubtitleTrackInfo~~~ DLNA size =" + subTitleList.size());
			subTrackinfos = new SubtitleTrackInfo[subTitleList.size()];
			
		}else{
			Log.d(TAG,"getAllSubtitleTrackInfo~~~ smb for mtkPlayer get all sub~");
			subTrackinfos = mtkPlayer.getAllSubtitleTrackInfo();
		}
		if(subTrackinfos!= null){
			Log.d(TAG,"getAllSubtitleTrackInfo~~~ subTrackNum = "+ subTrackinfos.length);
		}
		return subTrackinfos;
    }

    public boolean setAudioTrack(int track){
		Log.d(TAG,"setAudioTrack  ~ sourceType ="+ sourceType);
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            return mPlayer.setAudioTrackEx(track);
        }else if(mtkPlayer != null){
            return mtkPlayer.setAudioTrack(track);
        }
        return false;


    }

     public void step(int amount){
         Log.d(TAG,"step ~");

        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.step(amount);
        }else if(mtkPlayer != null){
            mtkPlayer.step(amount);
        }
        
     }

     public void setSubtitleTrack(int track){
         Log.d(TAG,"setSubtitleTrack track ="+track);

        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            TrackInfo[] tracks = mPlayer.getTrackInfo();
            int textTrackNum = 0;
            int trackNum = 0;
            if(tracks != null && tracks.length >0){
                for(TrackInfo info: tracks){
                    
                    if(info.getTrackType() == TrackInfo.MEDIA_TRACK_TYPE_TIMEDTEXT){
                        if(textTrackNum == track){
                            Log.d(TAG,"setSubtitleTrack trackNum ="+trackNum +"textTrackNum ="+textTrackNum);
                            mPlayer.selectTrack(trackNum);
                            return;
                        }else{
                            textTrackNum++;
                        }
                    }
                    trackNum++;
                }
            }

            
			//return mPlayer.setSubtitleTrack(track);
		}else if(sourceType ==	MODE_DLNA){

			int size = subTitleList.size();
			if(track < size){
				setSubtitleDataSourceEx(subTitleList.get(track), null);
			}
			Log.d(TAG,"getAllSubtitleTrackInfo~~~ DLNA set size = "+ size );
		}else{
			Log.d(TAG,"setSubtitleTrack smb for mtkPlayer set sub~");
			mtkPlayer.setSubtitleTrack(track);

		}

			
		//return false;


     }


     public boolean onSubtitleTrack(){
         Log.d(TAG,"onSubtitleTrack ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            return mPlayer.onSubtitleTrackEx();
        }else if(mtkPlayer != null){
            mtkPlayer.onSubtitleTrack();
            return true;
        }
        return false;


     }

    public boolean setSubtitleDataSourceEx(String UriSub, String UriIdx){
         Log.d(TAG,"setSubtitleDataSourceEx ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            return mPlayer.setSubtitleDataSourceEx(UriSub,  UriIdx);
        }else if(mtkPlayer != null){
            mtkPlayer.setSubtitleDataSourceEx(UriSub,  UriIdx);
            return true;
        }
        return false;
    }

    public boolean offSubtitleTrack(){
		Log.d(TAG,"offSubtitleTrack ~sourceType = "+ sourceType +"mPlayer = "+ mPlayer);

        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            return mPlayer.offSubtitleTrackEx();
        }else if(mtkPlayer != null){
            mtkPlayer.offSubtitleTrack();
            return true;
        }
        return false;
    }
    
    public boolean setTS(int index){
		Log.d(TAG,"setTS ~ index ="+ index);
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            return mPlayer.setTS(index);
        }else if(mtkPlayer != null){
            return mtkPlayer.setTS((short)index);
        }
        return false;


    }

    public boolean canDoSeek(Object speed){
	 	Log.d(TAG,"canDoSeek ~speed = "+ speed);
		boolean doSeek = false;
		if( sourceType ==  MODE_LOCAL && mPlayer != null){
			doSeek = mPlayer.canDoSeek((MediaPlayer.PlayerSpeed)speed);
		}else if(mtkPlayer != null){
			doSeek = mtkPlayer.canDoSeek((MediaPlayer.PlayerSpeed)speed);
		}
		Log.d(TAG,"canDoSeek ~ doSeek = "+ doSeek);
		return doSeek;
    }

    public VideoCodecInfo getVideoInfo(){
		Log.d(TAG,"getVideoInfo ~ local not handle");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            //return mPlayer.canDoTrick((MediaPlayer.PlayerSpeed)speed);
        }else if(mtkPlayer != null){
            return mtkPlayer.getVideoInfo();
        }
        return null;
    }

     public boolean canDoTrick(Object speed) {
	 	Log.d(TAG,"canDoTrick ~ speed = "+ speed);
		boolean doTrick = false;
		if( sourceType ==  MODE_LOCAL && mPlayer != null){
			doTrick = mPlayer.canDoTrick((MediaPlayer.PlayerSpeed)speed);
		}else if(mtkPlayer != null){
			doTrick = mtkPlayer.canDoTrick((MediaPlayer.PlayerSpeed)speed);
		}
		Log.d(TAG,"canDoTrick ~ doTrick = "+ doTrick);

		
		return doTrick;
    }


    public byte[] getThumbnailInfo(ThumbnailInfo thumbNailInfo){
		Log.d(TAG,"getThumbnailInfo ~ local not handle");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            //return mPlayer.ifMP3Media();
        }else if(mtkPlayer != null){
            return mtkPlayer.getThumbnailInfo(thumbNailInfo);
        }
        return null;
    }

    
     public boolean ifMP3Media(){
        Log.d(TAG,"ifMP3Media ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            return mPlayer.ifMP3Media();
        }else if(mtkPlayer != null){
            return mtkPlayer.ifMP3Media();
        }
        return false;
     }
     
     
     
     public byte[] getEmbeddedPicture() {
		 	Log.d(TAG,"getEmbeddedPicture ~ local not handle");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            //return mPlayer.getEmbeddedPicture();
        }else if(mtkPlayer != null){
            return mtkPlayer.getEmbeddedPicture();
        }
        return null;
     }
     
    public int getCurrentPosition() {
        Log.d(TAG,"getCurrentPosition ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            return mPlayer.getCurrentPosition();
        }else if(mtkPlayer != null){
            return mtkPlayer.getCurrentPosition();
        }
        return 0;
    }
    
     public int getCurrentBytePosition(){
        Log.d(TAG,"getCurrentBytePosition ~");
                int position = 0;
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            position = mPlayer.getCurrentBytePosition();
        }else if(mtkPlayer != null){
            position = mtkPlayer.getCurrentBytePosition();
        }
                Log.d(TAG,"getCurrentBytePosition ~position = "+position);
        return position;
    }


    public int getDuration() {
        Log.d(TAG,"getDuration ~");
                 int dur = 0;
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            dur = mPlayer.getDuration();
        }else if(mtkPlayer != null){
            dur = mtkPlayer.getDuration();
        }
                Log.d(TAG,"getDuration ~dur = "+dur);
        return dur;
    }

    public void setPcmMediaInfo(PcmMediaInfo pcmMediaInfo) {
		Log.d(TAG,"setPcmMediaInfo ~ local not handle");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            //mPlayer.getMetaDataInfo();
        }else if(mtkPlayer != null){
            mtkPlayer.setPcmMediaInfo(pcmMediaInfo);
        }
        
        
    }
    
    private MetaDataInfo getMetaDataInfoByRetriever(){
        Log.d(TAG,"getMetaDataInfoByRetriever ~");

        /*try 
        {
             new Exception("getMetaDataInfoByRetriever");
        } catch (Exception e) {
            e.printStackTrace();
        } */
        
        MetaDataInfo mMetaInfo = null;
        MediaMetadataRetriever retriever = new MediaMetadataRetriever();
        
        
        try{
            //retriever.setDataSource(mPath);
            
            String KEY_RETRIEVER_PLAYER = "X-tv-Retriever-Player";
            String KEY_THUMBNAIL_PATH = "X-tv-Thumbnail-Path";
            
            String VALUE_RETRIEVER_PLAYER = "CMPB_PLAYER";
            String VALUE_THUMBNAIL_PATH = "THRD_USB";
            Map<String, String> Headers_t=new HashMap<String, String>();
            
            Headers_t = new HashMap<String,String>();
            
            //Use CMPB for MetaDataInfo
            Headers_t.put(KEY_RETRIEVER_PLAYER,VALUE_RETRIEVER_PLAYER);
            
            Log.e("Thumbnail", "setDataSource 1");
            retriever.setDataSource(mPath,Headers_t); //OK for old style, change framwork to use cmpb

            //Option 2
            /*
            retriever.setPlayerType(6);
            retriever.setDataSource(mPath); */

        }catch(Exception e){
            retriever.release();
             Log.d(TAG,"setdataSource fail ~");
            return null;
        }



        String mtitle = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_TITLE);
        //cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.TITLE));

        String mdirector =retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DIRECTOR);
        String mcopyright = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_COPYRIGHT);

        String myear = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_YEAR);
        //cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.DATE_ADDED));
        String mgenre = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_GENRE);

        String martist =    retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_ARTIST);
        //cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.ARTIST));

        String malbum = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_ALBUM);
        //cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.ALBUM));

        String mbitrate = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_BITRATE);
        Log.e("mbitrate", "mbitrate:"+"null:"+mbitrate+"_re:"+mgenre+"_mdirector:"+mdirector+"_mcopyright:"+mcopyright);

        String mdur = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION);

        int dur = 0;
        try{
        dur = Integer.valueOf(mdur);

        }catch(Exception ex){
        Log.d(TAG,"duration to int error~~");
        }




        int mbitratet= 200;
        try{
        mbitratet = Integer.valueOf(mbitrate);

        }catch(Exception ex){
        Log.d(TAG,"mbitrate to int error~~");
        }

        retriever.release();

        
        Log.e(TAG, " getMetaDataInfoByRetriever  myear:"+myear+"_mtitle:"+mtitle+"_martist:"+martist
          +"_malbum:"+malbum+"_mgenre:"+mgenre);

        mMetaInfo = new MetaDataInfo(-1, mbitratet, dur,
        -1, myear, mtitle, malbum, martist,
        -1, mgenre, mdirector, mcopyright);
        
        return mMetaInfo;


    }

    public MetaDataInfo getMetaDataInfo() {
        Log.d(TAG,"getMetaDataInfo ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            return getMetaDataInfoByRetriever();//mPlayer.getMetaDataInfo();
        }else if(mtkPlayer != null){
            return mtkPlayer.getMetaDataInfo();
        }
        
        return null;//super.getMetadata(update_only, apply_filter);
    }
    public AudioTrackInfo getAudioTrackInfo(final boolean typeIsAudio){
        Log.d(TAG,"getAudioTrackInfo ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            TrackInfo[] tracks = mPlayer.getTrackInfo();
            int audioTrackNum = 0;
            if(tracks != null && tracks.length >0){
                for(TrackInfo info: tracks){
                    if(info.getTrackType() == TrackInfo.MEDIA_TRACK_TYPE_AUDIO){
                        audioTrackNum++;

                    }
                }
            }
            AudioTrackInfo audioTrackInfo = new AudioTrackInfo(audioTrackNum, -1, -1, -1,-1, -1, "", "", "", "",-1, "", "", "");

            return audioTrackInfo;
        }else if(mtkPlayer != null){
            return mtkPlayer.getAudioTrackInfo(true);
        }
        return null;
        
        
    }
    
    public Metadata getMetadata(boolean update_only, boolean apply_filter) {
        Log.d(TAG,"getMetadata ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            return mPlayer.getMetadata(update_only, apply_filter);
        }else if(mtkPlayer != null){
            return mtkPlayer.getMetadata(update_only, apply_filter);
        }
        
        return null;//super.getMetadata(update_only, apply_filter);
    }


    public TrackInfo[] getTrackInfo() throws IllegalStateException {
        Log.d(TAG,"getTrackInfo ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            return mPlayer.getTrackInfo();
        }else if(mtkPlayer != null){
            //return mtkPlayer.getTrackInfo();
        }
        return null;//super.getTrackInfo();
    }



    public int getVideoHeight() {
        Log.d(TAG,"getVideoHeight ~");
                int videoHeight = 0;
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            videoHeight = mPlayer.getVideoHeight();
        }else if(mtkPlayer != null){
            videoHeight = mtkPlayer.getVideoHeight();
        }
		Log.d(TAG,"getVideoHeight ~ videoHeight = "+ videoHeight);
        return videoHeight;//super.getVideoHeight();
    }



    public int getVideoWidth() {
        Log.d(TAG,"getVideoWidth ~");
                int videoWidth = 0;
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            videoWidth = mPlayer.getVideoWidth();
        }else if(mtkPlayer != null){
            videoWidth = mtkPlayer.getVideoWidth();
        }
		Log.d(TAG,"getVideoWidth ~ videoWidth = "+ videoWidth);
        return videoWidth;//super.getVideoWidth();
    }

    public boolean isLooping() {
        Log.d(TAG,"isLooping ~");
		boolean isLooping = false;
		if( sourceType ==  MODE_LOCAL && mPlayer != null){
			isLooping = mPlayer.isLooping();
		}else if(mtkPlayer != null){
			isLooping = mtkPlayer.isLooping();
		}
		Log.d(TAG,"isLooping ~ isLooping ="+isLooping);
		return isLooping;//super.isLooping();
    }

    public boolean isPlaying() {
        Log.d(TAG,"isPlaying ~");
		boolean isPlaying = false;
		if( sourceType ==  MODE_LOCAL && mPlayer != null){
			isPlaying = mPlayer.isPlaying();
		}else if(mtkPlayer != null){
			isPlaying = mtkPlayer.isPlaying();
		}
		Log.d(TAG,"isPlaying ~ isPlaying ="+ isPlaying);
		return isPlaying;//super.isPlaying();
    }

    public void pause() throws IllegalStateException {
        Log.d(TAG,"pause sourceType ="+sourceType +"mPlayer ="+ mPlayer +"mtkPlayer ="+mtkPlayer);
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.pause();
        }else if(mtkPlayer != null){
            mtkPlayer.pause();
        }
    }



    public void prepare() throws IOException, IllegalStateException {
        Log.d(TAG,"prepare ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.prepare();
        }else if(mtkPlayer != null){
            mtkPlayer.prepare();
        }
    }



    public void prepareAsync() throws IllegalStateException {
        Log.d(TAG,"prepareAsync ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.prepareAsync();
        }else if(mtkPlayer != null){
            mtkPlayer.prepareAsync();
        }
    }



    public void release() {
        Log.d(TAG,"release ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.release();
        }else if(mtkPlayer != null){
            closeStream();
            mtkPlayer.release();
        }
    }



    public void reset() {
        Log.d(TAG,"reset ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.reset();
        }else if(mtkPlayer != null){
            mtkPlayer.reset();
        }
    }


    public void seekTo(int arg0) throws IllegalStateException {
		Log.d(TAG,"seekTo ~ arg0 = "+ arg0);
		if( sourceType ==  MODE_LOCAL && mPlayer != null){
			mPlayer.seekTo(arg0);
		}else if(mtkPlayer != null){
			mtkPlayer.seekTo(arg0);
		}
	}

	
	public void selectTrack(int index) throws IllegalStateException {
		Log.d(TAG,"selectTrack net not handle~ indexb = "+ index);
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.selectTrack(index);
        }else if(mtkPlayer != null){
//          mtkPlayer.selectTrack(index);
        }
//      super.selectTrack(index);
    }

    public void setSvctxPath(String path){
		Log.d(TAG,"setSvctxPath ~ path ="+ path);
		if( sourceType ==  MODE_LOCAL && mPlayer != null){
			mPlayer.setSvctxPath(path);
		}else if(mtkPlayer != null){
			mtkPlayer.setSvctxPath(path);
		}

	}

	public void setSubtitleDataSource(String path){
		Log.d(TAG,"setSubtitleDataSource ~path = "+ path +" sourceType = "+ sourceType);
		if( sourceType ==  MODE_LOCAL && mPlayer != null){
			mPlayer.setSubtitleDataSource(path);
		}else if(sourceType ==  MODE_DLNA){
			setDLNASubtitle(path);
		}else{
			Log.d(TAG,"setSubtitleDataSource do nothing");
		}

	}

	private ArrayList<String> subTitleList = new ArrayList<String>();

	private void setDLNASubtitle(String path){
		Log.d(TAG,"setDLNASubtitle  path = "+ path);
		subTitleList.clear();
		subTitleList.addAll(DLNAManager.getInstance().getSubTitleList(path));
	}
	
	public void setDataSource(String path) throws IOException,
			IllegalArgumentException, SecurityException, IllegalStateException {
			Log.d(TAG,"setDataSource ~ path ="+ path);
        mPath = path;
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.setDataSource(path);
        }else if(mtkPlayer != null){
            mtkPlayer.setDataSource(this);
        }
//      super.setDataSource(path);
    }

    
    public void setDisplay(SurfaceHolder arg0) {
        Log.d(TAG,"setDisplay ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.setDisplay(arg0);
        }else if(mtkPlayer != null){
            mtkPlayer.setDisplay(arg0);
        }
    }

    
    public void setLooping(boolean arg0) {
		Log.d(TAG,"setLooping ~ arg0 = "+ arg0);
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.setLooping(arg0);
        }else if(mtkPlayer != null){
            mtkPlayer.setLooping(arg0);
        }
//      super.setLooping(arg0);
    }

    public void setNextMediaPlayer(MediaPlayer arg0) {
        Log.d(TAG,"setNextMediaPlayer ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.setNextMediaPlayer(arg0);
        }else if(mtkPlayer != null){
//          mtkPlayer.setNextMediaPlayer(arg0);
        }
//      super.setNextMediaPlayer(arg0);
    }

    
    public void setOnBufferingUpdateListener(Object listener) {
        Log.d(TAG,"setOnBufferingUpdateListener ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.setOnBufferingUpdateListener((OnBufferingUpdateListener)listener);
        }else if(mtkPlayer != null){
            mtkPlayer.setOnBufferingUpdateListener((com.mediatek.mmp.MtkMediaPlayer.OnBufferingUpdateListener)listener);
        }
//      super.setOnBufferingUpdateListener(listener);
    }

    
    public void setOnCompletionListener(Object listener) {
        Log.d(TAG,"setOnCompletionListener ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.setOnCompletionListener((OnCompletionListener)listener);
        }else if(mtkPlayer != null){
            mtkPlayer.setOnCompletionListener((com.mediatek.mmp.MtkMediaPlayer.OnCompletionListener)listener);
        }
    }

    
    public void setOnErrorListener(Object listener) {
        Log.d(TAG,"setOnErrorListener ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.setOnErrorListener((OnErrorListener)listener);
        }else if(mtkPlayer != null){
            mtkPlayer.setOnErrorListener((com.mediatek.mmp.MtkMediaPlayer.OnErrorListener)listener);
        }
    }

    
    public void setOnInfoListener(Object listener) {
        Log.d(TAG,"setOnInfoListener ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.setOnInfoListener((OnInfoListener)listener);
        }else if(mtkPlayer != null){
            mtkPlayer.setOnInfoListener((com.mediatek.mmp.MtkMediaPlayer.OnInfoListener)listener);
        }
    }

    
    public void setOnPreparedListener(Object listener) {
        Log.d(TAG,"setOnPreparedListener ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.setOnPreparedListener((OnPreparedListener)listener);
        }else if(mtkPlayer != null){
            mtkPlayer.setOnPreparedListener((com.mediatek.mmp.MtkMediaPlayer.OnPreparedListener)listener);
        }
    }

    
    public void setOnSeekCompleteListener(Object listener) {
        Log.d(TAG,"setOnSeekCompleteListener ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.setOnSeekCompleteListener((OnSeekCompleteListener)listener);
        }else if(mtkPlayer != null){
            mtkPlayer.setOnSeekCompleteListener((com.mediatek.mmp.MtkMediaPlayer.OnSeekCompleteListener)listener);
        }
    }

    
    public void setOnTimedTextListener(Object listener) {
        Log.d(TAG,"setOnTimedTextListener ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.setOnTimedTextListener((OnTimedTextListener)listener);
        }else if(mtkPlayer != null){
            mtkPlayer.setOnTimedTextListener((com.mediatek.mmp.MtkMediaPlayer.OnTimedTextListener)listener);
        }
    }

    
    public void setOnVideoSizeChangedListener(Object listener) {
        Log.d(TAG,"setOnVideoSizeChangedListener ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.setOnVideoSizeChangedListener((OnVideoSizeChangedListener)listener);
        }else if(mtkPlayer != null){
            mtkPlayer.setOnVideoSizeChangedListener((com.mediatek.mmp.MtkMediaPlayer.OnVideoSizeChangedListener)listener);
        }
    }
    
    
    public void setPlayerRole(int playerRole){
		Log.d(TAG,"setPlayerRole ~ playerRole ="+ playerRole);
		if( sourceType ==  MODE_LOCAL && mPlayer != null){
			mPlayer.setPlayerRole(playerRole);
		}else if(mtkPlayer != null){
			mtkPlayer.setPlayerRole(playerRole);
		}
		
	}
	
	
	public void setPlayerType(int playertype) {
		Log.d(TAG,"setPlayerType ~ playertype ="+ playertype);
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.setPlayerType(playertype);
        }else if(mtkPlayer != null){
            //mtkPlayer.setPlayerType(playertype);
        }
    }

    
    public void setScreenOnWhilePlaying(boolean screenOn) {
        Log.d(TAG,"setScreenOnWhilePlaying ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.setScreenOnWhilePlaying(screenOn);
        }else if(mtkPlayer != null){
            mtkPlayer.setScreenOnWhilePlaying(screenOn);
        }
    }

    
    public void setSurface(Surface surface) {
        Log.d(TAG,"setSurface ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.setSurface(surface);
        }else if(mtkPlayer != null){
            mtkPlayer.setSurface(surface);
        }
    }

    
    public void setVideoRect(Rect rect) {
		Log.d(TAG,"setVideoRect ~rect = "+ rect);
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.setVideoRect(rect);
        }else if(mtkPlayer != null){
            mtkPlayer.setVideoRect(rect);
        }
    }

    
     public void setDataSourceMetadata(DataSourceMetadata dataSourceMetadata) throws IllegalStateException {
         Log.d(TAG,"setDataSourceMetadata ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            //mPlayer.setDataSourceMetadata(dataSourceMetadata);
        }else if(mtkPlayer != null){
            mtkPlayer.setDataSourceMetadata(dataSourceMetadata);
        }
         
     }
    
    public int setPlayMode(Object speed){
        Log.d(TAG,"setPlayMode ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            return mPlayer.setPlayMode((MediaPlayer.PlayerSpeed)speed);
        }else if(mtkPlayer != null){
            return mtkPlayer.setPlayMode((MediaPlayer.PlayerSpeed)speed);
        }
        return -1;
    }

    public int setPlayModeEx(Object speed){
        Log.d(TAG,"setPlayModeEx ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            return mPlayer.setPlayMode((MediaPlayer.PlayerSpeed)speed);
        }else if(mtkPlayer != null){
            return mtkPlayer.setPlayMode((MediaPlayer.PlayerSpeed)speed);
        }
        return -1;
    }
    
    public void start() throws IllegalStateException {
        Log.d(TAG,"start ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.start();
        }else if(mtkPlayer != null){
            mtkPlayer.start();
        }
    }

    
    public void stop() throws IllegalStateException {
        Log.d(TAG,"stop ~");
        if( sourceType ==  MODE_LOCAL && mPlayer != null){
            mPlayer.stop();
        }else if(mtkPlayer != null){
            mtkPlayer.stop();
        }
    }

    public void closeStream() {
        Log.d(TAG,"closeStream ~");
        if (null != mInputStream) {
            try {
                mInputStream.close();
            } catch (IOException e) {
                Log.d(TAG, "video closeStream() fail" + e.toString());
            }
        } else {
            Log.d(TAG, "video closeStream()  stream is null");
        }
        mInputStream = null;
    }

    @Override
    public long getSourceSize() {
        Log.d(TAG,"getSourceSize ~");
        long fileSize = 0;
        switch (sourceType) {
        case MODE_DLNA: {
            DLNADataSource dlnaSource = DLNAManager.getInstance()
                    .getDLNADataSource(mPath);
            if (dlnaSource != null) {
                fileSize = dlnaSource.getContent().getSize();
                Log.d(TAG, "getVideoFileSize = dlna" + fileSize);
            }
        }
            break;
        case MODE_SAMBA: {
            SambaManager sambaManager = SambaManager.getInstance();
            try {
                fileSize = sambaManager.size(mPath);
                Log.d(TAG, "getVideoFileSize = samba" + fileSize);
            } catch (Exception e) {
                e.printStackTrace();
            } 
        }
            break;
        case MODE_LOCAL: {
            MtkFile mFile=null;
            if (mPath != null) {
                mFile = new MtkFile(mPath);
            }
            if (mFile == null) {
                fileSize = 0;
                break;
            }
            fileSize = mFile.getFileSize();
            Log.d(TAG, "getVideoFileSize = fileSize" + fileSize);
        }
            break;
        case MODE_HTTP:
            break;
        default:
            break;
        }

        return fileSize;
    }


    @Override
    public DataSourceType getSourceType() {
        Log.d(TAG,"getSourceType ~");
        DataSourceType eSourceType = null;
        switch (sourceType) {
        case MODE_DLNA:
            eSourceType = DataSourceType.SOURCE_TYPE_DLNA;
            break;
        case MODE_SAMBA:
            eSourceType = DataSourceType.SOURCE_TYPE_SAMBA;
            break;
        case MODE_LOCAL:
            eSourceType = DataSourceType.SOURCE_TYPE_LOCAL;
            break;
        default:
            break;
        }
        return eSourceType;
    }

    @Override
    public boolean isSeekable() {
    Log.d(TAG,"isSeekable ~");
        switch (sourceType) {
        case MODE_DLNA:
            DLNADataSource dlnaSource = DLNAManager.getInstance()
                    .getDLNADataSource(mPath);
            return dlnaSource.getContent().canSeek();
        case MODE_SAMBA:
            break;
        case MODE_LOCAL:
            break;
        default:
            break;
        }
        return true;
    }

    @Override
    public InputStream newInputStream() {

        Log.d(TAG, "newInputStream mCurrentPath:"+mPath+"mPlayerMode:"+sourceType);
//      closeStream();
//      if (isEnd) {
//          return null;
//      }
        if (mPath == null) {
            return null;
        } else {
            if (sourceType== MODE_LOCAL) {
                try {
                    mInputStream = new FileInputStream(mPath);
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                }

                if (mInputStream == null) {
                    // sendMsg(VideoConst.MSG_INPUT_STREAM_FAIL);
                }
            } else if ( sourceType == MODE_SAMBA) {
                try {
                    mInputStream = SambaManager.getInstance()
                            .getSambaDataSource(mPath).newInputStream();
                } catch (Exception e) {
                    e.printStackTrace();
                } 

                if (mInputStream == null) {
                }
            } else if (sourceType == MODE_DLNA) {
                DLNADataSource source = DLNAManager.getInstance()
                        .getDLNADataSource(mPath);
                Log.d(TAG, "PLAYER_MODE_DLNA mCurrentPath:"+mPath);
                if (source == null) {
                    mInputStream = null;

                } else {
                    mInputStream = source.newInputStream();
                    Log.d(TAG, "PLAYER_MODE_DLNA mInputStream:"+mInputStream);
                }

                if (mInputStream == null) {
                }
            }

            return mInputStream;
        }
    
    }
    
    
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
