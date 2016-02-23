/**
 * 
 */
package com.mediatek.tvcommon;

import com.mediatek.tv.common.TVCommon;

import android.util.Log;

public class DvbPreSetChannel {
	public static int MOD_UNKNOWN = 0;
	public static int MOD_PSK_8 = 1;
	public static int MOD_VSB_8 = 2;
	public static int MOD_VSB_16 = 3;
	public static int MOD_QAM_16 = 4;
	public static int MOD_QAM_32 = 5;
	public static int MOD_QAM_64 = 6;
	public static int MOD_QAM_80 = 7;
	public static int MOD_QAM_96 = 8;
	public static int MOD_QAM_112 = 9;
	public static int MOD_QAM_128 = 10;
	public static int MOD_QAM_160 = 11;
	public static int MOD_QAM_192 = 12;
	public static int MOD_QAM_224 = 13;
	public static int MOD_QAM_256 = 14;
	public static int MOD_QAM_320 = 15;
	public static int MOD_QAM_384 = 16;
	public static int MOD_QAM_448 = 17;
	public static int MOD_QAM_512 = 18;
	public static int MOD_QAM_640 = 19;
	public static int MOD_QAM_768 = 20;
	public static int MOD_QAM_896 = 21;
	public static int MOD_QAM_1024 = 22;
	public static int MOD_QPSK = 23;
	public static int MOD_OQPSK = 24;
	public static int MOD_BPSK = 25;
	public static int MOD_VSB_AM = 26;
	public static int MOD_QAM_4_NR = 27;
	public static int MOD_FM_RADIO = 28;

	private int channelNum;
	private String channelName;
	private int frequency;
	private int bindWidth;
	private int nwId;
	private int onId;
	private int tsId;
	private int progId;
	private int symRate;
	private int mod;

	/**
	 * 
	 * @param chNum
	 * @param chName
	 * @param color
	 *            reference macro of class ColorSystemOption
	 * @param audio
	 *            reference macro of class TVAudioSystemOption
	 */
	public DvbPreSetChannel(int chNum, String chName, int freq, int bindWidth,
			int nwId, int onId, int tsId, int progId, int symRate, int mod) {
		this.channelNum = chNum;
		this.channelName = chName;
		this.frequency = freq;
		this.bindWidth = bindWidth;
		this.nwId = nwId;
		this.onId = onId;
		this.tsId = tsId;
		this.progId = progId;
		this.symRate = symRate;
		this.mod = mod;
	}

	public void setChannelNum(int chNum) {
		this.channelNum = chNum;
	}

	public int getChannelNum() {
		return this.channelNum;
	}

	public void setChannelName(String chName) {
		this.channelName = chName;
	}

	public String getChannelName() {
		return this.channelName;
	}

	public void setFrequency(int freq) {
		this.frequency = freq;
	}

	public int getFrequency() {
		return this.frequency;
	}

	public void setBindWidth(int bindWidth) {
		this.bindWidth = bindWidth;
	}

	public int getBindWidth() {
		return this.bindWidth;
	}

	
	public void setNetworkId(int nwId) {
		this.nwId = nwId;
	}

	public int getNetworkId() {
		return this.nwId;
	}
	
	public void setOnId(int onId) {
		this.onId = onId;
	}

	public int getOnId() {
		return this.onId;
	}
	
	
	public void setTsId(int tsId) {
		this.tsId = tsId;
	}

	public int getTsId() {
		return this.tsId;
	}

	public void setProgId(int progId) {
		this.progId = progId;
	}

	public int getProgId() {
		return this.progId;
	}
	
	public void setSymRate(int symRate) {
		this.symRate = symRate;
	}

	public int getSymRate() {
		return this.symRate;
	}
	
	public void setMod(int mod) {
		this.mod = mod;
	}

	public int getMod() {
		return this.mod;
	}
}