package com.mediatek.tvcommon;

import java.sql.Date;
import java.util.List;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tv.service.InputService;
import android.os.IBinder;
import android.os.IInterface;
import android.os.RemoteException;

/**
 * All APIS in {@code ITVCommon} IInterface are supported IPC.
 */
public interface ITVCommon extends IInterface {

	public final static String INPUT_TYPE_TV = InputService.INPUT_TYPE_TV;
	public final static String INPUT_TYPE_AV = InputService.INPUT_TYPE_AV;
	public final static String INPUT_TYPE_SVIDEO = InputService.INPUT_TYPE_SVIDEO;
	public final static String INPUT_TYPE_COMPONENT = InputService.INPUT_TYPE_COMPONENT;
	public final static String INPUT_TYPE_COMPOSITE = InputService.INPUT_TYPE_COMPOSITE;
	public final static String INPUT_TYPE_HDMI = InputService.INPUT_TYPE_HDMI;
	public final static String INPUT_TYPE_VGA = InputService.INPUT_TYPE_VGA;

	/**
	 * PIP stands for Picture In Picture. This allows you to be able to watch
	 * two separate inputs at one time. You will have one large picture with a
	 * smaller picture inside of it.
	 */
	public static final int OUTPUT_MODE_PIP = 1;

	/**
	 * POP stands for Picture Outside of Picture. This is the same function as
	 * PIP only this allows you to see two inputs side by side instead of a
	 * smaller picture inside of the larger one.
	 */
	public static final int OUTPUT_MODE_POP = 2;

	/**
	 * This is a new PIP which interact between Android application (e.g.
	 * Chrome) and output sub. For instance, the Chrome take the large picture,
	 * while the smaller picture inside of it are playing TV.
	 */
	public static final int OUTPUT_MODE_APP_PIP = 3;
	
	/**
	 * This is a option for coming mmp with output mode is OUTPUT_MODE_PIP or OUTPUT_MODE_POP
	 * with this mode, get source list as so as OUTPUT_MODE_NORMAL
	 * but with this mode, change output mode not using,or less,set output mode with OUTPUT_MODE_NORMAL
	 */
	public static final int OUTPUT_MODE_MMP = 4;

	/**
	 * There are only one input showing on output main in normal mode.
	 */
	public static final int OUTPUT_MODE_NORMAL = 0;

	/**
	 * TV input source types
	 */
	public static final String inputTypes[] = { INPUT_TYPE_TV, INPUT_TYPE_AV,
			INPUT_TYPE_SVIDEO, INPUT_TYPE_COMPONENT, INPUT_TYPE_COMPOSITE,
			INPUT_TYPE_HDMI, INPUT_TYPE_VGA };

	/**
	 * Cable mode
	 * 
	 * @see #TUNNER_AIR
	 * @see #setTunnerMode(int)
	 */
	public static final int TUNNER_CABLE = ConfigType.BS_SRC_CABLE;

	/**
	 * Antenna mode
	 * 
	 * @see #TUNNER_CABLE
	 * @see #setTunnerMode(int)
	 */
	public static final int TUNNER_AIR = ConfigType.BS_SRC_AIR;

	/**
	 * The set of callback APIs that will be invoked when change a input source
	 * or select a channel.
	 */
	public interface TVSelectorListener {

		/**
		 * Called when select a channel.
		 * 
		 * @param ch
		 *            The selected channel.
		 */
		public void onChannelSelected(TVChannel ch);

		/**
		 * Called when block a channel. When a channel be blocked, the channel
		 * will not play until unblock the channel.
		 * 
		 * @param ch
		 *            The blocked channel.
		 * @see TVChannel#setBlocked(boolean)
		 * @see TVChannel#tempUnblocked()
		 */
		public void onChannelBlocked(TVChannel ch);

