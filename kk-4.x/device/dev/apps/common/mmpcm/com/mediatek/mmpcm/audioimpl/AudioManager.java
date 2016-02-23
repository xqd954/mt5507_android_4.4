package com.mediatek.mmpcm.audioimpl;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.UnknownHostException;

import jcifs.smb.SmbException;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.mediatek.mmp.*;
import com.mediatek.mmp.util.*;
import com.mediatek.mmp.MtkMediaPlayer.*;
import com.mediatek.mmpcm.UIMediaPlayer;

import android.media.MediaPlayer;

import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnPreparedListener;
import android.media.MediaPlayer.OnErrorListener;
import android.media.MediaPlayer.OnInfoListener;
import android.media.MediaPlayer.OnSeekCompleteListener;

import com.mediatek.mmpcm.MmpTool;
import com.mediatek.mmpcm.audio.IPlayback;
import com.mediatek.mmpcm.fileimpl.MtkFile;
import com.mediatek.mmpcm.mmcimpl.Const;
import com.mediatek.mmpcm.mmcimpl.PlayList;
import com.mediatek.netcm.dlna.DLNADataSource;
import com.mediatek.netcm.dlna.DLNAManager;
import com.mediatek.netcm.dlna.FileSuffixConst;
import com.mediatek.netcm.samba.SambaManager;

import android.media.MediaPlayer.DataSourceType;
import android.media.MediaPlayer.DivxDrmInfoType;
import android.media.MediaPlayer.PlayerSpeed;
import android.media.MediaPlayer.ABRpeatType;
import java.io.BufferedReader;
import java.io.InputStreamReader;

import android.media.SubtitleTrackInfo;
import android.media.AudioTrackInfo;
import android.media.SubtitleAttr;

public class AudioManager implements IPlayback{//, DataSource {
	
	private static final String TAG = "AudioManager";

	

    private UIMediaPlayer mtkMediaPlayer;
    private String dataSource;
    private int speedStep;
    private int mPlayStatus = AudioConst.PLAY_STATUS_INITED;
    private int mPlayMode = AudioConst.PLAYER_MODE_LOCAL;
    private boolean pcmMode = false;
    private PcmMediaInfo pcmInfo;

	private int mTmpPlayStatus = AudioConst.PLAY_STATUS_INITED;
	
	private PlayerSpeed mPlayerSpeed = PlayerSpeed.SPEED_1X;
	private boolean isSeeking_pend_to_Play = false;


	AudioManager(int srcType) {
		Log.d(TAG , "AudioManager srcType ="+srcType);
		mPlayMode = srcType;
		mTmpPlayStatus = AudioConst.PLAY_STATUS_INITED;
		mtkMediaPlayer = new UIMediaPlayer(srcType);
		if(srcType == UIMediaPlayer.MODE_LOCAL){
			mtkMediaPlayer.setOnCompletionListener(newCompletionListener);
			mtkMediaPlayer.setOnPreparedListener(preparedListener);
			mtkMediaPlayer.setOnErrorListener(errorListener);
			//mtkMediaPlayer.setOnTotalTimeUpdateListener(totalTimeListener);
			mtkMediaPlayer.setOnInfoListener(newInfoListener);
			mtkMediaPlayer.setOnSeekCompleteListener(seekCompletionListener);


		}else{
			mtkMediaPlayer.setOnCompletionListener(newCompletionListener);
			mtkMediaPlayer.setOnPreparedListener(mtkPreparedListener);
			mtkMediaPlayer.setOnErrorListener(mtkErrorListener);
			//mtkMediaPlayer.setOnTotalTimeUpdateListener(totalTimeListener);
			mtkMediaPlayer.setOnInfoListener(newInfoListener);
			mtkMediaPlayer.setOnSeekCompleteListener(mtkSeekCompletionListener);

		}
		
	}

	
	private void cleanListener() {
		if (mtkMediaPlayer != null) {
			mtkMediaPlayer.setOnErrorListener(null);
			mtkMediaPlayer.setOnPreparedListener(null);
			mtkMediaPlayer.setOnBufferingUpdateListener(null);
			mtkMediaPlayer.setOnSeekCompleteListener(null);
			mtkMediaPlayer.setOnCompletionListener(null);
			mtkMediaPlayer.setOnInfoListener(null);
		}
	}

	private void resetListener() {
		if (mtkMediaPlayer != null) {


			if(mPlayMode == UIMediaPlayer.MODE_LOCAL){
				mtkMediaPlayer.setOnCompletionListener(newCompletionListener);
				mtkMediaPlayer.setOnPreparedListener(preparedListener);
				mtkMediaPlayer.setOnErrorListener(errorListener);
					//mtkMediaPlayer.setOnTotalTimeUpdateListener(totalTimeListener);
				mtkMediaPlayer.setOnInfoListener(newInfoListener);
				mtkMediaPlayer.setOnSeekCompleteListener(seekCompletionListener);


			}else{
				mtkMediaPlayer.setOnCompletionListener(newCompletionListener);
				mtkMediaPlayer.setOnPreparedListener(mtkPreparedListener);
				mtkMediaPlayer.setOnErrorListener(mtkErrorListener);
				//mtkMediaPlayer.setOnTotalTimeUpdateListener(totalTimeListener);
				mtkMediaPlayer.setOnInfoListener(newInfoListener);
				mtkMediaPlayer.setOnSeekCompleteListener(mtkSeekCompletionListener);

			}
		}
	}
	
