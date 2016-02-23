package com.mediatek.tvcommon;

import java.util.Enumeration;
import java.util.Vector;

import com.mediatek.tv.TVManager;
import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.service.InputService;
import com.mediatek.tv.service.InputService.InputServiceListener;
import com.mediatek.tv.service.InputService.InputServiceListener.InputSignalStatus;
import com.mediatek.tv.service.ComponentService;
import com.mediatek.tv.service.CecComponentService;
import com.mediatek.tv.common.ConfigType;

import android.content.Context;
import android.os.Handler;
import android.os.Message;

public class TVInputCommon {

	private TVManager tvMngr = null;
	private InputService inpSrv = null;
	private Context mContext = null;
	private Handler mHandler = null;
	private TVOutputCommon mTVOutputCommon = null;	
	private TVConfigurer configurer;
	private ComponentService compSrv;
	private CecComponentService cecCompSrv;
	private TVManager tvMgr;
	
	private TVInputCommon(Context context) {
		mContext = context.getApplicationContext();
		tvMngr = TVManager.getInstance(mContext);
		inpSrv = (InputService) tvMngr.getService(InputService.InputServiceName);
		mHandler = new Handler(mContext.getMainLooper(),
								new Handler.Callback() {
									@Override
									public boolean handleMessage(Message msg) {
										// TODO Auto-generated method stub
										TVInputCommon.this.handleMessage(msg);
										return true;
									}
								});
		
		mTVOutputCommon = TVOutputCommon.getInstance(mContext);		
		tvMgr = TVManager.getInstance(mContext);
		configurer = TVConfigurer.getInstance(mContext);
		compSrv = (ComponentService) tvMgr.getService(ComponentService.CompServiceName);
		cecCompSrv = (CecComponentService) compSrv.getSpecificComponet(CecComponentService .CecComponentServiceName);
		
		if (inpSrv != null) {
			inpSrv.setInputListener(new InputListenerDelegate());
		} else {
			TVCommonNative.TVLog("TVInputCommon: get InputService failed!!!");
		}

		if (null == tvMgr || null == compSrv) {
			throw new IllegalAccessError("Fatal Error happens when initiliaze TVInputCommon");
		}
	}

	protected void handleMessage(Message msg) {
		// TODO Auto-generated method stub

	}

	private static TVInputCommon tvInputCommon = null;

	public static TVInputCommon getInstance(Context context) {
		if (tvInputCommon == null) {
			tvInputCommon = new TVInputCommon(context);
		}

		return tvInputCommon;
	}

	protected Vector<TVInputSourceListener> listeners = new Vector<TVInputSourceListener>();
	protected Vector<InputSourceEventListener> eventListeners = new Vector<InputSourceEventListener>();

	public static interface TVInputSourceListener {

		public void onOutputSignalChange(String output, boolean hasSignal);

		public void onInputSignalChange(String input, boolean hasSignal);

		public void onInputGotSignal(String input);
	}

	public static interface InputSourceEventListener {
		final static int STATE_UNKNOW = 0;
		final static int InputEventUnknown = 1;
		final static int InputEventNoSignal = 2;
		final static int InputEventWithSignal = 3;
		final static int InputEventVideoUpdate = 4;
		final static int InputEventVdpMuteAllFinished = 5;

		public void onOutputEventChange(String output, int eventType);
	}

	public synchronized void registerSourceListener(TVInputSourceListener listener) {
		listeners.add(listener);
	}

	public synchronized void removeSourceListener(TVInputSourceListener listener) {
		listeners.remove(listener);
	}

	public synchronized void registerSourceEventListener(
			InputSourceEventListener listener) {
		eventListeners.add(listener);
	}

	public synchronized void removeSourceEventListener(
			InputSourceEventListener listener) {
		eventListeners.remove(listener);
	}