		/**
		 * Called when change a input source to screen output.
		 * 
		 * @param outputName
		 *            Screen output name, the value is either "main" or "sub".
		 * @param inputName
		 *            Input source name.
		 */
		public void onInputSelected(String outputName, String inputName);

		/**
		 * Called when block a input source. When a input source be blocked, the
		 * the input source will not play until invoke methods
		 * {@link #blockInputSource(String, boolean)} or
		 * {@link #tempUnblockInputSource(String)}the channel.
		 * 
		 * @param inputName
		 *            Input source name.
		 * @see #blockInputSource(String, boolean)
		 * @see #tempUnblockInputSource(String)
		 */
		public void onInputBlocked(String inputName);

	}

	/**
	 * The callback API will be invoked when the channel list changed. One of
	 * following operations may cause the channel list to change:
	 * {@link #deleteChannel(TVChannel)},
	 * {@link #insertChannel(TVChannel, TVChannel)}, {@link #clearChannels()},
	 * {@link #swapChannel(TVChannel, TVChannel)}, {@link #setTunnerMode(int)},
	 * {@link TVChannel#setChannelNum(int)} and all scan operations.
	 */
	public interface ChannelsChangedListener {
		/**
		 * This is called when channel list changed.
		 */
		void onChanged();
	}

	/**
	 * Channel filter constrains data with a filtering pattern.
	 */
	public interface ChannelFilter {
		/**
		 * Specified filtering pattern which filter out the channel which is not
		 * skipped.
		 */
		final static ChannelFilter TVSkipFilter = new ChannelFilter() {
			public boolean filter(TVChannel channel) {
				return channel.isSkip();
			}
		};

		/**
		 * Specified filtering pattern which filter out the channel which is
		 * skipped.
		 */
		final ChannelFilter TVNotSkipFilter = new ChannelFilter() {
			public boolean filter(TVChannel channel) {
				return !channel.isSkip();
			}
		};

		/**
		 * Specified filtering pattern which filter out the channel which is not
		 * favorite.
		 */
		final static ChannelFilter TVFavoriteFilter = new ChannelFilter() {
			public boolean filter(TVChannel channel) {
				return channel.isFavorite();
			}
		};

		/**
		 * Specified filtering pattern which filter out nothing.
		 */
		final static ChannelFilter TVNothingFilter = new ChannelFilter() {

			@Override
			public boolean filter(TVChannel channel) {
				return true;
			}
		};

		/**
		 * 
		 */
		public boolean filter(TVChannel channel);
	}

	/**
	 * Select a channel by channel number
	 * 
	 * @param num
	 *            Channel number
	 */
	public boolean select(int num) throws RemoteException;

	/**
	 * Select a channel by {@code TVChannel} object.
	 * 
	 * @param ch
	 *            The selected {@code TVChannel} object
	 */
	public boolean select(TVChannel ch) throws RemoteException;

	/**
	 * Select the pre-selected channel.
	 */
	public void selectPrev() throws RemoteException;

	/**
	 * Select the previous channel in the channel list which exclude skipped
	 * channels.
	 */
	public void channelUp() throws RemoteException;

	/**
	 * Select the next channel in the channel list which exclude skipped
	 * channels.
	 */
	public void channelDown() throws RemoteException;

	/**
	 * Select the previous channel in current channel list.
	 */
	public void selectUp() throws RemoteException;

	/**
	 * Select the next channel in current channel list.
	 */
	public void selectDown() throws RemoteException;

	/**
	 * Select the next channel in the channel list which only include favorite
	 * channels.
	 */
	public void favoriteChannelDown() throws RemoteException;

	/**
	 * Select the previous channel in the channel list which only include
	 * favorite channels.
	 */
	public void favoriteChannelUp() throws RemoteException;

	/**
	 * Retrieve the number of currently selected channel.
	 * 
	 * @return Return the channel number. The current input is not TV if the
	 *         return value is -1;
	 */
	public int getCurrentChannelNum() throws RemoteException;