	public void setPlayMode(int playMode) {
		Log.d(TAG , "setPlayMode playMode ="+playMode+"mPlayMode ="+mPlayMode);
		if(playMode != mPlayMode){
			mPlayMode = playMode;
			mtkMediaPlayer = new UIMediaPlayer(mPlayMode);
			if(mPlayMode == UIMediaPlayer.MODE_LOCAL){
				mtkMediaPlayer.setOnCompletionListener(newCompletionListener);
				mtkMediaPlayer.setOnPreparedListener(preparedListener);
				mtkMediaPlayer.setOnErrorListener(errorListener);
				//mtkMediaPlayer.setOnTotalTimeUpdateListener(totalTimeListener);
				mtkMediaPlayer.setOnInfoListener(newInfoListener);
				mtkMediaPlayer.setOnSeekCompleteListener(seekCompletionListener);
	
			}else{
				mtkMediaPlayer.setOnCompletionListener(newCompletionListener);
				mtkMediaPlayer.setOnPreparedListener(mtkPreparedListener);
				mtkMediaPlayer.setOnErrorListener(mtkErrorListener);
				//mtkMediaPlayer.setOnTotalTimeUpdateListener(totalTimeListener);
				mtkMediaPlayer.setOnInfoListener(newInfoListener);
				mtkMediaPlayer.setOnSeekCompleteListener(mtkSeekCompletionListener);
			}

		}
		

		
	}

	public boolean isPlaying() {
		return mtkMediaPlayer.isPlaying();
	}

	public int getSpeed() {
		return speedStep;
	}
	/**
	 * set normal play, ff, fr and so on.
	 * @param speed
	 */
	public void setSpeed(int speed){
		speedStep = speed;
	}
	/**
	 * Set fastForward play, 2x, 4x and so on.
	 * @throws  IllegalStateException
	 */
	public void fastForward(){
		// TODO Auto-generated method stub
		//MtkMediaPlayer.PlayerSpeed eSpeed;
		if(!mtkMediaPlayer.isSeekable()){
			notifyInfo(AudioConst.MEDIA_INFO_FEATURE_NOT_SUPPORT);
		    return;
		}
		int tmpSpeedStep = speedStep;
		PlayerSpeed tmpPlayerSpeed = mPlayerSpeed;
		Log.d(TAG,"fastForward mPlayStatus = "+mPlayStatus);
		try{
			switch (mPlayStatus) {
			case AudioConst.PLAY_STATUS_FF:
				speedStep <<= 1;

				if (speedStep > 32) {
					play();

				} else {
					switch (speedStep) {

					case 2:
						mPlayerSpeed = PlayerSpeed.SPEED_FF_2X;
						break;
					case 4:
						mPlayerSpeed = PlayerSpeed.SPEED_FF_4X;
						break;
					case 8:
						mPlayerSpeed = PlayerSpeed.SPEED_FF_8X;
						break;
					case 16:
						mPlayerSpeed = PlayerSpeed.SPEED_FF_16X;
						break;
					case 32:
						mPlayerSpeed = PlayerSpeed.SPEED_FF_32X;
						break;
					default:
						mPlayerSpeed = PlayerSpeed.SPEED_FF_2X;
						break;
					}
					setPlayModeEx(mPlayerSpeed,AudioConst.PLAY_STATUS_FF);
				}

				break;

			case AudioConst.PLAY_STATUS_STARTED:
			case AudioConst.PLAY_STATUS_PAUSED:
			case AudioConst.PLAY_STATUS_FR:
			case AudioConst.PLAY_STATUS_SF:
				speedStep = 2;	
				mPlayerSpeed = PlayerSpeed.SPEED_FF_2X;
				setPlayModeEx(mPlayerSpeed,AudioConst.PLAY_STATUS_FF);
				break;
			default:
				break;
			}
		}catch(RuntimeException ex){
			speedStep = tmpSpeedStep;
			mPlayerSpeed = tmpPlayerSpeed;
			Log.d(TAG,"fastForward Exception ex= "+ex);

			throw ex;

		}

	}
	/**
	 * Set fastRewind play, 2x, 4x and so on.
	 * @throws NotSupportException, IllegalStateException
	 */
	public void fastRewind() {
		// TODO Auto-generated method stub
		if(!mtkMediaPlayer.isSeekable()){
			notifyInfo(AudioConst.MEDIA_INFO_FEATURE_NOT_SUPPORT);
		    return;
		}
		int tmpSpeedStep = speedStep;
		PlayerSpeed tmpPlayerSpeed = mPlayerSpeed;
		Log.d(TAG,"fastRewind mPlayStatus = "+mPlayStatus);
		try{
			switch (mPlayStatus) {
			case AudioConst.PLAY_STATUS_FR:
				speedStep <<= 1;
	     
				if (speedStep > 32) {
					play();
				} else {
					switch (speedStep) {
					case 2:
						mPlayerSpeed = PlayerSpeed.SPEED_FR_2X;
						break;
					case 4:
						mPlayerSpeed = PlayerSpeed.SPEED_FR_4X;
						break;
					case 8:
						mPlayerSpeed = PlayerSpeed.SPEED_FR_8X;
						break;
					case 16:
						mPlayerSpeed = PlayerSpeed.SPEED_FR_16X;
						break;
					case 32:
						mPlayerSpeed = PlayerSpeed.SPEED_FR_32X;
						break;
					default:
						mPlayerSpeed = PlayerSpeed.SPEED_FR_2X;
						break;
					}

					setPlayModeEx(mPlayerSpeed, AudioConst.PLAY_STATUS_FR);

				}
				break;
			case AudioConst.PLAY_STATUS_STARTED:
			case AudioConst.PLAY_STATUS_PAUSED:
			case AudioConst.PLAY_STATUS_FF:
			case AudioConst.PLAY_STATUS_SF:
				mPlayerSpeed = PlayerSpeed.SPEED_FR_2X;			
				speedStep = 2;
				setPlayModeEx(mPlayerSpeed,AudioConst.PLAY_STATUS_FR);
				
				break;
			default:
				break;
			}
		}catch(RuntimeException ex){
			speedStep = tmpSpeedStep;
			mPlayerSpeed = tmpPlayerSpeed;

			Log.d(TAG,"fastRewind Exception ex= "+ex);
		
			throw ex;

		}
	}
	