	protected void notifyInputSignal(final String inputName,
			final boolean signal) {
		mHandler.post(new Runnable() {
			public void run() {
				Enumeration<TVInputSourceListener> e = listeners.elements();
				TVInputSourceListener item;
				while (e.hasMoreElements()) {
					item = e.nextElement();
					item.onInputSignalChange(inputName, signal);
				}
			}
		});
	}

	protected void notifyOutputSignalChange(final String outputName,
			final boolean hasSignal) {
		mHandler.post(new Runnable() {
			public void run() {
				// mTVOutputCommon.signal = hasSignal;
				mTVOutputCommon.setSignalState(outputName,hasSignal);
				Enumeration<TVInputSourceListener> e = listeners.elements();
				TVInputSourceListener item;
				while (e.hasMoreElements()) {
					item = e.nextElement();
					item.onOutputSignalChange(outputName, hasSignal);
				}
			}
		});
	}

	protected void notifyOutputEventChange(final String outputName, final int eventType) {
		mHandler.post(new Runnable() {
			public void run() {
				Enumeration<InputSourceEventListener> e = eventListeners.elements();
				InputSourceEventListener item;
				while (e.hasMoreElements()) {
					item = e.nextElement();
					item.onOutputEventChange(outputName, eventType);
				}
			}
		});
	}

	class InputListenerDelegate implements InputServiceListener {

		public void notifyInputGotSignal(String arg0) {
			// notifyInputSignal(arg0, true);
			TVCommonNative.TVLog(arg0);
		}

		public void notifyOutputOperatorDone(String arg0) {
			TVCommonNative.TVLog(arg0);
		}

		public void notifyOutputSignalSatus(String arg0, InputListenerSignalSatus arg1) {
			TVCommonNative.TVLog(arg0 + ": " + arg1);
		}

		public void notifyInputSignalStatus(String arg0, InputSignalStatus arg1) {
			if (arg1 == InputSignalStatus.SignalStatusLocked) {
				notifyInputSignal(arg0, true);
			} else {
				notifyInputSignal(arg0, false);
			}
			TVCommonNative.TVLog(arg0 + ": " + arg1);
		}

		public void notifyOutputSignalStatus(String arg0, InputSignalStatus arg1) {
			TVCommonNative.TVLog(arg0 + ": " + arg1 + ". [main/sub black mute] " + CrossRoad.isForceBlackMuteMain + "/" + CrossRoad.isForceBlackMuteSub);
			if (arg1 == InputSignalStatus.SignalStatusLocked) {
				notifyOutputSignalChange(arg0, true);
			} else {
				notifyOutputSignalChange(arg0, false);
				//  signal=false could be trust to resume mute color 
				resumeNonTvMuteColor(arg0);
				updateCecCompStatus(CecComponentService.CEC_COMP_SET_STATUS_NO_SIGNAL);
			}
		}

		public void notifyOutputEvent(String arg0, InputListenerEventNotify arg1) {
			TVCommonNative.TVLog(arg0 + ": " + arg1);
			if (arg1 == InputListenerEventNotify.InputEventUnknown) {
				notifyOutputEventChange(arg0, InputSourceEventListener.InputEventUnknown);
			} else if (arg1 == InputListenerEventNotify.InputEventNoSignal) {
				notifyOutputEventChange(arg0, InputSourceEventListener.InputEventNoSignal);
			} else if (arg1 == InputListenerEventNotify.InputEventWithSignal) {
				notifyOutputEventChange(arg0, InputSourceEventListener.InputEventWithSignal);
			} else if (arg1 == InputListenerEventNotify.InputEventVideoUpdate) {
				notifyOutputEventChange(arg0, InputSourceEventListener.InputEventVideoUpdate);
			} else if (arg1 == InputListenerEventNotify.InputEventVdpMuteAllFinished) {
				// now don't need notify to ap layer because no ap is listening to this one, which is only used to resume muteColor by tvcommmon 
				// notifyOutputEventChange(arg0, InputSourceEventListener.InputEventVdpMuteAllFinished);

				// when non-TV source has signal, vdp notify mute_all_finished to ap, which could be trust
				resumeNonTvMuteColor(arg0);
				updateCecCompStatus(CecComponentService.CEC_COMP_SYS_STATUS_AFTER_SOURCE_CHANGE);
			}
		}
	}

