package com.mediatek.ui.nav.util;

import android.util.Log;

public class ScrambleKey {
	private boolean mIsAudioScrambled = false;
	private boolean mIsVideoScrambled = false;
	private static String TAG="ScrambleKey";

	public synchronized void unLockAudio() {
		mIsAudioScrambled = false;
	}

	public synchronized void lockAudio() {
		mIsAudioScrambled = true;
	}

	public synchronized void unLockVideo() {
		mIsVideoScrambled = false;
	}

	public synchronized void lockVideo() {
		mIsVideoScrambled = true;
	}

	public boolean isAudioScrambled() {
		return mIsAudioScrambled;
	}

	public boolean isVideoScrambled() {
		return mIsVideoScrambled;
	}

	public void resetScramble() {
		Log.v(TAG, "****************reset scrambled******************");
		mIsAudioScrambled = false;
		mIsVideoScrambled = false;
	}

	@Override
	public String toString() {
		// TODO Auto-generated method stub
		return "" + mIsAudioScrambled + "\n" + mIsVideoScrambled;
	}

	@Override
	public boolean equals(Object o) {
		// TODO Auto-generated method stub
		boolean flag = false;
		ScrambleKey sk = (ScrambleKey) o;
		if (sk.isAudioScrambled() == this.isAudioScrambled()
				&& sk.isVideoScrambled() == this.isVideoScrambled()) {
			flag = true;
		}
		return flag;
	}
	
	
	@Override
	public int hashCode() {
		return super.hashCode();
	}
	

	public void copy(ScrambleKey sk) {
		mIsAudioScrambled = sk.isAudioScrambled();
		mIsVideoScrambled = sk.isVideoScrambled();

	}
}