	/**
	 * Retrieve the channel object of the currently selected channel.
	 * 
	 * @return Returns the {@code TVChannel} object.
	 */
	public TVChannel getCurrentChannel() throws RemoteException;

	/**
	 * @return Returns all input sources.
	 */
	public String[] getInputSourceArray() throws RemoteException;

	/**
	 * Change a input source {@code inputName} to a screen output
	 * {@code outputName}. This operation will set {@code outputName} as default
	 * output.
	 * 
	 * @param outputName
	 *            The screen output name, the value is either "main" or "sub".
	 * @param inputName
	 *            The input source name.
	 */
	public void changeInputSource(String outputName, String inputName)
			throws RemoteException;

	/**
	 * Change input source of current output to next one. If current output mode
	 * is PIP/POP, the incompatible input wills be ignored.
	 */
	public void changeNextInputSource() throws RemoteException;

	/**
	 * @return Return the input source name of specified screen output
	 *         {@code outputName}
	 * @param outputName
	 *            The screen output name, the value is either "main" or "sub".
	 */
	public String getInputSource(String outputName) throws RemoteException;

	/**
	 * Block or unblock the specified input source {@code inputName}. The system
	 * will permanently store the block attribute of this input source.
	 * 
	 * @param inputName
	 *            The input source Name.
	 * @param block
	 *            If true and {@link #tempUnblockInputSource(String)} never
	 *            called on this boot up, The input source will be blocked.
	 */
	public void blockInputSource(String inputName, boolean block)
			throws RemoteException;

	/**
	 * Temporarily unblock the specified input source {@code inputName}. Once
	 * this method be invoked, this input source is always in unblocked state
	 * until next boot up.
	 * 
	 * @param inputName
	 *            The input source Name.
	 */
	public void tempUnblockInputSource(String inputName) throws RemoteException;

	/**
	 * Check the specified input source {@code inputName} whether blocked or
	 * unblocked. Only the block attribute is set and
	 * {@link #tempUnblockInputSource(String)} never be called, this input
	 * source is blocked.
	 * 
	 * @param inputName
	 *            The input source Name.
	 * @return The block state.
	 */
	public boolean isInputSourceBlocked(String inputName)
			throws RemoteException;

	/**
	 * Whether the attribute of input source {@code inputName} is set.
	 * 
	 * @param inputName
	 *            The input source Name.
	 * @return The block attribute.
	 */
	public boolean isInputSourcePhysicalBlocked(String inputName)
			throws RemoteException;

	/**
	 * @return the input source which has binding with default screen. The
	 *         default screen output is the last one when invoke
	 *         {@link #changeInputSource(String, String)} or
	 *         {@link #setDefaultOutput(String)}.
	 */
	public String getCurrentInputSource() throws RemoteException;

	/**
	 * Returns a {@code TVChannel} {@code List} of current input source.If
	 * current input source is neither "atv" nor "dtv", the channel list is
	 * null.
	 * 
	 * @return a {@code List} of channels.
	 */
	public List<TVChannel> getChannels() throws RemoteException;

	/**
	 * 
	 */
	public List<TVChannel> getChannels(ChannelFilter filter)
			throws RemoteException;

	/**
	 * Returns a {@code List} of the specified portion of this {@code TVChannel}
	 * {@code List} from the given {@code startIdx} index to the end index
	 * {@code startIdx+count} minus one.
	 * 
	 * @param startIdx
	 *            the index at which to start.
	 * @param count
	 *            the count of the channels.
	 * @param filter
	 * @return a list of a portion of this {@code List}.
	 * @throws IndexOutOfBoundsException
	 *             if {@code startIdx < 0, startIdx > size()} or
	 *             {@code startIdx + count >
	 *                size()}
	 */
	public List<TVChannel> getChannels(int startIdx, int count,
			ChannelFilter filter) throws RemoteException;

