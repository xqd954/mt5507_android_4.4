package com.mediatek.mmpcm.audioimpl;

import com.mediatek.mmp.MtkMediaPlayer;
import com.mediatek.mmpcm.MmpTool;
import com.mediatek.mmpcm.audio.IPlayback;

import com.mediatek.mmpcm.UIMediaPlayer;


import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Binder;
import android.os.IBinder;
import android.media.MediaPlayer;

public class PlaybackService extends Service {

    private final LocalBinder mBinder = new LocalBinder();
    private IPlayback mPlayback = null;
	public final static String PLAY_TYPE = "player_type";
	
    private AudioBroadcastReceiver audioBroadcastReceiver = new AudioBroadcastReceiver();

    public class LocalBinder extends Binder {
        public PlaybackService getService() {
            return PlaybackService.this;
        }
    };
   
    public class AudioBroadcastReceiver extends BroadcastReceiver {

        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if ("com.mediatek.closeAudio".equals(action) == true) {
                MmpTool.LOG_DBG("onReceive");
                stopSelf();
            }
        }

    }

    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    public void onCreate() {

        MmpTool.LOG_DBG("onCreate");
        super.onCreate();
		//getIntent()

        IntentFilter filter = new IntentFilter("com.mediatek.closeAudio");
        registerReceiver(audioBroadcastReceiver, filter);
    }

    public int onStartCommand(Intent intent, int flags, int startId) {

        MmpTool.LOG_DBG("onStartCommand");

        super.onStartCommand(intent, flags, startId);
		int value = intent.getIntExtra(PLAY_TYPE,AudioConst.PLAYER_MODE_LOCAL);

		mPlayback = new AudioManager(value);

        return START_STICKY;
    }

    public void onDestroy() {

        MmpTool.LOG_DBG("onDestroy");
        super.onDestroy();
        mPlayback.release();
        mPlayback = null;
        unregisterReceiver(audioBroadcastReceiver);
    }

    public boolean onUnbind(Intent intent) {

        MmpTool.LOG_DBG("onUnbind");
        return super.onUnbind(intent);
    }

    public boolean isPlaying() {
        return mPlayback.isPlaying();
    }

    public int getSpeed() {
        return mPlayback.getSpeed();
    }
    /**
     * set normal play, ff, fr and so on.
     */
    public void setSpeed(int speed){
    	mPlayback.setSpeed(speed);
    }
    public int getPlayStatus() {
        return mPlayback.getPlayStatus();
    }

    public void setPlayMode(int playMode) {
        mPlayback.setPlayMode(playMode);
    }

    public void setDataSource(final String path) {
 
		new Thread(new Runnable() {
			public void run() {
				mPlayback.setDataSource(path);
			}
		}).start();

    }

    public void play() throws IllegalStateException {
        mPlayback.play();
    }
    public boolean canSeek(){
    	return mPlayback.isSeekable();
    }

    public void pause() throws IllegalStateException {
        mPlayback.pause();
    }

    public void stop() throws IllegalStateException {
        mPlayback.stop();
    }
	public String getFilePath(){
		return mPlayback.getFilePath();

	}
    public void playNext() throws IllegalStateException {
        mPlayback.playNext();
    }

    public void playPrevious() throws IllegalStateException {
        mPlayback.playPrevious();
    }

    public void seekToCertainTime(long time) throws IllegalStateException {
         mPlayback.seekToCertainTime(time);
    }

    public int getPlaybackProgress() {

        return mPlayback.getPlaybackProgress();
    }

    public int getTotalPlaybackTime() {

        return mPlayback.getTotalPlaybackTime();
    }
	public long getFileSize() {
	
		  return mPlayback.getFileSize();
	}

	public int getCurrentBytePosition() {
		
			  return mPlayback.getCurrentBytePosition();
	}

	

    public void fastForward() throws  IllegalStateException {

        // TODO to be implemented
        mPlayback.fastForward();
    }

    public void fastRewind() throws  IllegalStateException {

        // TODO to be implemented
        mPlayback.fastRewind();
    }

    public int getBitRate() {
        return mPlayback.getBitRate();
    }

    public int getSampleRate() {
        return mPlayback.getSampleRate();
    }

    public String getAudioCodec() {
        return mPlayback.getAudioCodec();
    }

    public String getTitle() {
        return mPlayback.getMusicTitle();
    }

    public String getArtist() {
        return mPlayback.getMusicArtist();
    }

    public String getAlbum() {
        return mPlayback.getMusicAlbum();
    }

    public String getGenre() {
        return mPlayback.getMusicGenre();
    }

    public String getYear() {
        return mPlayback.getMusicYear();
    }
/**
 *  Register AudioCompletion Listener
 * @param completionListener
 */
    public void registerAudioCompletionListener(Object completionListener) {
        mPlayback.registerAudioCompletionListener(completionListener);
        
    }
/**
 * Unregister AudioCompletion Listener
 */
    public void unregisterAudioCompletionListener() {
        mPlayback.unregisterAudioCompletionListener();
    }
/**
 * Register AudioPrepared Listener
 * @param preparedListener
 */
    public void registerAudioPreparedListener(Object preparedListener) {
        mPlayback.registerAudioPreparedListener(preparedListener);
      
    }

    public void unregisterAudioPreparedListener() {
        mPlayback.unregisterAudioPreparedListener();
    }

    //updata by hs_haizhudeng
    /**
     * register AudioError Listener
     */
    public void registerAudioErrorListener(Object errorListener) {
        
        mPlayback.registerAudioErrorListener(errorListener);
       
    }

    public void unregisterAudioErrorListener() {
        mPlayback.unregisterAudioErrorListener();
    }
    
    public void registerAudioDurationUpdateListener(Object durationListener){
        //mPlayback.registerAudioDurationUpdateListener((MtkMediaPlayer.OnTotalTimeUpdateListener) durationListener);
    }   
    
    public void unregisterAudioDurationUpdateListener(){
        mPlayback.unregisterAudioDurationUpdateListener();
    }
    
    public void registerInfoListener(Object replayListener){
    	mPlayback.registerInfoListener(replayListener);
    }
    
    public void unregisterInfoListener(){
    	mPlayback.unregisterInfoListener();
    }
}
