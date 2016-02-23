package com.mediatek.tvcommon;

import java.util.HashMap;
import java.util.Hashtable;
import java.util.List;
import java.util.Vector;

import android.content.Context;
import android.os.RemoteException;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;

import com.mediatek.tv.TVManager;
import com.mediatek.tv.common.ChannelCommon;
import com.mediatek.tv.model.DtmbScanRF;
import com.mediatek.tv.model.NetWorkVersionListener;
import com.mediatek.tv.service.BroadcastService;
import com.mediatek.tv.service.ChannelService;
import com.mediatek.tv.service.ComponentService;
import com.mediatek.tv.service.ScanService;


public class TVScanner {
	final public static int STATE_COMPLETED = 0;
	final public static int STATE_SCANNING = 1;

	public static final int TYPE_AIR   = 0;
	public static final int TYPE_CABLE = 1;

	private static TVScanner scanner;
	private ScanTask task;
	private Hashtable<String, TVOptionRange<Integer>> optionTable = new Hashtable<String, TVOptionRange<Integer>>();

	public static final String SCAN_OPTION_TV_SYSTEM = "TvAudioSystem";
	public static final String SCAN_OPTION_COLOR_SYSTEM = "ColorSystem";
	public static final String SCAN_OPTION_OPERATOR_NAME = "OperatorName";
	public static final String SCAN_OPTION_SCAN_EMOD = "ScanEMod";
	public static final String SCAN_OPTION_SYM_RATE = "SymRate";
	public static final String SCAN_OPTION_NETWOK_ID = "NetworkID";

	protected final Context mContext;
	protected ScanService scanService;
	protected ChannelService channelService;
	protected BroadcastService brdSrv;
	protected TVConfigurer cfg;
	protected ComponentService componentService;
	protected TVCommonManager tvcommonManager;

	/* my thread with handler, we could NOT do much in lower lay thread (like mw scan) */
	protected static HandlerThread mThread = null;

	/* my handler */
	protected static Handler mHandler = null;

	private Handler mMainLooperHandler;
	
	private DtmbScanRF curDtmbScanRF = null;

	public DtmbScanRF getCurDtmbScanRF() {
		return curDtmbScanRF;
	}

	private TVScanner(Context context) {
		mContext = context;
		mMainLooperHandler = new Handler(Looper.getMainLooper());
		cfg = TVConfigurer.getInstance(mContext);
		TVManager tvManager = TVManager.getInstance(mContext);
		scanService = (ScanService) tvManager.getService(ScanService.ScanServiceName);
		channelService = (ChannelService) tvManager.getService(ChannelService.ChannelServiceName);
		brdSrv = (BroadcastService) tvManager.getService(BroadcastService.BrdcstServiceName);
		componentService = (ComponentService) tvManager.getService(ComponentService.CompServiceName);
		tvcommonManager = TVCommonManager.getInstance(mContext);
		init();
		
		if (null == mThread) {
			TVCommonNative.TVLog("new \"TVScannerSelf\"");
			mThread = new HandlerThread("TVScannerSelf");
			mThread.start();
		}
		
		if (null == mHandler && null != mThread) {
			TVCommonNative.TVLog("new \"TVScannerSelf\" handler");
			mHandler = new Handler(mThread.getLooper());
		}

		if (null == mMainLooperHandler || null == cfg || null == tvManager || null == scanService || null == channelService || null == brdSrv ||
			null == componentService || null == tvcommonManager || null == mThread || null == mHandler) {
			throw new IllegalAccessError("Fatal Error in init TVScanner: mMainLooperHandler: " + mMainLooperHandler + ", cfg: " + cfg 
											+ ", tvManager: " + tvManager + ", scanService: " + scanService + ", channelService: " 
											+ channelService + ", brdSrv: " + brdSrv + ", componentService: " + componentService 
											+ ", tvcommonManager: " + tvcommonManager + ", mThread: " + mThread + ", mHandler: " + mHandler);
		}
	}