	/**
	 * Delete the specified {@code TVChannel}, this channel will be permanently
	 * removed from persistence storage.
	 * 
	 * @param ch
	 *            The deleted {@code TVChannel} object.
	 */
	public void deleteChannel(TVChannel ch) throws RemoteException;

	/**
	 * Returns the type of the specified input source.
	 * 
	 * @param inputName
	 *            the specified input source name.
	 * @see #inputTypes
	 */
	public String getInputSourceType(String inputName) throws RemoteException;

	/**
	 * Update state of current screen output. If the current screen output has
	 * been stopped, for example, application invoked
	 * {@link TVOutputCommon#stop(String)} to stop output, this method can
	 * restore the before state of screen output.
	 */
	public void updateCurrentOutput() throws RemoteException;

	/**
	 * Stop the output {@code outputName}
	 * 
	 * @param outputName
	 *            output name
	 */
	public void stopOutput(String outputName) throws RemoteException;

	/**
	 * Swap channel {@code ch1} and {@code ch2}
	 * 
	 * @param ch1
	 * @param ch2
	 */
	public void swapChannel(TVChannel ch1, TVChannel ch2)
			throws RemoteException;

	/**
	 * This method is only for internal using.
	 */

	public void resetChannelAttribute(TVChannel ch, String str, int... para)
			throws RemoteException;

	/**
	 * Schedule system power off after a specified delay. If there are a new
	 * schedule, the previous one will be canceled.
	 * 
	 * @param delay
	 *            amount of time in miliseconds before power off
	 */
	public void schedulePowerOff(long delay) throws RemoteException;

	/**
	 * Schedule system power off at specified time. If {@code date} is less than
	 * current time, the system will be schedule to power off as soon as
	 * possible.If there are a new schedule, the previous one will be canceled.
	 * 
	 * @param date
	 *            time of execution.
	 */
	public void schedulePowerOff(Date date) throws RemoteException;

	/**
	 * Cancel the exist power off schedule.
	 */
	public void cancelScheduledPowerOff() throws RemoteException;

	/**
	 * Returns the remaining time before system power off. If the result is less
	 * than 0, it mean that power off schedule never set or be canceled.
	 * 
	 * @return the remaining time in miliseconds.
	 */
	public long getRemainingPowerOffTime() throws RemoteException;

	/**
	 * If the channel number of {@code dst} is less than {@code src}, the
	 * {@code dst} will be inserted after {@code src}. It swap the channel
	 * number between {@code dst} and {@code src}, and the channel number of
	 * which between {@code dst} and {@code src} will automatically minus one.
	 * 
	 * <p>
	 * Vice versa, the {@code dst} will be inserted before {@code src}. It swap
	 * the channel number between {@code dst} and {@code src}, and the channel
	 * number of which between {@code dst} and {@code src} will automatically
	 * plus one.
	 * </p>
	 */
	public void insertChannel(TVChannel dst, TVChannel src)
			throws RemoteException;

	/**
	 * Returns the {@code TVChannel} object by the channel number.
	 * 
	 * @return the target {@code TVChannel} object
	 */
	public TVChannel findChannelByNumber(int channelNum) throws RemoteException;

	/**
	 * Add a {@link TVSelectorListener} which be run in main UI thread to
	 * system. Note that when you do not want receive the notifycations use
	 * {@link removeSelectorListener} to remove it.
	 * 
	 * @param listener
	 */
	public void addSelectorListener(TVSelectorListener listener)
			throws RemoteException;

	/**
	 * Remove a {@code TVSelectorListener} which was previously register with
	 * {@link addSelectorListener}
	 */
	public void removeSelectorListener(TVSelectorListener listener)
			throws RemoteException;

	/**
	 * Add a {@link ChannelsChangedListener} which be run in main UI thread to
	 * system. Note that when you do not want receive the notifycations use
	 * {@link removeChannelsChangedListener} to remove it.
	 * 
	 * @param listener
	 */
	public void addChannelsChangedListener(ChannelsChangedListener listener)
			throws RemoteException;