	public void setPlayModeEx(Object speed, int playStatus) {
		int result = mtkMediaPlayer.setPlayModeEx(speed);
		if ( result < 0 ) {
			if(result == UIMediaPlayer.IMTK_PB_BUFFER_NOT_ENOUGH){
				throw new IllegalStateException("BUFFER NOT ENOUGH");
			}else{

				throw new RuntimeException("MEDIA_INFO_NOT_SUPPORT");

			}
			
		}
		mPlayStatus = playStatus;
	}


	private void setNormalSpeed() {
		Log.d(TAG,"setNormalSpeed ~~~ ");
		try{
			setPlayModeEx(PlayerSpeed.SPEED_1X, AudioConst.PLAY_STATUS_STARTED);
			speedStep = 1;
			mPlayerSpeed = PlayerSpeed.SPEED_1X;
		}catch(RuntimeException ex){
			
			Log.d(TAG,"setNormalSpeed Exception ex= "+ex);
			throw ex;

		}
	}





	
	/**
	 * Pause a audio
	 * 
	 * @throws IllegalStateException
	 */
	public void pause()  {
		Log.d(TAG,"pause mPlayStatus ="+mPlayStatus);
		if (mPlayStatus == AudioConst.PLAY_STATUS_STARTED
				|| mPlayStatus == AudioConst.PLAY_STATUS_FF
				|| mPlayStatus == AudioConst.PLAY_STATUS_FR
				|| mPlayStatus == AudioConst.PLAY_STATUS_SF){
			if (mPlayMode == AudioConst.PLAYER_MODE_DLNA) {
				DLNADataSource dlnaDataSource = DLNAManager.getInstance()
						.getDLNADataSource(dataSource);
				if (dlnaDataSource != null) {
					if (!dlnaDataSource.getContent().canPause()) {

						throw new IllegalStateException(
								AudioConst.MSG_ERR_CANNOTPAUSE);
						

					}
				}
			}

			try {
				mtkMediaPlayer.pause();
				mPlayStatus = AudioConst.PLAY_STATUS_PAUSED;

			} catch (Exception e) {
				// TODO: handle exception
				throw new IllegalStateException();
			}
			
		} 

	}


	private MetaDataInfo mMetaData = null;
	
	public MetaDataInfo getMetaDataInfo() {
		
		if(mMetaData == null){
			
			if(mtkMediaPlayer != null){
				mMetaData = mtkMediaPlayer.getMetaDataInfo();
			}
		}
		
		return mMetaData;
		
	}


	
	/**
	 * Set data source
	 * 
	 * @param path
	 * @return return true success , return false fail
	 */
	public boolean setDataSource(String path) {
		Log.d(TAG,"setDataSource path ="+path);
		pcmMode = false;
		mMetaData = null;
		if (path == null) {
			//if (mPlayStatus != AudioConst.PLAY_STATUS_COMPLETED) {
			stop();
			notifyInfo(AudioConst.MEDIA_INFO_METADATA_COMPLETE);
			//}

			return false;
		}
		dataSource = path;
	
		stop();

		boolean ret = getPcmMetaInfo(dataSource);
	  	Log.d(TAG,"setDataSource getPcmMetaInfo ="+ret+"pcmMode ="+pcmMode);
		if (ret == false) {
			return false;
		}

		return setDataSource();
	}

	private final String TK_MMP_CMPB_FLAG = "use.cmpb.in.tkplayer";

	private int getProperty(String proName){
		String line =null;
		Process ifc = null;
		int result = 1;
		try{
			ifc = Runtime.getRuntime().exec("getprop "+proName);
			BufferedReader bis = new BufferedReader(new InputStreamReader(ifc.getInputStream()));
			line = bis.readLine();
			Log.w(TAG,"line ="+line);
		}catch(Exception e){
			e.printStackTrace();
		}finally{
			ifc.destroy();
		}
		
		Log.w(TAG, proName+"line ="+line);
		if(line != null && line.length() > 0){
			try{
				result = Integer.valueOf(line);
			}catch(Exception ex){
				
			}
		}
		
		return result;

	}



