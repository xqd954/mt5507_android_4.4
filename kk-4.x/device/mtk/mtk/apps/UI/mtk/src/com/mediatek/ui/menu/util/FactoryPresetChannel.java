package com.mediatek.ui.menu.util;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.List;

import android.content.Context;
import com.mediatek.dm.MountPoint;
import com.mediatek.mmpcm.device.DevManager;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tvcommon.ITVCommon.ChannelsChangedListener;
import com.mediatek.tvcommon.PalPreSetChannel;
import com.mediatek.tvcommon.DvbPreSetChannel;
import com.mediatek.tvcommon.TVCommonManager;
import com.mediatek.tvcommon.TVOutputCommon.ColorSystemOption;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVScanner;
import com.mediatek.tvcommon.TvAudioSystemOption;
import com.mediatek.tvcommon.CrossRoad;
import com.mediatek.tvcommon.TVCommonNative;

public class FactoryPresetChannel {
	private static final int STARTCH_NUM = 900;
	private TVCommonManager mTVManager;
	private TVScanner mTVScanner;
	private Context mContext;
	private List<TVChannel> channels;
	private List<PalPreSetChannel> palPreSetChannel  = new ArrayList<PalPreSetChannel>();
	private List<DvbPreSetChannel> dvbcPreSetChannel = new ArrayList<DvbPreSetChannel>();
	private List<DvbPreSetChannel> dtmbPreSetChannel = new ArrayList<DvbPreSetChannel>();
	private DevManager mDevManager;
	private ArrayList<MountPoint> mDevice;
	private String mpath = "/";
	private static Thread testThread1 = null;
	private static Thread testThread2 = null;
	private static Thread testThread3 = null;

	private final int[][] mChannelPresetList = {
			{ STARTCH_NUM + 0, 136250000, ColorSystemOption.COLOR_SYS_PAL,
					TvAudioSystemOption.TV_BG },

			{ STARTCH_NUM + 1, 168250000, ColorSystemOption.COLOR_SYS_PAL,
					TvAudioSystemOption.TV_DK },

			{ STARTCH_NUM + 2, 152300000, ColorSystemOption.COLOR_SYS_PAL,
					TvAudioSystemOption.TV_DK },

			{ STARTCH_NUM + 3, 160300000, ColorSystemOption.COLOR_SYS_PAL,
					TvAudioSystemOption.TV_DK },

			{ STARTCH_NUM + 4, 176200000, ColorSystemOption.COLOR_SYS_PAL,
					TvAudioSystemOption.TV_DK },
					
			{ STARTCH_NUM + 5, 128200000, ColorSystemOption.COLOR_SYS_PAL,
					TvAudioSystemOption.TV_DK },
					
	};

	private final int cableDtvParam[][] = {
		/*(int chNum,  int freq, int bindWidth, int nwId, int onId, int tsId, int progId, int symRate, int mod) */
		{	  1, 	  642000000, 	    3, 	       0xfa0,   0x4000,     0x2e,       5248,     6875000,     6	},
		{	  2, 	  690000000, 	    3, 	       0x302,   0x5000,     0x3f,        576,     6875000,     6	},
	};

	private final String cableDtvParamName[] = {
		/* 	String chName, */
			" cablePre-1",
			" cablePre-2",
	};

	private final int airDtvParam[][] = {
		/*(int chNum,  int freq, int bindWidth, int nwId, int onId, int tsId, int progId, int symRate, int mod) */
		{	  1, 	  706000000, 		3, 		   32766, 	 32766, 	   1, 		  82, 	  6875000,     6	},
		{	  2, 	  706000000, 		3, 		   32766, 	 32766, 	   1, 		  83, 	  6875000,     6	},
		{	  3, 	  706000000, 		3, 		   32766, 	 32766, 	   1, 		  85, 	  6875000,     6	},
		{	  4, 	  754000000, 		3,     		   0,  	  4369, 	   1,  		   1, 	  6875000,    23	},
		{	  5, 	  754000000, 		3,	  		   0,  	  4369, 	   1,		   2, 	  6875000,    23	},
	};

	private final String airDtvParamName[] = {
		/* 	String chName, */
			" airPre-J2",
			" airPre-iNews",
			" airPre-High Definition Jade",
			" airPre-AMTV",
			" airPre-AMTV-1",
	};

	private final String testTag = "TEST ";   
	private static ChannelsChangedListener mChannelsIsChangedListenner = null;
	private static boolean hasAddListener = false;

	
	public FactoryPresetChannel(Context context) {
		mContext = context;
	}

