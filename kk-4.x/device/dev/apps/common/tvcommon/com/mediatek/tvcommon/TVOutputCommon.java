package com.mediatek.tvcommon;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;

import android.content.Context;
import android.graphics.Rect;
import android.graphics.RectF;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;

import com.mediatek.tv.TVManager;
import com.mediatek.tv.common.ChannelCommon;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.model.AudioInfo;
import com.mediatek.tv.model.AudioLanguageInfo;
import com.mediatek.tv.model.SignalLevelInfo;
import com.mediatek.tv.model.SubtitleInfo;
import com.mediatek.tv.service.BroadcastService;
import com.mediatek.tv.service.InputService;
import com.mediatek.tv.service.InputService.AutoAdjustType;

public class TVOutputCommon {

	public static final int AUTO_ADJUST = 0;
	public static final int AUTO_PHASE = 1;
	public static final int AUTO_COLOR = 2;

	private TVManager tvMngr = null;
	private InputService inpSrv = null;
	private BroadcastService brdcstSrv = null;
	private Context mContext = null;
	private Handler mHandler = null;
	private TVMethods methods = null;
	private TVConfigurer cfger = null;
	protected boolean mainSignal = false;
	protected boolean signal = false;
	protected boolean subSignal = false;
	
	private TVOutputCommon(Context context) {
		mContext = context.getApplicationContext();
		tvMngr = TVManager.getInstance(mContext);
		inpSrv = (InputService) tvMngr.getService(InputService.InputServiceName);
		brdcstSrv = (BroadcastService) tvMngr.getService(BroadcastService.BrdcstServiceName);
		methods = TVMethods.getInstance(mContext);
		cfger = TVConfigurer.getInstance(mContext);
		mHandler = new Handler(mContext.getMainLooper(),
								new Handler.Callback() {
									@Override
									public boolean handleMessage(Message msg) {
										// TODO Auto-generated method stub
										TVOutputCommon.this.handleMessage(msg);
										return true;
									}
								});
		if (null == tvMngr || null == inpSrv || null == brdcstSrv || null == methods || null == cfger || null == mHandler) {
			throw new IllegalAccessError("Fatal Error happens when init TVOutputCommon");
		}
	}

	protected void handleMessage(Message msg) {
		// TODO Auto-generated method stub
	}

	private static TVOutputCommon tvOutputCommon = null;

	public static TVOutputCommon getInstance(Context context) {
		if (tvOutputCommon == null) {
			tvOutputCommon = new TVOutputCommon(context);
		}

		return tvOutputCommon;
	}

	public static interface AdjustListener {
		public static final int EVENT_FINISHED = 0;
		public static final int EVENT_NOT_SUPPORT = 1;

		// ...
		public void onEvent(int event);
	}

