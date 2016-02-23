package com.mediatek.tvcommon;

import java.util.Enumeration;
import java.util.Vector;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.Message;

import android.util.Log;

import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.TVManager;
import com.mediatek.tv.service.BroadcastService;
import com.mediatek.tv.service.ComponentService;

public class TVChannelState {

	private Context mContext = null;
	private Handler mHandler = null;

	private boolean hasSignal = false;
	private boolean hasScramble = false;
	private String scrambleState = ChannelListener.STATE_UNSCRAMBLED;

	private Vector<ChannelListener> listeners = new Vector<ChannelListener>();

	private static TVChannelState state = null;

	private TVManager tvMgr;
	private ComponentService compSrv;

	public static synchronized TVChannelState getInstance(Context context) {
		if (state == null) {
			state = new TVChannelState(context);
		}

		return state;
	}

	TVChannelState(Context context) {		
		mContext = context.getApplicationContext();
		mHandler = new Handler(mContext.getMainLooper(),
				new Handler.Callback() {
					@Override
					public boolean handleMessage(Message msg) {
						TVChannelState.this.handleMessage(msg);
						return true;
					}
				});

		IntentFilter filter = new IntentFilter();
		filter.addAction(BroadcastService.ACTION_SVCTX_NFY);
		mContext.registerReceiver(new BroadcastReceiver() {
			private ScrambleState state = new ScrambleState();

			@Override
			public void onReceive(Context context, Intent intent) {
				String nfyCode = intent.getStringExtra(BroadcastService.SVCTX_NFY_CODE);
				TVCommonNative.TVLog("notify : " + nfyCode);
				if (nfyCode.equals(BroadcastService.SVCTX_NFY_WITH_SIGNAL)) 
				{
					setHasSignal(true);
					new DelegaterSignalChange(true).run();
				}
				else if (nfyCode.equals(BroadcastService.SVCTX_NFY_NO_SIGNAL)) 
				{
					setHasSignal(false);
					new DelegaterSignalChange(false).run();
				}
				else if (nfyCode.equals(BroadcastService.SVCTX_NFY_AUDIO_AND_VIDEO_SCRAMBLED)  	|| 
					     nfyCode.equals(BroadcastService.SVCTX_NFY_AUDIO_CLEAR_VIDEO_SCRAMBLED) || 
					     nfyCode.equals(BroadcastService.SVCTX_NFY_AUDIO_NO_VIDEO_SCRAMBLED)	|| 
					     nfyCode.equals(BroadcastService.SVCTX_NFY_VIDEO_CLEAR_AUDIO_SCRAMBLED) || 
					     nfyCode.equals(BroadcastService.SVCTX_NFY_VIDEO_NO_AUDIO_SCRAMBLED)) 
				{
					state.setState(nfyCode);
				}
				else if (nfyCode.equals(BroadcastService.SVCTX_NFY_AUDIO_UPDATED)) 
				{
					state.updateAudio();
				}
				else if (nfyCode.equals(BroadcastService.SVCTX_NFY_VIDEO_UPDATED)) 
				{
					state.updateVideo();
				} 
				else if (nfyCode.equals(BroadcastService.SVCTX_NFY_NO_AUDIO_VIDEO)	|| 
						 nfyCode.equals(BroadcastService.SVCTX_NFY_AUDIO_ONLY)		|| 
						 nfyCode.equals(BroadcastService.SVCTX_NFY_VIDEO_ONLY)		|| 
						 nfyCode.equals(BroadcastService.SVCTX_NFY_AUDIO_VIDEO_NORMAL)) 
				{
					state.setState(ChannelListener.STATE_UNSCRAMBLED);
				}
				else if (nfyCode.equals(BroadcastService.SVCTX_NFY_VDP_MUTE_ALL_FINISHED))
				{
					// now TV(atv/dtv) exist only in one output, main or sub
					// when TV has signal(current), vdp will nfy mute_all_finished, which could be trust.
					// when TV has no signal, setHasSignal(false) is the last msg/notify, which could be trust.
					resumeTvMuteColor();
				}
			}

		}, filter);

		tvMgr = TVManager.getInstance(mContext);
		compSrv = (ComponentService) tvMgr.getService(ComponentService.CompServiceName);
		if (tvMgr == null || compSrv == null) {
			throw new IllegalAccessError("Fatal Error happens when initiliaze TVChannelState");
		}
	}


	protected void handleMessage(Message msg) {
		return;
	}

	public interface ChannelListener {
		final static String STATE_AUDIO_AND_VIDEO_SCRAMBLED = "videoAndAudioScrambled";
		final static String STATE_AUDIO_CLEAR_VIDEO_SCRAMBLED = "audioScrambled";
		final static String STATE_AUDIO_NO_VIDEO_SCRAMBLED = "audioScrambledAndNoVideo";
		final static String STATE_VIDEO_CLEAR_AUDIO_SCRAMBLED = "videoScrambled";
		final static String STATE_VIDEO_NO_AUDIO_SCRAMBLED = "videoScrambledAndNoAudio";
		final static String STATE_UNSCRAMBLED = "unscrambled";

		public void onChannelSignalChange(boolean hasSignal);

		public void onChannelScramble(boolean hasScramble, String state);
	}