	public void stopDesignateOutput(String outPutName, boolean needSync) {
		TVCommonNative.TVLog("stopOutput(" + outPutName + ", " + needSync + ")");
		try {
			if (inpSrv != null) {
				inpSrv.stopDesignateOutput(outPutName, needSync);
			}
		} catch (TVMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public static final int OUTPUT_MODE_PIP = 1;
	public static final int OUTPUT_MODE_POP = 2;
	public static final int OUTPUT_MODE_NORMAL = 0;

	public static final int INPUT_STATE_UNKNOWN = 0;
	public static final int INPUT_STATE_NORMAL  = 1;
	public static final int INPUT_STATE_PIP 	= 2;
	public static final int INPUT_STATE_POP 	= 3;

	public int getOuputMode() {
		int rawStateValue = inpSrv.getCurrentStateValue();
		switch (rawStateValue) {
		case INPUT_STATE_PIP:
			return OUTPUT_MODE_PIP;
		case INPUT_STATE_POP:
			return OUTPUT_MODE_POP;
		case INPUT_STATE_NORMAL:
			return OUTPUT_MODE_NORMAL;
		default:
			return OUTPUT_MODE_NORMAL;
		}
	}


	private void resumeNonTvMuteColor(String arg0) {
		String winStringTvIn = TVCommonService.getWinStringTvIn();
		TVCommonNative.TVLog("TV(atv/dtv) is bound to " + winStringTvIn + ". [main/sub black mute] " + CrossRoad.isForceBlackMuteMain + "/" + CrossRoad.isForceBlackMuteSub);
		if (!(arg0.equals(winStringTvIn))) {
			// TV is NOT in this "arg0" output, which means "arg0" output connect non-TV(atv/dtv) source.
			// In this case, notifyOutputSignalStatus could be trust. It works with "SignalStatusLocked" and "SignalStatusLoss"
			// By the way, TV(atv/dtv) should trust TVChannelState notify. Or blue mute resume will be earlier than wanted.
		
			// in one word: current updating input is non-TV
			if (CrossRoad.isForceBlackMuteMain && arg0.equals(InputService.INPUT_OUTPUT_MAIN)) {
				// just set once
				CrossRoad.isForceBlackMuteMain = false;
				new Thread(new Runnable() {
					public void run() {
						int ret = TVMethods.setVideoMuteById(TVCommonService.TV_WIN_ID_MAIN, true, 0, 0, 255);
						TVCommonNative.TVLog("setVideoMuteById(main(0), true, 0, 0, 255) ret " + ret);
					}
				}).start();
			} else if (CrossRoad.isForceBlackMuteSub && arg0.equals(InputService.INPUT_OUTPUT_SUB)) {
				// just set once
				CrossRoad.isForceBlackMuteSub  = false;
				new Thread(new Runnable() {
					public void run() {
						int ret = TVMethods.setVideoMuteById(TVCommonService.TV_WIN_ID_SUB, true, 0, 0, 255);
						TVCommonNative.TVLog("setVideoMuteById(sub(1), true, 0, 0, 255) ret " + ret);
					}
				}).start();
			}
		}
	}

	
	// update status for cecCompService
	private void updateCecCompStatus (final String status) {	
	    TVOptionRange<Integer> opt = (TVOptionRange<Integer>) configurer.getOption(ConfigType.CEC_CEC_FUNC);		
		if (null != opt && opt.get() == ConfigType.CEC_FUNC_ON) {
			new Thread(new Runnable() {
				public void run() {					
					try {
						int ret = cecCompSrv.updateSysStatus(status);
						TVCommonNative.TVLog("update status: "+ status + " ret: " + ret);
					} catch (TVMException e) {
						e.printStackTrace();
					}
				}
			}).start();
		}
	}
	
}