	/**
	 * Remove a {@code ChannelsChangedListener} which was previously register
	 * with {@link addChannelsChangedListener}
	 */
	public void removeChannelsChangedListener(ChannelsChangedListener listener)
			throws RemoteException;

	/**
	 * Set the tunner mode, both "atv" and "dtv" have different channel list in
	 * different tunner {@code mode}.
	 * 
	 * @param mode
	 *            tunner mode, the value is either {@link #TUNNER_CABLE} or
	 *            {@link #TUNNER_AIR}
	 */
	public void setTunnerMode(int mode) throws RemoteException;

	/**
	 * Returns the current tunner mode.
	 * 
	 * @return tunner mode
	 * @see {@link #TUNNER_CABLE}
	 * @see {@link #TUNNER_AIR}
	 */
	public int getTunnerMode() throws RemoteException;

	/**
	 * Clear the channel list of current input source. These channels will be
	 * permanently removed.
	 */
	public void clearChannels() throws RemoteException;

	/**
	 * Clear all air channels (including analog and digital), there are no any air channels left after this operation.
	 */
	public void clearAllAirChannels() throws RemoteException;
	
	/**
	 * Clear all cable channels (including analog and digital), there are no any cable channels left after this operation.
	 */
	public void clearAllCableChannels() throws RemoteException;
	
	/**
	 * Clear all channels, there are no any channels left after this operation.
	 */
	public void clearAllChannels() throws RemoteException;

	/**
	 * Flush the modified channel data to persistence storage.
	 */
	public void flush() throws RemoteException;

	/**
	 * Flush channel data to specified persistence storage by dbName (curr support dbName: ChannelCommon DB_AIR/CABEL).
	 */
	public void flushByDBName(String dbName) throws RemoteException;

	/**
	 * Set the default screen output to {@code outputName}. The default output
	 * is main after boot up.
	 * 
	 * @param outputName
	 *            the new default output name
	 */
	public void setDefaultOutput(String outputName) throws RemoteException;

	/**
	 * Set the Config group index according to the main input source or sub input source.
	 * 
	 * @param outputName
	 *            can be "main" or "sub"
	 */
	public void setInputCfgGrpIdx(String outputName) throws RemoteException;

	/**
	 * @return the default output, the value is either main or sub.
	 */
	public String getCurrentOutput() throws RemoteException;

	/**
	 * @return the current output mode, the value can be
	 *         {@link #OUTPUT_MODE_NORMAL}, {@link #OUTPUT_MODE_PIP} or
	 *         {@link #OUTPUT_MODE_POP}
	 */
	public int getCurrentOutputMode() throws RemoteException;

	/**
	 * Gradually switch output mode to PIP, POP and then NORMAL and again. When
	 * boot up, the default mode is NORMAL.
	 */
	public void switchOutputMode() throws RemoteException;

	/**
	 * Swap the inputs of output main and sub. It legally invoked only in
	 * PIP/POP mode.
	 * 
	 */
	public void swapInputSource() throws RemoteException;

	/**
	 * In PIP/POP mode, the input source which bind with current output may
	 * conflict with other inputs. For instance, it cann't play input hdmi0 and
	 * hdmi1 at the same time.
	 * 
	 * @return the inputs which are incompatible with the input of current
	 *         output.
	 */
	public List<String> getConflictInputs() throws RemoteException;

	/**
	 * Return the channel list of specified input {@code inputName}. If current
	 * output is not bound with {@code inputName}, invoke
	 * {@link #select(TVChannel)} to select a channel in {@code List<TVChannel>}
	 * will do noting. This is a ambiguous API only for MTK TurnKey's
	 * requirements.
	 * 
	 * @param inputName
	 *            the specified input source name.
	 * @return the channel list, If {@code inputName} is neither "atv" nor
	 *         "dtv", it return null.
	 */
	public List<TVChannel> getChannels(String inputName) throws RemoteException;