	public void adjust(final String outputName, final AdjustListener listener,
			final int item) {
		new Thread(new Runnable() {
			public void run() {
				try {
					AutoAdjustType mAutoAdjustType = AutoAdjustType.AUTO_TYPE_COLOR;
					switch (item) {
					case AUTO_ADJUST:
						mAutoAdjustType = AutoAdjustType.AUTO_TYPE_VGA_ADJUST;
						break;
					case AUTO_PHASE:
						mAutoAdjustType = AutoAdjustType.AUTO_TYPE_PHASE;
						break;
					case AUTO_COLOR:
						mAutoAdjustType = AutoAdjustType.AUTO_TYPE_COLOR;
						break;
					default:
						mAutoAdjustType = AutoAdjustType.AUTO_TYPE_COLOR;
						break;
					}
					inpSrv.setAutoAdjust(outputName, mAutoAdjustType);
					// Work around:
					try {
						Thread.sleep(2000);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				} catch (TVMException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}

				mHandler.post(new Runnable() {
					public void run() {
						if (listener != null) {
							listener.onEvent(AdjustListener.EVENT_FINISHED);
						}
					}
				});
			}
		}).start();
	}

	public boolean aspectRatioCanSet(String outputName) {
		return inpSrv.getAspectRatioEnable(outputName);
	}

	public int getMHLPortNum(String outputName) {
		int mhlPortNum = -1;
		try {
			mhlPortNum = inpSrv.getMHLPortNum(outputName);
			return mhlPortNum;
		} catch (TVMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return mhlPortNum;
	}

	private VideoResolution inst = null;

	public VideoResolution getInstanceVideoResolution(String output) {
		if (inst == null) {
			inst = new VideoResolution(output);
		}
		inst.outputName = output;
		return inst;
	}

	public static enum TimingType {
		TIMING_TYPE_UNKNOWN, TIMING_TYPE_VIDEO, TIMING_TYPE_GRAPHIC, TIMING_TYPE_NOT_SUPPORT
	}

	public class VideoResolution {

		String outputName;

		VideoResolution(String outputName) {
			this.outputName = outputName;
		}

		public boolean isVideoProgressive() {
			try {
				InputService.VideoResolution res = inpSrv
						.getVideoResolution(outputName);
				return res.isProgressive;
			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return false;
		}

		public Rect getVideoResolution() {
			try {
				InputService.VideoResolution res = inpSrv
						.getVideoResolution(outputName);
				return new Rect(0, 0, res.width, res.height);
			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return null;
		}

		public String getVideoFormat() {
			try {
				InputService.VideoResolution res = inpSrv
						.getVideoResolution(outputName);
				return res.sdHdString;
			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return null;
		}

		public int getVideoFramerate() {
			try {
				InputService.VideoResolution res = inpSrv
						.getVideoResolution(outputName);
				return res.frameRate;
			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return 0;
		}

		public TimingType getTimingType() {
			try {
				InputService.VideoResolution res = inpSrv
						.getVideoResolution(outputName);
				return TimingType.values()[res.timingType.ordinal()];
			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return TimingType.values()[0];
		}
	}

	public class MTSOption extends TVOptionRange<Integer> {
		public static final int MTS_UNKNOWN = 0;
		public static final int MTS_MONO = 1;
		public static final int MTS_STEREO = 2;
		public static final int MTS_SUB_LANG = 3;
		public static final int MTS_DUAL1 = 4;
		public static final int MTS_DUAL2 = 5;
		public static final int MTS_NICAM_MONO = 6;
		public static final int MTS_NICAM_STEREO = 7;
		public static final int MTS_NICAM_DUAL1 = 8;
		public static final int MTS_NICAM_DUAL2 = 9;
		public static final int MTS_FM_MONO = 10;
		public static final int MTS_FM_STEREO = 11;
		public static final int MTS_END = 11;

		final private String outputName;
		BroadcastService srv;

		MTSOption(String outputName) {
			this.outputName = outputName;
		}

		private List<Integer> arrayCanSet;

		private void updateCanset(int af) {
			arrayCanSet = new ArrayList<Integer>();
			switch (af) {
			case BroadcastService.SVCTX_AUD_ALTERNATIVE__CHANNELS_FM_MONO_NICAM_MONO:
				arrayCanSet.add(BroadcastService.SVCTX_AUD_MTS_MONO);
				arrayCanSet.add(BroadcastService.SVCTX_AUD_MTS_NICAM_MONO);
				break;
			case BroadcastService.SVCTX_AUD_ALTERNATIVE__CHANNELS_FM_MONO_NICAM_STEREO:
				arrayCanSet.add(BroadcastService.SVCTX_AUD_MTS_MONO);
				arrayCanSet.add(BroadcastService.SVCTX_AUD_MTS_NICAM_STEREO);
				break;
			case BroadcastService.SVCTX_AUD_ALTERNATIVE__CHANNELS_FM_MONO_NICAM_DUAL:
				arrayCanSet.add(BroadcastService.SVCTX_AUD_MTS_MONO);
				arrayCanSet.add(BroadcastService.SVCTX_AUD_MTS_NICAM_DUAL1);
				arrayCanSet.add(BroadcastService.SVCTX_AUD_MTS_NICAM_DUAL2);
				break;
			case BroadcastService.SVCTX_AUD_ALTERNATIVE_CHANNELS_MONO:
				arrayCanSet.add(BroadcastService.SVCTX_AUD_MTS_MONO);
				break;
			case BroadcastService.SVCTX_AUD_ALTERNATIVE__CHANNELS_DUAL_MONO:
				arrayCanSet.add(BroadcastService.SVCTX_AUD_MTS_DUAL1);
				arrayCanSet.add(BroadcastService.SVCTX_AUD_MTS_DUAL2);
				break;
			case BroadcastService.SVCTX_AUD_ALTERNATIVE__CHANNELS_MONO_SUB:
				arrayCanSet.add(BroadcastService.SVCTX_AUD_MTS_MONO);
				arrayCanSet.add(BroadcastService.SVCTX_AUD_MTS_SUB_LANG);
				break;
			case BroadcastService.SVCTX_AUD_ALTERNATIVE__CHANNELS_STEREO:
				arrayCanSet.add(BroadcastService.SVCTX_AUD_MTS_MONO);
				arrayCanSet.add(BroadcastService.SVCTX_AUD_MTS_STEREO);
				break;
			case BroadcastService.SVCTX_AUD_ALTERNATIVE__CHANNELS_STEREO_SUB:
				arrayCanSet.add(BroadcastService.SVCTX_AUD_MTS_MONO);
				arrayCanSet.add(BroadcastService.SVCTX_AUD_MTS_STEREO);
				arrayCanSet.add(BroadcastService.SVCTX_AUD_MTS_SUB_LANG);
				break;
			default:
				break;
			}
		}

		public boolean canSet(int val) {

			// // TODO Auto-generated method stub
			try {
				srv = (BroadcastService) tvMngr
						.getService(BroadcastService.BrdcstServiceName);
				if (srv != null) {
					AudioInfo ai = srv.getAudioInfo(outputName);
					int af = ai.getAlternativeAudio();
					updateCanset(af);
				} else {
					TVCommonNative.TVLog("TVOutputCommon: get BroadcastService failed!!!");
					return false;
				}
			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			if (arrayCanSet == null || arrayCanSet.size() <= 0) {
				return false;
			} else {
				return arrayCanSet.contains(val);
			}
		}

		public Integer getMax() {
			return MTS_END;

		}

		@Override
		public Integer getMin() {
			return MTS_UNKNOWN;
		}

		@Override
		public Integer get() {
			// TODO Auto-generated method stub
			srv = (BroadcastService) tvMngr
					.getService(BroadcastService.BrdcstServiceName);
			try {
				if (srv != null) {
					AudioInfo ai = srv.getAudioInfo(outputName);
					if (canSet(ai.getMts())) {
						return ai.getMts();
					} else {
						return MTS_MONO;
					}
				} else {
					TVCommonNative.TVLog("TVOutputCommon: get BroadcastService failed!!!");
				}
			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return MTS_MONO;
		}

		public int getMtsIndex(int curMts) {
			srv = (BroadcastService) tvMngr
					.getService(BroadcastService.BrdcstServiceName);
			try {
				if (srv != null) {
					AudioInfo ai = srv.getAudioInfo(outputName);
					updateCanset(ai.getAlternativeAudio());
					if (arrayCanSet == null || arrayCanSet.size() <= 0) {
						return -1;
					} else {
						return arrayCanSet.indexOf(curMts);
					}
				} else {
					TVCommonNative.TVLog("TVOutputCommon: get BroadcastService failed!!!");
				}
			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return -1;
		}

		@Override
		public boolean set(Integer val) {
			// TODO Auto-generated method stub
			try {
				srv = (BroadcastService) tvMngr
						.getService(BroadcastService.BrdcstServiceName);
				if (srv != null) {
					srv.setAudioInfo(outputName, val);
				} else {
					TVCommonNative.TVLog("TVOutputCommon: get BroadcastService failed!!!");
				}
			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return false;
		}

		public int getTotalNum() {
			try {
				srv = (BroadcastService) tvMngr
						.getService(BroadcastService.BrdcstServiceName);
				if (srv != null) {
					AudioInfo ai = srv.getAudioInfo(outputName);
					int af = ai.getAlternativeAudio();
					updateCanset(af);
					switch (af) {
					case BroadcastService.SVCTX_AUD_ALTERNATIVE__CHANNELS_FM_MONO_NICAM_MONO:
						return 2;
					case BroadcastService.SVCTX_AUD_ALTERNATIVE__CHANNELS_FM_MONO_NICAM_STEREO:
						return 2;
					case BroadcastService.SVCTX_AUD_ALTERNATIVE__CHANNELS_FM_MONO_NICAM_DUAL:
						return 3;
					case BroadcastService.SVCTX_AUD_ALTERNATIVE_CHANNELS_MONO:
						return 1;
					case BroadcastService.SVCTX_AUD_ALTERNATIVE__CHANNELS_DUAL_MONO:
						return 2;
					case BroadcastService.SVCTX_AUD_ALTERNATIVE__CHANNELS_MONO_SUB:
						return 2;
					case BroadcastService.SVCTX_AUD_ALTERNATIVE__CHANNELS_STEREO:
						return 2;
					case BroadcastService.SVCTX_AUD_ALTERNATIVE__CHANNELS_STEREO_SUB:
						return 3;
					default:
						return 0;
					}
				} else {
					TVCommonNative.TVLog("TVOutputCommon: get BroadcastService failed!!!");
				}

			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return 0;
		}

	}

	public MTSOption getMTSOption(String outputName) {
		return new MTSOption(outputName);
	}

	public void stop(String outputName) {
		if (null == outputName) {
			TVCommonNative.TVLog("return reason: null == outputName");
			return;
		}

		int ret = -100;
		int winId = TVCommonService.TV_WIN_ID_ALL;
		TVOption<Integer> blueOpt = (TVOption<Integer>) cfger.getOption(ConfigType.CFG_BLUE_SCREEN);
		if (null != blueOpt && blueOpt.get() == ConfigType.COMMON_ON) {
			if (outputName.equals(InputService.INPUT_OUTPUT_MAIN)) {
				winId = TVCommonService.TV_WIN_ID_MAIN;
			} else if (outputName.equals(InputService.INPUT_OUTPUT_SUB)) {
				winId = TVCommonService.TV_WIN_ID_SUB;
			} 
			switch(winId) {
				case TVCommonService.TV_WIN_ID_MAIN:
					CrossRoad.isForceBlackMuteMain = true;
					break;
				case TVCommonService.TV_WIN_ID_SUB:
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
			if (null != inpSrv) {
				ret = inpSrv.stopDesignateOutput(outputName, true);
				TVCommonNative.TVLog("stopDesignateOutput(" + outputName + ", true) ret " + ret);
			}
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}

	public void stopEx(String outputName, boolean isForce) {
		if (null == outputName) {
			TVCommonNative.TVLog("return reason: null == outputName");
			return;
		}

		int ret = -100;
		int winId = TVCommonService.TV_WIN_ID_ALL;
		TVOption<Integer> blueOpt = (TVOption<Integer>) cfger.getOption(ConfigType.CFG_BLUE_SCREEN);
		if (null != blueOpt && blueOpt.get() == ConfigType.COMMON_ON) {
			if (outputName.equals(InputService.INPUT_OUTPUT_MAIN)) {
				winId = TVCommonService.TV_WIN_ID_MAIN;
			} else if (outputName.equals(InputService.INPUT_OUTPUT_SUB)) {
				winId = TVCommonService.TV_WIN_ID_SUB;
			} 
			switch(winId) {
				case TVCommonService.TV_WIN_ID_MAIN:
					CrossRoad.isForceBlackMuteMain = true;
					break;
				case TVCommonService.TV_WIN_ID_SUB:
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
			if(null != inpSrv) {
				ret = inpSrv.stopDesignateOutputEx(outputName, true, isForce);
				TVCommonNative.TVLog("stopDesignateOutputEx(" + outputName + ", true, " + isForce + ") ret " + ret);
		 	}
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}

	public boolean setScreenRectangle(String outputName, RectF rect) {
		Rect r = new Rect((int) (rect.left * 10000.0f),
				(int) (rect.top * 10000.0f), (int) (rect.right * 10000.0f),
				(int) (rect.bottom * 10000.0f));
		inpSrv.setScreenOutputRect(outputName, r);
		return true;
	}

	public RectF getScreenRectangle(String outputName) {
		if (inpSrv != null) {
			Rect rect = inpSrv.getScreenOutputRect(outputName);
			if(rect != null)
				return new RectF((float) rect.left / 10000.0f,
					(float) rect.top / 10000.0f, (float) rect.right / 10000.0f,
					(float) rect.bottom / 10000.0f);
			else 
				return null;
		} else {
			TVCommonNative.TVLog("TVOutputCommon: get InputService failed!!!");
			return null;
		}
	}

	public boolean setSrcRectangle(String outputName, RectF rect) {
		Rect r = new Rect((int) (rect.left * 10000.0f),
				(int) (rect.top * 10000.0f), (int) (rect.right * 10000.0f),
				(int) (rect.bottom * 10000.0f));
		if (inpSrv != null) {
			inpSrv.setScreenOutputVideoRect(outputName, r);
			return true;
		} else {
			TVCommonNative.TVLog("TVOutputCommon: get InputService failed!!!");
			return false;
		}
	}

	public RectF getSrcRectangle(String outputName) {
		if (inpSrv != null) {
			Rect rect = inpSrv.getScreenOutputVideoRect(outputName);
			if(rect != null)
				return new RectF((float) rect.left / 10000.0f,
					(float) rect.top / 10000.0f, (float) rect.right / 10000.0f,
					(float) rect.bottom / 10000.0f);
			else
				return null;
		} else {
			TVCommonNative.TVLog("TVOutputCommon: get InputService failed!!!");
			return null;
		}
	}

	public boolean isFreeze(String outputName) {
		boolean freeze = false;

		BroadcastService brdSrv = null;
		brdSrv = (BroadcastService) tvMngr
				.getService(BroadcastService.BrdcstServiceName);

		if (brdSrv != null) {
			try {
				freeze = brdSrv.isFreeze(outputName);
			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}

		return freeze;
	}

	public interface AudioUpdated {
		public void onUpdate();
	}

	private List<AudioUpdated> list = new ArrayList<AudioUpdated>();

	public void registerAudioUpdatedListener(AudioUpdated update) {
		if (update == null) {
			throw new IllegalArgumentException();
		}
		synchronized (this) {
			if (!list.contains(update)) {
				list.add(update);
			}
		}
	}

	public void removeAudioUpdatedListener(AudioUpdated update) {
		list.remove(update);
	}

	public void clearAudioUpdatedListener(AudioUpdated update) {
		list.clear();
	}

	public void setFreeze(String outputName, boolean isFreeze) {
		BroadcastService brdSrv = null;
		brdSrv = (BroadcastService) tvMngr
				.getService(BroadcastService.BrdcstServiceName);
		if (brdSrv != null) {
			try {
				brdSrv.setFreeze(outputName, isFreeze);
			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}
	/**
	* get instance of AudioLanguageInfo
	*/
	public AudioLanguageInfo getAudioLanguageInfoInstance() {
		BroadcastService srv = (BroadcastService) tvMngr.getService(
				BroadcastService.BrdcstServiceName);
		try {
				return srv.getDtvAudioLangInfo();
		}catch (TVMException e) {
			e.printStackTrace();
		}
		return null;
	}

	public Hashtable<Integer, String> getCurAudLang() {
		Hashtable<Integer, String> table = new Hashtable<Integer, String>();
		BroadcastService srv = (BroadcastService) tvMngr
				.getService(BroadcastService.BrdcstServiceName);
		AudioLanguageInfo ai;
		try {
			if (srv != null) {
				ai = srv.getDtvAudioLangInfo();
				table.put(new Integer(ai.getCurrentAudioLangIndex()),
						ai.getCurrentLanguage());
				return table;
			} else {
				TVCommonNative.TVLog("TVOutputCommon: get BroadcastService failed!!!");
			}
		} catch (TVMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;
	}

	public Hashtable<Integer, String> getAudLangTable() {
		Hashtable<Integer, String> table = new Hashtable<Integer, String>();
		BroadcastService srv = (BroadcastService) tvMngr
				.getService(BroadcastService.BrdcstServiceName);
		String str = null;
		try {
			if (srv != null) {
				str = srv.getDtvAudioLangInfo().getAudioLanguage();
			} else {
				TVCommonNative.TVLog("TVOutputCommon: get BroadcastService failed!!!");
				return null;
			}
		} catch (TVMException e) {
			e.printStackTrace();
		}
		if (str == null || str.length() == 0 || str.length() % 3 != 0) {
			return null;
		}
		for (int i = 0; i <= str.length() - 3; i += 3) {
			table.put(new Integer(i / 3 + 1), str.substring(i, i + 3));
		}
		return table;
	}

	public void setAudLangByIndex(int audIndex) {
		BroadcastService srv = (BroadcastService) tvMngr
				.getService(BroadcastService.BrdcstServiceName);

		try {
			if (srv != null) {
				srv.setDtvAudioLangByIndex(audIndex);
			} else {
				TVCommonNative.TVLog("TVOutputCommon: get BroadcastService failed!!!");
			}
		} catch (TVMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public class ColorSystemOption extends TVOptionRange<Integer> {
		public static final int COLOR_SYS_AUTO = 0;
		public static final int COLOR_SYS_PAL = 1;
		public static final int COLOR_SYS_SECAM = 2;
		public static final int COLOR_SYS_NTSC = 3;
		public static final int COLOR_SYS_END = COLOR_SYS_NTSC;

		final int color_sys_tbl[] = { ChannelCommon.COLOR_SYS_UNKNOWN,
				ChannelCommon.COLOR_SYS_PAL, ChannelCommon.COLOR_SYS_SECAM,
				ChannelCommon.COLOR_SYS_NTSC };

		@Override
		public Integer getMax() {
			return COLOR_SYS_END;
		}

		@Override
		public Integer getMin() {
			return COLOR_SYS_AUTO;
		}

		private int val = COLOR_SYS_AUTO;

		@Override
		public Integer get() {
			return val;
		}

		@Override
		public boolean set(Integer val) {
			this.val = val.intValue();
			return true;
		}

		int getRawColorSystem() {
			return color_sys_tbl[val];
		}

		int getRawEnable() {
			// This is used for arg0, :)
			if (val == COLOR_SYS_AUTO)
				return 0;
			else
				return 1;
		}
	}

	public class OutputColorSystemOption extends ColorSystemOption {
		String outputName;

		public OutputColorSystemOption(String outputName) {
			super();
			this.outputName = outputName;
		}

		public Integer get() {
			int cs;
			try {
				cs = inpSrv.getColorSystem(outputName);
				for (int i = 0; i < color_sys_tbl.length; i++) {
					if (color_sys_tbl[i] == cs) {
						return i;
					}
				}

			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return OutputColorSystemOption.COLOR_SYS_AUTO;
		}

		@Override
		public boolean set(Integer val) {
			// Throw exception here?
			return true;
		}
	}

	public void setScreenPosition(String output, float x, float y, float w,
			float h) {
		Rect r = new Rect((int) (x * 10000.0f), (int) (y * 10000.0f),
				(int) (w * 10000.0f), (int) (h * 10000.0f));
		inpSrv.setScreenOutputRect(output, r);
	}

	public void setMjcBypassWindow(int windowId, Rect r) {
		try {
			inpSrv.setMjcBypassWindow(windowId, r);
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}

	public String getSubtitleLang() {
		SubtitleInfo si;
		try {
			si = brdcstSrv.getSubtitleInfo();
			return si.getSubtitleLang();
		} catch (TVMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return "";
	}

	public String getCurSubtitleLang() {
		SubtitleInfo si;
		try {
			si = brdcstSrv.getSubtitleInfo();
			return si.getCurrentSubtitleLang();
		} catch (TVMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return "";
	}

	public void setSubtitleLang(String lang) {
		try {
			brdcstSrv.setSubtitleLang(lang);

		} catch (TVMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void syncStopSubtitleStream() {
		try {
			brdcstSrv.syncStopSubtitleStream();

		} catch (TVMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	private String currentOutput = "main";

	public String getDefaultOutput() {
		return currentOutput;
	}

	public void setDefaultOutput(String name) {
		currentOutput = name;
	}

	public static final int OUTPUT_MODE_PIP = 1;
	public static final int OUTPUT_MODE_POP = 2;
	public static final int OUTPUT_MODE_NORMAL = 0;

	public String enterOutputMode(int outputMode) {
		String outputName = "main";
		TVCommonNative.TVLog("Try to enter output mode " + outputMode);
		int mode = getOuputMode();
		try {
			if (inpSrv != null) {
				if (mode != outputMode) {
					switch (outputMode) {
					case OUTPUT_MODE_PIP:
						outputName = inpSrv.enterPIPAndReturnCurrentFocusOutput();
						outputName = "sub";
						break;
					case OUTPUT_MODE_POP:
						outputName = inpSrv.enterPOPAndReturnCurrentFocusOutput();
						break;
					case OUTPUT_MODE_NORMAL:
						outputName = inpSrv.enterNormalAndReturnCurrentOutput();
						outputName = "main";
						break;
					default:
						throw new IllegalArgumentException();
					}
					
					if(outputName != null) {
						if (outputName.equals("main") || outputName.equals("sub")) {
							this.currentOutput = outputName;
							TVCommonNative.TVLog("Success to enter output mode "
								+ outputMode);
						} else {
							TVCommonNative.TVLog("TVManager returns bad output "
								+ outputName);
						}
					}	
				}
		} else {
			TVCommonNative.TVLog("TVOutputCommon: get InputService failed!!!");
		}

		} catch (TVMException e) {
			e.printStackTrace();
		}

		return currentOutput;
	}

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

	public int getSignalLevel(String outputName) {
		if (brdcstSrv != null) {
			try {
				SignalLevelInfo sigInfo = brdcstSrv
						.getSignalLevelInfo(outputName);
				return sigInfo.getSignalLevel();
			} catch (TVMException e) {
				e.printStackTrace();
			}
		}
		return 0;
	}

	public int getSignalBer(String outputName) {
		if (brdcstSrv != null) {
			try {
				SignalLevelInfo sigInfo = brdcstSrv
						.getSignalLevelInfo(outputName);
				return sigInfo.getBer();
			} catch (TVMException e) {
				e.printStackTrace();
			}
		}
		return 0;
	}

	public boolean isEnableFreeze(String outputName) {
		boolean enableFreeze = false;
		if (brdcstSrv != null) {
			try {
				enableFreeze = brdcstSrv.enableFreeze(outputName);
			} catch (TVMException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		return enableFreeze;
	}
	
	public String getInput(String outputName) {
		try {
			ITVCommon iTVCommon = TVCommonNative.getDefault(mContext);
			if (iTVCommon != null) {
				return iTVCommon.getInputSource(outputName);
			} else {
				TVCommonNative.TVLog("TVOutputCommon: get ITVCommon failed!!!");
			}
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return "atv";
	}
	
	public void connect(String outputName, String inputName) {
		try {
			ITVCommon iTVCommon = TVCommonNative.getDefault(mContext);
			if (iTVCommon != null) {				
				iTVCommon.changeInputSource(outputName, inputName);
			} else {
				TVCommonNative.TVLog("TVOutputCommon: get ITVCommon failed!!!");
			}
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public boolean hasSignal(String outputName) {
		 return outputName == "main" ? mainSignal : subSignal;
	}
	public void setSignalState(String outputName, boolean signal) {
        if (outputName == "main") {
            mainSignal = signal;
        } else {
            subSignal = signal;
        }
    }
}
