/**
 * 
 */
package com.mediatek.tvcommon;

public class PalPreSetChannel {
	private int channelNum;
	private String channelName;
	private int frequency;
	private int rawColorSys;
	private int rawTvSys;
	private int rawAudioSys;

	/**
	 * 
	 * @param chNum
	 * @param chName
	 * @param color
	 *            reference macro of class ColorSystemOption
	 * @param audio
	 *            reference macro of class TVAudioSystemOption
	 */
	public PalPreSetChannel(int chNum, String chName, int freq, int color,
			int tvAudio) {
		ColorSystemOption colorOpt = new ColorSystemOption();
		TvAudioSystemOption tvAudioOpt = new TvAudioSystemOption();
		int defcolor = colorOpt.get();
		int defaudio = tvAudioOpt.get();
		this.channelNum = chNum;
		this.channelName = chName;
		this.frequency = freq;

		if (color >= colorOpt.getMin() && color <= colorOpt.getMax()) {
			colorOpt.set(color);
			this.rawColorSys = colorOpt.getRawColorSystem();
		} else {
			this.rawColorSys = defcolor;
		}

		if (tvAudio >= tvAudioOpt.getMin() && tvAudio <= tvAudioOpt.getMax()) {
			tvAudioOpt.set(tvAudio);
		} else {
			tvAudioOpt.set(defaudio);
		}
		this.rawTvSys = tvAudioOpt.getRawTvSystem();
		this.rawAudioSys = tvAudioOpt.getRawAudSystem();
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

	public void setColorSys(int color) {
		this.rawColorSys = color;
	}

	public int getColorSys() {
		return this.rawColorSys;
	}

	public int getTvSys() {
		return this.rawTvSys;
	}

	public void setAudioSys(int audio) {
		this.rawAudioSys = audio;
	}

	public int getAudioSys() {
		return this.rawAudioSys;
	}
}