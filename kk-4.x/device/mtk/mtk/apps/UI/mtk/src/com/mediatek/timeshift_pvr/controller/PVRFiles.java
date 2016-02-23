/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.controller;

import java.io.File;

import com.mediatek.timeshift_pvr.manager.Util;

/**
 *PVR file info.
 */
public class PVRFiles extends File {

	private String channelNum = "";
	private String channelName = "";
	private String programName = "";
	private String date = "";
	private String week = "";
	private String time = "";
	private String fileName = "";
	private int duration = 0;
	private String indexAndName="";
	
	public boolean isRecording=false;
	public boolean isPlaying=false;
	
	private String debugStr="Debug";
	
	/**
	 * Only for debug.!!!
	 * @param debugStr
	 */
	@Deprecated
	public PVRFiles(String debugStr,int num) {
		super("");
		this.debugStr = debugStr;
		channelName=debugStr;
		channelNum=num+"";
	}
	
	public PVRFiles(String path) {
		super(path);
	}
	
	/**
	 * Only for debug.!!!
	 * @param debugStr
	 */
	@Deprecated
	public PVRFiles(String debugStr,String chNum) {
		super("");
		this.debugStr = debugStr;
		channelName=debugStr;
		channelNum=chNum;
	}


	public String getDurationStr() {
	    return Util.secondToString((int)duration);
//		return duration+"";
    }

    public long getDuration() {
        return duration;
    }

	public void setDuration(int duration) {
		this.duration = duration;
	}

	public String getChannelNum() {
		return channelNum;
	}

	public void setChannelNum(String channelNum) {
		this.channelNum = channelNum;
	}

	public String getChannelName() {
		return channelName;
	}

	public void setChannelName(String channelName) {
		this.channelName = channelName;
	}

	public String getProgramName() {
		return programName;
	}

	public void setProgramName(String programName) {
		this.programName = programName;
	}

	public String getDate() {
		return date;
	}

	public void setDate(String date) {
		this.date = date;
	}

	public String getWeek() {
		return week;
	}

	public void setWeek(String week) {
		this.week = week;
	}

	public String getTime() {
		return time;
	}

	public void setTime(String time) {
		this.time = time;
	}

	public String getFileName() {
		return fileName;
	}

	public void setFileName(String fileName) {
		this.fileName = fileName;
	}

	@Override
	public String toString() {
		// TODO Auto-generated method stub
		return this.getName();
	}
	public String getIndexAndName() {
		return indexAndName;
	}

	public void setIndexAndName(String indexAndName) {
		this.indexAndName = indexAndName;
	}
	
}
