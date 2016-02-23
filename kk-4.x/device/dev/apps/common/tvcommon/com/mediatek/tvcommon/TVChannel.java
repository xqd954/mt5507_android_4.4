package com.mediatek.tvcommon;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import android.util.Log;
import android.os.RemoteException;

import com.mediatek.tv.TVManager;
import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.model.AnalogChannelInfo;
import com.mediatek.tv.model.ChannelInfo;
import com.mediatek.tv.model.DvbChannelInfo;
import com.mediatek.tv.service.BroadcastService;
import com.mediatek.tv.service.ChannelService;
import com.mediatek.tv.service.ChannelService.ChannelOperator;

final public class TVChannel {

	static final int CH_MASK_STORED  = 1 << 0;
	static final int CH_MASK_SKIP 	 = 1 << 1;
	static final int CH_MASK_FAVR 	 = 1 << 2;
	static final int CH_MASK_BLOCKED = 1 << 3;
	private int mask;
	private boolean tempUnblocked = false;

	private ChTvSystemOption tvSysOption;
	private ChColorSystemOption tvColorOption;
	final ChannelInfo rawInfo;
	final private ChannelService rawSrv;

	private int orgFreq;

	TVChannel(ChannelInfo info) {
		rawSrv = (ChannelService) TVManager.getInstance().getService(
				ChannelService.ChannelServiceName);
		rawInfo = info;
		if (info instanceof AnalogChannelInfo) {
			tvSysOption = new ChTvSystemOption((AnalogChannelInfo) info);
			tvColorOption = new ChColorSystemOption((AnalogChannelInfo) info);
			orgFreq = ((AnalogChannelInfo) info).getFrequency();
		} else if (rawInfo instanceof DvbChannelInfo) {
			orgFreq = ((DvbChannelInfo) info).getFrequency();
		}

		loadPrivate();
	}

	public ChColorSystemOption getColorSystemOption() {
		return tvColorOption;
	}

	public int getOriginalFreq() {
		return this.orgFreq;
	}

