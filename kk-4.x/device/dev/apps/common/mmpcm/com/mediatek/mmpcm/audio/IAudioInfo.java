package com.mediatek.mmpcm.audio;

import com.mediatek.mmpcm.MetaData;

import android.graphics.Bitmap;

public interface IAudioInfo {
	/**
	 * get albumArt work
	 * 
	 * @return Bitmap
	 */
	//Bitmap getAlbumArtwork();

	/**
	 * According to the parameters for the get albumArt work
	 * 
	 * @param path
	 * @return
	 */
	//Bitmap getAlbumArtwork(String path);

	/**
	 * According to the parameters for the get albumArt work
	 * 
	 * @param artwork
	 */
	//void setAlbumArtwork(Bitmap artwork);

	/**
	 * according to the parameters for the set albumArt work
	 * 
	 * @param Path
	 */
	//void setAlbumArtwork(String Path);

	/**
	 * According to the parameters for the meta data info
	 * 
	 * @param path
	 * @param srcType
	 * @return return meta data info
	 */
	public MetaData getMetaDataInfo(String path, int srcType);

	/**
	 * stop get meta data
	 */
	public void stopMetaData();
}