	private void init() {
		optionTable.put(SCAN_OPTION_TV_SYSTEM, new TvAudioSystemOption());
		optionTable.put(SCAN_OPTION_COLOR_SYSTEM, new ColorSystemOption());
		optionTable.put(SCAN_OPTION_OPERATOR_NAME, new OperatorNameOption());
		optionTable.put(SCAN_OPTION_SCAN_EMOD, new ScanEModOption());
		optionTable.put(SCAN_OPTION_SYM_RATE, new SymRateOption());
		optionTable.put(SCAN_OPTION_NETWOK_ID, new NetworkIDOption());
	}

	public final TVOption<?> getOption(String name) {
		return optionTable.get(name);
	}

	public interface ScannerListener {
		public static final int PROGRESS_MAX = 100;
		public static final int PROGRESS_MIN = 0;

		public static final int COMPLETE_OK = 0;
		public static final int COMPLETE_CANCEL = 1;
		public static final int COMPLETE_ERROR = 2;

		public void onProgress(int progress, int channels);

		public void onFrequence(int freq);

		public void onCompleted(int completeValue);
	}


	public interface NwVersionListener {
		/**
		 * Network version change notify
		 */
		public void onNwVersionChanged();
	}


	private Vector<NwVersionListener> nwVersionListeners = new Vector<NwVersionListener>();


	/**
	 * Add network version listener
	 * 
	 * @param listener
	 */
	public synchronized void addNwVersionListener(NwVersionListener listener) {
		nwVersionListeners.add(listener);
	}


	/**
	 * Remove network version listener
	 * 
	 * @param listener
	 */
	public synchronized void removeNwVersionListener(NwVersionListener listener) {
		nwVersionListeners.remove(listener);
	}


	protected class nwVersionListenerDelegater implements NetWorkVersionListener {
		private NwVersionListener usrListener = null;

		nwVersionListenerDelegater(NwVersionListener usrListener) {
			this.usrListener = usrListener;
		}

		public void setNwVersionChangeNfy(String scanMode) {
			mMainLooperHandler.post(new Runnable() {
				public void run() {
					if (null != usrListener) {
						TVCommonNative.TVLog("Start notify");
						usrListener.onNwVersionChanged();
					} else {
						TVCommonNative.TVLog("fail: null == usrListener");
					}
				}
			});
		}
	}


	/**
	 * Enable/disable monitor network version change in digital tv
	 * 
	 * @param enableMonitor
	 *            true, enable monitor false, disable monitor
	 * @param listener
	 */
	public void enableMonitorNwVersion(boolean enableMonitor, NwVersionListener listener) {
		if (null != scanService) {
			TVCommonNative.TVLog("Start enableMonitorNwVersion. enableMonitor = " + enableMonitor);
			scanService.isMonitorNwVersion(ScanService.SCAN_MODE_DVB_CABLE,
					enableMonitor, new nwVersionListenerDelegater(listener));
		} else {
			TVCommonNative.TVLog("fail: null == scanService");
		}
	}


	public static TVScanner getInstance(Context context) {
		if (scanner == null) {
			scanner = new TVScanner(context);
		}
		return scanner;
	}


	/**
	 * Scan all analog TV channels.
	 * 
	 * @param listener
	 *            Listener which display scan process.
	 */
	public synchronized void atvScan(ScannerListener listener) {
		if (getScanState() == STATE_SCANNING) {
			TVCommonNative.TVLog("Notice!!!  Can't start scan in scanning state ");
			return;
		}
		
		TVCommonNative.TVLog("ATV Full Scan");
		task = new PalScanner(this);
		task.scan(listener);
	}
	

	/**
	 * Scan analog TV channels range from frequency {@code freqStart} to
	 * {@code freqEnd}.
	 * 
	 * @param freqStart
	 *            The start frequency.
	 * @param freqEnd
	 *            The end frequency.
	 * @param listener
	 *            Listener which display scan process.
	 */
	public synchronized void atvRangeScan(int freqStart, int freqEnd, ScannerListener listener) {
		if (getScanState() == STATE_SCANNING) {
			TVCommonNative.TVLog("Notice!!!  Can't start scan in scanning state ");
			return;
		}

		if (freqStart < 0) {
			throw new IllegalArgumentException();
		}

		TVCommonNative.TVLog("ATV Range Scan");
		PalScanner scanner = new PalScanner(this);
		task = scanner;
		scanner.setFreqRange(freqStart, freqEnd);
		scanner.scan(listener);
	}
	