	public void setFreq(int freq) {
		if (freq == getFreq()) {
			return;
		}
	    ITVCommon tvCommon = TVCommonNative.getDefault(null);
		try {
			if(tvCommon !=null)
			tvCommon.resetChannelAttribute(this, "",
					UpdateReason.RESET_CH_FREQ, freq);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	public ChTvSystemOption getTvSystemOption() {
		return tvSysOption;
	}

	public class ChTvSystemOption extends TvAudioSystemOption {
		private AnalogChannelInfo rawInfo = null;

		public ChTvSystemOption(AnalogChannelInfo rawInfo) {
			super();
			this.rawInfo = rawInfo;
		}

		// Don't return AUTO
		public Integer getMin() {
			return TV_BG;
		}

		@Override
		public Integer get() {

			int good = -1;
			int better = -1;
			Log.d("TVChannel", "rawInfo.getTvSys() = " + rawInfo.getTvSys() + " rawInfo.getAudioSys() = " + rawInfo.getAudioSys());
			for (int i = 0; i < tv_sys_tbl.length; i++) {
				// Work around: TV Manager will give a mask with first garbage
				// bit.

				if (tv_sys_tbl[i] == (rawInfo.getTvSys() & 0xFFFF)) {
					if (good < 0) {
						good = (i == 9 ? (i - 1) : i);
					}
					if (aud_sys_tbl[i] == rawInfo.getAudioSys()) {
						better = (i == 9 ? (i - 1) : i);
						break;
					}
				}
			}
			
			Log.d("TVChannel", " better = " + better + " good = " + good);
			if (better > 0) {
				return better;
			}

			if (good > 0) {
				return good;
			}

			return TvAudioSystemOption.TV_BG;
		}

		@Override
		public boolean set(Integer val) {
			int i = val.intValue();

			if (rawInfo != null) {
				rawInfo.setTvSys(tv_sys_tbl[i]);
				rawInfo.setAudioSys(aud_sys_tbl[i]);
				update();
				// TO-DO Need Flush
			}
			return true;
		}
	}

	public class ChColorSystemOption extends ColorSystemOption {
		private AnalogChannelInfo rawInfo = null;
		private int tvColorSys;

		public ChColorSystemOption(AnalogChannelInfo rawInfo) {
			super();
			this.rawInfo = rawInfo;
		}

		public ChColorSystemOption(int tvColorSys) {
			this.tvColorSys = tvColorSys;
		}

		@Override
		public Integer get() {
			if (rawInfo != null) {
				tvColorSys = rawInfo.getColorSys();
			}
			for (int i = 0; i < color_sys_tbl.length; i++) {
				if (color_sys_tbl[i] == tvColorSys) {
					return i;
				}
			}
			return ColorSystemOption.COLOR_SYS_PAL;
		}

		@Override
		public boolean set(Integer val) {

			int i = val.intValue();
			if (rawInfo != null) {
				rawInfo.setColorSys(color_sys_tbl[i]);
				update();
				// TO-DO Need Flush
			}
			return true;
		}
	}

	public synchronized boolean isRadio() {
		return rawInfo.isRadioService();
	}

	public synchronized boolean isAnalog() {
		return rawInfo.isAnalogService();
	}

	public synchronized boolean isDTV() {
		return rawInfo.isTvService();
	}

	public synchronized int getChannelNum() {
		return rawInfo.getChannelNumber();
	}

	public synchronized void setChannelNum(int num) {
		TVCommonNative.TVLog("ch num: orig " + rawInfo.getChannelNumber() + " --> dest " + num);
		if (num == rawInfo.getChannelNumber()) {
			return;
		}
		ITVCommon tvCommon = TVCommonNative.getDefault(null);
		try {
			if(tvCommon !=null)
			tvCommon.resetChannelAttribute(this, "", UpdateReason.RESET_CH_NUM, num);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	// for dtmb now. dvbc is also suitable.
	public synchronized int createDtvChIdByNum(int origChId, int modifiedChNum) {
		return (origChId & 0x3ffff) | ((modifiedChNum & 0x3fff) << 18);
	}

	public synchronized String getChannelName() {
		String name = rawInfo.getServiceName();
		if (name == null) {
			name = "";
		}
		return name;
	}

	public synchronized void setChannelName(String name) {
		if (name.equals(rawInfo.getServiceName())) {
			return;
		}
		ITVCommon tvCommon = TVCommonNative.getDefault(null);
		try {
			if(tvCommon !=null)
			tvCommon.resetChannelAttribute(this, name,
					UpdateReason.RESET_CH_NAME);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	public synchronized int getFreq() {
		// WTF...
		if (rawInfo instanceof AnalogChannelInfo) {
			return ((AnalogChannelInfo) rawInfo).getFrequency();
		} else if (rawInfo instanceof DvbChannelInfo) {
			return ((DvbChannelInfo) rawInfo).getFrequency();
		}
		return 0;
	}

	public synchronized boolean isSkip() {
		return isMaskSet(CH_MASK_SKIP);
	}

	public synchronized boolean isFavorite() {
		return isMaskSet(CH_MASK_FAVR);
	}

	/**
	 * 
	 */
	public synchronized boolean isBlocked() {
		if (CrossRoad.isShowBlockDetails) {
			TVCommonNative.TVLog("ch physicalBlocked: " + isMaskSet(CH_MASK_BLOCKED) + ", tempUnblocked: " + tempUnblocked);
		}
		return (isMaskSet(CH_MASK_BLOCKED) && !tempUnblocked);
	}

	public synchronized boolean isPhysicalBlocked() {
		return isMaskSet(CH_MASK_BLOCKED);
	}

	/**
	 * @deprecated
	 */
	public synchronized void tempUnblocked() {
		tempUnblocked = true;
		if (isMaskSet(CH_MASK_BLOCKED)) {
			ITVCommon tvCommon = TVCommonNative.getDefault(null);
			try {
				if(null != tvCommon) {
					tvCommon.updateCurrentOutput();
				}
			} catch (RemoteException e) {
				e.printStackTrace();
			}
		}
	}

	void setTempUnblocked(boolean block) {
		tempUnblocked = block;
	}

	boolean isTempUnblocked() {
		return tempUnblocked;
	}

	public synchronized void setBlocked(boolean isBlock) {
		setMask(CH_MASK_BLOCKED, isBlock);
		ITVCommon tvCommon = TVCommonNative.getDefault(null);
		try {
			if(tvCommon !=null)
			tvCommon.updateCurrentOutput();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	public synchronized void setSkip(boolean skip) {
		setMask(CH_MASK_SKIP, skip);
	}

	public synchronized void setFavorite(boolean favr) {
		setMask(CH_MASK_FAVR, favr);
	}

	public  void setMask(int mask, boolean set) {
		if (set) {
			this.mask |= mask;
		} else {
			this.mask &= ~mask;
		}
		ITVCommon tvCommon = TVCommonNative.getDefault(null);
		try {
			if(tvCommon !=null)
			tvCommon.resetChannelAttribute(this, "",
					UpdateReason.RESET_CH_MASK, this.mask);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	public boolean isMaskSet(int mask) {
		if ((this.mask & mask) == 0) {
			return false;
		}
		return true;
	}

	void setMask(int mask) {
		this.mask = mask;
	}

	void loadPrivate() {

		byte[] storage = rawInfo.getPrivateData();

		DataInputStream st = new DataInputStream(new ByteArrayInputStream(
				storage));
		try {
			mask = st.readInt();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private void update() {
		if (rawSrv != null) {
			List<ChannelInfo> list = new ArrayList<ChannelInfo>();
			list.add(rawInfo);
			try {
				rawSrv.setChannelList(ChannelOperator.UPDATE,
						TVCommonNative.getNativeDBName(), list);
			} catch (TVMException e) {
				e.printStackTrace();
			}
		}
	}

	public void finetune(int step) throws IllegalAccessException {
		if (rawInfo instanceof AnalogChannelInfo) {
			BroadcastService brdcstSrv = (BroadcastService) TVManager
					.getInstance().getService(
							BroadcastService.BrdcstServiceName);
			AnalogChannelInfo aInfo = (AnalogChannelInfo) rawInfo;
			aInfo.setFrequency(aInfo.getFrequency() + step);
			rawInfo.setFrequencyEdited(true);
			try {
				if(brdcstSrv !=null)
				brdcstSrv.fineTune(rawInfo);
			} catch (TVMException e) {
				e.printStackTrace();
			}
		} else {
			throw new IllegalAccessException("Channel is not Analog.");
		}
	}

	public void exitFinetune() {
		update();
		BroadcastService brdcstSrv = (BroadcastService) TVManager.getInstance()
				.getService(BroadcastService.BrdcstServiceName);
		try {
			if(brdcstSrv !=null)
			brdcstSrv.exitFineTune(rawInfo);
		} catch (TVMException e) {
			e.printStackTrace();
		}
	}

	public void autoFinetune(boolean tune) throws IllegalAccessException {
		if (rawInfo instanceof AnalogChannelInfo) {
			AnalogChannelInfo aInfo = (AnalogChannelInfo) rawInfo;
			aInfo.setNoAutoFineTune(!tune);
			update();
		} else {
			throw new IllegalAccessException("Channel is not Analog.");
		}
	}

	public boolean isAutoFinetune() throws IllegalAccessException {
		if (rawInfo instanceof AnalogChannelInfo) {
			AnalogChannelInfo aInfo = (AnalogChannelInfo) rawInfo;
			return !aInfo.isNoAutoFineTune();
		} else {
			throw new IllegalAccessException("Channel is not Analog.");
		}
	}

}