	private boolean setDataSource() {
		Log.e(TAG, "setDataSource()...status="+mPlayStatus);
		mPlayStatus = AudioConst.PLAY_STATUS_INITED;
		mtkMediaPlayer.reset();
		resetListener();
		notifyInfo(AudioConst.MEDIA_INFO_DATA_PREPAREED_STATE);
		try {
			
			//mtkMediaPlayer.configAudioPlayer();
			Log.e(TAG, "dataSource="+dataSource);
			Log.e(TAG, "playmode="+mPlayMode);
			
			if(getProperty(TK_MMP_CMPB_FLAG) == 1){
				mtkMediaPlayer.setPlayerType(6);
			}
			mtkMediaPlayer.setDataSource(dataSource);
			mtkMediaPlayer.setPlayerRole(MtkMediaPlayer.PlayerRole.ROLE_AUDIO_PLAYBACK);
			if (pcmMode == true) {
				mtkMediaPlayer.setDataSourceMetadata(pcmInfo);
			}
			
			mtkMediaPlayer.prepareAsync();
			mPlayStatus = AudioConst.PLAY_STATUS_PREPARING;
			speedStep = 1;
			mPlayerSpeed = PlayerSpeed.SPEED_1X;
		} catch (Exception e) {
			Log.d(TAG,"setDataSource Exception~");

			mPlayStatus = AudioConst.PLAY_STATUS_ERROR;
			// TODO Auto-generated catch block
			e.printStackTrace();
			handleErr(MtkMediaPlayer.MEDIA_ERROR_OPEN_FILE_FAILED,0);
			return false;
		} 

		return true;

	}
    
	/**
	 * Get file size
	 * @return
	 */
	public long getFileSize(){	
		long fileSize = 0;
		String mcurPath = dataSource;
		switch (mPlayMode) {
		case AudioConst.PLAYER_MODE_DLNA: {
			DLNADataSource dlnaSource = DLNAManager.getInstance()
					.getDLNADataSource(mcurPath);
			if (dlnaSource != null) {
				fileSize = dlnaSource.getContent().getSize();	
				MmpTool.LOG_INFO("getAudioFileSize dlna $$$$$$$$$$$$$$" 
						+ fileSize);
			}
		}
			break;
		case AudioConst.PLAYER_MODE_SAMBA: {
			SambaManager sambaManager = SambaManager.getInstance();
			try {
				fileSize = sambaManager.size(mcurPath);
				MmpTool.LOG_INFO("getAudioFileSize samba $$$$$$$$$$$$$$" 
						+ fileSize);
			} catch (MalformedURLException e) {
				e.printStackTrace();
			} catch (SmbException e) {
				e.printStackTrace();
			} catch (UnknownHostException e) {
				e.printStackTrace();
			}
		}
			break;
		case AudioConst.PLAYER_MODE_LOCAL: {
			MtkFile mFile = null;
			if (mcurPath != null) {
				mFile = new MtkFile(mcurPath);
			}
			MmpTool.LOG_INFO("getAudioFileSize = $$$$$$$$$$$$$$" + mcurPath);

			if (mFile == null) {
				fileSize = 0;
				break;
			}
			fileSize = mFile.getFileSize();
			MmpTool
					.LOG_INFO("getAudioFileSize local $$$$$$$$$$$$$$"
							+ fileSize);
		}
			break;
		case AudioConst.PLAYER_MODE_HTTP:
			break;
		default:
			break;
		}
		
		return fileSize;
	}
	/**
	 * Play a audio
	 * 
	 * @throws IllegalStateException
	 */
	public void play()  {
		
		try{
			
			if (mPlayStatus == AudioConst.PLAY_STATUS_PAUSED){
				start();
			}else if(mPlayStatus == AudioConst.PLAY_STATUS_FF
				|| mPlayStatus == AudioConst.PLAY_STATUS_FR) {
				setNormalSpeed();
				start(); 
			} else if (mPlayStatus == AudioConst.PLAY_STATUS_STARTED
					|| mPlayStatus == AudioConst.PLAY_STATUS_PREPARING
					|| mPlayStatus == AudioConst.PLAY_STATUS_PREPARED) {
				MmpTool.LOG_DBG("Has played or prepared!");
			} else if (mPlayStatus == AudioConst.PLAY_STATUS_STOPPED) {
				boolean ret = setDataSource();
				if (ret == false) {
					MmpTool.LOG_ERROR("setDataSource error!");
				}
			} else if(mPlayStatus == AudioConst.PLAB_STATUS_SEEKING){
				isSeeking_pend_to_Play = true;
			}else{
				MmpTool.LOG_ERROR("Please setDataSource firstly!");
			}
			
		}catch(Exception ex){
			Log.d(TAG,"play Exception ex ="+ex);
			throw new IllegalStateException(ex);

		}
	}


	private boolean playNext(int flag)  {
        Log.e(TAG, "playNext(flag)...:"+flag);

		String path = null;
        
		PlayList mPlayList = PlayList.getPlayList();
 
		path = mPlayList.getNext(Const.FILTER_AUDIO, flag);
		Log.e(TAG, "path="+path + "play status="+mPlayStatus);
		if(path == null){
			notifyInfo(AudioConst.MEDIA_INFO_METADATA_COMPLETE);
			return false;
		}
		return setDataSource(path);
	}
	/**
	 * Play next audio
	 * 
	 * @throws IllegalStateException
	 */
	public void playNext()  {
		if (mPlayMode == AudioConst.PLAYER_MODE_HTTP) {
			MmpTool.LOG_ERROR("This player mode can't do next!");
			throw new IllegalStateException("Can't do Next!!!");
		} else {
			playNext(Const.MANUALNEXT);
		}
	}
	/**
	 * Play previous audio
	 * 
	 * @throws IllegalStateException
	 */
	public void playPrevious()  {

		if (mPlayMode == AudioConst.PLAYER_MODE_HTTP) {
			MmpTool.LOG_ERROR("This player mode can't do prev!");
			throw new IllegalStateException("Can't do Prev!!!");
		}else {
			playNext(Const.MANUALPRE);
		}

		
	}
	/**
	 * Stop a audio
	 * 
	 * @throws IllegalStateException
	 */
	public void stop()  {

		cleanListener();
		Log.d(TAG,"stop mPlayStatus ="+mPlayStatus);
		
		if ((mPlayStatus >= AudioConst.PLAY_STATUS_PREPARING
				&& mPlayStatus != AudioConst.PLAY_STATUS_STOPPED)) {
			try {
				mtkMediaPlayer.stop();
				mPlayStatus = AudioConst.PLAY_STATUS_STOPPED;
			} catch (IllegalStateException e) {
				e.printStackTrace();
				throw new IllegalStateException(e);
			}
			
		}
	}

