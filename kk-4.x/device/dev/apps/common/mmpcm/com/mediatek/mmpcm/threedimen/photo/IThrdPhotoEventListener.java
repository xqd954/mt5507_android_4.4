package com.mediatek.mmpcm.threedimen.photo;

public interface IThrdPhotoEventListener {
	/**
	 * complete while play all files
	 */
	public void onCompleted();
	/**
	 * failed while play a failed.
	 */
	public void onPlayFailed();
	/**
	 * connect vdp failed.
	 */
	public void onOpenFailed();
	/**
	 * disconnect vdp failed.
	 */
	public void onCloseFailed();
//add by shuming 	
	public void playDone();
	
	public void decodeSuccess();
}
