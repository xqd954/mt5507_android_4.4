package com.mediatek.mmpcm.audio;

import android.graphics.Bitmap;

public interface ICorverPic {
	/**
	 * Get audio corver picture
	 * @param srcType
	 * @param filepath
	 * @param width
	 * @param height
	 * @return
	 * @throws IllegalArgumentException
	 */
    public Bitmap getAudioCorverPic(int srcType, String filepath, int width,
            int height) throws IllegalArgumentException ;
    /**
     * Stop get meta data thumbnail
     */
    public void stopThumbnail();
}
