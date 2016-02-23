package com.mediatek.mmpcm.audio;

import java.util.Vector;

import com.mediatek.mmpcm.audioimpl.LyricTimeContentInfo;

public interface ILyric {
	/**
	 * Get lyric title 
	 * @return string lyric title 
	 */
	String getLyricTitle();	
	/**
	 * Get lyric artist 
	 * @return string lyric artist
	 */
	String getLyricArtist();
	/**
	 * Get lyric album 
	 * @return string lyric album
	 */
	String getLyricAlbum();
	/**
	 *  Get lyric editor
	 * @return string lyric editor
	 */
	String getLyricEditor();
	/**
	 * Get lyric timeOffset
	 * @return long  lyric timeOffset
	 */
	long getLyricTimeOffset();
	/**
	 * According to the parameters set lyric timeOffset
	 * @param timeOffset
	 */
	void setLyricTimeOffset(long timeOffset);
	/**
	 * Get line of time
	 * @param time
	 * @return int time line
	 */
	int getLine(long time);
	/**
	 * Get lyric time content info
	 * @return Vector lyric time content info
	 */
	Vector<LyricTimeContentInfo> getLyricTimeContentInfo();
}