	/**
	 * Release resource and stop audio play.
	 * 
	 * @throws IllegalStateException
	 */
	public void release()  {
		stop();
		mtkMediaPlayer.closeStream();
		mtkMediaPlayer.release();
		mPlayStatus = AudioConst.PLAY_STATUS_INITED;
		mtkMediaPlayer = null;
	}

	public String getFilePath(){

		return dataSource;


	}
	/**
	 * Get playback progress value
	 * @return long, playback progress
	 */
	public int getPlaybackProgress() {
		if (mPlayStatus >= AudioConst.PLAY_STATUS_PREPARED
				&& mPlayStatus < AudioConst.PLAY_STATUS_STOPPED) {

			int curpos = mtkMediaPlayer.getCurrentPosition();
			Log.d(TAG,"getPlaybackProgress curpos ="+curpos);
			return curpos;
		} else {
			return 0;
		}
	}


		/**
	 * Get playback progress value
	 * @return long, playback progress
	 */
	public int getCurrentBytePosition() {
		if (mPlayStatus >= AudioConst.PLAY_STATUS_PREPARED
				&& mPlayStatus < AudioConst.PLAY_STATUS_STOPPED ) {

			int curpos = mtkMediaPlayer.getCurrentBytePosition();
			Log.d(TAG,"getCurrentBytePosition curpos ="+curpos);
			return curpos;
		} else {
			return 0;
		}
	}

	/**
	 * Get total playback time
	 * 
	 * @return
	 */
	public int getTotalPlaybackTime() {
		int dur = 0;
		if (mPlayStatus >= AudioConst.PLAY_STATUS_PREPARED) {
			dur =  mtkMediaPlayer.getDuration();
		} 
		Log.d(TAG,"getTotalPlaybackTime dur ="+dur);

		return dur;
	}

	/**
	 * Seek to certain time
	 * 
	 * @param time
	 * @return return true if success, return false if fail
	 */
	public void seekToCertainTime(long time) {

		if (mPlayStatus == AudioConst.PLAY_STATUS_STARTED
				|| mPlayStatus == AudioConst.PLAY_STATUS_PAUSED) {
			if (time < 0) {
				time = 0;
			}
			if (time > mtkMediaPlayer.getDuration()) {
				time = mtkMediaPlayer.getDuration();
			}

			if (mPlayMode == AudioConst.PLAYER_MODE_DLNA) {
				DLNADataSource dlnaDataSource = DLNAManager.getInstance()
						.getDLNADataSource(dataSource);
				if (dlnaDataSource != null) {
					if (!dlnaDataSource.getContent().canSeek()) {
						throw new IllegalStateException(AudioConst.NOT_SUPPORT);
					}
				}
			}
			try {
				
				Log.d(TAG,"seekToCertainTime time = "+ time +"curtime = "+System.currentTimeMillis());
				mtkMediaPlayer.seekTo((int) time);
				mTmpPlayStatus = mPlayStatus;
				mPlayStatus = AudioConst.PLAB_STATUS_SEEKING;
				
				
			} catch (IllegalStateException e) {

				Log.d(TAG,"seekToCertainTime exception =" + e);

				
				throw e;
			}

			
		} 
	}
	/**
	 * Get bit rate for a aduio
	 * @return return 
	 */
	public int getBitRate() {

		MetaDataInfo info = getMetaDataInfo();
		if (info != null) {
			return info.getBiteRate();
		} else {
			return -1;
		}

	}
	/**
	 * Get sample rate for a aduio
	 * @return return  sample rate
	 */
	public int getSampleRate() {
		MetaDataInfo info = getMetaDataInfo();
		if (info != null) {
			switch (info.getSampleRate()) {
			case 1:
				return 8000;
			case 2:
				return 16000;
			case 3:
				return 32000;
			case 4:
				return 11000;
			case 5:
				return 22000;
			case 6:
				return 44000;
			case 7:
				return 12000;
			case 8:
				return 24000;
			case 9:
				return 48000;
			case 10:
				return 96000;
			case 11:
				return 192000;
			default:
				return -1;
			}
		} else {
			return -1;
		}

	}