	/**
	 * Enter to another output mode.
	 * 
	 * @param mode
	 *            the target output mode, the value can be
	 *            {@link #OUTPUT_MODE_APP_PIP}, {@link #OUTPUT_MODE_NORMAL},
	 *            {@link #OUTPUT_MODE_PIP} or {@link #OUTPUT_MODE_POP}.
	 */
	public void enterOutputMode(int mode) throws RemoteException;

	public int changeFocusTo(String outputName) throws RemoteException;

	public int setDtmbGUILang(String lang) throws RemoteException;

	public void adjustTunnerMode(String changingToInputName) throws RemoteException;

	String descriptor = "com.mediatek.tvcm.ITVCommon";

	int SCAN_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION;
	int SELECT_INT_CHANNEL_TRANSCATION = IBinder.FIRST_CALL_TRANSACTION + 1;
	int SELECT_TV_CHANNEL_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 2;
	int GET_CURRENT_INT_CHANNEL_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 3;
	int GET_TV_CHANNEL_TRANSCATION = IBinder.FIRST_CALL_TRANSACTION + 4;
	int CHANNEL_UP_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 5;
	int CHANNEL_DOWN_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 6;
	int DELETE_CHANNEL_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 7;
	int CHANGE_INPUR_SOURCE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 8;
	int GET_INPUT_SOURCE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 9;
	int BLOCK_INPUT_SOURCE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 10;
	int TEMP_UNBLOCK_INPUT_SROUCE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 11;
	int IS_INPUT_SOURCE_BLOCKED_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 12;
	int IS_INPUT_SOURCE_BLOCKED_PHYSICAL_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 13;
	int GET_INPUT_SOURCE_ARRAY_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 14;
	int SELECT_PRE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 15;
	int GET_CHANNELS_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 16;
	int GET_CURRENT_INPUT_SOURCE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 17;
	int GET_INPUT_SOURCE_TYPE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 18;
	int UPDATE_CURRENT_OUTPUT_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 19;
	int SWAP_CHANNEL_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 20;
	int SCHEDULE_POWEROFF_LONG_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 22;
	int CANCEL_SCHEDULED_POWEROFF_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 23;
	int GET_REMAINING_POWEROFF_TIME_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 24;
	int INSERT_CHANNEL_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 25;
	int ADD_ITVSELECTOR_LISTENER_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 26;
	int ADD_IUPDATE_LISTENER_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 27;
	int SET_TUNNER_MODE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 28;
	int FAVORITE_CHANNEL_UP_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 29;
	int FAVORITE_CHANNEL_DOWN_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 30;
	int CLEAR_CHANNELS_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 31;
	int CLEAR_ALL_CHANNELS_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 32;
	int RESET_CHANNEL_ATTRIBUTE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 33;
	int SELECT_UP_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 34;
	int SELECT_DOWN_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 35;
	int SET_DEFAULT_OUTPUT_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 36;
	int GET_CURRENT_OUTPUT_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 37;
	int GET_CURRENT_OUTPUT_MODE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 38;
	int SWITCH_OUTPUT_MODE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 39;
	int SWAP_INPUT_SOURCE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 40;
	int GET_CONFLICTS_INPUTS_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 41;
	int ENTER_OUTPUT_MODE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 42;
	int STOP_OUTPUT_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 43;
	int CHANGE_NEXT_INPUT_SROUCE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 44;
	int SET_INPUT_CFG_GRP_IDX_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 45;
	int CLEAR_ALL_AIR_CHANNELS_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 46;
	int CLEAR_ALL_CABLE_CHANNELS_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 47;
	int CHANGE_FOCUS_TO_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 48;
	int SET_DTMB_GUI_LANG_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 49;
	int ADJUST_TUNNER_MODE_TRANSACTION = IBinder.FIRST_CALL_TRANSACTION + 50;
}

