package com.mediatek.tvcommon;

interface ITVSelectorListener {

	void onChannelSelected(int channelNum);

	void onChannelBlocked(int channelNum);
	
	void onInputSelected(String outputName, String inputName);
	
	void onInputBlocked(String inputName);
		
}