	/**
	 *  Get audio codec information.
	 * @return sting, audio codec information
	 */
	public String getAudioCodec() {

		AudioTrackInfo info = mtkMediaPlayer.getAudioTrackInfo(true);
		if (info != null) {
			switch (info.getCodecID()) {
			case 0:
				return "UNKNOWN";
			case MtkMediaPlayer.AUDIO_ENCODE_FORMAT_LPCM:
				return "LPCM";
			case MtkMediaPlayer.AUDIO_ENCODE_FORMAT_ADPCM:
				return "ADPCM";
			case MtkMediaPlayer.AUDIO_ENCODE_FORMAT_MP3:
				return "MP3";
			case MtkMediaPlayer.AUDIO_ENCODE_FORMAT_MPEG1_LAYER1_2:
				return "MPEG1_LAYER1_2";
			case MtkMediaPlayer.AUDIO_ENCODE_FORMAT_AC3:
				return "AC3";
			case MtkMediaPlayer.AUDIO_ENCODE_FORMAT_AAC:
				return "AAC";
			case MtkMediaPlayer.AUDIO_ENCODE_FORMAT_HE_AAC:
				return "HE_AAC";
			case MtkMediaPlayer.AUDIO_ENCODE_FORMAT_WMA:
				return "WMA";
			case MtkMediaPlayer.AUDIO_ENCODE_FORMAT_CDDA:
				return "CDDA";
			case MtkMediaPlayer.AUDIO_ENCODE_FORMAT_REALAUDIO8_LBR:
				return "REALAUDIO8_LBR";
			case MtkMediaPlayer.AUDIO_ENCODE_FORMAT_DD51:
				return "DD51";
			case MtkMediaPlayer.AUDIO_ENCODE_FORMAT_DDPLUS:
				return "DDPLUS";
			case MtkMediaPlayer.AUDIO_ENCODE_FORMAT_DTS_5_1:
				return "DTS_5_1";
			case MtkMediaPlayer.AUDIO_ENCODE_FORMAT_DTS_HD:
				return "DTS_HD";
			case MtkMediaPlayer.AUDIO_ENCODE_FORMAT_DRA:
				return "DRA";
			default:
				return null;
			}
		} else {
			return null;
		}
	
	}

	/**
	 * Get file title for audio
	 * 
	 * @return music title
	 */
	public String getMusicTitle() {
		String title = "";

		MetaDataInfo info = getMetaDataInfo();
		if(info == null){
			return "";
		}
		title = info.getTitle();
	

		MmpTool.LOG_DBG(title);
		return title;
	}
	/**
	 * Get artist of a audio.
	 * 
	 * @return music artist
	 */
	public String getMusicArtist() {
		String artist = "";
			MetaDataInfo info = getMetaDataInfo();
			if(info == null){
				return "";
			}
			artist = info.getArtist();

		MmpTool.LOG_DBG(artist);
		return artist;
	}
	/**
	 * Get album of a audio.
	 * 
	 * @return music album
	 */
	public String getMusicAlbum() {
		String album = "";
			MetaDataInfo info = getMetaDataInfo();
			if(info == null){
				return "";
			}
			album = info.getAlbum();

		MmpTool.LOG_DBG(album);
		return album;
	}
	/**
	 * Get genre of a audio.
	 * 
	 * @return music genre
	 */
	public String getMusicGenre() {
		String genre = "";
			MetaDataInfo info = getMetaDataInfo();
			if(info == null){
				return "";
			}
			genre = info.getGenre();			

		MmpTool.LOG_DBG(genre);
		return genre;
	}
	/**
	 * Get music year
	 * 
	 * @return music year
	 */
	public String getMusicYear() {
		String year = "";
			MetaDataInfo info = getMetaDataInfo();
			if(info == null){
				return "";
			}
			year = info.getYear();

		MmpTool.LOG_DBG(year);
		return year;
	}
	/**
	 * Get current play status.
	 * 
	 * @return current play status.
	 */	
	public int getPlayStatus() {
		return mPlayStatus;
	}
	/**
	 * Register a listener to notify a aduio play complete.
	 * @param completionListener
	 * 
	 */	
	public void registerAudioCompletionListener(Object completionListener) {
		
           newCompletionListener =  completionListener;
      
	}
	/**
	 * Unregister a listener to notify a aduio play complete.
	 * 
	 * 
	 */	
	public void unregisterAudioCompletionListener() {

		newCompletionListener = null;
        
		
	}
	/**
	 * Register a listener to notify a aduio prepare play.
	 * @param completionListener
	 * 
	 */	
	public void registerAudioPreparedListener(Object preparedListener) {
		
            newPreparedListener = preparedListener;
        
		
	}
	/**
	 * Unregister a listener to notify a aduio prepare play.
	 * 
	 * 
	 */	
	public void unregisterAudioPreparedListener() {
	
		newPreparedListener = null;
        
		
	}
	/**
	 * Register a listener to notify playing audio occur error.
	 * @param errorListener
	 * 
	 */	
	public void registerAudioErrorListener(Object errorListener) {
	 	
         newErrorListener =  errorListener;
        
		
	}
	/**
	 * Unregister a listener to notify playing audio occur error.
	 * 
	 * 
	 */	
	public void unregisterAudioErrorListener() {
    
		newErrorListener = null;
        
		
	}
	/**
	 * Register a listener to notify a aduio play duration update.
	 * @param updateListener
	 * 
	 */	
	public void registerAudioDurationUpdateListener(Object updateListener) {
		//newTotalTimeListener = (MtkMediaPlayer.OnTotalTimeUpdateListener) updateListener;
	}

	/**
	 * Unregister a listener to notify a aduio play duration update.
	 * 
	 * 
	 */	
	public void unregisterAudioDurationUpdateListener() {
		//newTotalTimeListener = null;
	}
	
	/**
	 * Register a listener to notify a audio replay.
	 * @param infoListener
	 */
	public void registerInfoListener(Object infoListener){

	 	
           newInfoListener = infoListener;
       
	}
	/**
	 * Unregister a listener to notify a aduio replay.
	 */
	public void unregisterInfoListener(){
		   
		newInfoListener = null;
        
	}