	/**
	 * Search the new analog TV channel which current channel list do not
	 * include.
	 * 
	 * @param listener
	 *            Listener which display scan process.
	 */
	public synchronized void atvUpdateScan(ScannerListener listener) {
		if (getScanState() == STATE_SCANNING) {
			TVCommonNative.TVLog("Notice!!!  Can't start scan in scanning state ");
			return;
		}

		TVCommonNative.TVLog("ATV Update Scan");
		PalScanner scanner = new PalScanner(this);
		task = scanner;
		scanner.setUpdate(true);
		scanner.scan(listener);
	}
	

	/**
	 * Scan all digital TV channels.
	 * 
	 * @param listener
	 *            Listener which display scan process.
	 */
	public synchronized void dtvScan(ScannerListener listener) {
		if (getScanState() == STATE_SCANNING) {
			TVCommonNative.TVLog("Notice!!!  Can't start scan in scanning state ");
			return;
		}
		
		TVCommonNative.TVLog("DTV Full Scan");
		task = new DTVScanner(this);
		task.scan(listener);
	}
	

	/**
	 * Scan DVB-C channel in specified frequency {@code freq}, this method must
	 * be invoked in {@link ITVCommon#TUNNER_CABLE} mode.
	 * 
	 * @param freq
	 *            The specified frequency.
	 */
	public synchronized void dvbcSingleRFScan(int freq, ScannerListener listener) throws IllegalAccessException {
		if (getScanState() == STATE_SCANNING) {
			TVCommonNative.TVLog("Notice!!!  Can't start scan in scanning state ");
			return;
		}

		DTVScanner scanner = new DTVScanner(this);
		task = scanner;
		if (task.getRawScanMode() != ScanService.SCAN_MODE_DVB_CABLE) {
			throw new IllegalAccessException("Must be invoked in dvb-cable mode ");
		}
		
		TVCommonNative.TVLog("DVB-C Single RF Scan");
		scanner.setFreq(freq);
		scanner.scan(listener);
	}


	/**
	 * Scan DVB-C channel with specified {@code scanEMod}, {@code sysRate} and
	 * {@code freq}.
	 * 
	 * @param scanEMod
	 *            The specified scan mode.
	 * @param sysRate
	 *            The specified system rate.
	 * @param freq
	 *            The specified system frequency.
	 * @param listener
	 *            Listener which display scan process.
	 */
	public synchronized void dvbcSingleRFScan(int scanEMod, int sysRate, int freq, ScannerListener listener) throws IllegalAccessException {
		if (getScanState() == STATE_SCANNING) {
			TVCommonNative.TVLog("Notice!!!  Can't start scan in scanning state ");
			return;
		}

		TVOptionRange<Integer> option = optionTable.get(SCAN_OPTION_SCAN_EMOD);
		if (option != null) {
			ScanEModOption eModOpt = (ScanEModOption) option;
			eModOpt.set(scanEMod);
		}
		
		option = optionTable.get(TVScanner.SCAN_OPTION_SYM_RATE);
		if (option != null) {
			SymRateOption symRateOpt = (SymRateOption) option;
			symRateOpt.set(sysRate);
		}

		DTVScanner scanner = new DTVScanner(this);
		task = scanner;
		if (task.getRawScanMode() != ScanService.SCAN_MODE_DVB_CABLE) {
			throw new IllegalAccessException("Must be invoked in dvb-cable mode.");
		}
		
		TVCommonNative.TVLog("DVB-C Single RF Scan");
		scanner.setFreq(freq);
		scanner.scan(listener);
	}


	private HashMap<String, DtmbScanRF> rfMap = new HashMap<String, DtmbScanRF>();