	class ScrambleState {
		String state;

		public ScrambleState() {
			this.state = ChannelListener.STATE_UNSCRAMBLED;
		}

		public ScrambleState(String state) {
			this.state = state;
		}

		public void setState(String state) {
			this.state = state;
			scrambleState = state;
			if (this.state.equals(ChannelListener.STATE_UNSCRAMBLED)) {
				TVCommonNative.TVLog("setHasScramble: false, state: " + this.state);
				setHasScramble(false);
				mHandler.post(new DegegaterScrambled(false, this.state));
			} else {
				TVCommonNative.TVLog("setHasScramble: true,  state: " + this.state);
				setHasScramble(true);
				mHandler.post(new DegegaterScrambled(true, this.state));
			}
		}

		public String getState() {
			return this.state;
		}

		public synchronized void updateAudio() {
			if (state.equals(ChannelListener.STATE_AUDIO_AND_VIDEO_SCRAMBLED)) 
			{
				setState(ChannelListener.STATE_VIDEO_CLEAR_AUDIO_SCRAMBLED);
			} 
			else if (state.equals(ChannelListener.STATE_AUDIO_CLEAR_VIDEO_SCRAMBLED) 	|| 
				     state.equals(ChannelListener.STATE_AUDIO_NO_VIDEO_SCRAMBLED)) 
			{
				setState(ChannelListener.STATE_UNSCRAMBLED);
			}
		}

		public synchronized void updateVideo() {
			if (state.equals(ChannelListener.STATE_AUDIO_AND_VIDEO_SCRAMBLED)) 
			{
				setState(ChannelListener.STATE_AUDIO_CLEAR_VIDEO_SCRAMBLED);
			} 
			else if (state.equals(ChannelListener.STATE_VIDEO_CLEAR_AUDIO_SCRAMBLED)	|| 
					 state.equals(ChannelListener.STATE_VIDEO_NO_AUDIO_SCRAMBLED)) 
			{
				setState(ChannelListener.STATE_UNSCRAMBLED);
			}
		}
	}

	class DelegaterSignalChange implements Runnable {
		private boolean hasSignal = false;

		public DelegaterSignalChange(boolean hasSignal) {
			this.hasSignal = hasSignal;
		}

		public void run() {
			Enumeration<ChannelListener> e = listeners.elements();
			ChannelListener item;
			while (e.hasMoreElements()) {
				item = e.nextElement();
				item.onChannelSignalChange(hasSignal);
			}
		}
	}

	class DegegaterScrambled implements Runnable {
		private boolean hasScramble;
		private String state;

		public DegegaterScrambled(boolean hasScramble, String state) {
			this.hasScramble = hasScramble;
			this.state = state;
		}

		public void run() {
			Enumeration<ChannelListener> e = listeners.elements();
			ChannelListener item;
			while (e.hasMoreElements()) {
				item = e.nextElement();
				item.onChannelScramble(hasScramble, state);
			}
		}
	}

	protected synchronized void setHasSignal(boolean hasSignal) {
		int ret = -100;
		this.hasSignal = hasSignal;
		TVCommonNative.TVLog("hasSignal: " + hasSignal);

		if (!hasSignal) {
			// now TV(atv/dtv) exist only in one output, main or sub
			// when TV has signal, vdp will nfy mute_all_finished, which could be trust.
			// when TV has no signal(currect), setHasSignal(false) is the last msg/notify, which could be trust.
			resumeTvMuteColor();
		}
	}

	protected synchronized void setHasScramble(boolean hasScramble) {
		this.hasScramble = hasScramble;
	}

	public synchronized boolean hasScramble() {
		return hasScramble;
	}

	public synchronized boolean hasSignal() {
		return hasSignal;
	}
	
	public String getScrambleState() {
		return scrambleState;
	}
	
	public synchronized void addChannelListener(ChannelListener listener) {
		listeners.add(listener);
	}

	public synchronized void removeChannelListener(ChannelListener listener) {
		listeners.remove(listener);
	}

	private void resumeTvMuteColor() {
		int ret = -100;
		int winIdTvIn = TVCommonService.getWinIdTvIn();
		TVCommonNative.TVLog("winIdTvIn " + winIdTvIn + ", main/sub black mute: " + CrossRoad.isForceBlackMuteMain + "/" + CrossRoad.isForceBlackMuteSub);
		if (CrossRoad.isForceBlackMuteMain && TVCommonService.TV_WIN_ID_MAIN == winIdTvIn) {
			// just set once
			CrossRoad.isForceBlackMuteMain = false;
			ret = TVMethods.setVideoMuteById(winIdTvIn, true, 0, 0, 255);
			TVCommonNative.TVLog("setVideoMuteById(main(0), true, 0, 0, 255) ret " + ret);
		} else if (CrossRoad.isForceBlackMuteSub && TVCommonService.TV_WIN_ID_SUB == winIdTvIn) {
			// just set once
			CrossRoad.isForceBlackMuteSub  = false;
			ret = TVMethods.setVideoMuteById(winIdTvIn, true, 0, 0, 255);
			TVCommonNative.TVLog("setVideoMuteById(sub(1), true, 0, 0, 255) ret " + ret);
		} 
	}
	
}

