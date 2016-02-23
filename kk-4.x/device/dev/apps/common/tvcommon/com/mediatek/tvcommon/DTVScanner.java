package com.mediatek.tvcommon;

import java.util.ArrayList;
import java.util.List;

import com.mediatek.tv.common.ChannelCommon;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tv.common.TVCommon;
import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.model.ChannelInfo;
import com.mediatek.tv.model.DvbChannelInfo;
import com.mediatek.tv.model.ScanParaDtmb;
import com.mediatek.tv.model.ScanParaDvbc;
import com.mediatek.tv.model.ScanParams;
import com.mediatek.tv.service.ChannelService;
import com.mediatek.tv.service.ScanService;
import com.mediatek.tvcommon.TVScanner.ScannerListener;

class DTVScanner extends ScanTask {
	private int freq = -1;
	private int index;
	private TVScanner mScanner;

	DTVScanner(TVScanner tvScanner) {
		if (null == tvScanner) {
			throw new IllegalAccessError("Fatal Error in init DTVScanner: tvScanner: " + tvScanner);
		}
		mScanner = tvScanner;
		mContext = mScanner.mContext;
		cfg = mScanner.cfg;
		scanService = mScanner.scanService;
		channelService = mScanner.channelService;
		brdSrv = mScanner.brdSrv;
		componentService = mScanner.componentService;
		tvCommonManager = mScanner.tvcommonManager;
		mThread = mScanner.mThread;
		mHandler = mScanner.mHandler;
	}
	

	void setFreq(int freq) {
		this.freq = freq;
	}

	void setIndex(int index) {
		this.index = index;
	}

	@Override
	public boolean scan(ScannerListener listener) {
		prepareScan();
		type = ScanTask.TYPE_DTV;
		ScanParams para = null;
		@SuppressWarnings("unchecked")
		TVOption<Integer> option = (TVOption<Integer>) cfg.getOption(IntegerOption.CFG_BS_SRC);
		if (option != null) {
			int mode = option.get();
			switch (mode) {
			case ConfigType.BS_SRC_AIR:
				/* dtmb */
				if (freq > 0) {
					para = new ScanParaDtmb("CHN", ScanParaDtmb.DTMB_SCAN_TYPE_SINGLE_RF_CHANNEL, 0, index, index, freq);
				} else {
					para = new ScanParaDtmb("CHN", ScanParaDtmb.DTMB_SCAN_TYPE_FULL_MODE, 0, 0, 0, 0);
				}
				break;

			case ConfigType.BS_SRC_CABLE:
				/* dvbc */
				OperatorNameOption operatorOpt = (OperatorNameOption) mScanner.getOption(TVScanner.SCAN_OPTION_OPERATOR_NAME);
				if (null != operatorOpt) {
					TVCommonNative.TVLog("freq: " + freq + " operatorName: " + operatorOpt.getRawOperatorName());
				} else {
					TVCommonNative.TVLog("freq: " + freq + " operatorOpt == null");
				}
				
				if (freq > 0) {
					ScanEModOption eModOpt = (ScanEModOption) mScanner.getOption(TVScanner.SCAN_OPTION_SCAN_EMOD);
					SymRateOption symRateOpt = (SymRateOption) mScanner.getOption(TVScanner.SCAN_OPTION_SYM_RATE);
					NetworkIDOption networkIDOpt = (NetworkIDOption) mScanner.getOption(TVScanner.SCAN_OPTION_NETWOK_ID);
					int validMask = 0;
					if (eModOpt != null && networkIDOpt != null && symRateOpt != null && operatorOpt != null) {
						validMask |= ScanParaDvbc.SB_DVBC_SCAN_INFO_START_FREQ_VALID 
									 | ScanParaDvbc.SB_DVBC_SCAN_INFO_END_FREQ_VALID
									 | eModOpt.getMask()
									 | symRateOpt.getMask()
									 | networkIDOpt.getMask();

						/* singleRF scan */
						para = new ScanParaDvbc("CHN", 
												operatorOpt.getRawOperatorName(),				// operatorName
												ScanParaDvbc.SB_DVBC_NIT_SEARCH_MODE_OFF,		// searchMode
												ScanParaDvbc.SB_DVBC_SCAN_TYPE_MANUAL_FREQ,		// scanType
												0,												// scanCfg
												validMask, 										// validMask
												eModOpt.get(), 									// eMod
												symRateOpt.get(),								// symRate
												freq, 											// startFreq
												freq, 											// endFreq
												networkIDOpt.get());							// netWorkID
					}

				} else {
					/* full scan */
					para = new ScanParaDvbc("CHN", 
											operatorOpt.getRawOperatorName(),			// operatorName
											ScanParaDvbc.SB_DVBC_NIT_SEARCH_MODE_OFF,	// searchMode
											ScanParaDvbc.SB_DVBC_SCAN_TYPE_FULL_MODE,	// scanType
											0,											// scanCfg
											ScanParaDvbc.SB_DVBC_SCAN_FREQ_RANGE_START,	// startFreq
											ScanParaDvbc.SB_DVBC_SCAN_FREQ_RANGE_END);	// endFrq
				}
				break;

			default:
				TVCommonNative.TVLog("unsupported sort type: " + mode);
				break;
			}
		}

		if (freq > 0) {
			TVCommonNative.TVLog("Don't setChannelInfo to protect svl mask info");
			/* The reason why don't do it is: 
			 * Ch(s) in below "dels" list could NOT be deleted actually.
			 * In channel_service_wrapper.c, the delete_by_user bit of each one is set when setChannelList(DELETE) instead of delete 
			 * channels in svl, which leads to the dirty of delete_by_user bit.
			 */
			/*
			try {
				List<ChannelInfo> dels = new ArrayList<ChannelInfo>();
				List<ChannelInfo> mChannelInfoList = channelService
						.getChannelList(getNativeDBName());
				if (mChannelInfoList != null && mChannelInfoList.size() > 0) {
					for (ChannelInfo info : mChannelInfoList) {
						if (info instanceof DvbChannelInfo
								&& ((DvbChannelInfo) info).getFrequency() == freq) {
							dels.add(info);
						}
					}
					channelService.setChannelList(
							ChannelService.ChannelOperator.DELETE,
							getNativeDBName(), dels);
				}
			} catch (TVMException e) {
				e.printStackTrace();
			}
			*/
		} else {
			try {
				channelService.digitalDBClean(getNativeDBName());
			} catch (TVMException e) {
				e.printStackTrace();
			}
		}
			
		String rawScanMode = getRawScanMode();
		TVCommonNative.TVLog("rawScanMode: " + rawScanMode);
		int ret = scanService.startScan(rawScanMode, para, new ScanListenerDelegater(listener));
		return 0 == ret;
	}
	

