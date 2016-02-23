package com.mediatek.tvcommon;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.sql.Date;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Timer;
import java.util.TimerTask;
import java.util.Vector;
import com.mediatek.tv.TVManager;
import com.mediatek.tv.common.ChannelCommon;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tv.common.TVCommon;
import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.model.AnalogChannelInfo;
import com.mediatek.tv.model.ChannelInfo;
import com.mediatek.tv.model.DvbChannelInfo;
import com.mediatek.tv.service.BroadcastService;
import com.mediatek.tv.service.ChannelService;
import com.mediatek.tv.service.ComponentService;
import com.mediatek.tv.service.CecComponentService;
import com.mediatek.tv.service.ConfigService;
import com.mediatek.tv.service.IChannelNotify;
import com.mediatek.tv.service.InputService;
import com.mediatek.tv.service.ChannelService.ChannelOperator;
import com.mediatek.tv.service.ScanService;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.MessageQueue;
import android.os.RemoteException;
import android.os.SystemClock;


class TVCommonService extends TVCommonNative implements Handler.Callback,
		MessageQueue.IdleHandler {

	Looper mLooper;
	MessageQueue mQueue;
	Handler handler;

	private volatile HashMap<String, ChannelQueue> channelQueueMap = new HashMap<String, ChannelQueue>();
	private TVManager tvMgr;
	private InputService inputSrv;
	private ChannelService rawChSrv;
	private BroadcastService brdcstSrv;
	private TVStorage storage;
	private TVConfigurer configurer;
	private ComponentService compSrv;
	private CecComponentService cecCompSrv;
	private ScanService scanService;
	private TVMethods methods;

	private static final String ANALOG_TV = "atv";
	private static final String DIGITAL_TV = "dtv";

	// see brdcst.c TV_WIN_ID_T
	public static final int TV_WIN_ID_MAIN = 0;
	public static final int TV_WIN_ID_SUB  = 1;
	// special item which is not real output
	public static final int TV_WIN_ID_NONE = 254;	// None
	public static final int TV_WIN_ID_ALL  = 255;	// All TV_WIN

	private HashMap<String, ChannelQueue> channelSet = new HashMap<String, ChannelQueue>();

	final private List<TVInput> inputs = new ArrayList<TVInput>();
	final private List<TVOutput> outputs = new ArrayList<TVOutput>();

	private static final String START_CHANNEL_PREFIX = "start_channel_";
	private static final String DEFAULT_OUTPUT_PREFIX = "default_output";

	Comparator<TVChannel> mComparator = new Comparator<TVChannel>() {

		@Override
		public int compare(TVChannel lhs, TVChannel rhs) {
			if (lhs.getChannelNum() < rhs.getChannelNum()) {
				return -1;
			} else if (lhs.getChannelNum() > rhs.getChannelNum()) {
				return 1;
			} else {
				return 0;
			}
		}
	};

	TVCommonService() {

		super();
		tvMgr = TVManager.getInstance(getContext());
		rawChSrv = (ChannelService) tvMgr.getService(ChannelService.ChannelServiceName);
		inputSrv = (InputService) tvMgr.getService(InputService.InputServiceName);
		brdcstSrv = (BroadcastService) tvMgr.getService(BroadcastService.BrdcstServiceName);
		compSrv = (ComponentService) tvMgr.getService(ComponentService.CompServiceName);
		cecCompSrv	= (CecComponentService) compSrv.getSpecificComponet(CecComponentService .CecComponentServiceName);
		scanService = (ScanService) tvMgr.getService(ScanService.ScanServiceName);

		if (tvMgr == null || rawChSrv == null || inputSrv == null || brdcstSrv == null || compSrv == null) {
			throw new IllegalAccessError("Fatal Error happens when initiliaze TVM");
		}

		mLooper = Looper.getMainLooper();
		mQueue = mLooper.myQueue();

		mQueue.addIdleHandler(this);
		handler = new Handler(mLooper, this);
		storage = TVStorage.getInstance(getContext());
		configurer = TVConfigurer.getInstance(getContext());
        methods = TVMethods.getInstance(getContext()); 

		init();

		IntentFilter shutdownFilter = new IntentFilter();
		shutdownFilter.addAction(Intent.ACTION_SHUTDOWN);
		shutdownFilter.setPriority(IntentFilter.SYSTEM_LOW_PRIORITY);
		getContext().registerReceiver(new BroadcastReceiver() {
			public void onReceive(Context context, Intent intent) {
				uninit();
			}
		}, shutdownFilter);

		IntentFilter scanFilter = new IntentFilter(SCAN_COMPLETE_ACTION);
		getContext().registerReceiver(new BroadcastReceiver() {
			public void onReceive(Context context, Intent intent) {
				int type = intent.getExtras().getInt("scan_type");
				TVLog("Scan completed:" + (type == 0 ? ANALOG_TV : DIGITAL_TV));

				// Scan completed, remap ChannelInfo to TVChannel objects.
				update(type);
				notifyUpdate(UpdateReason.SCAN_COMPLETE, "");
			}
		}, scanFilter);

		if (null != rawChSrv) {
			rawChSrv.addListener(new IChannelNotify() {
				public void notifyChannelUpdated(int condition, int reason, int data) {

					/**	details in u_svl.h:
					 	I)conditon: 1: SVL_UPDATING: unused actually, when svl_lock is called
									2: SVL_UPDATED:  used actually, when svl_unlock is called
									3: SVL_CLOSED:	 unused actually, when svl_delete is called, means svl handle is gone.
									4: SVL_UNKNOWN
						II) reason: 0:			SVL_REASON_UNKNOWN	 
									2(1 << 1):	SVL_RECORD_ADD: Reason for adding records 
									4(1 << 2):	SVL_RECORD_DEL: Reason for deleting records
									8(1 << 3):	SVL_RECORD_MOD: Reason for modifying records
							ADD, DEL, MOD reason may happens in one notify, so they could '|' with each other. For example:
							reason = 10 means 2 + 8: ADD and MOD at the same time
					*/
					if (condition == 2 && (reason & (1 << 1 | 1 << 2 | 1 << 3)) != 0) {
						TVLog("Got notification: channels update. cond: " + condition + ", reason: " + reason);
						
						/* TO_DO: below modification need be tested more and then submit.
								  It cause fatal error.
								  The original objective is : could refresh ch info from dtv monitor notify
						
						new Thread(new Runnable() {
							public void run() {
								TVLog("Due to channelSrv notify: ---> update() ");
								update();
							}
						}).start();
						notifyUpdate(UpdateReason.ICH_NOTIFY, "");
						*/
					}
				}
			});
		}
	}
	

	private Object mSync = new Object();
	TVOutput mCurrentOutput;
	static String  mCurrentInputName;
	static String  mChangingToInputName;
	static boolean mInputIsChanged = false;

	private void init() {
		// Initialise TV input source and output source.

		for (String type : inputTypes) {
			String[] hwName = inputSrv.getDesignatedTypeInputsString(type);
			if (hwName == null) {
				TVLog("Input source type:" + type + " have no physical input source.");
				continue;
			}
			if (type.equals(INPUT_TYPE_TV)) {
				// Seperate a physical tv input source to atv and dtv
				inputs.add(new TVInput(type, ANALOG_TV, hwName[0]));
				inputs.add(new TVInput(type, DIGITAL_TV, hwName[0]));
				TVLog("Seperate input source:" + hwName[0] + " to atv and dtv");
			} else {
				for (String name : hwName) {
					inputs.add(new TVInput(type, name));
					TVLog("Initilize input source:" + name);
				}
			}
		}

		for (String outputName : inputSrv.getScreenOutputs()) {
			outputs.add(new TVOutput(outputName));
			TVLog("Initilize output:" + outputName);
		}
		autoCorrectInputsConfliction();

		TVOutput main = getTVOutput("main");
		TVOutput sub = getTVOutput("sub");

		if (storage.getString(DEFAULT_OUTPUT_PREFIX, "main").equals("sub")) {
			TVInput temp = main.getInput();
			main.connect(sub.getInput());
			sub.connect(temp);
		}

		setDefaultOutput(main);

		if (CrossRoad.isOnlyCableAtv) {
			TVInput tmp = main.getInput();
			if (null != tmp) {
				adjustTunnerMode(tmp.getName());
			}
		}
		
		// disable bluemute update in acfg post init. Update it here.
		TVOption<Integer> blueOpt = (TVOption<Integer>) configurer.getOption(ConfigType.CFG_BLUE_SCREEN);
		if (null != blueOpt && blueOpt.get() == ConfigType.COMMON_ON) {
			TVLog("trigger blue mute setting");
			blueOpt.set(ConfigType.COMMON_ON);
		}

		update();
	}

	private void autoCorrectInputsConfliction() {
		TVOutput main = getTVOutput("main");
		TVOutput sub = getTVOutput("sub");
		if (main != null && sub != null && main.getInput().conflictWith(sub.getInput())) {
			List<String> confcs = getConflictInputs(main.getInput().getName());
			for (TVInput input : inputs) {
				if (!confcs.contains(input.getName())) {
					sub.connect(input);
					break;
				}
			}
		}
	}

	private void uninit() {
		TVLog("Shutting down TV .... ");
		ConfigService cfgSrv = (ConfigService) tvMgr
				.getService(ConfigService.ConfigServiceName);
		try {
			// ???
			if (cfgSrv != null)
				cfgSrv.powerOff();
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}

	/**
	 * Each input source map to a {@code TVInput} object.
	 */
	final class TVInput {

		private final int cfgGrpForType[] = { TVConfigurer.CFG_GRP_ATV,
				TVConfigurer.CFG_GRP_AV, TVConfigurer.CFG_GRP_AV,
				TVConfigurer.CFG_GRP_COMPONENT, TVConfigurer.CFG_GRP_AV,
				TVConfigurer.CFG_GRP_HDMI, TVConfigurer.CFG_GRP_VGA };

		private String type;
		private String name;
		private String hwName;
		private int cfgIndex;
		boolean blocked;
		private boolean tempUnblocked = false;

		private static final String INPUT_BLOCK_PREFIX = "inp_block_";

		TVInput(String type, String name, String hwName) {
			this.type = type;
			this.name = name;
			this.hwName = hwName;
			cfgIndex = getCfgGrpIdxForType(type);
			blocked = storage.getBoolean(INPUT_BLOCK_PREFIX + name, false);
		}

		TVInput(String type, String name) {
			this(type, name, name);
		}

		boolean isBlocked() {
			if (CrossRoad.isShowBlockDetails) {
				TVLog(name + ": blocked: " + blocked + ", tempUnblocked: " + tempUnblocked);
			}
			return blocked && !tempUnblocked;
		}

		boolean isPhysicalBlocked() {
			return blocked;
		}

		void tempUnblocked() {
			TVLog(name + ": blocked: " + blocked + ", tempUnblocked: " + tempUnblocked + " --> true");
			tempUnblocked = true;
		}

		boolean isTempUnblocked() {
			return tempUnblocked;
		}

		void setBlocked(boolean blocked) {
			TVLog(name + ": blocked: " + this.blocked + " --> " + blocked + ", tempUnblocked: " + tempUnblocked + " --> false");
			this.blocked = blocked;	

			/* when set or reset physicalBlocked, tempUnblocked will be clean
			 * because this is the latest user operation
			 */
			tempUnblocked = false;

			storage.setBoolean(INPUT_BLOCK_PREFIX + name, blocked);
		}

		String getType() {
			return type;
		}

		String getName() {
			return name;
		}

		String getHWName() {
			return hwName;
		}

		boolean conflictWith(TVInput other) {
			if (other == null) {
				return false;
			}
			for (String type : inputSrv.getConflictType(this.type)) {
				if (other.getType() == type) {
					return true;
				}
			}

			return false;
		}

		boolean conflictWith(String inputName) {
			return conflictWith(getTVInput(inputName));
		}

		int getCfgGrpIdxForType(String type) {
			int cfgIndex = 0;
			if (type == null) {
				throw new IllegalArgumentException();
			}
			for (int i = 0; i < inputTypes.length; i++) {
				if (type.equals(inputTypes[i])) {
					if (type.equals(INPUT_TYPE_TV)) {
						cfgIndex = name.equals(ANALOG_TV) ? TVConfigurer.CFG_GRP_ATV
								: TVConfigurer.CFG_GRP_DTV;
					} else {
						cfgIndex = cfgGrpForType[i];
					}
					return cfgIndex;
				}
			}
			return 0;
		}
	}

	/**
	 * Each screen output map to a {@code TVOutput} object.
	 */
	class TVOutput {
		private static final String START_INP_PREFIX = "start_input_";

		String name;
		TVInput input;

		TVOutput(String name) {

			String defName = null;
			TVOutput other = getTVOutput(name.equals("main") ? "sub" : "main");
			if (null != other) {
				TVInput otherInput = other.getInput();
				if (null != otherInput) {
					List<String> confcs = getConflictInputs(otherInput.getName());
					for (TVInput input : inputs) {
						if (!confcs.contains(input.getName())) {
							defName = input.getName();
							break;
						}
					}
				} else {
					TVLog("otherInput == null"); 
				}
			} else {
				TVLog("other == null");	
			}

			this.name = name;
			String inpName = storage.getString(START_INP_PREFIX + name,
					defName != null ? defName : ANALOG_TV);
			input = TVCommonService.this.getTVInput(inpName);
		}

		void connect(TVInput input) {
			this.input = input;
			storage.setString(START_INP_PREFIX + name, input.getName());
		}

		String getName() {
			return name;
		}

		TVInput getInput() {
			return input;
		}
	}

	
    private	static List<ChannelInfo> cableRawList = new ArrayList<ChannelInfo>();	// cable raw list
    private	static List<ChannelInfo> airRawList   = new ArrayList<ChannelInfo>();   // air raw list

	// rebuild real raw list
	synchronized private void freshRawList(String type) {
		try {
			if (type.equals("cable")) {
				// fresh cable
				cableRawList.clear();
				cableRawList = rawChSrv.getChannelList(ChannelCommon.DB_CABEL);
			} else if (type.equals("air")) {
				// fresh air
				airRawList.clear();
				airRawList = rawChSrv.getChannelList(ChannelCommon.DB_AIR);
			} else {
				// (others) fresh both
				cableRawList.clear();
				cableRawList = rawChSrv.getChannelList(ChannelCommon.DB_CABEL);
				airRawList.clear();
				airRawList = rawChSrv.getChannelList(ChannelCommon.DB_AIR);
			}
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}


	// update status for compService
	private void updateCompStatus (String status) {
		int ret = -100;
		if (OUTPUT_MODE_NORMAL == mOutputMode) {
			try {
				ret = compSrv.updateSysStatus(status);
				TVLog(status + " ret: " + ret);
			} catch (TVMException e) {
				e.printStackTrace();
			}
		} else {
			TVLog("don't update " + status + " because compSrv don't work in: " 
					+ (OUTPUT_MODE_PIP == mOutputMode ? "PIP(" : (OUTPUT_MODE_POP == mOutputMode ? "POP(" : 
						(OUTPUT_MODE_MMP == mOutputMode ? "MMP(" : (OUTPUT_MODE_APP_PIP == mOutputMode ? "APP_PIP(" : "???("))))
					+ mOutputMode + ") mode");
		}
	}
	

	synchronized void update() {
		long startTime = SystemClock.elapsedRealtime();

		if (CrossRoad.isOnlyCableAtv) {   
			/*************************
			* just only cable atv.   *
            *************************/

			freshRawList("both");
			
			List<TVChannel> analogs  = new ArrayList<TVChannel>();
			List<TVChannel> airChs	 = new ArrayList<TVChannel>();
			List<TVChannel> cableChs = new ArrayList<TVChannel>();
			
			TVChannel ch = null;
			if(null != cableRawList) {
				for (ChannelInfo info : cableRawList) {
					ch = new TVChannel(info);
					if (info instanceof AnalogChannelInfo) {
						TVLog("add analog ch " + ch.getChannelNum());
						analogs.add(ch);
					} else if (info instanceof DvbChannelInfo) {
						if (info.isUserDelete()) {
							TVLog("ignore cable digital ch " + ch.getChannelNum() + " [" + ch.getChannelName() + "]");
						} else {
							TVLog("add cable digital ch " + ch.getChannelNum() + " [" + ch.getChannelName() + "]");
							cableChs.add(ch);
						}
					}
				}
			} else {
				TVLog("null == cableRawList");
			}
			
			ch = null;
			if(null != airRawList) {
				for (ChannelInfo info : airRawList) {
					ch = new TVChannel(info);
					if (info instanceof DvbChannelInfo) {
						if (info.isUserDelete()) {
							TVLog("ignore air digital ch " + ch.getChannelNum() + " [" + ch.getChannelName() + "]");
						} else {
							TVLog("add air digital ch " + ch.getChannelNum() + " [" + ch.getChannelName() + "]");
							airChs.add(ch);
						}
					}
				}
			} else {
				TVLog("null == airRawList");
			}
			
			Collections.sort(analogs,  mComparator);
			Collections.sort(cableChs, mComparator);
			Collections.sort(airChs,   mComparator);
			
			TVLog("ATV got " + analogs.size() + " channels");
			TVLog("Cable DTV got " + cableChs.size() + " channels");
			TVLog("Air DTV got " + airChs.size()   + " channels");

			synchronized (channelQueueMap) {
				// atv
				ChannelQueue queue0 = new ChannelQueue(analogs);
				if (null != queue0) {
					int anaNum = storage.getInt(START_CHANNEL_PREFIX + ChannelCommon.DB_CABEL + "_" + ANALOG_TV, -1);
					TVLog("resume ch_num: " + START_CHANNEL_PREFIX + ChannelCommon.DB_CABEL + "_" + ANALOG_TV + ", " + anaNum);
					if (anaNum < 0) {
						queue0.head();
					} else {
						queue0.findChannelByNumber(anaNum);
					}
				} else {
					TVLog("queue0 == null");
				}
				channelSet.put(ANALOG_TV + ConfigType.BS_SRC_CABLE, queue0);
				channelQueueMap.put(ANALOG_TV, queue0);

				// cable dtv
				ChannelQueue queue1 = new ChannelQueue(cableChs);
				if (null != queue1) {
					int cableDigNum = storage.getInt(START_CHANNEL_PREFIX + ChannelCommon.DB_CABEL + "_" + DIGITAL_TV, -1);
					TVLog("resume ch_num: " + START_CHANNEL_PREFIX + ChannelCommon.DB_CABEL + "_" + DIGITAL_TV + ", " + cableDigNum);
					if (cableDigNum < 0) {
						queue1.head();
					} else {
						queue1.findChannelByNumber(cableDigNum);
					}
				} else {
					TVLog("queue1 == null");
				}
				channelSet.put(DIGITAL_TV + ConfigType.BS_SRC_CABLE, queue1);

				// air dtv 
				ChannelQueue queue2 = new ChannelQueue(airChs);
				if (null != queue2) {
					int airDigNum = storage.getInt(START_CHANNEL_PREFIX + ChannelCommon.DB_AIR + "_" + DIGITAL_TV, -1);
					TVLog("resume ch_num: " + START_CHANNEL_PREFIX + ChannelCommon.DB_AIR + "_" + DIGITAL_TV + ", " + airDigNum);
					if (airDigNum < 0) {
						queue2.head();
					} else {
						queue2.findChannelByNumber(airDigNum);
					}
				} else {
					TVLog("queue2 == null");
				}
				channelSet.put(DIGITAL_TV + ConfigType.BS_SRC_AIR, queue2);


				// get the real tunerMode for putting right queue to digital QueueMap 
				TVOption<Integer> changeOpt = (TVOption<Integer>) configurer.getOption(IntegerOption.CFG_TUNNER_MODE_CHG);
				int tunerModeForceChanged = changeOpt.get();
				int realDigitalTunerMode;
				if (ConfigType.COMMON_ON == tunerModeForceChanged) {
					realDigitalTunerMode = ConfigType.BS_SRC_AIR;
				} else {
					realDigitalTunerMode = getTunnerMode();
				}
				TVLog("real tunerMode for digital(0_t, 1_c):  " + realDigitalTunerMode);

				if (ConfigType.BS_SRC_AIR == realDigitalTunerMode) {
					channelQueueMap.put(DIGITAL_TV, queue2);
					if (null == airRawList || airRawList.size() == 0) {
						updateCompStatus(ComponentService.COMP_TTX_SYS_STATUS_EMPTY_SVL);
					}
				} else {
					channelQueueMap.put(DIGITAL_TV, queue1);
					if (null == cableRawList || cableRawList.size() == 0) {
						updateCompStatus(ComponentService.COMP_TTX_SYS_STATUS_EMPTY_SVL);
					}
				}
			}

		} else {   
			 /*************************************
		     * air and cable atv exist separately * 
		     *************************************/

			List<ChannelInfo> rawList = null;
			try {
				// Raw channel Information store either in DB_ANALOG or DB_AIR.
				if (rawChSrv != null)
					rawList = rawChSrv.getChannelList(getNativeDBName());
			} catch (TVMException e) {
				e.printStackTrace();
			}
			
			List<TVChannel> analogs = new ArrayList<TVChannel>();
			List<TVChannel> digitals = new ArrayList<TVChannel>();
			
			TVChannel ch = null;
			if (rawList != null) {
				if (rawList.size() == 0) {
					updateCompStatus(ComponentService.COMP_TTX_SYS_STATUS_EMPTY_SVL);
				}
				for (ChannelInfo info : rawList) {
					ch = new TVChannel(info);
					if (info instanceof AnalogChannelInfo) {
						analogs.add(ch);
					} else if (info instanceof DvbChannelInfo) {
						DvbChannelInfo rec = (DvbChannelInfo)info;
						if (!info.isUserDelete()) {
							TVLog("add ch " + ch.getChannelNum() + " [" + ch.getChannelName() + "]");
							digitals.add(ch);
						} else {
							TVLog("ignore ch " + ch.getChannelNum() + " [" + ch.getChannelName() + "]");
						}
					}
				}
			}
			
			Collections.sort(analogs, mComparator);
			Collections.sort(digitals, mComparator);
			TVLog("ATV got " + analogs.size() + " channels");
			TVLog("DTV got " + digitals.size() + " channels");
			synchronized (channelQueueMap) {
				ChannelQueue queue = new ChannelQueue(analogs);
				channelQueueMap.put(ANALOG_TV, queue);
				channelSet.put(ANALOG_TV + getTunnerMode(), queue);
			
				queue = new ChannelQueue(digitals);
				channelQueueMap.put(DIGITAL_TV, queue);
				channelSet.put(DIGITAL_TV + getTunnerMode(), queue);
			}
			
			// resume TV status
			for (String key : channelQueueMap.keySet()) {
				int num = storage.getInt(START_CHANNEL_PREFIX + getNativeDBName() + "_" + key, -1);
				if (num < 0) {
					channelQueueMap.get(key).head();
				} else {
					channelQueueMap.get(key).findChannelByNumber(num);
				}
			}
		}

		// update pipe line
		if (mOutputMode != OUTPUT_MODE_NORMAL) {
			TVLog("Update pipe all");
			sendMessageOnce(MSG_UPDATE_PIPE_ALL, null);
		} else {
			TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
			sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
		}

		TVLog("Update operation spend "	+ (SystemClock.elapsedRealtime() - startTime) + " miliseconds");
	}
	

	void update(int type) {
		if (CrossRoad.isOnlyCableAtv) {  
			/*************************
			* just only cable atv.   *
            *************************/
			List<TVChannel> analogs  = new ArrayList<TVChannel>();
			List<TVChannel> airChs   = new ArrayList<TVChannel>();
			List<TVChannel> cableChs = new ArrayList<TVChannel>();

			TVChannel ch = null;
			switch (type) {
			case ScanTask.TYPE_ATV: {
				TVLog("Update ATV");
				freshRawList("cable");
				if (null != cableRawList) {
					for (ChannelInfo info : cableRawList) {
						if (info instanceof AnalogChannelInfo) {
							ch = new TVChannel(info);
							TVLog("add analog ch " + ch.getChannelNum());
							analogs.add(ch);
						}
					}
				} else {
					TVLog("null == cableRawList");
				}
				break;
			}
			case ScanTask.TYPE_DTV: {
				TVLog("Update DTV");
				freshRawList("both");
				if (null != airRawList) {
					for (ChannelInfo info : airRawList) {
						if (info instanceof DvbChannelInfo) {
							ch = new TVChannel(info);
							if (info.isUserDelete()) {
								TVLog("ignore air digital ch " + ch.getChannelNum() + " [" + ch.getChannelName() + "]");
							} else {
								TVLog("add air digital ch " + ch.getChannelNum() + " [" + ch.getChannelName() + "]");
								airChs.add(ch);
							}
						}
					}
				} else {
					TVLog("null == airRawList");
				}

				if (null != cableRawList) { 
					for (ChannelInfo info : cableRawList) {
						if (info instanceof DvbChannelInfo) {
							ch = new TVChannel(info);
							if (info.isUserDelete()) {
								TVLog("ignore cable digital ch " + ch.getChannelNum() + " [" + ch.getChannelName() + "]");
							} else {
								TVLog("add cable digital ch " + ch.getChannelNum() + " [" + ch.getChannelName() + "]");
								cableChs.add(ch);
							}
						}
					}
				} else {
					TVLog("null == cableRawList");
				}
				break;
			}

			default:
				TVLog("Notice: Update unknown " + type);
				return;
			}

			Collections.sort(analogs,  mComparator);
			Collections.sort(cableChs, mComparator);
			Collections.sort(airChs,   mComparator);
			
			synchronized (channelQueueMap) {
				if (ScanTask.TYPE_ATV == type) {
					ChannelQueue queue0 = new ChannelQueue(analogs);
					queue0.head();
					channelSet.put(ANALOG_TV + ConfigType.BS_SRC_CABLE, queue0);
					channelQueueMap.put(ANALOG_TV, queue0);
				} 
				else if (ScanTask.TYPE_DTV == type) {
					ChannelQueue queue1 = new ChannelQueue(cableChs);
					queue1.head();
					channelSet.put(DIGITAL_TV + ConfigType.BS_SRC_CABLE, queue1);
					
					ChannelQueue queue2 = new ChannelQueue(airChs);
					queue2.head();
					channelSet.put(DIGITAL_TV + ConfigType.BS_SRC_AIR, queue2);


					// get the real tunerMode for putting right queue to digital QueueMap 
					TVOption<Integer> changeOpt = (TVOption<Integer>) configurer.getOption(IntegerOption.CFG_TUNNER_MODE_CHG);
					int tunerModeForceChanged = changeOpt.get();
					int realDigitalTunerMode;
					if (ConfigType.COMMON_ON == tunerModeForceChanged) {
						realDigitalTunerMode = ConfigType.BS_SRC_AIR;
					} else {
						realDigitalTunerMode = getTunnerMode();
					}
					TVLog("real tunerMode for digital(0_t, 1_c):  " + realDigitalTunerMode);
					
					if (ConfigType.BS_SRC_AIR == realDigitalTunerMode) {
						channelQueueMap.put(DIGITAL_TV, queue2);
						if (null == airRawList || airRawList.size() == 0) {
							updateCompStatus(ComponentService.COMP_TTX_SYS_STATUS_EMPTY_SVL);
						}
					} else {
						channelQueueMap.put(DIGITAL_TV, queue1);
						if (null == cableRawList || cableRawList.size() == 0) {
							updateCompStatus(ComponentService.COMP_TTX_SYS_STATUS_EMPTY_SVL);
						}
					}
				}
			}
			
		} else {  	
			/**************************************
		     * air and cable atv exist separately * 
		     **************************************/

			List<ChannelInfo> rawList = null;  // raw channel info stored either in DB_ANALOG or DB_AIR.
			try {
				if (rawChSrv != null)
					rawList = rawChSrv.getChannelList(getNativeDBName());
			} catch (TVMException e) {
				e.printStackTrace();
			}
			
			if (rawList != null) {
				if (rawList.size() == 0) {
					updateCompStatus(ComponentService.COMP_TTX_SYS_STATUS_EMPTY_SVL);
				}
			} else {
				return;
			}
			
			List<TVChannel> channels = new ArrayList<TVChannel>();
			TVChannel ch = null;
			switch (type) {
			case ScanTask.TYPE_ATV: {
				TVLog("Update ATV");
				for (ChannelInfo info : rawList) {
					ch = new TVChannel(info);
					if (info instanceof AnalogChannelInfo) {
						channels.add(ch);
					}
				}
				break;
			}
			case ScanTask.TYPE_DTV: {
				TVLog("Update DTV");
				for (ChannelInfo info : rawList) {
					ch = new TVChannel(info);
					if (info instanceof DvbChannelInfo) {
						DvbChannelInfo rec = (DvbChannelInfo)info;
						TVLog(rec.getProgId() + " " + rec.getShortName() + ": freq: " + rec.getFrequency()+ ", nwid: " + rec.getNwId() + ", onid: " + rec.getOnId() 
								+ ", tsId: " + rec.getTsId() + ", mod: " + rec.getMod() + ", sym: " + rec.getSymRate() + ", bw: " + rec.getBandWidth() 
								+ ", brdcstM: " + rec.getBrdcstMedium());
						
						if (!info.isUserDelete()) {
							TVLog("add ch " + ch.getChannelNum() + " [" + ch.getChannelName() + "]");
							channels.add(ch);
						} else {
							TVLog("ignore ch " + ch.getChannelNum() + " [" + ch.getChannelName() + "]");
						}
					}
				}
				break;
			}
			
			default:
				break;
			}
			
			Collections.sort(channels, mComparator);
			synchronized (channelQueueMap) {
				ChannelQueue queue = new ChannelQueue(channels);
				queue.head();
				String tv = type == 0 ? ANALOG_TV : DIGITAL_TV;
				channelQueueMap.put(tv, queue);
				channelSet.put(tv + getTunnerMode(), queue);
			}
		}
	}
	
	public void stopOutput(String outputName) {
		int ret = -100;
		int winId = TV_WIN_ID_ALL;
		TVOption<Integer> blueOpt = (TVOption<Integer>) configurer.getOption(ConfigType.CFG_BLUE_SCREEN);
		if (null != blueOpt && blueOpt.get() == ConfigType.COMMON_ON) {
			if (outputName.equals(InputService.INPUT_OUTPUT_MAIN)) {
				winId = TV_WIN_ID_MAIN;
			} else if (outputName.equals(InputService.INPUT_OUTPUT_SUB)) {
				winId = TV_WIN_ID_SUB;
			} 
			switch(winId) {
				case TV_WIN_ID_MAIN:
					CrossRoad.isForceBlackMuteMain = true;
					break;
				case TV_WIN_ID_SUB:
					CrossRoad.isForceBlackMuteSub  = true;
					break;
				default:
					CrossRoad.isForceBlackMuteMain = true;
					CrossRoad.isForceBlackMuteSub  = true;
					break;
			}
			
			ret = methods.setVideoMuteById(winId, true, 0, 0, 0);
			TVCommonNative.TVLog("setVideoMuteById(" + winId + ", true, 0, 0, 0) ret " + ret);
		}

		try {
			ret = inputSrv.stopDesignateOutput(outputName, true);
			TVLog("stopDesignateOutput(" + outputName + ", true) ret " + ret);
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}


	private int stopOutputLazyMute(String outputName) {
		int ret = -100;
		int winId = TV_WIN_ID_ALL;
		try {
			ret = inputSrv.stopDesignateOutput(outputName, true);
			TVLog("stopDesignateOutput("+ outputName + ", true) ret " + ret);
		} catch (TVMException e) {
			e.printStackTrace();
		}

		TVOption<Integer> blueOpt = (TVOption<Integer>) configurer.getOption(ConfigType.CFG_BLUE_SCREEN);
		if (null != blueOpt && blueOpt.get() == ConfigType.COMMON_ON) {
			if (outputName.equals(InputService.INPUT_OUTPUT_MAIN)) {
				winId = TV_WIN_ID_MAIN;
			} else if (outputName.equals(InputService.INPUT_OUTPUT_SUB)) {
				winId = TV_WIN_ID_SUB;
			} 
			switch(winId) {
				case TV_WIN_ID_MAIN:
					CrossRoad.isForceBlackMuteMain = true;
					break;
				case TV_WIN_ID_SUB:
					CrossRoad.isForceBlackMuteSub  = true;
					break;
				default:
					CrossRoad.isForceBlackMuteMain = true;
					CrossRoad.isForceBlackMuteSub  = true;
					break;
			}
			
			ret = methods.setVideoMuteById(winId, true, 0, 0, 255);
			TVCommonNative.TVLog("setVideoMuteById(" + winId + ", true, 0, 0, 255) ret " + ret);
		}
		
		return ret;
	}
	

	/* tv in which window, main or sub */
	private static int winIdTvIn = TV_WIN_ID_MAIN;
	
	public static int getWinIdTvIn() {
		return winIdTvIn;
	}
	
	public static String getWinStringTvIn() {
		switch (winIdTvIn) {
			case TV_WIN_ID_SUB:
				return InputService.INPUT_OUTPUT_SUB;
			case TV_WIN_ID_NONE:
				return "none";
			case TV_WIN_ID_MAIN:
			default:
				return InputService.INPUT_OUTPUT_MAIN;
		}
	}

	// cn support only one running tv now, atv and dtv could not run at the same time
	private static int runningTvType = ScanTask.TYPE_ATV;	
	
	private static String getRunningTvName() {
		switch (runningTvType) {
			case ScanTask.TYPE_ATV:
				return ANALOG_TV;
			case ScanTask.TYPE_DTV:
				return DIGITAL_TV;
			default:
				return "none";
		}
	}
	

	synchronized void updatePipeLine() {
		if (null == outputs || outputs.size() < 1) {
			TVLog("return reason: unavailable outputs");
			return;
		}

		for (TVOutput output : outputs) {
			updatePipeLine(output);
		}
		return;
	}
	
	/**
	 * input[dtv], 			inputHW[tv0],			inputType[tv]
	 * input[atv], 			inputHW[tv0],			inputType[tv]
	 * input[av0], 			inputHW[av0],			inputType[av]
	 * input[component0],	inputHW[component0],	inputType[component]
	 * input[hdmi0],		inputHW[hdmi0], 		inputType[hdmi]
	 * input[hdmi1],		inputHW[hdmi1], 		inputType[hdmi]
	 * input[hdmi2],		inputHW[hdmi2], 		inputType[hdmi]
	 * input[hdmi3],		inputHW[hdmi3], 		inputType[hdmi]
	 * input[vga0],			inputHW[vga0],			inputType[vga]
	 */

	synchronized void updatePipeLine(TVOutput output) {
		if (null == output) {
			TVLog("return reason: output == null.");
			return;
		}
		
		TVInput input = output.getInput();
		if (null == input) {
			TVLog("return reason: input == null.");
			return;
		}

		String outputName  = output.getName();
		String inputName   = input.getName();
		String inputHWName = input.getHWName();
		String inputType   = input.getType();
		TVLog("start: output[" + outputName + "], input[" + inputName + "], inputHW[" + inputHWName + "], inputType[" + inputType + "]");
		if (null != inputType) {
			TVLog("input cfgGrp: " + input.getCfgGrpIdxForType(inputType));
		}
		
		TVChannel ch = null;
		ChannelQueue queue = channelQueueMap.get(inputName);
		if (null != queue) {
			TVLog(inputName + " queue != null, " + queue.getChannels().size() + " channel in queue");
			ch = queue.getChannel();
			if (null != ch) {
				storage.setInt(START_CHANNEL_PREFIX + getNativeDBName() + "_" + inputName, ch.getChannelNum());
				TVLog("store ch_num: " + START_CHANNEL_PREFIX + getNativeDBName() + "_" + inputName + ", ch_num: " + ch.getChannelNum());
				handler.post(new DelegaterChannelChange(ch));
			} else {
				TVLog("ch == null");
			}
		} else {
			TVLog(inputName + " queue == null");
		}

		int ret = -100;
		boolean b_isBeforeSvcChgUpdated = false;
		if (mInputIsChanged) {
			mInputIsChanged = false;
			TVLog("InputSource: " + mCurrentInputName + " --> " + mChangingToInputName + ". stopOutput(" + outputName + ") first");
			stopOutput(outputName);
			
			TVOptionRange<Integer> cecOpt = (TVOptionRange<Integer>) configurer.getOption(ConfigType.CEC_CEC_FUNC);
			if (null != cecOpt && cecOpt.get() == ConfigType.CEC_FUNC_ON) {
				try {
					ret = cecCompSrv.updateSysStatus(CecComponentService.CEC_COMP_SYS_STATUS_BEFORE_SOURCE_CHANGE);
					TVLog("Set_Status_Before_SVC_CHG ret: " + ret);
					if (CecComponentService.CEC_COMP_RET_OK == ret) {
						b_isBeforeSvcChgUpdated = true;
					}
				} catch (TVMException e) {
					e.printStackTrace();
				}
			}
		} else {
			TVLog("InputSource is the same. Needn't stopOutput(" + outputName + ") first");
		} 

		// recording the output which tv bind to
		if (inputHWName.equals("tv0")) {
			// TV(atv/dtv) will bind to one output main/sub
			if (outputName.equals(InputService.INPUT_OUTPUT_MAIN)) {
				winIdTvIn = TV_WIN_ID_MAIN;
			} else {
				winIdTvIn = TV_WIN_ID_SUB;
			}

			if (inputName.equals(ANALOG_TV)) {
				runningTvType = ScanTask.TYPE_ATV;
			} else {
				runningTvType = ScanTask.TYPE_DTV;
			}
		} else {
			// check current updating output is whether the output TV bound to or not
			if (outputName.equals(getWinStringTvIn())) {
				// TV is NOT bound to any output now
				winIdTvIn = TV_WIN_ID_NONE;
				runningTvType = 255;
			}
		}

		ret = inputSrv.bind(outputName, inputHWName);
		TVLog("Bind output: " + outputName + " with input: " + inputName + ", ret " + ret);
		if (ret < 0) {
			TVLog("bind fail. stopOutputLazyMute(" + outputName + ")");
			// bind fail. There is none notify coming back. So we need stop it and check blue mute
			stopOutputLazyMute(outputName);
		}

		TVOptionRange<Integer> opt = (TVOptionRange<Integer>) configurer.getOption(ConfigType.CFG_EARLY_PLAY);
		if (inputType != INPUT_TYPE_TV && outputName.equals("main")) {				
			if (null != opt) {
				opt.set(input.isBlocked() ? ConfigType.EARLY_PLAY_DISENABLED : ConfigType.EARLY_PLAY_ENABLED);
		    }
		}	
		
		TVOption<Integer> blockOpt = (TVOption<Integer>) configurer.getOption(IntegerOption.CFG_INPUT_SOURCE_BLOCK);
		blockOpt.set((input.isPhysicalBlocked())? 1 : 0);

		if (input.isPhysicalBlocked()) {
			// show lock info
			TVLog(inputName + ": inputType " + inputType + ", blocked " + input.isPhysicalBlocked() + ", TempUnblocked " + input.isTempUnblocked());
			if (INPUT_TYPE_TV == inputType) {
				if (null == ch) {
					TVLog("null == ch");
				} else {
					TVLog(ch.getChannelNum() + " [" + ch.getChannelName() + "]: physicalBlocked " + ch.isPhysicalBlocked() + ", TempUnblocked " + ch.isTempUnblocked());
				}
			}

			// Once the channel is temporarily unblocked, it will be always
			// unblocked even the current input source is blocked.
			if (INPUT_TYPE_TV == inputType && input.isTempUnblocked() && null != ch && ch.isTempUnblocked()) {
				TVLog("Ch is tempUnblocked although " + inputName + " is blocked");
			} else if (!input.isTempUnblocked()) {
				TVLog(inputName + " is blocked. stopOutputLazyMute(" + outputName + ", true)");
				handler.post(new DelegaterInputBlocked(inputName));
				// input is blocked. Don't allow to select svc. No notify comes back. Stop it and check blue mute
				stopOutputLazyMute(outputName);
				return;
			} 
		}		
				
		if (inputType == INPUT_TYPE_TV) {
			if (null != opt && null != ch && outputName.equals("main")) {
			 	opt.set(ch.isPhysicalBlocked() || input.isPhysicalBlocked() ? ConfigType.EARLY_PLAY_DISENABLED : ConfigType.EARLY_PLAY_ENABLED);
			} else { 				
   			   	opt.set(ConfigType.EARLY_PLAY_DISENABLED);
			}

			if (ch != null) {
				if (!ch.isBlocked()) {
					try {
						if (!b_isBeforeSvcChgUpdated) {
							// ttxComp before_svc_chg msg is actually the same as cecComp before_source_chg msg 
							updateCompStatus(ComponentService.COMP_TTX_SYS_BEFORE_SVC_CHG);
						}
						
						ret = brdcstSrv.channelSelect(ch.rawInfo);
						if (ret < 0) {
							TVLog("select ch fail ret " + ret + ". stopOutputLazyMute(" + outputName + ")");
							// ch select fail. No notify comes back. Stop it and check blue mute
							stopOutputLazyMute(outputName);
						} else {
							TVLog("select ch success. ret " + ret);
						}
					} catch (TVMException e) {
						e.printStackTrace();
					}

				} else {
					TVLog("Channel is blocked. stopOutputLazyMute(" + outputName + ")");
					if(mCurrentOutput.getName().equals(output.getName())){
						handler.post(new DelegaterChannelBlocked(ch));
					}
					// ch is blocked. Don't allowed to select svc. No notify comes back. Stop it and check blue mute
					stopOutputLazyMute(outputName);
					
					TVOption<Integer> preSelectResetOpt = (TVOption<Integer>) configurer.getOption(IntegerOption.CFG_PRE_SELECT_RESET);
					preSelectResetOpt.set(ConfigType.COMMON_ON);
				}

			} else {
				TVLog("Channel is null. stopOutputLazyMute(" + outputName + ")");
				// No ch in current atv/dtv. Don't select svc and no notify comes back. Stop it and check blue mute
				stopOutputLazyMute(outputName);
			
				TVOption<Integer> preSelectResetOpt = (TVOption<Integer>) configurer.getOption(IntegerOption.CFG_PRE_SELECT_RESET);
				preSelectResetOpt.set(ConfigType.COMMON_ON);
			}
		}
		else {
			TVLog("Input != TV");
		}

		return;
	}
	

	@Override
	protected void finalize() throws Throwable {
		super.finalize();
	}

	private static final int MSG_UPDATE_PIPE = 1;
	private static final int MSG_UPDATE_PIPE_ALL = 2;
	private static final int MSG_UPDATE_CHANNEL = 3;

	@Override
	public boolean handleMessage(Message msg) {
		switch (msg.what) {
		case MSG_UPDATE_PIPE:
			updatePipeLine((TVOutput) msg.obj);
			break;
		case MSG_UPDATE_PIPE_ALL:
			updatePipeLine();
			break;
		case MSG_UPDATE_CHANNEL:
			break;

		default:
			break;
		}
		return false;
	}

	void sendMessageOnce(int what, Object obj) {
		if (!handler.hasMessages(what)) {
			handler.sendMessage(handler.obtainMessage(what, obj));
		}
	}

	@Override
	public boolean queueIdle() {
		// TODO Auto-generated method stub
		return false;
	}

	TVInput getTVInput(String name) {
		for (TVInput input : inputs) {
			if (input.getName().equals(name)) {
				return input;
			}
		}
		return null;
	}

	TVOutput getTVOutput(String name) {
		for (TVOutput output : outputs) {
			if (output.getName().equals(name)) {
				return output;
			}
		}
		return null;
	}

	public void channelDown() {
		channelDown(ChannelFilter.TVNotSkipFilter);
	}

	public void channelUp() {
		channelUp(ChannelFilter.TVNotSkipFilter);
	}

	@Override
	public void selectUp() throws RemoteException {
		channelUp(ChannelFilter.TVNothingFilter);
	}

	@Override
	public void selectDown() throws RemoteException {
		channelDown(ChannelFilter.TVNothingFilter);
	}

	public void favoriteChannelDown() {
		channelDown(ChannelFilter.TVFavoriteFilter);
	}

	public void favoriteChannelUp() {
		channelUp(ChannelFilter.TVFavoriteFilter);
	}

	void channelUp(ChannelFilter filter) {
		ChannelQueue queue = channelQueueMap.get(mCurrentOutput.getInput().getName());
		if (queue != null) {
			TVChannel curChannel = queue.getChannel();
			if (curChannel == null) {
				TVLog("fail: curChannel == null");
				return;
			}
			TVChannel nextChannel;
			do {
				nextChannel = queue.next();
				if (filter.filter(nextChannel)) {
					break;
				}
			} while (curChannel != nextChannel);
		}
		TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
		sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
	}

	void channelDown(ChannelFilter filter) {
		ChannelQueue queue = channelQueueMap.get(mCurrentOutput.getInput().getName());
		if (queue != null) {
			TVChannel curChannel = queue.getChannel();
			if (curChannel == null) {
				TVLog("fail: curChannel == null");
				return;
			}
			TVChannel preChannel;
			do {
				preChannel = queue.previous();
				if (filter.filter(preChannel)) {
					break;
				}

			} while (curChannel != preChannel);
		}
		TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
		sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
	}

	public TVChannel getCurrentChannel() {
		ChannelQueue queue = channelQueueMap.get(mCurrentOutput.getInput().getName());
		if (queue != null) {
			return queue.getChannel();
		}
		return null;
	}

	public int getCurrentChannelNum() {
		ChannelQueue queue = channelQueueMap.get(mCurrentOutput.getInput()
				.getName());
		if (queue != null) {
			TVChannel ch = queue.getChannel();
			if (ch != null) {
				return ch.getChannelNum();
			}
		}
		return -1;
	}

	public boolean select(int num) {
		TVChannel ch = null;
		ChannelQueue queue = channelQueueMap.get(mCurrentOutput.getInput().getName());
		if (queue != null) {
			ch = queue.findChannelByNumber(num);
		}
		TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
		sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
		return ch != null;
	}

	public boolean select(TVChannel ch) {
		TVChannel channel = null;
		ChannelQueue queue = channelQueueMap.get(mCurrentOutput.getInput().getName());
		if (queue != null) {
			channel = queue.findChannel(ch);
		}
		TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
		sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
		return channel != null;
	}

	public void selectPrev() {
		ChannelQueue queue = channelQueueMap.get(mCurrentOutput.getInput().getName());
		if (queue != null) {
			queue.getPreSelected();
			TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
			sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
		} else {
			TVLog("fail: queue == null");
		}
	}


	/**
     * OK Case: 
     * ATV tuner mode only Cable.
     * DTV tuner mode could be Cable or Air.
     * ForceChange ON is only in ATV, but ATV ForceChange could ON/OFF.
     * DTV ForceChange only OFF.
     *
     * Some cases should be noticed:
     * (1) after upgrade img, default input is ATV, so
     *     1) select defult tuner mode Air in wizard:   OK Case [ForceChange ON, tuner mode Cable].
     *     2) select defult tuner mode Cable in wizard: OK Case [OFF, Cable]
     * (2) change input source: 
     *     1) go out of ATV -> others(include DTV)
     *        i)  if ForceChange was ON,  OK Case [OFF, Air]
     *        ii) if ForceChange was OFF, OK Case [OFF, Cable]
     *     2) others(include DTV) go into ATV
     *     	  i)  if tuner mode is Cable: OK Case [OFF, Cable]
     *        ii) if tuner mode is Air:   OK Case [ON,  Cable]
     * (3) most dangerous case: as far as i know, preset channels, which don't care currect input source (WTF...)
     *     preset channels may takes action in ATV and DTV(air/cable):  
     *     should prepare real tuner mode for set right queue to QueueMap(DTV, XXX).
     *	   1) if ForceChange ON,  digital tuner mode is Air
     *     2) if ForceChange OFF, digital tuner mode is Cable
	 */
	public void adjustTunnerMode(String changingToInputName) {
		if (null == changingToInputName) {
			TVLog("return reason: changingToInputName is null");
			return;
		}

		TVOption<Integer> modeOpt = (TVOption<Integer>) configurer.getOption(IntegerOption.CFG_BS_SRC);
		TVOption<Integer> changeOpt = (TVOption<Integer>) configurer.getOption(IntegerOption.CFG_TUNNER_MODE_CHG);
		if (null == modeOpt) {
			TVLog("return reason: modeOpt is null");
			return;
		} else if (null == changeOpt) {
			TVLog("return reason: changeOpt is null");
			return;
		}

		int tunnerMode  = modeOpt.get();
		int forceChange = changeOpt.get();
		TVLog("Before rejust: tuner(0_t, 1_c):  " + tunnerMode + ", change(0_off, 1_on):  " + forceChange + ", input to-be: " + changingToInputName);

		if (changingToInputName.equals(ANALOG_TV)) {
			// when set or change to atv
			if (ConfigType.BS_SRC_AIR == tunnerMode) {
				// tuner is air, change to cable, and set flag
				TVLog("Force rejust tunnerMode to: " + ConfigType.BS_SRC_CABLE);
				modeOpt.set(ConfigType.BS_SRC_CABLE);
				
				if (ConfigType.COMMON_ON != forceChange) {
					TVLog("Set forceChange: " + ConfigType.COMMON_ON);
					changeOpt.set(ConfigType.COMMON_ON);
				} else {
					TVLog("don't need set forceChange " + forceChange + " again");
				}
			} else {
				// tuner is cable, just use it and don't set flag
				TVLog("cable tuner don't need control");
			}
			
		} else {
			// when set or change to other input source 
			if (ConfigType.COMMON_OFF != forceChange) {
				// tuner was changed forcibly to air, so resume it to cable and clear the flag
				if (ConfigType.BS_SRC_AIR != tunnerMode) {
					TVLog("Force resume tunnerMode to: " + ConfigType.BS_SRC_AIR);
					modeOpt.set(ConfigType.BS_SRC_AIR);
				} else {
					TVLog("don't need set tuner " + tunnerMode + " again");
				}

				changeOpt.set(ConfigType.COMMON_OFF);
				
			} else {
				// clean flag means that tuner was NOT changed forcibly, so use it 
				TVLog("didn't force change ago, so don't need set forceChange");
			}
		}

		TVLog("After   rejust: tuner(0_t, 1_c):  " + modeOpt.get() + ", change(0_off, 1_on):  " + changeOpt.get());
		return;
	}


	public void changeInputSource(String inputName) {
		changeInputSource(mCurrentOutput.getName(), inputName);
	}

	@Override
	public synchronized void changeInputSource(String outputName, String inputName) {
		TVOutput output = getTVOutput(outputName);
		TVInput  input  = getTVInput(inputName);
		TVOutput other  = getTVOutput(outputName.equals("main") ? "sub" : "main");

		if (output == null || input == null || other == null) {
			throw new IllegalArgumentException();
		}
		
		boolean conflict = input.conflictWith(other.getInput());

		switch (mOutputMode) {
		case OUTPUT_MODE_PIP:
		case OUTPUT_MODE_POP:
			if (conflict) {
				TVLog("[" + other.getName() + "<-->"
						+ other.getInput().getName() + "] and " + "["
						+ outputName + "<-->" + inputName
						+ "] are incompatible.");
				return;
			}
			break;
		case OUTPUT_MODE_APP_PIP:
		case OUTPUT_MODE_NORMAL:

			if (conflict) {
				other.connect(output.getInput());
			}

			break;

		default:
			break;
		}

		mCurrentInputName    = output.getInput().getName();
		mChangingToInputName = inputName;
		mInputIsChanged      = !mCurrentInputName.equals(mChangingToInputName);
		TVLog("mInputIsChanged = " + mInputIsChanged + "  InputSource: " + mCurrentInputName + " --> " + mChangingToInputName);

		synchronized (mSync) {
			output.connect(input);
			setDefaultOutput(output);
		}

		if (CrossRoad.isOnlyCableAtv) {
			adjustTunnerMode(mChangingToInputName);
		}
		
		handler.post(new DelegaterInputSelected(outputName, inputName));
		TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
		sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
	}


	public void changeNextInputSource() {
		List<String> confcs = getConflictInputs();
		TVInput curInput = mCurrentOutput.getInput();
		int index = inputs.indexOf(curInput);
		TVInput nextInput;
		
		do {
			index = (index + 1) % inputs.size();
			nextInput = inputs.get(index);
			if (!confcs.contains(nextInput.getName())) {
				TVLog("The next compatible input is" + nextInput.getName());
				break;
			}
		} while (curInput != nextInput);

		mCurrentInputName    = curInput.getName();
		mChangingToInputName = nextInput.getName();
		mInputIsChanged      = !mCurrentInputName.equals(mChangingToInputName);
		TVLog("mInputIsChanged = " + mInputIsChanged + "  InputSource is changed: " + mCurrentInputName + " --> " + mChangingToInputName);

		mCurrentOutput.connect(nextInput);
		int cfgIdx = mCurrentOutput.getInput().cfgIndex;
		configurer.setGroup(cfgIdx);

		if (CrossRoad.isOnlyCableAtv) {
			adjustTunnerMode(mChangingToInputName);
		}

		//TVLog("Set Configure Group to " + TVConfigurer.CFG_GRP_NAMES[cfgIdx]);
		handler.post(new DelegaterInputSelected(mCurrentOutput.getName(),
				nextInput.getName()));
		TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
		sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
	}


	@Override
	public String getInputSource(String outputName) {
		TVOutput output = getTVOutput(outputName);
		TVInput input = null;
		if (null != output) {
			input = output.getInput();
		}
		return null != input ? input.getName() : null;
	}
	

	public String getInputSourceType(String inputName) {
		TVInput input = getTVInput(inputName);
		return input != null ? input.getType() : null;
	}

	public String getCurrentInputSource() {
		return mCurrentOutput.getInput().getName();
	}

	public void blockInputSource(String inputName, boolean block) {
		TVInput input = getTVInput(inputName);
		if (null != input) {
			input.setBlocked(block);
			switch (mOutputMode) {
				case OUTPUT_MODE_NORMAL:
					TVLog("Normal mode:  update pipe: " + mCurrentOutput.getName() + ", " + inputName + ", block: " + block);
					sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
					break;
				case OUTPUT_MODE_PIP:
				case OUTPUT_MODE_POP:
					String otherOutputName = mCurrentOutput.getName().equals("main") ? "sub" : "main";
					if (inputName.equals(getInputSource(otherOutputName))) {
						TVLog((OUTPUT_MODE_PIP == mOutputMode ? "PIP" : "POP") + " mode: update pipe: " + otherOutputName + ", " + inputName + ", block: " + block);
						sendMessageOnce(MSG_UPDATE_PIPE, getTVOutput(otherOutputName));
					}else{
						TVLog((OUTPUT_MODE_PIP == mOutputMode ? "PIP" : "POP") + " mode: update pipe: " + mCurrentOutput.getName() + ", " + inputName + ", block: " + block);
						sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
					}
					break;
				default:
					TVLog("???? mode: mOutputMode: " + mOutputMode + ", do nothing");
					break;
			}
		} else {
			TVLog("fail: null == input");
		}
	}

	public void tempUnblockInputSource(String inputName) {
		TVInput input = getTVInput(inputName);
		if (input != null) {
			input.tempUnblocked();
			TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
			sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
		} else {
			TVLog("fail: input == null");
		}
	}

	public boolean isInputSourceBlocked(String inputName) {
		TVInput input = getTVInput(inputName);
		if (input != null) {
			return input.isBlocked();
		}
		return false;
	}

	public boolean isInputSourcePhysicalBlocked(String inputName) {
		TVInput input = getTVInput(inputName);
		if (input != null) {
			return input.isPhysicalBlocked();
		}
		return false;
	}

	public String[] getInputSourceArray() {
		List<String> inps = new ArrayList<String>();
		for (TVInput input : inputs) {
			inps.add(input.getName());
		}
		return inps.toArray(new String[inputs.size()]);
	}

	private List<String> getConflictInputs(String inputName) {
		List<String> inps = new ArrayList<String>();
		String inpType = getInputSourceType(inputName);
		for (TVInput input : inputs) {
			for (String type : inputSrv.getConflictType(inpType)) {
				if (input.getType() == type) {
					inps.add(input.getName());
				}
			}

		}
		return inps;
	}

	public List<String> getConflictInputs() {
		List<String> inputs = new ArrayList<String>();
		switch (mOutputMode) {
		case OUTPUT_MODE_NORMAL:
		case OUTPUT_MODE_APP_PIP:

			break;
		case OUTPUT_MODE_PIP:
		case OUTPUT_MODE_POP:
			TVOutput other = getTVOutput(mCurrentOutput.getName()
					.equals("main") ? "sub" : "main");
			if (other != null) {
				return getConflictInputs(other.getInput().getName());
			}
		default:
			break;
		}
		return inputs;
	}

	public List<TVChannel> getChannels(String inputName) {
		TVInput input = getTVInput(inputName);
		if (input != null) {
			ChannelQueue queue = channelQueueMap.get(input.getName());
			if (queue != null) {
				return queue.getChannels();
			}
		}
		return null;
	}

	public List<TVChannel> getChannels() {
		ChannelQueue queue = channelQueueMap.get(mCurrentOutput.getInput()
				.getName());
		if (queue != null) {
			return queue.getChannels();
		}
		return null;
	}

	public List<TVChannel> getChannels(ChannelFilter filter) {
		ChannelQueue queue = channelQueueMap.get(mCurrentOutput.getInput()
				.getName());
		if (queue != null) {
			List<TVChannel> list = queue.getChannels();
			if (list == null) {
				return null;
			}
			List<TVChannel> channels = new ArrayList<TVChannel>();
			Iterator<TVChannel> iterator = list.iterator();
			while (iterator.hasNext()) {
				TVChannel ch = (TVChannel) iterator.next();
				if (filter.filter(ch)) {
					channels.add(ch);
				}
			}
			return channels;
		}
		return null;
	}

	public List<TVChannel> getChannels(int start, int count,
			ChannelFilter filter) {
		List<TVChannel> list = getChannels(filter);
		if (list != null) {
			int end = start + count;
			if (end > list.size()) {
				end = list.size();
			}
			if (start >= 0 && start <= end) {
				return list.subList(start, end);
			} else {
				throw new IllegalArgumentException();
			}
		}
		return null;
	}

	@Override
	public synchronized void deleteChannel(TVChannel ch) throws RemoteException {
		String inputName = mCurrentOutput.getInput().getName();
		ChannelQueue queue = channelQueueMap.get(inputName);
		if (queue != null) {
			TVLog("Delete " + inputName + " channel:" + ch.getChannelNum());
			queue.remove(ch);
			permanentDeleteChannel(ch);
			notifyUpdate(UpdateReason.DELETE_CH, "",
					inputName.equals(ANALOG_TV) ? 0 : 1, ch.getChannelNum());

			List<ChannelInfo> rawList = null;
			try {
				rawList = rawChSrv.getChannelList(getNativeDBName());
			} catch (TVMException e) {
				e.printStackTrace();
			}

			if (rawList != null && rawList.size() == 0) {
				updateCompStatus(ComponentService.COMP_TTX_SYS_STATUS_EMPTY_SVL);
			}
			
			queue = channelQueueMap.get(inputName);
			
			if (!queue.hasNext()) {
				TVLog("queue is null, pre_select reset!");
				
				TVOption<Integer> preSelectResetOpt = (TVOption<Integer>) configurer
						.getOption(IntegerOption.CFG_PRE_SELECT_RESET);
				preSelectResetOpt.set(ConfigType.COMMON_ON);
			}	
		}
	}

	private void permanentDeleteChannel(TVChannel ch) {
		ArrayList<ChannelInfo> rawList = new ArrayList<ChannelInfo>();
		rawList.add(ch.rawInfo);
		ch.rawInfo.setChannelDeleted(true);

		try {
			rawChSrv.setChannelList(ChannelService.ChannelOperator.DELETE,
					TVCommonNative.getNativeDBName(), rawList);
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}

	public void updateCurrentOutput() {
		TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
		sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
	}


	public void setDefaultOutput(String outputName) {
		TVOutput output = getTVOutput(outputName);
		if (output != null) {
			setDefaultOutput(output);
		}
	}

	private void setDefaultOutput(TVOutput output) {
		int index = output.getInput().cfgIndex;
		configurer.setGroup(index);
		TVLog("Set Configure Group to " + TVConfigurer.CFG_GRP_NAMES[index]);

		if (mCurrentOutput != output) {
			synchronized (mSync) {
				mCurrentOutput = output;
			}
			storage.setString(DEFAULT_OUTPUT_PREFIX, output.getName());
			@SuppressWarnings("unchecked")
			TVOption<Integer> focusOption = (TVOption<Integer>) configurer.getOption(IntegerOption.CFG_TV_FOCUS_WIN);
			focusOption.set(output.getName() == "main" ? ConfigType.TV_FOCUS_WIN_MAIN : ConfigType.TV_FOCUS_WIN_SUB);
		}
	}
		
	public void setInputCfgGrpIdx(String outputName) {
		int index = 0;
		TVOutput output = getTVOutput(outputName);
		index = output.getInput().cfgIndex;
		configurer.setGroup(index);
		TVLog("Set Configure Group to " + TVConfigurer.CFG_GRP_NAMES[index]);
	}

	public String getCurrentOutput() {
		return mCurrentOutput.getName();
	}

	private static int mOutputMode = OUTPUT_MODE_NORMAL;

	public synchronized void switchOutputMode() {
		// The system limit to switch from normal to pop mode directly, it must be normal, PIP and then POP and again.
		if (mOutputMode == OUTPUT_MODE_APP_PIP) {
			return;
		}

		TVLog("Current output mode before switch: " + mOutputMode);
		autoCorrectInputsConfliction();
		
		int ret = -100;
		TVOption<Integer> blueOpt = (TVOption<Integer>) configurer.getOption(ConfigType.CFG_BLUE_SCREEN);
		TVOptionRange<Integer> tvModeCfg = (TVOptionRange<Integer>) configurer.getOption(ConfigType.CFG_TV_MODE);
		switch (mOutputMode) {
		case OUTPUT_MODE_PIP:
			try {
				inputSrv.enterPOPAndReturnCurrentFocusOutput();
			} catch (TVMException e) {
				e.printStackTrace();
			}
			mOutputMode = OUTPUT_MODE_POP;
			tvModeCfg.set(ConfigType.TV_MODE_POP);
			TVLog("Update pipe all");
			sendMessageOnce(MSG_UPDATE_PIPE_ALL, null);
			break;
			
		case OUTPUT_MODE_POP:
			if (null != blueOpt && blueOpt.get() == ConfigType.COMMON_ON) {
				/* there are blue flash on screen when main is no signal */
				CrossRoad.isForceBlackMuteMain = true;
				ret = methods.setVideoMuteById(TV_WIN_ID_MAIN, true, 0, 0, 0);
				TVCommonNative.TVLog("setVideoMuteById(" + TV_WIN_ID_MAIN + ", true, 0, 0, 0) ret " + ret);
			}

			try {
				inputSrv.enterNormalAndReturnCurrentOutput();
			} catch (TVMException e) {
				e.printStackTrace();
			}

			TVOutput main = getTVOutput("main");
			TVOutput sub = getTVOutput("sub");
			if (main == null || sub == null) {
				TVLog("return reason: null output");
				return;
			}

			if (mCurrentOutput == sub) {
				TVInput temp = main.getInput();
				main.connect(sub.getInput());
				sub.connect(temp);
				setDefaultOutput(main);
			}

			try {
				TVLog("stopDesignateOutput(sub)");
				inputSrv.stopDesignateOutput(sub.getName(), true);
			} catch (TVMException e1) {
				e1.printStackTrace();
			}
			
			mOutputMode = OUTPUT_MODE_NORMAL;
			tvModeCfg.set(ConfigType.TV_MODE_NORMAL);
			TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
			sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
			break;
			
		case OUTPUT_MODE_NORMAL:
			try {
				inputSrv.enterPIPAndReturnCurrentFocusOutput();
			} catch (TVMException e) {
				e.printStackTrace();
			}
			mOutputMode = OUTPUT_MODE_PIP;
			tvModeCfg.set(ConfigType.TV_MODE_PIP);
			TVLog("Update pipe all");
			sendMessageOnce(MSG_UPDATE_PIPE_ALL, null);
			break;

		default:
			TVLog("unknown outputMode: " + mOutputMode);
			break;
		}
		
		TVOptionRange<Integer> opt = (TVOptionRange<Integer>) configurer.getOption(ConfigType.CEC_CEC_FUNC);
		if ((opt!= null)&& (opt.get()==ConfigType.CEC_FUNC_ON)) {
			try {
				TVLog("cecCompSrv.updateSysStatus = " + CecComponentService.CEC_COMP_SET_STATUS_AFTER_TV_MODE_CHANGE);
				cecCompSrv.updateSysStatus(CecComponentService.CEC_COMP_SET_STATUS_AFTER_TV_MODE_CHANGE);
			} catch (TVMException e) {
				e.printStackTrace();
			}
		}

		TVLog("switched to next output mode: " + mOutputMode);
	}
	

	public synchronized void enterOutputMode(int mode) {
		TVOutput main = getTVOutput("main");
		TVOutput sub = getTVOutput("sub");
		if (main == null || sub == null) {
			TVLog("return reason: null output ");
			return;
		}
		TVLog("Enter output mode from " + mOutputMode + " to " + mode);
		int val = mOutputMode;
		mOutputMode = mode;

		switch (mode) {
		case OUTPUT_MODE_APP_PIP:
			try {
				inputSrv.enterPIPAndReturnCurrentFocusOutput();
			} catch (TVMException e) {
				e.printStackTrace();
			}

			if (mCurrentOutput == main) {
				TVInput temp = main.getInput();
				main.connect(sub.getInput());
				sub.connect(temp);
				setDefaultOutput(sub);
			}

			TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
			sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
			break;
			
		case OUTPUT_MODE_NORMAL:
			try {
				inputSrv.enterNormalAndReturnCurrentOutput();
			} catch (TVMException e) {
				e.printStackTrace();
			}

			if (mCurrentOutput == sub) {
				TVInput temp = main.getInput();
				main.connect(sub.getInput());
				sub.connect(temp);
				setDefaultOutput(main);
			}

			try {
				inputSrv.stopDesignateOutput(sub.getName(), true);
			} catch (TVMException e1) {
				e1.printStackTrace();
			}

			TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
			sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
			break;
			
		case OUTPUT_MODE_PIP:
			try {
				inputSrv.enterPIPAndReturnCurrentFocusOutput();
			} catch (TVMException e) {
				e.printStackTrace();
			}
			TVLog("Update pipe all");
			sendMessageOnce(MSG_UPDATE_PIPE_ALL, null);
			break;
			
		case OUTPUT_MODE_POP:
			try {
				inputSrv.enterPOPAndReturnCurrentFocusOutput();
			} catch (TVMException e) {
				e.printStackTrace();
			}
			TVLog("Update pipe all");
			sendMessageOnce(MSG_UPDATE_PIPE_ALL, null);
			break;
			
		case OUTPUT_MODE_MMP:
			TVLog("come in OUTPUT_MODE_MMP  mOutputMode: " + mOutputMode);
			break;
			
		default:
			TVLog("unknown mode: " + mode);
			mOutputMode = val;
			break;
		}
	}
	

	public synchronized void swapInputSource() {
		TVLog("Swap input source.");
		if (mOutputMode == OUTPUT_MODE_NORMAL
				|| mOutputMode == OUTPUT_MODE_APP_PIP) {
			throw new IllegalArgumentException(
					"Swap input source operation is legal only in PIP/POP mode.");
		}

		TVOutput main = getTVOutput("main");
		TVOutput sub = getTVOutput("sub");

		if (main == null || sub == null) {
			return;
		}

		TVInput temp = main.getInput();
		main.connect(sub.getInput());
		sub.connect(temp);
		int index = mCurrentOutput.getInput().cfgIndex;
		configurer.setGroup(index);
		TVLog("Set Configure Group to " + TVConfigurer.CFG_GRP_NAMES[index]);

		for (TVOutput output : outputs) {
			try {
				inputSrv.stopDesignateOutput(output.getName(), true);
			} catch (TVMException e) {
				e.printStackTrace();
			}
		}
		
		TVLog("Update pipe all");
		sendMessageOnce(MSG_UPDATE_PIPE_ALL, null);
		
		TVOptionRange<Integer> opt = (TVOptionRange<Integer>) configurer
				.getOption(ConfigType.CEC_CEC_FUNC);
		if ((opt!= null)&& (opt.get()==ConfigType.CEC_FUNC_ON)) {
			try {
				TVLog("cecCompSrv.updateSysStatus = " + CecComponentService.CEC_COMP_SET_STATUS_AFTER_SVCS_SWAP);
				cecCompSrv.updateSysStatus(CecComponentService.CEC_COMP_SET_STATUS_AFTER_SVCS_SWAP);
			} catch (TVMException e) {
				e.printStackTrace();
			}
		}
	}

	public int getCurrentOutputMode() {
		return mOutputMode;
	}

	public synchronized void swapChannel(TVChannel ch1, TVChannel ch2) {
		if (ch1 == ch2 || ch1 == null || ch2 == null) {
			TVLog("return reason: null ch1 or ch2, or ch1 == ch2 ");
			return;
		}
		String inputName = mCurrentOutput.getInput().getName();
		ChannelQueue queue = channelQueueMap.get(inputName);
		int lNum = ch1.getChannelNum();
		int rNum = ch2.getChannelNum();
		String ch1Name = ch1.getChannelName();
		String ch2Name = ch2.getChannelName();
		int orgChId = 0;
		int newChId = 0;
		if (queue != null) {
			TVLog("[" + ch1Name + "] num: " + lNum + ", [" + ch2Name + "] num: " + rNum);
			// ch1
			ch1.rawInfo.setChannelNumber(rNum);
			ch1.rawInfo.setChannelNumberEdited(true);
			if (ch1.rawInfo.getBrdcstType() == TVCommon.BRDCST_TYPE_DTMB ||
				ch1.rawInfo.getBrdcstType() == TVCommon.BRDCST_TYPE_DVB) {
				orgChId = ch1.rawInfo.getChannelId();
				newChId = ch1.createDtvChIdByNum(orgChId, rNum);
				ch1.rawInfo.setChannelId(newChId);
				TVLog("[" + ch1Name + "] newNum: " + rNum + ", org/new ChId: " + orgChId + "/" + newChId);
			}

			// ch2
			ch2.rawInfo.setChannelNumber(lNum);
			ch2.rawInfo.setChannelNumberEdited(true);
			if (ch2.rawInfo.getBrdcstType() == TVCommon.BRDCST_TYPE_DTMB ||
				ch2.rawInfo.getBrdcstType() == TVCommon.BRDCST_TYPE_DVB) {
				orgChId = ch2.rawInfo.getChannelId();
				newChId = ch2.createDtvChIdByNum(orgChId, lNum);
				ch2.rawInfo.setChannelId(newChId);
				TVLog("[" + ch2Name + "] newNum: " + lNum + ", org/new ChId: " + orgChId + "/" + newChId);
			}

			List<ChannelInfo> list = new ArrayList<ChannelInfo>();
			list.add(ch1.rawInfo);
			list.add(ch2.rawInfo);
			try {
				rawChSrv.setChannelList(ChannelOperator.UPDATE, getNativeDBName(), list);
			} catch (TVMException e) {
				e.printStackTrace();
			}

			TVChannel ch = queue.getChannel();
			queue.remove(ch1);
			queue.remove(ch2);
			queue.add(ch1);
			queue.add(ch2);
			queue.findChannel(ch);

			notifyUpdate(UpdateReason.SWAP_CH, "", inputName.equals(ANALOG_TV) ? 0 : 1, lNum, rNum);
		}
	}

	synchronized void swapChannel(int ch1, int ch2) {
		ChannelQueue queue = channelQueueMap.get(mCurrentOutput.getInput().getName());
		if (queue != null) {
			swapChannel(findChannelByNumber(ch1), findChannelByNumber(ch2));
		}
	}


	/**
	 * Insert {@code dst} channel to the position of {@code src} channel, If the
	 * channel number of {@code dst} is smaller than src, the {@code dst} will
	 * be insert after {@code src}, or it will be inserted before {@code src}.
	 * 
	 * @param dst
	 * @param src
	 */
	public synchronized void insertChannel(TVChannel dst, TVChannel src) {
		if (dst == src || dst == null || src == null) {
			TVLog("return reason: null dst or src, or dst == src ");
			return;
		}
		String inputName = mCurrentOutput.getInput().getName();
		ChannelQueue queue = channelQueueMap.get(inputName);
		int dstNum = dst.getChannelNum();
		int srcNum = src.getChannelNum();
		String dstName = dst.getChannelName();
		String srcName = src.getChannelName();
		int offset = 0;
		int orgChId = 0;
		int newChId = 0;
		if (queue != null) {
			int dstIndex = queue.getChannels().indexOf(dst);
			int srcIndex = queue.getChannels().indexOf(src);
			List<TVChannel> subList;
			if (dstIndex > srcIndex) {
				subList = queue.getChannels().subList(srcIndex, dstIndex);
				offset = 1;
			} else {
				subList = queue.getChannels().subList(dstIndex + 1, srcIndex + 1);
				offset = -1;
			}
			TVLog("dst["   + dstName + "] index/num: " + dstIndex + "/" + dstNum + 
				  ", src[" + srcName + "] index/num: " + srcIndex + "/" + srcNum + ". offset: " + offset);

			dst.rawInfo.setChannelNumber(srcNum);
			dst.rawInfo.setChannelNumberEdited(true);
			if (dst.rawInfo.getBrdcstType() == TVCommon.BRDCST_TYPE_DTMB ||
				dst.rawInfo.getBrdcstType() == TVCommon.BRDCST_TYPE_DVB) {
				orgChId = dst.rawInfo.getChannelId();
				newChId = dst.createDtvChIdByNum(orgChId, srcNum);
				dst.rawInfo.setChannelId(newChId);
				TVLog("[" + dstName + "] newNum: " + srcNum + ", org/new ChId: " + orgChId + "/" + newChId);
			}

			int newNum = 0;
			List<ChannelInfo> channelInfos = new ArrayList<ChannelInfo>();
			for (TVChannel ch : subList) {
				newNum = ch.rawInfo.getChannelNumber() + offset;
				ch.rawInfo.setChannelNumber(newNum);
				ch.rawInfo.setChannelNumberEdited(true);
				if (ch.rawInfo.getBrdcstType() == TVCommon.BRDCST_TYPE_DTMB ||
					ch.rawInfo.getBrdcstType() == TVCommon.BRDCST_TYPE_DVB) {
					orgChId = ch.rawInfo.getChannelId();
					newChId = ch.createDtvChIdByNum(orgChId, newNum);
					ch.rawInfo.setChannelId(newChId);
				}
				channelInfos.add(ch.rawInfo);
			}
			channelInfos.add(dst.rawInfo);
			try {
				rawChSrv.setChannelList(ChannelOperator.UPDATE, getNativeDBName(), channelInfos);
			} catch (TVMException e) {
				e.printStackTrace();
			}

			TVChannel ch = queue.getChannel();
			queue.remove(dst);
			queue.add(dst);
			queue.findChannel(ch);

			notifyUpdate(UpdateReason.INSERT_CH, "", inputName.equals(ANALOG_TV) ? 0 : 1, dstNum, srcNum);
		}
	}
	

	synchronized void insertChannel(int dst, int src) {
		ChannelQueue queue = channelQueueMap.get(mCurrentOutput.getInput().getName());
		if (queue != null) {
			insertChannel(findChannelByNumber(dst), findChannelByNumber(src));
		}
	}

	public void sendPowerOff() {
		handler.post(new Runnable() {

			public void run() {
				Intent i = new Intent(
						"android.intent.action.ACTION_REQUEST_SHUTDOWN");
				i.putExtra("android.intent.extra.KEY_CONFIRM", false);
				i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				getContext().startActivity(i);

			}
		});
	}

	private long timeToPowerOff = 0L;
	private Timer timer = new Timer("poweroff");;
	private Object lock = new Object();
	private TimerTask timerTask;

	public void schedulePowerOff(long delay) {
		synchronized (lock) {
			timeToPowerOff = System.currentTimeMillis() + delay;
		}
		if (timerTask != null) {
			timerTask.cancel();
			timerTask = null;
		}
		synchronized (lock) {
			timerTask = new TimerTask() {

				@Override
				public void run() {
					sendPowerOff();
				}
			};
		}
		timer.schedule(timerTask, delay);
	}

	@Override
	public void schedulePowerOff(Date date) {
		synchronized (lock) {
			timeToPowerOff = date.getTime();
		}
		schedulePowerOff(timeToPowerOff - System.currentTimeMillis());
	}

	@Override
	public void cancelScheduledPowerOff() {
		if (timerTask != null) {
			synchronized (lock) {
				timerTask.cancel();
				timeToPowerOff = 0;
			}
		}
	}

	@Override
	public long getRemainingPowerOffTime() {
		long remainningTime;
		synchronized (lock) {
			remainningTime = timeToPowerOff - System.currentTimeMillis();
		}
		return remainningTime;
	}

	@Override
	public TVChannel findChannelByNumber(int channelNum) {
		ChannelQueue queue = channelQueueMap.get(mCurrentOutput.getInput()
				.getName());
		if (queue != null) {
			List<TVChannel> channels = queue.getChannels();
			if (channels.size() <= 0) {
				return null;
			}
			int low = 0;
			int high = channels.size() - 1;
			// Binary search
			while (low <= high) {
				int mid = (low + high) / 2;
				TVChannel ch = channels.get(mid);
				int num = ch.getChannelNum();
				if (channelNum == num) {
					return ch;
				} else if (channelNum < num) {
					high = mid - 1;
				} else {
					low = mid + 1;
				}
			}
		}
		return null;
	}

	private Vector<TVSelectorListener> selectorListeners = new Vector<TVSelectorListener>();

	public void addSelectorListener(TVSelectorListener listener) {
		synchronized (selectorListeners) {
			selectorListeners.add(listener);
		}
	}

	public void removeSelectorListener(TVSelectorListener listener) {
		synchronized (selectorListeners) {
			selectorListeners.remove(listener);
		}
	}

	private class DelegaterChannelChange implements Runnable {
		private TVChannel ch;

		public DelegaterChannelChange(TVChannel ch) {
			this.ch = ch;
		}

		public void run() {
			Enumeration<TVSelectorListener> e = selectorListeners.elements();
			TVSelectorListener item;
			while (e.hasMoreElements()) {
				item = e.nextElement();
				item.onChannelSelected(ch);
			}
		}
	}

	private class DelegaterChannelBlocked implements Runnable {
		private TVChannel ch;

		public DelegaterChannelBlocked(TVChannel ch) {
			this.ch = ch;
		}

		public void run() {
			Enumeration<TVSelectorListener> e = selectorListeners.elements();
			TVSelectorListener item;
			while (e.hasMoreElements()) {
				item = e.nextElement();
				item.onChannelBlocked(ch);
			}
		}
	}

	private class DelegaterInputSelected implements Runnable {
		private String outputName;
		private String inputName;

		public DelegaterInputSelected(String outputName, String inputName) {
			this.outputName = outputName;
			this.inputName = inputName;
		}

		@Override
		public void run() {
			Enumeration<TVSelectorListener> e = selectorListeners.elements();
			TVSelectorListener item;
			while (e.hasMoreElements()) {
				item = e.nextElement();
				item.onInputSelected(outputName, inputName);
			}
		}
	}

	private class DelegaterInputBlocked implements Runnable {
		private String inputName;

		public DelegaterInputBlocked(String inputName) {
			this.inputName = inputName;
		}

		@Override
		public void run() {
			Enumeration<TVSelectorListener> e = selectorListeners.elements();
			TVSelectorListener item;
			while (e.hasMoreElements()) {
				item = e.nextElement();
				item.onInputBlocked(inputName);
			}
		}
	}

	private Vector<ChannelsChangedListener> channelListListeners = new Vector<ChannelsChangedListener>();

	@Override
	public void addChannelsChangedListener(ChannelsChangedListener listener) {
		synchronized (channelListListeners) {
			channelListListeners.add(listener);
		}
	}

	@Override
	public void removeChannelsChangedListener(ChannelsChangedListener listener) {
		synchronized (channelListListeners) {
			channelListListeners.remove(listener);
		}
	}

	private class DelegaterChannelsChanged implements Runnable {
		@Override
		public void run() {
			Enumeration<ChannelsChangedListener> e = channelListListeners.elements();
			ChannelsChangedListener item;
			int i = 0;
			while (e.hasMoreElements()) {
				i++;
				TVCommonNative.TVLog("DelegaterChannelsChanged: " + i + " times");
				item = e.nextElement();
				item.onChanged();
			}
		}
	}

	List<IUpdateListener> iUpdateListeners = new ArrayList<IUpdateListener>();

	void addIUpdateListener(IUpdateListener listener) {
		iUpdateListeners.add(listener);
	}

	void removeIUpdateListener(IUpdateListener listener) {
		iUpdateListeners.remove(listener);
	}

	private void notifyUpdate(int reason, String str, int... para) {
		int i = 0;
		for (IUpdateListener listener : iUpdateListeners) {
			try {
				i++;
				TVCommonNative.TVLog("listener.onUpdate: " + i + " times");
				listener.onUpdate(reason, str, para);
			} catch (RemoteException e) {
				e.printStackTrace();
			}
		}
		TVCommonNative.TVLog("DelegaterChannelsChanged !!! ");
		handler.post(new DelegaterChannelsChanged());
	}

	public void setTunnerMode(int mode) {
		@SuppressWarnings("unchecked")
		TVOption<Integer> option = (TVOption<Integer>) configurer.getOption(IntegerOption.CFG_BS_SRC);
		int oldMode = 0;
		if (option == null) {
			return;
		}
		oldMode = option.get();
		if (oldMode != mode) {
			TVLog("Set Tunner mode to " + mode);
			option.set(mode);

			try {
				brdcstSrv.syncStopService();
			} catch (TVMException e) {
				e.printStackTrace();
			}
			
			if (CrossRoad.isOnlyCableAtv) {
				if (channelSet.containsKey(ANALOG_TV + ConfigType.BS_SRC_CABLE) || 
					channelSet.containsKey(DIGITAL_TV + mode)) {
					synchronized (channelQueueMap) {
						channelQueueMap.put(ANALOG_TV,  channelSet.get(ANALOG_TV  + ConfigType.BS_SRC_CABLE));
						channelQueueMap.put(DIGITAL_TV, channelSet.get(DIGITAL_TV + mode));
						TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
						sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
					}
				} else {
					TVLog("Update directly");
					update();
				}
			} else {
				if (channelSet.containsKey(ANALOG_TV + mode) ||
					channelSet.containsKey(DIGITAL_TV + mode)) {
					synchronized (channelQueueMap) {
						channelQueueMap.put(ANALOG_TV,  channelSet.get(ANALOG_TV  + mode));
						channelQueueMap.put(DIGITAL_TV, channelSet.get(DIGITAL_TV + mode));
						TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
						sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
					}
				} else {
					TVLog("Update directly");
					update();
				}
			}

			notifyUpdate(UpdateReason.SET_TUNNER, "");
		}
		return;
	}

	public int getTunnerMode() {
		@SuppressWarnings("unchecked")
		TVOption<Integer> option = (TVOption<Integer>) configurer.getOption(IntegerOption.CFG_BS_SRC);
		if (option != null)
			return option.get();
		else
			return 0;
	}

	public synchronized void clearChannels() {
		String inputName = mCurrentOutput.getInput().getName();
		ChannelQueue queue = channelQueueMap.get(inputName);
		if (queue != null) {
			channelQueueMap.put(inputName, null);
			if (inputName.equals(ANALOG_TV)) {
				try {
					rawChSrv.analogDBClean(getNativeDBName());
				} catch (TVMException e) {
					e.printStackTrace();
				}
			} else if (inputName.equals(DIGITAL_TV)) {
				try {
					rawChSrv.digitalDBClean(getNativeDBName());
				} catch (TVMException e) {
					e.printStackTrace();
				}
			}

			List<ChannelInfo> rawList = null;
			try {
				// Raw channel Information store either in DB_ANALOG or
				// DB_AIR.
				rawList = rawChSrv.getChannelList(getNativeDBName());
			} catch (TVMException e) {
				e.printStackTrace();
			}

			if (rawList != null && rawList.size() == 0) {
				updateCompStatus(ComponentService.COMP_TTX_SYS_STATUS_EMPTY_SVL);
			}
		}
		TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
		sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
		notifyUpdate(UpdateReason.CLEAR_CHANNELS, "");
	}


	public synchronized void clearAllAirChannels() {
		TVLog("clear air queueMap and set");

		/* clean air part of the queue, leave cable part */
		synchronized (channelQueueMap) {
			channelQueueMap.put(DIGITAL_TV, channelSet.get(DIGITAL_TV + ConfigType.BS_SRC_CABLE));
			channelQueueMap.put(ANALOG_TV,	channelSet.get(ANALOG_TV  + ConfigType.BS_SRC_CABLE));
		}

		/* clean air set */
		synchronized (channelSet) {
			channelSet.put(DIGITAL_TV + ConfigType.BS_SRC_AIR, null);
			channelSet.put(ANALOG_TV  + ConfigType.BS_SRC_AIR, null);
		}

		/* clean air channel info and svl */
		try {
			TVLog("clear air svl");
			rawChSrv.analogDBClean(ChannelCommon.DB_AIR);
			rawChSrv.digitalDBClean(ChannelCommon.DB_AIR);
		} catch (TVMException e) {
			e.printStackTrace();
		}
		
		// check whether raw channel Info stored in DB_AIR is empty or not.
		List<ChannelInfo> rawList = null;
		try {
			rawList = rawChSrv.getChannelList(ChannelCommon.DB_AIR);
		} catch (TVMException e) {
			e.printStackTrace();
		}
		
		if (rawList != null && rawList.size() == 0) {
			updateCompStatus(ComponentService.COMP_TTX_SYS_STATUS_EMPTY_SVL);
		}

		// refresh
		TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
		sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
		notifyUpdate(UpdateReason.CLEAR_CHANNELS, "");
	}


	public synchronized void clearAllCableChannels() {
		TVLog("Clear cable svl, queueMap and set");

		/* clean cable part of the queue, leave air part */
		synchronized (channelQueueMap) {
			channelQueueMap.put(DIGITAL_TV, channelSet.get(DIGITAL_TV + ConfigType.BS_SRC_AIR));
			channelQueueMap.put(ANALOG_TV,	channelSet.get(ANALOG_TV  + ConfigType.BS_SRC_AIR));
		}

		/* clean air set */
		synchronized (channelSet) {
			channelSet.put(DIGITAL_TV + ConfigType.BS_SRC_CABLE, null);
			channelSet.put(ANALOG_TV  + ConfigType.BS_SRC_CABLE, null);
		}

		/* clean air channel info and svl */
		try {
			rawChSrv.analogDBClean(ChannelCommon.DB_CABEL);
			rawChSrv.digitalDBClean(ChannelCommon.DB_CABEL);
		} catch (TVMException e) {
			e.printStackTrace();
		}
		
		// check whether raw channel Info stored in DB_AIR is empty or not.
		List<ChannelInfo> rawList = null;
		try {
			rawList = rawChSrv.getChannelList(ChannelCommon.DB_CABEL);
		} catch (TVMException e) {
			e.printStackTrace();
		}
		
		if (rawList != null && rawList.size() == 0) {
			updateCompStatus(ComponentService.COMP_TTX_SYS_STATUS_EMPTY_SVL);
		}

		// refresh
		TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
		sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
		notifyUpdate(UpdateReason.CLEAR_CHANNELS, "");
	}


	public synchronized void clearAllChannels() {
		try {
			rawChSrv.analogDBClean(ChannelCommon.DB_AIR);
			rawChSrv.analogDBClean(ChannelCommon.DB_CABEL);
			rawChSrv.digitalDBClean(ChannelCommon.DB_AIR);
			rawChSrv.digitalDBClean(ChannelCommon.DB_CABEL);
		} catch (TVMException e) {
			e.printStackTrace();
		}
		
		TVLog("clear channelQueueMap and channelSet");
		channelQueueMap.clear();
		channelSet.clear();
				
		updateCompStatus(ComponentService.COMP_TTX_SYS_STATUS_EMPTY_SVL);

		TVLog("Update pipe. curOutput: " + mCurrentOutput.getName());
		sendMessageOnce(MSG_UPDATE_PIPE, mCurrentOutput);
		notifyUpdate(UpdateReason.CLEAR_CHANNELS, "");
	}

	public synchronized void flush() {
		try {
			rawChSrv.fsStoreChannelList(getNativeDBName());
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}


	public synchronized void flushByDBName(String dbName) {
		try {
			if (
					null != dbName 
					&& 
					(	
						dbName.equals(ChannelCommon.DB_AIR) 
						|| 
						dbName.equals(ChannelCommon.DB_CABEL)
					)
				) 
			{
				int ret = rawChSrv.fsStoreChannelList(dbName);
				TVLog("fsStoreChannelList(" + dbName + ") ret: " + ret);
			} 
			else 
			{
				TVLog("Notice: Unsupported DBName: " + dbName);
			}
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}
	

	@Override
	public synchronized void resetChannelAttribute(TVChannel ch, String str,
			int... para) throws RemoteException {
		int reason = para[0];
		List<ChannelInfo> list = new ArrayList<ChannelInfo>();
		String inputName = mCurrentOutput.getInput().getName();
		ChannelQueue queue = channelQueueMap.get(inputName);
		if (queue == null) {
			TVLog("return reason: queue == null");
			return;
		}

		TVLog("reset reason: " + reason + " @ " + ch.rawInfo.toString());
		switch (reason) {
		case UpdateReason.RESET_CH_NUM: {

			TVChannel curCH = queue.getChannel();
			/* for debug
			TVLog("curCH rawinfo: " + curCH.rawInfo.toString());
			List<TVChannel> tmplist = queue.getChannels();
			ListIterator<TVChannel> itr = tmplist.listIterator();
			TVLog("before change, queue chlist info : #########################");
			int idx = 0;
			while (itr.hasNext()) {
				TVChannel each = itr.next();
				TVLog(idx + ". " + each.rawInfo.toString());
				idx++;
			}
			TVLog("before change, queue chlist info : ######################### " + idx + " chs exist");
			*/
			
			boolean isRemoveOk = queue.remove(ch);
			TVLog("isRemoveOk: " + isRemoveOk);
			if (isRemoveOk) {
				int newNum = para[1];
				ch.rawInfo.setChannelNumber(newNum);
				ch.rawInfo.setChannelNumberEdited(true);
				if (ch.rawInfo.getBrdcstType() == TVCommon.BRDCST_TYPE_DTMB ||
					ch.rawInfo.getBrdcstType() == TVCommon.BRDCST_TYPE_DVB) {
					int orgChId = ch.rawInfo.getChannelId();
					int newChId = ch.createDtvChIdByNum(orgChId, newNum);
					ch.rawInfo.setChannelId(newChId);
					TVLog("[" + ch.getChannelName() + "] newNum: " + newNum + ", org/new ChId: " + orgChId + "/" + newChId);
				}
				queue.add(ch);
				queue.findChannel(curCH);
				/* for debug
				TVChannel tmpCh = queue.findChannel(curCH);
				if (null == tmpCh) {
					TVLog("null == tmpCh");
				} else {
					TVLog("tmpCh rawinfo: " + tmpCh.rawInfo.toString());
				}
				*/

				list.add(ch.rawInfo);
				notifyUpdate(reason, "", inputName.equals(ANALOG_TV) ? 0 : 1, ch.getChannelNum(), newNum);
			}

			/* for debug
			tmplist = queue.getChannels();
			itr = tmplist.listIterator();
			TVLog("after change, queue chlist info : #########################");
			idx = 0;
			while (itr.hasNext()) {
				TVChannel each = itr.next();
				TVLog(idx + ". " + each.rawInfo.toString());
				idx++;
			}
			TVLog("before change, queue chlist info : ######################### " + idx + " chs exist");
			*/
			break;
		}

		case UpdateReason.RESET_CH_NAME: {
			ch.rawInfo.setServiceName(str);
			ch.rawInfo.setChannelNameEdited(true);
			list.add(ch.rawInfo);
			notifyUpdate(reason, str, inputName.equals(ANALOG_TV) ? 0 : 1,
					ch.getChannelNum());
			break;
		}

		case UpdateReason.RESET_CH_MASK: {
			int mask = para[1];
			ByteArrayOutputStream bs = new ByteArrayOutputStream();
			DataOutputStream st = new DataOutputStream(bs);
			int nwMask;
			
			try {
				st.writeInt(mask);
				ch.rawInfo.setPrivateData(bs.toByteArray());
				nwMask = ch.rawInfo.getNwMask();
				if((mask & TVChannel.CH_MASK_BLOCKED) != 0){
					TVLog("Channel is blocked, set nwMask: " + nwMask + " --> " + (nwMask | (1 << 8)));
					nwMask |= (1 << 8);
					ch.rawInfo.setNwMask(nwMask);
				} else {
					TVLog("Channel is not blocked, unset nwMask: " + nwMask + " --> " + (nwMask & ~(1 << 8)));
					nwMask &= ~(1 << 8);
					ch.rawInfo.setNwMask(nwMask);
				}

				// whatever ch block is set true or false, tempUnblock should be clean to false, because it is the latest user operation
				ch.setTempUnblocked(false);
				
				// The mask maybe come from client.
				ch.setMask(mask);
				list.add(ch.rawInfo);
			} catch (IOException e) {
				e.printStackTrace();
			}

			notifyUpdate(reason, str, inputName.equals(ANALOG_TV) ? 0 : 1, ch.getChannelNum(), mask);
			break;
		}

		case UpdateReason.RESET_CH_FREQ: {
			int freq = para[1];
			if (ch.rawInfo instanceof AnalogChannelInfo) {
				((AnalogChannelInfo) ch.rawInfo).setFrequency(freq);
			} else if (ch.rawInfo instanceof DvbChannelInfo) {
				((DvbChannelInfo) ch.rawInfo).setFrequency(freq);
			}
			ch.rawInfo.setFrequencyEdited(true);
			list.add(ch.rawInfo);
			notifyUpdate(reason, str, inputName.equals(ANALOG_TV) ? 0 : 1,
					ch.getChannelNum(), freq);
			break;
		}

		default:
			break;
		}

		try {
			if (list.size() > 0) {
				rawChSrv.setChannelList(ChannelOperator.UPDATE, getNativeDBName(), list);
			}
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}

	
	@Override
	public int changeFocusTo(String outputName) throws RemoteException {
		int ret = 0;
		if (null == outputName) {
			TVLog("fail: null == outputName");
			return -98;
		} else {
			TVLog("TEST changeFocusTo: " + outputName);
		}
		
		try { 
			ret = inputSrv.focusChangeTo(outputName);
			TVLog("TEST ret: " + ret);
		} catch (TVMException e) {
			e.printStackTrace();
		}
		return ret;
	}

	
	public int setDtmbGUILang(String lang)
	{
		if (null == lang) {
			TVLog("return -1 reason: null paras");
			return -1;
		} else {
			TVLog("lang: \"" + lang + "\"");
		}

		if (lang.length() != 3) {
			TVLog("return -2 reason: wrong lang length: " + lang.length());
			return -2;
		}
		
		int ret = scanService.setDtmbGUILang(lang);
		TVLog("scanService.setDtmbGUILang ret: " + ret);

		if (0 == ret) {
			new Thread(new Runnable() {
				public void run() {
					TVLog("setDtmbGUILang success, update Queue ");
					update();
				}
			}).start();
		} 
		
		return ret;
	}

}

