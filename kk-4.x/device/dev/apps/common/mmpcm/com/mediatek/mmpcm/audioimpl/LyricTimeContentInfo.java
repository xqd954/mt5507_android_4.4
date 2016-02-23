package com.mediatek.mmpcm.audioimpl;

public class LyricTimeContentInfo implements Comparable<LyricTimeContentInfo> {
	private long startTime;
	private long endTime;
	private long duration;
	private String lyricContent;

	/**
	 * Construction method
	 * 
	 * @param currentLineStartTime
	 * @param currentLineLyricContent
	 */
	LyricTimeContentInfo(long currentLineStartTime,
			String currentLineLyricContent) {
		startTime = currentLineStartTime;
		lyricContent = currentLineLyricContent;
		endTime = 0;
		duration = 0;
	}
	/**
	 * Set LyricTimeContentInfo
	 * @param nextLineStartTime
	 */
	public void setLyricTimeContentInfo(long nextLineStartTime) {
		endTime = nextLineStartTime;
		duration = endTime - startTime;
	}

	public long getStartTime() {
		return startTime;
	}

	public void setStartTime(long newStartTime) {
		startTime = newStartTime;
	}

	public long getEndTime() {
		return endTime;
	}

	public void setEndTime(long newEndTime) {
		endTime = newEndTime;
	}

	public String getLyricContent() {
		return lyricContent;
	}

	public long getDuration() {
		return duration;
	}

	public int compareTo(LyricTimeContentInfo arg0) {
		// TODO Auto-generated method stub
		if (startTime < arg0.startTime) {
			return -1;
		} else if (startTime > arg0.startTime) {
			return 1;
		} else {
			return 0;
		}
	}

}