	private Object newInfoListener = null;

	private Object newErrorListener = null;

	private Object newPreparedListener = null;

	private Object newCompletionListener = null;



	private MtkMediaPlayer.OnSeekCompleteListener mtkSeekCompletionListener = new MtkMediaPlayer.OnSeekCompleteListener() {
		 
		public void onSeekComplete(MtkMediaPlayer arg0) {
			handleSeekComplete();
		}
	};

	//private MtkMediaPlayer.OnErrorListener mtkNewErrorListener = null;
	private MtkMediaPlayer.OnErrorListener mtkErrorListener = new MtkMediaPlayer.OnErrorListener() {
		public boolean onError(MtkMediaPlayer mp, int what, int extra) {
			MmpTool.LOG_DBG("what = " + what);
			return handleErr(what,extra);
		}
	};

	//private MtkMediaPlayer.OnPreparedListener mtkNewPreparedListener = null;
	private MtkMediaPlayer.OnPreparedListener mtkPreparedListener = new MtkMediaPlayer.OnPreparedListener() {
		public void onPrepared(MtkMediaPlayer mp) {
			handlePrepare();
		}
	};



	private OnSeekCompleteListener seekCompletionListener = new OnSeekCompleteListener() {
		public void onSeekComplete(MediaPlayer arg0) {
			handleSeekComplete();
		}
	};


	
	private OnErrorListener errorListener = new OnErrorListener() {
		public boolean onError(MediaPlayer mp, int what, int extra) {
			MmpTool.LOG_DBG("what = " + what);
			return handleErr(what,extra);
			
		}
	};

	
	private OnPreparedListener preparedListener = new OnPreparedListener() {
		public void onPrepared(MediaPlayer mp) {
			
			handlePrepare();
		}
	};





	private void handleSeekComplete(){
		Log.d(TAG,"handleSeekComplete mTmpPlayStatus = "+mTmpPlayStatus+"mPlayStatusb ="+mPlayStatus +"curTime = "+getPlaybackProgress()+"curtime = "+System.currentTimeMillis());
		if (mTmpPlayStatus != AudioConst.PLAY_STATUS_INITED) {
			mPlayStatus = mTmpPlayStatus;
			mTmpPlayStatus = AudioConst.PLAY_STATUS_INITED;
			if(isSeeking_pend_to_Play){
				isSeeking_pend_to_Play = false;
				play();

			}

		}else{
			
			Log.d(TAG,"handleSeekComplete state error~~~~~~~ ");

		}


		if (null != newInfoListener) {
			if(newInfoListener instanceof MtkMediaPlayer.OnInfoListener){
				((MtkMediaPlayer.OnInfoListener)newInfoListener).onInfo(mtkMediaPlayer.getMtkPlayer(), AudioConst.MSG_POSITION_UPDATE, 0);
			}else{
				((OnInfoListener)newInfoListener).onInfo(mtkMediaPlayer.getPlayer(), AudioConst.MSG_POSITION_UPDATE, 0);
			}
		}
		

	}


	private boolean handleErr(int what,int extra){
		Log.e(TAG, "MSG_FILE_NOT_SUPPORT.");
		
		mPlayStatus = AudioConst.PLAY_STATUS_ERROR;
		if (newErrorListener != null) {
			if(newErrorListener instanceof MtkMediaPlayer.OnErrorListener){
				((MtkMediaPlayer.OnErrorListener)newErrorListener).onError(mtkMediaPlayer.getMtkPlayer(),what, extra);
			}else{
				((OnErrorListener)newErrorListener).onError(mtkMediaPlayer.getPlayer(),what, extra);
			}
		}
		return true;


	}

	private void handlePrepare(){
		mPlayStatus = AudioConst.PLAY_STATUS_PREPARED;
		Log.e(TAG, "onPrepared...start");
		if (pcmMode == true) {
			mtkMediaPlayer.setPcmMediaInfo(pcmInfo);
		}

	
		start();

		if (newPreparedListener != null) {

			if(newPreparedListener instanceof MtkMediaPlayer.OnPreparedListener){
				((MtkMediaPlayer.OnPreparedListener)newPreparedListener).onPrepared(mtkMediaPlayer.getMtkPlayer());
			}else{
				((OnPreparedListener)newPreparedListener).onPrepared(mtkMediaPlayer.getPlayer());
			}	

		}

			
		


	}
	