	@Override
	String getRawScanMode() {
		@SuppressWarnings("unchecked")
		TVOption<Integer> option = (TVOption<Integer>) cfg
				.getOption(IntegerOption.CFG_BS_SRC);
		if (option != null) {
			return option.get() == ConfigType.BS_SRC_AIR ? ScanService.SCAN_MODE_DTMB_AIR
					: ScanService.SCAN_MODE_DVB_CABLE;
		}
		return ScanService.SCAN_MODE_DVB_CABLE;
	}

	void preSetDigitalChannels(List<DvbPreSetChannel> channels) {
		byte brdcstMedium;
		String dbName;
		if (getRawScanMode() == ScanService.SCAN_MODE_DTMB_AIR) {
			brdcstMedium = TVCommon.BRDCST_MEDIUM_DIG_TERRESTRIAL;
			dbName = ChannelCommon.DB_AIR;
		} else {
			brdcstMedium = TVCommon.BRDCST_MEDIUM_DIG_CABLE;
			dbName = ChannelCommon.DB_CABEL;
		}

		preSetChs(channels, brdcstMedium, dbName);
	}

	
	void preSetDigChsByType(List<DvbPreSetChannel> channels, int type) {
		byte brdcstMedium;
		String dbName;
		if (TVScanner.TYPE_AIR == type) {
			brdcstMedium = TVCommon.BRDCST_MEDIUM_DIG_TERRESTRIAL;
			dbName = ChannelCommon.DB_AIR;
		} else {
			brdcstMedium = TVCommon.BRDCST_MEDIUM_DIG_CABLE;
			dbName = ChannelCommon.DB_CABEL;
		}

		preSetChs(channels, brdcstMedium, dbName);
	}


	private void preSetChs(List<DvbPreSetChannel> channels, byte brdcstMedium, String dbName) {
		TVCommonNative.TVLog("rawScanMode: " + getRawScanMode() + ", medium: " + brdcstMedium + ", dbName: " + dbName);
		List<ChannelInfo> raws = new ArrayList<ChannelInfo>();
		for (DvbPreSetChannel ch : channels) {
			DvbChannelInfo chInfo = new DvbChannelInfo(dbName);
			chInfo.setBrdcstMedium(brdcstMedium);
			chInfo.setChannelNumber(ch.getChannelNum());
			chInfo.setServiceName(ch.getChannelName());
			chInfo.setFrequency(ch.getFrequency());
			chInfo.setBandWidth(ch.getBindWidth());
			chInfo.setNwId(ch.getNetworkId());
			chInfo.setOnId(ch.getOnId());
			chInfo.setTsId(ch.getTsId());
			chInfo.setProgId(ch.getProgId());
			chInfo.setSymRate(ch.getSymRate());
			chInfo.setMod(ch.getMod());
			raws.add(chInfo);
		}

		prepareScan();
		type = ScanTask.TYPE_DTV;
		try {
			channelService.digitalDBClean(dbName);
			channelService.setChannelList(ChannelService.ChannelOperator.APPEND, dbName, raws);
		} catch (TVMException e) {
			e.printStackTrace();
		}

		endPreSet();
	}

}