	/**
	 * Scan DTMB TV channel in specified RF Channel.
	 * 
	 * @param rfChannel
	 *            The specified channel.
	 * @param listener
	 *            Listener which display scan process.
	 */
	public synchronized void dtmbSingleRFScan(String rfChannel, ScannerListener listener) throws IllegalAccessException {
		if (getScanState() == STATE_SCANNING) {
			TVCommonNative.TVLog("Notice!!!  Can't start scan in scanning state ");
			return;
		}

		if (null == rfChannel) {
			TVCommonNative.TVLog("return reason: null == rfChannel");
			return;
		}
		
		DtmbScanRF rf = rfMap.get(rfChannel);
		if (null == rf) {
			TVCommonNative.TVLog("return reason: rf == null ");
			return;
		}
		
		DTVScanner scanner = new DTVScanner(this);
		task = scanner;
		if (task.getRawScanMode() != ScanService.SCAN_MODE_DTMB_AIR) {
			throw new IllegalAccessException("Must be invoked in dtmb-air mode.");
		}

		TVCommonNative.TVLog("dtmb Single RF Scan");
		scanner.setFreq(rf.getRFScanFrequency());
		scanner.setIndex(rf.getRFScanIndex());
		scanner.scan(listener);
		return;
	}
	

	/**
	 * @return The current RF channel.
	 */
	public String getCurrentDtmbRFChannel() {
		ITVCommon tvCommon = TVCommonNative.getDefault(mContext);
		DtmbScanRF rf = null;
		TVChannel ch = null;
		String rfChannel = null;
		try {
			if (tvCommon != null)
				ch = tvCommon.getCurrentChannel();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		if (ch != null) {
			rf = scanService.getCurrentDtmbScanRF(
					ScanService.SCAN_MODE_DTMB_AIR, ch.rawInfo.getChannelId());
			if (rf != null) {
				curDtmbScanRF = rf;
				rfChannel = rf.getRFChannel();
				rfMap.put(rfChannel, rf);
			}
		}

		if (rf == null) {
			rfChannel = getFirstDtmbRFChannel();
		}

		return rfChannel;
	}

	/**
	 * @return the first RF channel.
	 */
	public synchronized String getFirstDtmbRFChannel() {
		DtmbScanRF rf = scanService
				.getFirstDtmbScanRF(ScanService.SCAN_MODE_DTMB_AIR);
		String rfChannel = null;
		if (rf != null) {
			rfChannel = rf.getRFChannel();
			rfMap.put(rfChannel, rf);
			curDtmbScanRF = rf;
		}
		return rfChannel;
	}

	/**
	 * @return the last RF channel.
	 */
	public synchronized String getLastDtmbRFChannel() {
		DtmbScanRF rf = scanService
				.getLastDtmbScanRF(ScanService.SCAN_MODE_DTMB_AIR);
		String rfChannel = null;
		if (rf != null) {
			rfChannel = rf.getRFChannel();
			rfMap.put(rfChannel, rf);
			curDtmbScanRF = rf;
		}
		return rfChannel;
	}

	/**
	 * @return the next RF channel.
	 */
	public synchronized String getNextDtmbRFChannel() {
		DtmbScanRF rf = scanService.getNextDtmbScanRF(
				ScanService.SCAN_MODE_DTMB_AIR, curDtmbScanRF);
		String rfChannel = null;
		if (rf != null) {
			rfChannel = rf.getRFChannel();
			rfMap.put(rfChannel, rf);
			curDtmbScanRF = rf;
		} else {
			rfChannel = getFirstDtmbRFChannel();
		}
		return rfChannel;
	}

	/**
	 * @return the previous RF channel.
	 */
	public synchronized String getPrevDtmbRFChannel() {
		DtmbScanRF rf = scanService.getPrevDtmbScanRF(
				ScanService.SCAN_MODE_DTMB_AIR, curDtmbScanRF);
		String rfChannel = null;
		if (rf != null) {
			rfChannel = rf.getRFChannel();
			rfMap.put(rfChannel, rf);
			curDtmbScanRF = rf;
		} else {
			rfChannel = getFirstDtmbRFChannel();
		}
		return rfChannel;
	}

	public int getDvbcRadioNum() {
		return scanService.getDvbcProgramTypeNumber(
				ScanService.SCAN_MODE_DVB_CABLE).getRadioNumber();
	}

	public int getDvbcTvNum() {
		return scanService.getDvbcProgramTypeNumber(
				ScanService.SCAN_MODE_DVB_CABLE).getTvNumber();
	}

	public int getDvbcAppNum() {
		return scanService.getDvbcProgramTypeNumber(
				ScanService.SCAN_MODE_DVB_CABLE).getAppNumber();
	}

	public int getDvbcLowerFreq() {
		int lowerFreq = 0;
		if (null != scanService) {
			lowerFreq = scanService.getDvbcFreqRange(
					ScanService.SCAN_MODE_DVB_CABLE).getLowerTunerFreqBound();
		}
		return lowerFreq;
	}

	public int getDvbcUpperFreq() {
		int upperFreq = 0;
		if (null != scanService) {
			upperFreq = scanService.getDvbcFreqRange(
					ScanService.SCAN_MODE_DVB_CABLE).getUpperTunerFreqBound();
		}
		return upperFreq;
	}

	
	public int getDvbcMainFrequence(){
		if (null != scanService) {
			int mainFreq = scanService.getDvbcMainFrequence(
								ScanService.SCAN_MODE_DVB_CABLE).getMainFrequence();
			TVCommonNative.TVLog("mainFreq = " + mainFreq);
			return mainFreq;
		} else {
			TVCommonNative.TVLog("scanService == null");
		}
		return 0;
	}

	
	public int getDvbcTsCount(){
		if (null != scanService) {
			int tsCount = scanService.getDvbcMainFrequence(
								ScanService.SCAN_MODE_DVB_CABLE).getTsCount();
			TVCommonNative.TVLog("tsCount = " + tsCount);
			return tsCount;
		} else {
			TVCommonNative.TVLog("scanService == null");
		}
		return 0;
	}

	
	public int getDvbcNitVersion(){
		if (null != scanService) {
			int nitVer = scanService.getDvbcMainFrequence(
								ScanService.SCAN_MODE_DVB_CABLE).getNitVersion();
			TVCommonNative.TVLog("nitVer = " + nitVer);
			return nitVer;
		} else {
			TVCommonNative.TVLog("scanService == null");
		}
		return 0;
	}


	public int getDtmbLowerFreq() {
		return scanService.getDtmbFreqRange(ScanService.SCAN_MODE_DTMB_AIR)
				.getLowerTunerFreqBound();
	}

	public int getDtmbUpperFreq() {
		return scanService.getDtmbFreqRange(ScanService.SCAN_MODE_DTMB_AIR)
				.getUpperTunerFreqBound();
	}


	/**
	 * Preset ATV channels.
	 */
	public synchronized void preSetAnalogChannels(List<PalPreSetChannel> channels) {
		PalScanner scanner = new PalScanner(this);
		if (null != scanner) {
			TVCommonNative.TVLog("analog preset starting ");
			task = scanner;
			scanner.preSetAnalogChannels(channels);
		} else {
			TVCommonNative.TVLog("null == scanner");
		}
	}


	/**
	 * Preset DTV channels.
	 */
	public synchronized void preSetDigitalChannels(List<DvbPreSetChannel> channels) {
		DTVScanner scanner = new DTVScanner(this);
		if (null != scanner) {
			TVCommonNative.TVLog("digital preset starting ");
			task = scanner;
			scanner.preSetDigitalChannels(channels);
		} else {
			TVCommonNative.TVLog("null == scanner");
		}
	}


	/**
	 * Preset DTV channels by type, allow to preset dtmb, cable ch at the same time
	 * type: TVScanner.TYPE_AIR, TVScanner.TYPE_CABLE
	 */
	public synchronized void preSetDigitalChannelsByType(List<DvbPreSetChannel> channels, int type) {
		DTVScanner scanner = new DTVScanner(this);
		if (null != scanner) {
			TVCommonNative.TVLog("digital preset starting. type = " + type);
			task = scanner;
			scanner.preSetDigChsByType(channels, type);
		} else {
			TVCommonNative.TVLog("null == scanner, type = " + type);
		}
	}


	/**
	 * Cancel the current scan progress.
	 */
	public synchronized void cancelScan() {
		if (task != null) {
			task.cancel();
		}
	}


	/**
	 * @return the current scan state.
	 */
	public int getScanState() {
		if (null != task) {
			return task.getState();
		}
		return STATE_COMPLETED;
	}
	
}