	private void start(){

		try {
			mtkMediaPlayer.start();
			mPlayStatus = AudioConst.PLAY_STATUS_STARTED;
			speedStep = 1;
			mPlayerSpeed = PlayerSpeed.SPEED_1X;

		} catch (Exception e) {
			// TODO: handle exception
			throw new IllegalStateException();
		}
		

	}

	
	private boolean getPcmMetaInfo(String path) {

		if (mPlayMode != AudioConst.PLAYER_MODE_DLNA) {
			pcmInfo = null;
			pcmMode = false;
		} else {
			DLNADataSource dlnaDataSource = DLNAManager.getInstance()
					.getDLNADataSource(path);

			if (dlnaDataSource == null) {
				pcmMode = false;
				return false;
			}

			if (dlnaDataSource.getContent().getMimeType().equals(
					FileSuffixConst.DLNA_MEDIA_MIME_TYPE_AUDIO_L16)) {

				pcmMode = true;

				MmpTool.LOG_DBG("pcm duration = "
						+ dlnaDataSource.getContent().getResDuration());

				String resDur = dlnaDataSource.getContent().getResDuration();
				int duration = 0;
				if (resDur != null){
					String[] time = resDur.split(":");

				MmpTool.LOG_DBG("pcm time = " + time[0]);
				MmpTool.LOG_DBG("pcm time = " + time[1]);
				MmpTool.LOG_DBG("pcm time = " + time[2]);

				int hour = Integer.parseInt(time[0]);
				int m = Integer.parseInt(time[1]);
				float s = Float.parseFloat(time[2]) * 1000;

				int second = Math.round(s);

				MmpTool.LOG_DBG("pcm time = " + hour);
				MmpTool.LOG_DBG("pcm time = " + m);
				MmpTool.LOG_DBG("pcm time = " + second);

					duration = (hour * 60 + m) * 60 * 1000 + second;
				MmpTool.LOG_DBG("pcm duration = " + duration);
				}
				long size = dlnaDataSource.getContent().getSize();
				MmpTool.LOG_DBG("pcm size = " + size);

				int type = PcmMediaInfo.AudioPcmInfo.AUD_PCM_TYPE_NORMAL;
				int channelNum = 0;
				String nac = dlnaDataSource.getContent().getNrAudioChannels();
				if (nac != null){
					channelNum = Integer.valueOf(nac);
				}
				MmpTool.LOG_DBG("pcm channelNum = " + channelNum);
				int sampleRate = 0;
				String sf = dlnaDataSource.getContent().getSampleFrequency();
				if (sf != null){
					sampleRate = Integer.valueOf(sf) / 1000;
					sampleRate = mapSampleRate(sampleRate * 1000);
				}
				MmpTool.LOG_DBG("pcm sampleRate = " + sampleRate);
				short blockAlign = 0;

				int bitsPerSample;
				String bits = dlnaDataSource.getContent().getBitsPerSample();
				if (bits == null) {
					bitsPerSample = 0;
				} else {
					bitsPerSample = Integer.valueOf(bits);
				}
				MmpTool.LOG_DBG("pcm bitsPerSample = " + bitsPerSample);
				int bigEndian = 1;

				pcmInfo = new PcmMediaInfo(duration, size, 0);

				PcmMediaInfo.AudioPcmInfo audInfo = pcmInfo.new AudioPcmInfo(
						type, channelNum, sampleRate, blockAlign,
						bitsPerSample, bigEndian);

				pcmInfo.setAudioPcmInfo(audInfo);
			} else {

				pcmInfo = null;
			
				pcmMode = false;
			}
		}
		MmpTool.LOG_DBG("pcmMode = " + pcmMode);
		return true;
	}

	// map sampleRate
	private int mapSampleRate(int sampleRate) {
		switch (sampleRate) {
		case 8000:
			sampleRate = PcmMediaInfo.AudioPcmInfo.AUD_SAMPLE_RATE_8K;
			break;
		case 16000:
			sampleRate = PcmMediaInfo.AudioPcmInfo.AUD_SAMPLE_RATE_16K;
			break;
		case 32000:
			sampleRate = PcmMediaInfo.AudioPcmInfo.AUD_SAMPLE_RATE_32K;
			break;
		case 11000:
			sampleRate = PcmMediaInfo.AudioPcmInfo.AUD_SAMPLE_RATE_11K;
			break;
		case 22000:
			sampleRate = PcmMediaInfo.AudioPcmInfo.AUD_SAMPLE_RATE_22K;
			break;
		case 44000:
			sampleRate = PcmMediaInfo.AudioPcmInfo.AUD_SAMPLE_RATE_44K;
			break;
		case 12000:
			sampleRate = PcmMediaInfo.AudioPcmInfo.AUD_SAMPLE_RATE_12K;
			break;
		case 24000:
			sampleRate = PcmMediaInfo.AudioPcmInfo.AUD_SAMPLE_RATE_24K;
			break;
		case 48000:
			sampleRate = PcmMediaInfo.AudioPcmInfo.AUD_SAMPLE_RATE_48K;
			break;
		case 96000:
			sampleRate = PcmMediaInfo.AudioPcmInfo.AUD_SAMPLE_RATE_96K;
			break;
		case 192000:
			sampleRate = PcmMediaInfo.AudioPcmInfo.AUD_SAMPLE_RATE_192K;
			break;

		default:
			Log.e("MtkPlayback", "SampleRate is error!!! sampleRate==="
					+ sampleRate);
			sampleRate = PcmMediaInfo.AudioPcmInfo.AUD_SAMPLE_RATE_48K;
			break;
		}
		return sampleRate;
	}

	 

	 
	public boolean isSeekable() {
		// TODO Auto-generated method stub
		if (mPlayMode == AudioConst.PLAYER_MODE_DLNA) {
			DLNADataSource dlnaDataSource = DLNAManager.getInstance()
					.getDLNADataSource(dataSource);
			if (dlnaDataSource != null) {
				return dlnaDataSource.getContent().canSeek();
			}
		}
		return true;
	}
	
	public void notifyInfo(int what) {
		if (null != newInfoListener) {
			if(newInfoListener instanceof MtkMediaPlayer.OnInfoListener){
				((MtkMediaPlayer.OnInfoListener)newInfoListener).onInfo(mtkMediaPlayer.getMtkPlayer(),what, what);
			}else{
				((OnInfoListener)newInfoListener).onInfo(mtkMediaPlayer.getPlayer(),what, what);
			}
			
		}

		
	}

}