	public void presetChannels() {
		mTVManager = TVCommonManager.getInstance(mContext);
		mTVScanner = TVScanner.getInstance(mContext);

		if (hasAddListener == false) {
			mChannelsIsChangedListenner = new ChannelsChangedListener() {
				@Override
				public void onChanged() {
					if(mTVManager.getChannels() != null && mTVManager.getChannels().size() > 0) {
						TVCommonNative.TVLog(testTag + "select svc after presetChannels");
						mTVManager.select(mTVManager.getChannels().get(0));
					} else {
						if (mTVManager.getChannels() != null) {
							TVCommonNative.TVLog(testTag + "channels size: " + mTVManager.getChannels().size());
						} else {
							TVCommonNative.TVLog(testTag + "channels == null");
						}
					}
				}
			};

			if (null != mChannelsIsChangedListenner) {
				TVCommonNative.TVLog(testTag + "add mChannelsIsChangedListenner");
				hasAddListener = true; 
				mTVManager.addChannelsChangedListener(mChannelsIsChangedListenner);
			} else {
				TVCommonNative.TVLog(testTag + "fail: null == mChannelsIsChangedListenner");
			}
		} else {
			TVCommonNative.TVLog(testTag + "has added Listener. Don't add again !");
		}

		/******************************************************
		* 1. just only cable atv. air atv is removed 
		* 2. preset 1 time to set atv_pre, dtmb_pre, dvbc_pre 
        ******************************************************/
		
		TVCommonNative.TVLog(testTag + "start preset all channels");
		
		/* clean chs */
		mTVManager.clearAllChannels();
		
		/* prepare analog params */
		for (int i = 0; i < 6; i++) {
			palPreSetChannel.add(new PalPreSetChannel(
					mChannelPresetList[i][0], 
					" ana-" + mChannelPresetList[i][0],
					mChannelPresetList[i][1], 
					mChannelPresetList[i][2],
					mChannelPresetList[i][3]));
		}
		
		/* preset analog here... */
		testThread1 = new Thread() {
			public void run() {
				if (mTVScanner != null) {
					TVCommonNative.TVLog(testTag + " preset analog-channels");
					mTVScanner.preSetAnalogChannels(palPreSetChannel);
				} else {
					TVCommonNative.TVLog(testTag + " fail: mTVScanner == null");
				}
			}
		};
		testThread1.start();
		
		
		/* prepare air digital params */
		for (int j = 0; j < 5; j++) {
			dtmbPreSetChannel.add(new DvbPreSetChannel(
				airDtvParam[j][0],		// chNum
				airDtvParamName[j], 	// chName
				airDtvParam[j][1],		// freq
				airDtvParam[j][2],		// bindWidth
				airDtvParam[j][3],		// nwId
				airDtvParam[j][4],		// onId
				airDtvParam[j][5],		// tsId
				airDtvParam[j][6],		// progId
				airDtvParam[j][7],		// symRate
				airDtvParam[j][8]));	// mod
		}
		
		/* preset air digital here... */
		testThread2 = new Thread() {
			public void run() {
				if (mTVScanner != null) {
					TVCommonNative.TVLog(testTag + " preset air digital-channels");
					mTVScanner.preSetDigitalChannelsByType(dtmbPreSetChannel, TVScanner.TYPE_AIR);
				} else {
					TVCommonNative.TVLog(testTag + " fail: mTVScanner == null");
				}
			}
		};
		testThread2.start();
		
		
		/* prepare cable digital params */
		for (int j = 0; j < 2; j++) {
			dvbcPreSetChannel.add(new DvbPreSetChannel(
				cableDtvParam[j][0],	//	chNum
				cableDtvParamName[j],	//	chName
				cableDtvParam[j][1],	//	freq
				cableDtvParam[j][2],	//	bindWidth
				cableDtvParam[j][3],	//	nwId
				cableDtvParam[j][4],	//	onId
				cableDtvParam[j][5],	//	tsId
				cableDtvParam[j][6],	//	progId
				cableDtvParam[j][7],	//	symRate
				cableDtvParam[j][8]));	//	mod
		}
		
		/* preset air digital here... */
		testThread3 = new Thread() {
			public void run() {
				if (mTVScanner != null) {
					TVCommonNative.TVLog(testTag + " preset cable digital Channels");
					mTVScanner.preSetDigitalChannelsByType(dvbcPreSetChannel, TVScanner.TYPE_CABLE);
				} else {
					TVCommonNative.TVLog(testTag + " fail: mTVScanner == null");
				}
			}
		};
		testThread3.start();
	}


	public void dumpChannels() {
		mTVManager = TVCommonManager.getInstance(mContext);
		channels = mTVManager.getChannels();
		mDevManager = DevManager.getInstance();
		mDevice = mDevManager.getMountList();
		if (mDevice != null && mDevice.size() > 0) {
			mpath = mDevice.get(0).mMountPoint;
		}
		File file = new File(mpath + "/channelInfo.txt");
		FileOutputStream fos = null;
		DataOutputStream dos = null;
		try {
			fos = new FileOutputStream(file);
			dos = new DataOutputStream(fos);
			if(null != channels){
			for (int i = 0; i < channels.size(); i++) {
				dos.writeUTF(channels.get(i).toString() + "\n");
				}	
			}
			dos.flush();
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				if(dos != null){
					dos.close();
					dos = null;
				}
				if(fos != null){
					fos.close();
					fos = null;
				}	
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		
		}
	}

}
