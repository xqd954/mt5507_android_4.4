package com.mediatek.tvcommon;

import java.sql.Date;
import java.util.List;
import com.mediatek.tvcommon.ITVCommon.ChannelFilter;
import com.mediatek.tvcommon.ITVCommon.ChannelsChangedListener;
import com.mediatek.tvcommon.ITVCommon.TVSelectorListener;

import android.content.Context;
import android.os.RemoteException;

import com.mediatek.tv.common.ConfigType;

/**
 * TVCommonManager is simple wrapper of {@link ITVCommon}. APPs can access this
 * methods to operate TV system.
 */
public class TVCommonManager {
	private Context mContext;
	private static TVCommonManager tvMgr;

	public static final String ATV = "atv";
	public static final String DTV = "dtv";

	private TVCommonManager(Context context) {
		mContext = context;
	}

	/**
	 * @return the singleton instance of {@link TVCommonManager}.
	 */
	public static TVCommonManager getInstance(Context context) {
		if (tvMgr == null) {
			tvMgr = new TVCommonManager(context);
		}
		return tvMgr;
	}

	/**
	 * @see {@link ITVCommon#select(int)}
	 */
	public boolean select(int num) {
		try {
			return TVCommonNative.getDefault(mContext).select(num);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return false;
	}

	/**
	 * @see {@link ITVCommon#select(TVChannel)}
	 */
	public boolean select(TVChannel ch) {
		try {
			return TVCommonNative.getDefault(mContext).select(ch);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return false;
	}

	/**
	 * @see {@link ITVCommon#selectPrev()}
	 */
	public void selectPrev() {
		try {
			TVCommonNative.getDefault(mContext).selectPrev();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#selectUp()}
	 */
	public void selectUp() {
		try {
			TVCommonNative.getDefault(mContext).selectUp();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#selectDown()}
	 */
	public void selectDown() {
		try {
			TVCommonNative.getDefault(mContext).selectDown();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#channelUp()}
	 */
	public void channelUp() {
		try {
			TVCommonNative.getDefault(mContext).channelUp();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#channelDown()}
	 */
	public void channelDown() {
		try {
			TVCommonNative.getDefault(mContext).channelDown();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#favoriteChannelDown()}
	 */
	public void favoriteChannelDown() {
		try {
			TVCommonNative.getDefault(mContext).favoriteChannelDown();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#favoriteChannelUp()}
	 */
	public void favoriteChannelUp() {
		try {
			TVCommonNative.getDefault(mContext).favoriteChannelUp();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#getCurrentChannelNum()}
	 */
	public int getCurrentChannelNum() {
		try {
			return TVCommonNative.getDefault(mContext).getCurrentChannelNum();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return -1;
	}

	/**
	 * @see {@link ITVCommon#getCurrentChannel()}
	 */
	public TVChannel getCurrentChannel() {
		try {
			return TVCommonNative.getDefault(mContext).getCurrentChannel();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return null;
	}

	/**
	 * @see {@link ITVCommon#getInputSourceArray()}
	 */
	public String[] getInputSourceArray() {
		try {
			return TVCommonNative.getDefault(mContext).getInputSourceArray();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return null;
	}

	/**
	 * @see {@link ITVCommon#changeInputSource(String, String)}
	 */
	public void changeInputSource(String outputName, String inputName) {
		try {
			TVCommonNative.getDefault(mContext).changeInputSource(outputName,
					inputName);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#getInputSource(String)}
	 */
	public String getInputSource(String outputName) {
		try {
			return TVCommonNative.getDefault(mContext).getInputSource(
					outputName);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return null;
	}

	/**
	 * @see {@link ITVCommon#blockInputSource(String, boolean)}
	 */
	public void blockInputSource(String inputName, boolean block) {
		try {
			TVCommonNative.getDefault(mContext).blockInputSource(inputName,
					block);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#tempUnblockInputSource(String)}
	 */
	public void tempUnblockInputSource(String inputName) {
		try {
			TVCommonNative.getDefault(mContext).tempUnblockInputSource(
					inputName);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#isInputSourceBlocked(String)}
	 */
	public boolean isInputSourceBlocked(String inputName) {
		try {
			return TVCommonNative.getDefault(mContext).isInputSourceBlocked(
					inputName);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return false;
	}

	/**
	 * @see {@link ITVCommon#isInputSourcePhysicalBlocked(String)}
	 */
	public boolean isInputSourcePhysicalBlocked(String inputName) {
		try {
			return TVCommonNative.getDefault(mContext)
					.isInputSourcePhysicalBlocked(inputName);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return false;
	}

	/**
	 * @see {@link ITVCommon#getCurrentInputSource()}
	 */
	public String getCurrentInputSource() {
		try {
			return TVCommonNative.getDefault(mContext).getCurrentInputSource();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return null;
	}

	/**
	 * @see {@link ITVCommon#getChannels()}
	 */
	public List<TVChannel> getChannels() {
		try {
			return TVCommonNative.getDefault(mContext).getChannels();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return null;
	}

	/**
	 * @see {@link ITVCommon#getChannels(ChannelFilter)}
	 */
	public List<TVChannel> getChannels(ChannelFilter filter) {
		try {
			return TVCommonNative.getDefault(mContext).getChannels(filter);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return null;
	}

	/**
	 * @see {@link ITVCommon#getChannels(int, int, ChannelFilter)}
	 */
	public List<TVChannel> getChannels(int startIdx, int count,
			ChannelFilter filter) {
		try {
			return TVCommonNative.getDefault(mContext).getChannels(startIdx,
					count, filter);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return null;
	}

	/**
	 * @see {@link ITVCommon#deleteChannel(TVChannel)}
	 */
	public void deleteChannel(TVChannel ch) {
		try {
			TVCommonNative.getDefault(mContext).deleteChannel(ch);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#getInputSourceType(String)}
	 */
	public String getInputSourceType(String inputName) {
		try {
			return TVCommonNative.getDefault(mContext).getInputSourceType(
					inputName);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return null;
	}

	/**
	 * @see {@link ITVCommon#updateCurrentOutput()}
	 */
	public void updateCurrentOutput() {
		try {
			TVCommonNative.getDefault(mContext).updateCurrentOutput();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#swapChannel(TVChannel, TVChannel)}
	 */
	public void swapChannel(TVChannel ch1, TVChannel ch2) {
		try {
			TVCommonNative.getDefault(mContext).swapChannel(ch1, ch2);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#schedulePowerOff(delay)}
	 */
	public void schedulePowerOff(long delay) {
		try {
			TVCommonNative.getDefault(mContext).schedulePowerOff(delay);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#schedulePowerOff(Date)}
	 */
	public void schedulePowerOff(Date date) {
		try {
			TVCommonNative.getDefault(mContext).schedulePowerOff(date);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#cancelScheduledPowerOff()}
	 */
	public void cancelScheduledPowerOff() {
		try {
			TVCommonNative.getDefault(mContext).cancelScheduledPowerOff();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#getRemainingPowerOffTime()}
	 */
	public long getRemainingPowerOffTime() {
		try {
			return TVCommonNative.getDefault(mContext)
					.getRemainingPowerOffTime();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return 0L;
	}

	/**
	 * @see {@link ITVCommon#insertChannel(TVChannel, TVChannel)}
	 */
	public void insertChannel(TVChannel dst, TVChannel src) {
		try {
			TVCommonNative.getDefault(mContext).insertChannel(dst, src);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#findChannelByNumber(int)}
	 */
	public TVChannel findChannelByNumber(int channelNum) {
		try {
			return TVCommonNative.getDefault(mContext).findChannelByNumber(
					channelNum);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return null;
	}

	/**
	 * @see {@link ITVCommon#addSelectorListener(TVSelectorListener)}
	 */
	public void addSelectorListener(TVSelectorListener listener) {
		try {
			TVCommonNative.getDefault(mContext).addSelectorListener(listener);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#removeSelectorListener(TVSelectorListener)}
	 */
	public void removeSelectorListener(TVSelectorListener listener) {
		try {
			TVCommonNative.getDefault(mContext)
					.removeSelectorListener(listener);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#addChannelsChangedListener(ChannelsChangedListener)}
	 */
	public void addChannelsChangedListener(ChannelsChangedListener listener) {
		try {
			TVCommonNative.getDefault(mContext).addChannelsChangedListener(
					listener);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#removeChannelsChangedListener(ChannelsChangedListener)}
	 */
	public void removeChannelsChangedListener(ChannelsChangedListener listener) {
		try {
			TVCommonNative.getDefault(mContext).removeChannelsChangedListener(
					listener);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#setTunnerMode(int)}
	 */
	public void setTunnerMode(int mode) {
		try {
			TVCommonNative.getDefault(mContext).setTunnerMode(mode);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#getTunnerMode()}
	 */
	public int getTunnerMode() {
		try {
			return TVCommonNative.getDefault(mContext).getTunnerMode();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return 0;
	}

	/**
	 * @see {@link ITVCommon#clearChannels()}
	 */
	public void clearChannels() {
		try {
			TVCommonNative.getDefault(mContext).clearChannels();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#clearAllAirChannels()}
	 */
	public void clearAllAirChannels() {
		try {
			TVCommonNative.getDefault(mContext).clearAllAirChannels();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#clearAllCableChannels()}
	 */
	public void clearAllCableChannels() {
		try {
			TVCommonNative.getDefault(mContext).clearAllCableChannels();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * @see {@link ITVCommon#clearAllChannels()}
	 */
	public void clearAllChannels() {
		try {
			TVCommonNative.getDefault(mContext).clearAllChannels();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#flush()}
	 */
	public void flush() {
		try {
			TVCommonNative.getDefault(mContext).flush();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}


	/**
	 * @see {@link ITVCommon#flushByDBName(String)}
	 */
	public void flush(String dbName) {
		try {
			TVCommonNative.getDefault(mContext).flushByDBName(dbName);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}
	

	/**
	 * @see {@link ITVCommon#setDefaultOutput(String)}
	 */
	public void setDefaultOutput(String outputName) {
		try {
			TVCommonNative.getDefault(mContext).setDefaultOutput(outputName);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		TVConfigurer configurer = TVConfigurer.getInstance(mContext);

		TVOption<Integer> focusOption = (TVOption<Integer>) configurer
				.getOption(IntegerOption.CFG_TV_FOCUS_WIN);
		focusOption
				.set(outputName == "main" ? ConfigType.TV_FOCUS_WIN_MAIN
						: ConfigType.TV_FOCUS_WIN_SUB);
	}

	/**
	 * @see {@link ITVCommon#getCurrentOutput()}
	 */
	public String getCurrentOutput() {
		try {
			return TVCommonNative.getDefault(mContext).getCurrentOutput();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return null;
	}

	/**
	 * @see {@link ITVCommon#getCurrentOutputMode()}
	 */
	public int getCurrentOutputMode() {
		try {
			return TVCommonNative.getDefault(mContext).getCurrentOutputMode();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return 0;
	}

	/**
	 * @see {@link ITVCommon#switchOutputMode()}
	 */
	public void switchOutputMode() {
		try {
			TVCommonNative.getDefault(mContext).switchOutputMode();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#swapInputSource()}
	 */
	public void swapInputSource() {
		try {
			TVCommonNative.getDefault(mContext).swapInputSource();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see {@link ITVCommon#getConflictInputs()}
	 */
	public List<String> getConflictInputs() {
		try {
			return TVCommonNative.getDefault(mContext).getConflictInputs();
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return null;
	}

	/**
	 * @see {@link ITVCommon#enterOutputMode(int)}
	 */
	public void enterOutputMode(int mode) {
		ITVCommon tv = TVCommonNative.getDefault(mContext);
		if(tv != null){
		try {
			tv.enterOutputMode(mode);
		} catch (RemoteException e) {
			e.printStackTrace();
			}
		}
	}

	/**
	 * @see ITVCommon#getChannels(String)
	 */
	public List<TVChannel> getChannels(String inputName) {
		ITVCommon tv = TVCommonNative.getDefault(mContext);
		try {
			// Klocwork, WTF...
			if (tv != null) {
				return tv.getChannels(inputName);
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return null;
	}

	/**
	 * @see ITVCommon#stopOutput(String)
	 */
	public void stopOutput(String outputName) {
		ITVCommon tv = TVCommonNative.getDefault(mContext);
		try {
			if (tv != null) {
				tv.stopOutput(outputName);
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * @see ITVCommon#changeNextInputSource()
	 */
	public void changeNextInputSource() {
		ITVCommon tv = TVCommonNative.getDefault(mContext);
		try {
			if (tv != null) {
				tv.changeNextInputSource();
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	
	public int changeFocusTo(String outputName) {
		ITVCommon tv = TVCommonNative.getDefault(mContext);
		int ret = -100;
		if (null == outputName) {
			return ret;
		}
		
		try {
			if (tv != null) {
				ret = tv.changeFocusTo(outputName);
			} else {
				ret = -99;
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return ret;
	}
	

	public int setDtmbGUILang(String lang) {
		ITVCommon tv = TVCommonNative.getDefault(mContext);
		int ret = -100;
		if (null == lang) {
			return ret;
		}
		
		try {
			if (tv != null) {
				ret = tv.setDtmbGUILang(lang);
			} else {
				ret = -99;
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return ret;
	}	


	public void adjustTunnerMode(String changingToInputName) {
		ITVCommon tv = TVCommonNative.getDefault(mContext);
		if (null == tv) {
			TVCommonNative.TVLog("return reason: null == tv");
			return;
		}

		try {
			TVCommonNative.TVLog("changingTo: " + changingToInputName);
			tv.adjustTunnerMode(changingToInputName);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		return;
	}

}

