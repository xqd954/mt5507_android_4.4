package com.mediatek.ui.menu.adapter;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

import android.app.AlarmManager;
import android.content.Context;
import android.graphics.Color;
import android.os.SystemClock;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;

import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVTimerManager;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.MenuMain;
import com.mediatek.ui.menu.commonview.ChannelListPoweronView;
import com.mediatek.ui.menu.commonview.ChannelListView;
import com.mediatek.ui.menu.commonview.ChannelPowerOnNoChannel;
import com.mediatek.ui.menu.commonview.DateTimeInputView;
import com.mediatek.ui.menu.commonview.EffectOptionView;
import com.mediatek.ui.menu.commonview.FactoryOptionView;
import com.mediatek.ui.menu.commonview.FactoryProgressView;
import com.mediatek.ui.menu.commonview.HaveSubPageView;
import com.mediatek.ui.menu.commonview.ListViewItemView;
import com.mediatek.ui.menu.commonview.NumAdjustView;
import com.mediatek.ui.menu.commonview.NumView;
import com.mediatek.ui.menu.commonview.OnValueChangedListener;
import com.mediatek.ui.menu.commonview.OptionView;
import com.mediatek.ui.menu.commonview.PasswordView;
import com.mediatek.ui.menu.commonview.PositionView;
import com.mediatek.ui.menu.commonview.ProgressView;
import com.mediatek.ui.menu.commonview.ScanChannelsOptionView;
import com.mediatek.ui.menu.commonview.SwitchOptionView;
import com.mediatek.ui.menu.commonview.TVSourceView;
import com.mediatek.ui.menu.commonview.TextCommView;
import com.mediatek.ui.menu.util.EditChannel;
import com.mediatek.ui.menu.util.GetTimeZone;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.SaveValue;
import com.mediatek.ui.menu.util.TimeConsume;
import com.mediatek.ui.util.MtkLog;

/**
 * SetConfigListViewAdapter is used to fill/show data for ListView
 * 
 * @author MTK40405
 * 
 */
public class SetConfigListViewAdapter extends BaseAdapter {
	protected static final String TAG = "SetConfigListViewAdapter";
	private Context mContext;
	// store the data of page which is shown on screen
	private int mSelectPos = -1;
	private List<DataItem> mGroup;
	private String[] viewNames = { "OptionView", "PositionView",
			"ProgressView", "HaveSubPageView", "EffectOptionView",
			"SwitchOptionView", "ChannelListView", "TextCommView",
			"ScanChannelsOptionView", "NumView", "NumAdjustView", "IPInputBox",
			"DateTimeInputView", "ChannelListPoweronView", "PasswordView",
			"FactoryOptionView", "FactoryProgressView", "TVSourceView" };

	/**
	 * get the current data of the page which is shown on screen
	 * 
	 * @return the current data of the current page
	 */
	public List<DataItem> getmGroup() {
		return mGroup;
	}

	/**
	 * set the current data of the page which is to be shown on screen
	 * 
	 * @param mGroup
	 *            the current data of the page
	 */
	public void setmGroup(List<DataItem> mGroup) {
		this.mGroup = mGroup;
	}

	/**
	 * Construct function
	 * 
	 * @param mContext
	 *            store the Context
	 */
	public SetConfigListViewAdapter(Context mContext) {
		super();
		this.mContext = mContext;
	}

	/**
	 * How many items are is shown in current page.
	 * 
	 * @return Count of items.
	 */
	public int getCount() {
		return mGroup == null ? 0 : mGroup.size();
	}

	public Object getItem(int position) {
		return (mGroup == null || mGroup.size() == 0)? null : mGroup.get(position);
	}

	public long getItemId(int position) {
		return mGroup == null ? 0 : position;
	}

	public boolean areAllItemsEnabled() {
		return false;
	}

	public boolean isEnabled(int position) {
		return mGroup.get(position).isEnable;
	}

	/**
	 * get the last enabled item position in current shown page
	 * 
	 * @return the index of the last enabled item position
	 */
	public int getLastEnableItemPosition() {
		int position = getCount() - 1;
		for (int i = getCount() - 1; i >= 0; i--) {
			if (isEnabled(i) == true) {
				position = i;
				return position;
			}
		}
		return position;
	}

	/**
	 * get the first enabled item position in current shown page
	 * 
	 * @return the index of the first enabled item position
	 */
	public int getFirstEnableItemPosition() {
		int position = 0;
		for (int i = 0; i <= getCount() - 1; i++) {
			if (isEnabled(i) == true) {
				position = i;
				return position;
			}
		}
		return position;
	}

	public View getView(int position, View convertView, ViewGroup parent) {
		final MenuConfigManager mg = MenuConfigManager.getInstance(mContext);

		if (position == 0) {
			TimeConsume.breforeTime = System.currentTimeMillis();
		}
		if (position == getCount() - 1) {
			TimeConsume.currentTime = System.currentTimeMillis();
		}
		ViewHolder hodler = new ViewHolder();

		if (mGroup != null && mGroup.size() > 0) {
			DataItem dataItem = mGroup.get(position);
			// set DataItem's position in ListView
			dataItem.setPosition(position);
			if (dataItem.getDataType() == DataItem.DataType.OPTIONVIEW) {
				if (hodler.mOptionView == null) {
					hodler.mOptionView = new OptionView(mContext);
				}
				// when no available,set text color gray
				if (!isEnabled(position)) {
					hodler.mOptionView.getNameView().setTextColor(Color.GRAY);
					hodler.mOptionView.getValueView().setTextColor(Color.GRAY);
				} else {
					if (dataItem.getmItemID().equals(
							MenuConfigManager.SETUP_TIME_ZONE)) {
						mg.setSetup(dataItem.getmItemID(), GetTimeZone
								.getInstance(mContext).getZoneId());
						dataItem.setmInitValue(GetTimeZone
								.getInstance(mContext).getZoneId());
					}
				}
				hodler.mOptionView.setAdapter(dataItem);
				hodler.mOptionView.setmId(dataItem.mItemID);
				hodler.mOptionView
						.setValueChangedListener(new OnValueChangedListener() {
							public void onValueChanged(View v, int value) {
								OptionView v1 = (OptionView) v;
								String selId = v1.getmId();
								if (selId.startsWith("UNDEFINE")) {
									
								} else if (selId.startsWith("SETUP")) {
									mg.setSetup(selId, value);
									if (selId
											.equals(MenuConfigManager.SETUP_TIME_ZONE)) {
										AlarmManager alarm = (AlarmManager) mContext
												.getSystemService(Context.ALARM_SERVICE);
										String[] zoneNames = mContext
												.getResources()
												.getStringArray(
														R.array.menu_setup_timezone_name_array);
										if (zoneNames[value]
												.equals("As Broadcast")) {
											setBroadcastUTC();

											SaveValue.getInstance(mContext)
													.saveBooleanValue(
															"Zone_time", true);
										} else {
											setUTCTime();
											alarm.setTimeZone(zoneNames[value]);
										}
									}
								} else if (selId.startsWith("CHANNELEDIT")) {
									EditChannel editChannel = EditChannel
											.getInstance(mContext);
									editChannel.editTVEditSystem(selId, value);
								} // add by jun for test because there is only
								// dummy
								else if (("RF Channel").equals(selId)
										||("define_audio_language1").equals(selId)
										||("define_audio_language2").equals(selId)) {
									// do nothing
								}
								 else if (selId.equals(MenuConfigManager.DTV_TSHIFT_OPTION))
									{
										mg.setValueToPrefer(MenuConfigManager.DTV_TSHIFT_OPTION,value);
								} else {
									mg
											.setValue(selId, value
													+ mg.getMin(selId));
								}
							}
						});
				hodler.mOptionView.setOnTouchListener(onTouchListener);
				if (position == mSelectPos && isEnabled(position)) {
					hodler.mOptionView.setRightImageSource(true);
				} else {
					hodler.mOptionView.setRightImageSource(false);
				}
				return hodler.mOptionView;
			}

			/**
			 * FactoryOptionView
			 */
			if (dataItem.getDataType() == DataItem.DataType.FACTORYOPTIONVIEW) {
				if (hodler.mFactoryOptionView == null) {
					hodler.mFactoryOptionView = new FactoryOptionView(mContext);
				}
				// when no available,set text color gray
				if (!isEnabled(position)) {
					hodler.mFactoryOptionView.getNameView().setTextColor(
							Color.GRAY);
					hodler.mFactoryOptionView.getmShowView().setTextColor(
							Color.GRAY);
				}
				hodler.mFactoryOptionView.setAdapter(dataItem);
				hodler.mFactoryOptionView.setmId(dataItem.mItemID);
				hodler.mFactoryOptionView
						.setValueChangedListener(new OnValueChangedListener() {
							public void onValueChanged(View v, int value) {
								FactoryOptionView v1 = (FactoryOptionView) v;
								String selId = v1.getmId();
								if (selId.startsWith("UNDEFINE")) {
									// add code later
								} else if (selId.startsWith("SETUP")) {
									mg.setSetup(selId, value);
								} else {
									mg
											.setValue(selId, value
													+ mg.getMin(selId));
								}
							}
						});
				hodler.mFactoryOptionView.setOnTouchListener(onTouchListener);
				return hodler.mFactoryOptionView;
			}

			if (dataItem.getDataType() == DataItem.DataType.NUMVIEW) {
				if (hodler.mNumView == null) {
					hodler.mNumView = new NumView(mContext);
				}
				hodler.mNumView.setAdapter(dataItem);
				hodler.mNumView.setmId(dataItem.mItemID);
				hodler.mNumView
						.setValueChangedListener(new OnValueChangedListener() {
							public void onValueChanged(View v, int value) {
								NumView v1 = (NumView) v;
								String selId = v1.getmId();
								if (selId.startsWith("UNDEFINE")) {
									if (selId
											.equals("UNDEFINE_channel_start_frequency")
											|| selId
													.equals("UNDEFINE_channel_frequency")) {
										int freFrom = value * 1000000;
										MenuMain.setFreFrom(freFrom);
									} else if (selId
											.equals("UNDEFINE_channel_end_frequency")) {
										int freTo = value * 1000000;
										MenuMain.setFreTo(freTo);
									}
								} else if (selId
										.equals(MenuConfigManager.SYM_RATE)) {
									mg.setScanValue(selId, value);
								}
							}
						});
				hodler.mNumView.setOnTouchListener(onTouchListener);
				return hodler.mNumView;
			}

			if (dataItem.getDataType() == DataItem.DataType.NUMADJUSTVIEW) {
				if (hodler.mNumAdjustView == null) {
					hodler.mNumAdjustView = new NumAdjustView(mContext);
				}
				hodler.mNumAdjustView.setAdapter(dataItem);
				hodler.mNumAdjustView.setmId(dataItem.mItemID);
				hodler.mNumAdjustView
						.setValueChangedListener(new OnValueChangedListener() {
							public void onValueChanged(View v, int value) {
								NumAdjustView v1 = (NumAdjustView) v;
								String selId = v1.getmId();
//								if (selId == "RF Channel") {
//
//								}
							}
						});
				hodler.mNumAdjustView.setOnTouchListener(onTouchListener);
				return hodler.mNumAdjustView;
			}

			if (dataItem.getDataType() == DataItem.DataType.EFFECTOPTIONVIEW) {
				if (hodler.mEffectOptionView == null) {
					hodler.mEffectOptionView = new EffectOptionView(mContext);
				}
				hodler.mEffectOptionView.setAdapter(dataItem);
				hodler.mEffectOptionView.setmId(dataItem.mItemID);
				hodler.mEffectOptionView
						.setValueChangedListener(new OnValueChangedListener() {
							public void onValueChanged(View v, int value) {
								EffectOptionView v1 = (EffectOptionView) v;
								String selId = v1.getmId();
								if (selId.startsWith("SETUP")) {
									mg.setSetup(selId, value);
								} else {
									mg
											.setValue(selId, value
													+ mg.getMin(selId));

								}
							}
						});
				hodler.mEffectOptionView.setOnTouchListener(onTouchListener);
				if (position == mSelectPos && isEnabled(position)) {
					hodler.mEffectOptionView.setRightImageSource(true);
				} else {
					hodler.mEffectOptionView.setRightImageSource(false);
				}
				return hodler.mEffectOptionView;
			}

			if (dataItem.getDataType() == DataItem.DataType.SWICHOPTIONVIEW) {
				if (hodler.mSwitchOptionView == null) {
					hodler.mSwitchOptionView = new SwitchOptionView(mContext);
				}
				if (!isEnabled(position)) {
					hodler.mSwitchOptionView.getNameView().setTextColor(
							Color.GRAY);
					hodler.mSwitchOptionView.getValueView().setTextColor(
							Color.GRAY);
				}
				hodler.mSwitchOptionView.setAdapter(dataItem);
				hodler.mSwitchOptionView.setmId(dataItem.mItemID);
				hodler.mSwitchOptionView
						.setValueChangedListener(new OnValueChangedListener() {
							public void onValueChanged(View v, int value) {
								SwitchOptionView v1 = (SwitchOptionView) v;
								String selId = v1.getmId();
								if (selId.startsWith("SETUP")) {
									mg.setSetup(selId, value);

								} else if (selId.startsWith("UNDEFINE")) {

								} else {
									mg.setValue(selId, value + mg.getMin(selId));
								}
							}
						});
				hodler.mSwitchOptionView.setOnTouchListener(onTouchListener);
				if (position == mSelectPos && isEnabled(position)) {
					hodler.mSwitchOptionView.setRightImageSource(true);
				} else {
					hodler.mSwitchOptionView.setRightImageSource(false);
				}
				return hodler.mSwitchOptionView;
			}

			if (dataItem.getDataType() == DataItem.DataType.POSITIONVIEW) {
				if (hodler.mPostionView == null) {
					hodler.mPostionView = new PositionView(mContext);
				}
				hodler.mPostionView.setAdapter(dataItem);
				hodler.mPostionView.setmId(dataItem.mItemID);
				hodler.mPostionView
						.setValueChangedListener(new OnValueChangedListener() {
							public void onValueChanged(View v, int value) {
								PositionView v1 = (PositionView) v;
								String selId = v1.getmId();
								if (selId.startsWith("SETUP")) {
									mg.setSetup(selId, value);
								}
								if (selId.startsWith("UNDEFINE")) {
									// add code later
								} else {
									mg.setValue(selId, value);
								}
							}
						});
				hodler.mPostionView.setOnTouchListener(onTouchListener);
				if (mSelectPos == position && isEnabled(position)) {
					hodler.mPostionView.setSeekBarSelect(true);
				} else {
					hodler.mPostionView.setSeekBarSelect(false);
				}
				return hodler.mPostionView;
			}

			if (dataItem.getDataType() == DataItem.DataType.PROGRESSBAR) {
				if (hodler.mProgressView == null) {
					hodler.mProgressView = new ProgressView(mContext);
				}

				if (!isEnabled(position) && !("Signal level").equals(dataItem.mItemID)) {
					hodler.mProgressView.getProgressView().setProgressDrawable(
							mContext.getResources().getDrawable(
									R.drawable.menu_probargray));
					hodler.mProgressView.getNameView().setTextColor(
							Color.DKGRAY);
					hodler.mProgressView.getValueView().setTextColor(
							Color.DKGRAY);
				}
				hodler.mProgressView.setAdapter(dataItem);
				hodler.mProgressView.setmId(dataItem.mItemID);
				hodler.mProgressView
						.setValueChangedListener(new OnValueChangedListener() {
							public void onValueChanged(View v, int value) {
								ProgressView v1 = (ProgressView) v;
								String selId = v1.getmId();
								if (selId.startsWith("SETUP")) {
									mg.setSetup(selId, value);
								} else if (selId.startsWith("UNDEFINE")) {

								} else {
									mg.setValue(selId, value);
								}
							}
						});
				hodler.mProgressView.setOnTouchListener(onTouchListener);
				return hodler.mProgressView;
			}

			/**
			 * FactoryProgressView
			 */
			if (dataItem.getDataType() == DataItem.DataType.FACTORYPROGRESSVIEW) {
				if (hodler.mFactoryProgressView == null) {
					hodler.mFactoryProgressView = new FactoryProgressView(
							mContext);
				}
				// when no available,set text color gray
				if (!isEnabled(position)) {
					hodler.mFactoryProgressView.getmNameTextView()
							.setTextColor(Color.DKGRAY);
					hodler.mFactoryProgressView.getmValueView().setTextColor(
							Color.DKGRAY);
				}
				hodler.mFactoryProgressView.setAdapter(dataItem);
				hodler.mFactoryProgressView.setmId(dataItem.mItemID);
				hodler.mFactoryProgressView
						.setValueChangedListener(new OnValueChangedListener() {
							public void onValueChanged(View v, int value) {
								FactoryProgressView v1 = (FactoryProgressView) v;
								String selId = v1.getmId();
								if (selId.startsWith("SETUP")) {
									mg.setSetup(selId, value);
								} else if (selId.startsWith("UNDEFINE")) {

								} else {
									mg.setValue(selId, value);
								}
							}
						});
				hodler.mFactoryProgressView.setOnTouchListener(onTouchListener);
				return hodler.mFactoryProgressView;
			}

			if (dataItem.getDataType() == DataItem.DataType.HAVESUBCHILD) {
				if (hodler.mHaveSubPageView == null) {
					hodler.mHaveSubPageView = new HaveSubPageView(mContext);
				}
				hodler.mHaveSubPageView.setAdapter(dataItem);
				hodler.mHaveSubPageView.setmId(dataItem.mItemID);
				if (!isEnabled(position)) {
					hodler.mHaveSubPageView.getNameView().setTextColor(
							Color.GRAY);
				}
				hodler.mHaveSubPageView.setOnTouchListener(onTouchListener);
				return hodler.mHaveSubPageView;
			}

			if (dataItem.getDataType() == DataItem.DataType.DATETIMEVIEW) {
				if (hodler.mDateTimeInputView == null) {
					hodler.mDateTimeInputView = new DateTimeInputView(mContext);
				}
				if (isEnabled(position)) {
					hodler.mDateTimeInputView.getmDateTimeView().flag = true;
				} else {
					hodler.mDateTimeInputView.getmTextViewName().setTextColor(
							Color.GRAY);
					// hodler.mDateTimeInputView.getmDateTimeView().getmPaint().setColor(Color.GRAY);
					hodler.mDateTimeInputView.getmDateTimeView().flag = false;
				}
				//Log.d("timestring","come in hodler.mDateTimeInputView.setAdapter ="+dataItem.mItemID);
				hodler.mDateTimeInputView.setAdapter(dataItem);
				hodler.mDateTimeInputView.setmId(dataItem.mItemID);
				hodler.mDateTimeInputView.setOnTouchListener(onTouchListener);
				return hodler.mDateTimeInputView;
			}

			if (dataItem.getDataType() == DataItem.DataType.SCANCHANNELSOPTIONVIEW) {
				if (hodler.mScanChannelsOptionView == null) {
					hodler.mScanChannelsOptionView = new ScanChannelsOptionView(
							mContext);
				}
				hodler.mScanChannelsOptionView.setAdapter(dataItem);
				hodler.mScanChannelsOptionView.setmId(dataItem.mItemID);

				hodler.mScanChannelsOptionView
						.setValueChangedListener(new OnValueChangedListener() {
							public void onValueChanged(View v, int value) {
								ScanChannelsOptionView v1 = (ScanChannelsOptionView) v;
								String scItem = v1.getmId();
								mg.setScanValue(scItem, value);
							}
						});
				hodler.mScanChannelsOptionView
						.setOnTouchListener(onTouchListener);
				if (position == mSelectPos && isEnabled(position)) {
					hodler.mScanChannelsOptionView.setRightImageSource(true);
				} else {
					hodler.mScanChannelsOptionView.setRightImageSource(false);
				}
				return hodler.mScanChannelsOptionView;
			}

			if (dataItem.getDataType() == DataItem.DataType.CHANNELLISTVIEW) {
				if (hodler.mChannelListView == null) {
					hodler.mChannelListView = new ChannelListView(mContext);
				}
				hodler.mChannelListView.setAdapter(dataItem);
				hodler.mChannelListView.setmId(dataItem.mItemID);
				hodler.mChannelListView.setOnTouchListener(onTouchListener);
				return hodler.mChannelListView;
			}

			if (dataItem.getDataType() == DataItem.DataType.TEXTCOMMVIEW) {
				if (hodler.textCommView == null) {
					hodler.textCommView = new TextCommView(mContext);
				}

				if (!isEnabled(position)) {
					hodler.textCommView.getTextName().setTextColor(Color.GRAY);
					hodler.textCommView.getTextContent().setTextColor(
							Color.GRAY);
				}
				hodler.textCommView.setAdapter(dataItem);
				hodler.textCommView.setmId(dataItem.mItemID);
				hodler.textCommView.setOnTouchListener(onTouchListener);
				return hodler.textCommView;
			}
			if (dataItem.getDataType() == DataItem.DataType.CHANNELPOWERNOCAHNNEL) {
				if (hodler.channelPowerNoChannel == null) {
					hodler.channelPowerNoChannel = new ChannelPowerOnNoChannel(
							mContext);

				}
				if (isEnabled(position)) {
					hodler.channelPowerNoChannel.setVisibility(View.VISIBLE);
				} else {
					hodler.channelPowerNoChannel.setVisibility(View.GONE);
				}
				hodler.channelPowerNoChannel.setAdapter(dataItem);
				hodler.channelPowerNoChannel.setmId(dataItem.mItemID);
				hodler.channelPowerNoChannel
						.setOnTouchListener(onTouchListener);
				return hodler.channelPowerNoChannel;
			}
			if (dataItem.getDataType() == DataItem.DataType.CHANNELPOWERONVIEW) {
				if (hodler.channelPowerView == null) {
					hodler.channelPowerView = new ChannelListPoweronView(
							mContext);

				}
				if (isEnabled(position)) {
					hodler.channelPowerView.setVisibility(View.VISIBLE);
				} else {
					hodler.channelPowerView.setVisibility(View.GONE);
				}
				hodler.channelPowerView.setAdapter(dataItem);
				hodler.channelPowerView.setmId(dataItem.mItemID);
				hodler.channelPowerView.setOnTouchListener(onTouchListener);
				return hodler.channelPowerView;
			}

			if (dataItem.getDataType() == DataItem.DataType.PASSWORDVIEW) {
				if (hodler.passwordView == null) {
					hodler.passwordView = new PasswordView(mContext);
				}
				hodler.passwordView.setAdapter(dataItem);
				hodler.passwordView.setmId(dataItem.mItemID);
				hodler.passwordView.setOnTouchListener(onTouchListener);
				InnerOnTouchListener innerOnTouchListener = new InnerOnTouchListener(
						hodler.passwordView);
				hodler.passwordView.getmShowView().setOnTouchListener(
						innerOnTouchListener);
				return hodler.passwordView;
			}
			if (dataItem.getDataType() == DataItem.DataType.TVSOURCEVIEW) {
				if (hodler.tvSourceView == null) {
					hodler.tvSourceView = new TVSourceView(mContext);
				}
				hodler.tvSourceView.setAdapter(dataItem);
				hodler.tvSourceView.setmId(dataItem.mItemID);
				hodler.tvSourceView.setOnTouchListener(onTouchListener);
				return hodler.tvSourceView;
			}

		}
		return convertView;
	}

	private View.OnTouchListener onTouchListener = new View.OnTouchListener() {
		public boolean onTouch(View v, MotionEvent event) {
			if (event.getAction() == MotionEvent.ACTION_DOWN) {
				String itemID = ((ListViewItemView) v).getmId();
				boolean enabled = ((ListViewItemView) v).getmDataItem().isEnable;
				String className = v.getClass().getName();
				((MenuMain) mContext).setIsMouse(true);
				if (!enabled) {
					((MenuMain) mContext).seekMouthFocus();
				} else {
					if (className.endsWith("TVSourceView")) {
						String sTemp = ((ListViewItemView) v).getmDataItem()
								.getmOptionValue()[0];
						int iTemp = Integer.parseInt(sTemp);
						iTemp--;
						MtkLog.v(TAG, "*************************" + iTemp);
						((MenuMain) mContext).seekMouthFocus(iTemp, -1);
					} else if (className.endsWith("ChannelListPoweronView")) {
						String sTemp = ((ListViewItemView) v).getmDataItem()
								.getmOptionValue()[0];
						int iTemp = Integer.parseInt(sTemp);
						((MenuMain) mContext).seekMouthFocus(iTemp, -1);
					} else if (className.endsWith("ChannelListView")) {
						String sTemp = ((ListViewItemView) v).getmDataItem()
								.getmOptionValue()[0];
						int iTemp = Integer.parseInt(sTemp);
						((MenuMain) mContext).seekMouthFocus(iTemp,
								MenuConfigManager.FOCUS_OPTION_CHANGE_CHANNEL);
					} else {
						((MenuMain) mContext).seekMouthFocus(itemID);
					}
				}
				return false;
			} else if (event.getAction() == MotionEvent.ACTION_MOVE) {
				printMotionEvent(event);
				return false;
			} else if (event.getAction() == MotionEvent.ACTION_CANCEL) {
				MtkLog.v(TAG, "canceled!!!!!!!!!!!!!!!!!!!!!!!!!!!");
				return false;
			}
			return true;
		}
	};

	private class InnerOnTouchListener implements View.OnTouchListener {

		private View parent;

		public InnerOnTouchListener(View parent) {
			this.parent = parent;
		}

		public boolean onTouch(View v, MotionEvent event) {
			// TODO Auto-generated method stub
			if (event.getAction() == MotionEvent.ACTION_DOWN) {
				String itemID = ((ListViewItemView) parent).getmId();
				boolean enabled = ((ListViewItemView) parent).getmDataItem().isEnable;
				String className = parent.getClass().getName();
				((MenuMain) mContext).setIsMouse(true);
				if (!enabled) {
					((MenuMain) mContext).seekMouthFocus();
				} else {
					if (className.endsWith("TVSourceView")) {
						String sTemp = ((ListViewItemView) parent)
								.getmDataItem().getmOptionValue()[0];
						int iTemp = Integer.parseInt(sTemp);
						iTemp--;
						MtkLog.v(TAG, "*************************" + iTemp);
						((MenuMain) mContext).seekMouthFocus(iTemp, -1);
					} else if (className.endsWith("ChannelListPoweronView")) {
						String sTemp = ((ListViewItemView) parent)
								.getmDataItem().getmOptionValue()[0];
						int iTemp = Integer.parseInt(sTemp);
						((MenuMain) mContext).seekMouthFocus(iTemp, -1);
					} else if (className.endsWith("ChannelListView")) {
						String sTemp = ((ListViewItemView) parent)
								.getmDataItem().getmOptionValue()[0];
						int iTemp = Integer.parseInt(sTemp);
						((MenuMain) mContext).seekMouthFocus(iTemp,
								MenuConfigManager.FOCUS_OPTION_CHANGE_CHANNEL);
					} else {
						((MenuMain) mContext).seekMouthFocus(itemID);
					}
				}
				return false;
			}
			return true;
		}
	}

	// for test
	private void printMotionEvent(MotionEvent event) {
		MtkLog.v(TAG, "************************************");
		// only in ACTION_MOVE
		MtkLog.v(TAG, "history size:" + event.getHistorySize());
		MtkLog.v(TAG, "histoty count:" + event.getPointerCount());
		for (int i = 0; i < event.getHistorySize(); i++) {
			// only in ACTION_MOVE
			MtkLog.v(TAG, "histoty " + i + " time:"
					+ event.getHistoricalEventTime(i));
			for (int j = 0; j < event.getPointerCount(); j++) {
				MtkLog.v(TAG, "history " + i + " and point " + j + " positon:"
						+ event.getHistoricalX(j, i) + ":"
						+ event.getHistoricalY(j, i));
			}
		}
		MtkLog.v(TAG, "current time:" + event.getEventTime());
		for (int j = 0; j < event.getPointerCount(); j++) {
			MtkLog.v(TAG, "point " + j + " positon:" + event.getX(j) + ":"
					+ event.getY(j));
		}
		MtkLog.v(TAG, "which edge:" + event.getEdgeFlags());
		MtkLog.v(TAG, "************************************");
	}

	private class ViewHolder {
		OptionView mOptionView;
		PositionView mPostionView;
		HaveSubPageView mHaveSubPageView;
		ProgressView mProgressView;
		EffectOptionView mEffectOptionView;
		SwitchOptionView mSwitchOptionView;
		ChannelListView mChannelListView;
		TextCommView textCommView;
		ScanChannelsOptionView mScanChannelsOptionView;
		NumView mNumView;
		NumAdjustView mNumAdjustView;
		DateTimeInputView mDateTimeInputView;
		ChannelListPoweronView channelPowerView;
		PasswordView passwordView;
		FactoryOptionView mFactoryOptionView;
		FactoryProgressView mFactoryProgressView;
		TVSourceView tvSourceView;
		ChannelPowerOnNoChannel channelPowerNoChannel;
	}

	/**
	 * Data item of ListView
	 * 
	 * @author MTK40405
	 * 
	 */
	public static class DataItem {
		// user custom options
		public int userDefined;
		// Parameters of ID
		public String mItemID;
		// whether or not available of parameters
		public boolean isEnable = true;
		// name of Configuration items
		public String mName;
		// OptionView's parameter item name
		public String[] mOptionValue;
		// parameter's minValue of ProgressView ,PositionView
		public int mStartValue;
		// parameter's maxValue of ProgressView ,PositionView
		public int mEndValue;
		// ProgressView ,PositionView initial data and OptionView initial data
		// flag
		public int mInitValue;
		public int mStepValue; // progressbar step length

		// A cascade effect, a subclass of reference data kept the father
		public DataItem mParent;
		// A cascade effect, the parent class save it affected parameters of
		// collections of data
		public List<DataItem> mEffectGroup;
		// Used to identify a cascade effect, the parent parameters do not
		// produce a cascade effect of the value (image mode subscript user,
		// coordinate values 0) under
		// public int mNoEffectIndex;
		// SwichOptionView selection of a value for some of the parameters to
		// control it is available
		public HashMap<Integer, Boolean[]> mSwitchHashMap;

		// A cascade of options under the father can coordinate values
		// correspond to each subclasses parameters (such as image mode
		// configuration value, sports (2), with the value of the brightness of
		// the 50)
		public HashMap<Integer, int[]> mHashMap;

		// With the many XiangZi configuration parameters configuration
		// parameters, such as HaveChildSubView
		public List<DataItem> mSubChildGroup;

		// Tape of a menu, keep a parent menu List are issued its reference, and
		// realize the flip back to the show
		public List<DataItem> mParentGroup;

		// The default is OPtionView, this DataItem data types
		private volatile DataType mDataType = DataType.OPTIONVIEW;
		// The date type, date or time string
		public String mDateTimeStr;
		// Distinguish between DateTimeView date or time (do is validated by the
		// input with)
		public int mDateTimeType;
		// The DataItem ListView in the position
		public int mPosition;
		public boolean mBrightBackGroundFlag = false;

		private boolean autoUpdate=true;
		public boolean ismBrightBackGroundFlag() {
			return mBrightBackGroundFlag;
		}

		public void setmBrightBackGroundFlag(boolean mBrightBackGroundFlag) {
			this.mBrightBackGroundFlag = mBrightBackGroundFlag;
		}

		public int getPosition() {
			return mPosition;
		}

		public void setPosition(int mPosition) {
			this.mPosition = mPosition;
		}

		public int getmDateTimeType() {
			return mDateTimeType;
		}

		public void setmDateTimeType(int mDateTimeType) {
			this.mDateTimeType = mDateTimeType;
		}

		public String getmDateTimeStr() {
			return mDateTimeStr;
		}

		public void setmDateTimeStr(String mDateTimeStr) {
			this.mDateTimeStr = mDateTimeStr;
		}

		public DataItem(String mItemID, String mName, int mStartValue,
				int mEndValue, int mInitValue, String[] mOptionVaule,
				int mStepValue, DataType mDataType) {
			this.mItemID = mItemID;
			this.mName = mName;
			this.mDataType = mDataType;
			// Item control is SeekBar, ProgressBar
			if (mDataType == DataType.POSITIONVIEW
					|| mDataType == DataType.PROGRESSBAR
					|| mDataType == DataType.NUMVIEW
					|| mDataType == DataType.FACTORYPROGRESSVIEW
					|| mDataType == DataType.NUMADJUSTVIEW) {
				this.mStartValue = mStartValue;
				this.mEndValue = mEndValue;
				this.mInitValue = mInitValue;
				this.mStepValue = mStepValue;
			}
			// Item control is OptionView
			if (mDataType == DataType.OPTIONVIEW
					|| mDataType == DataType.EFFECTOPTIONVIEW
					|| mDataType == DataType.SWICHOPTIONVIEW
					|| mDataType == DataType.CHANNELLISTVIEW
					|| mDataType == DataType.TEXTCOMMVIEW
					|| mDataType == DataType.CHANNELPOWERNOCAHNNEL
					|| mDataType == DataType.SCANCHANNELSOPTIONVIEW
					|| mDataType == DataType.CHANNELPOWERONVIEW
					|| mDataType == DataType.FACTORYOPTIONVIEW
					|| mDataType == DataType.FACTORYPROGRESSVIEW
					|| mDataType == DataType.TVSOURCEVIEW) {
				this.mOptionValue = mOptionVaule;
				this.mInitValue = mInitValue;
			}
			if (mDataType == DataType.INPUTBOX || mDataType == DataType.NUMVIEW) {
				this.mOptionValue = mOptionVaule;
			}
		}

		public enum DataType {
			OPTIONVIEW, POSITIONVIEW, PROGRESSBAR, HAVESUBCHILD, INPUTBOX, EFFECTOPTIONVIEW, SWICHOPTIONVIEW, CHANNELLISTVIEW, TEXTCOMMVIEW, SCANCHANNELSOPTIONVIEW, NUMVIEW, NUMADJUSTVIEW,  DATETIMEVIEW, CHANNELPOWERONVIEW, PASSWORDVIEW, FACTORYOPTIONVIEW, FACTORYPROGRESSVIEW, TVSOURCEVIEW, CHANNELPOWERNOCAHNNEL
		}

		public String toString() {
			return "DataItem [mDataType=" + mDataType + ", mEndValue="
					+ mEndValue + ", mInitValue=" + mInitValue
					+ ", mStepValue=" + mStepValue + ", mItemID=" + mItemID
					+ ", mName=" + mName + ", mOptionValue="
					+ Arrays.toString(mOptionValue) + ", mStartValue="
					+ mStartValue + ", mSubChildGroup=" + mSubChildGroup + "]";
		}

		public int getUserDefined() {
			return userDefined;
		}

		public void setUserDefined(int userDefined) {
			this.userDefined = userDefined;
		}

		public String getmItemID() {
			return mItemID;
		}

		public void setmItemID(String mItemID) {
			this.mItemID = mItemID;
		}

		public boolean isEnable() {
			return isEnable;
		}

		public void setEnable(boolean isEnable) {
			this.isEnable = isEnable;
		}

		public String getmName() {
			return mName;
		}

		public void setmName(String mName) {
			this.mName = mName;
		}

		public String[] getmOptionValue() {
			return mOptionValue;
		}

		public void setmOptionValue(String[] mOptionValue) {
			this.mOptionValue = mOptionValue;
		}

		public int getmStartValue() {
			return mStartValue;
		}

		public void setmStartValue(int mStartValue) {
			this.mStartValue = mStartValue;
		}

		public int getmEndValue() {
			return mEndValue;
		}

		public void setmEndValue(int mEndValue) {
			this.mEndValue = mEndValue;
		}

		public int getmInitValue() {
			return mInitValue;
		}

		public void setmInitValue(int mInitValue) {
			this.mInitValue = mInitValue;
		}

		public int getmStepValue() {
			return mStepValue;
		}

		public void setmStepValue(int mStepValue) {
			this.mStepValue = mStepValue;
		}

		public DataItem getmParent() {
			return mParent;
		}

		public void setmParent(DataItem mParent) {
			this.mParent = mParent;
		}

		public List<DataItem> getmEffectGroup() {
			return mEffectGroup;
		}

		public void setmEffectGroup(List<DataItem> mEffectGroup) {
			this.mEffectGroup = mEffectGroup;
		}

		/*
		 * public int getmNoEffectIndex() { return mNoEffectIndex; }
		 * 
		 * public void setmNoEffectIndex(int mNoEffectIndex) {
		 * this.mNoEffectIndex = mNoEffectIndex; }
		 */

		public HashMap<Integer, Boolean[]> getmSwitchHashMap() {
			return mSwitchHashMap;
		}

		public void setmSwitchHashMap(HashMap<Integer, Boolean[]> mSwitchHashMap) {
			this.mSwitchHashMap = mSwitchHashMap;
		}

		public HashMap<Integer, int[]> getmHashMap() {
			return mHashMap;
		}

		public void setmHashMap(HashMap<Integer, int[]> mHashMap) {
			this.mHashMap = mHashMap;
		}

		public List<DataItem> getmSubChildGroup() {
			return mSubChildGroup;
		}

		public void setmSubChildGroup(List<DataItem> mSubChildGroup) {
			this.mSubChildGroup = mSubChildGroup;
		}

		public List<DataItem> getmParentGroup() {
			return mParentGroup;
		}

		public void setmParentGroup(List<DataItem> mParentGroup) {
			this.mParentGroup = mParentGroup;
		}

		public DataType getDataType() {
			return mDataType;
		}

		public void setDataType(DataType mDataType) {
			this.mDataType = mDataType;
		}

		public boolean isAutoUpdate() {
			return autoUpdate;
		}

		public void setAutoUpdate(boolean autoUpdate) {
			this.autoUpdate = autoUpdate;
		}
	}

	void setUTCTime() {
		if (SaveValue.getInstance(mContext).readValue(
				MenuConfigManager.AUTO_SYNC) == 1) {
			TVContent tv = TVContent.getInstance(mContext);
			TVTimerManager tvManager = tv.getTimerManager();
			Long mBroadcastUTC = tvManager.getBroadcastUTC();
//			SystemClock.setCurrentTimeMillis(mBroadcastUTC);
		}

	}

	void setBroadcastUTC() {
		if (SaveValue.getInstance(mContext).readValue(
				MenuConfigManager.AUTO_SYNC) == 1) {
			TVContent tv = TVContent.getInstance(mContext);
			TVTimerManager tvManager = tv.getTimerManager();
			Long mBroadcastUTC = tvManager.getBroadcastUTC();
			Long mBroadcastTimeZone = tvManager.getBroadcastTZOffset();
			MtkLog.d(TAG, "----------mBroadcastTimeZone--------"
					+ mBroadcastTimeZone);

		}

	}

	public void setSelectPos(int position) {
		mSelectPos = position;
		if (getItem(0) != null && !((DataItem)getItem(0)).getmItemID().equals(MenuConfigManager.TV_SINGLE_SCAN_RF_CHANNEL)) {
			notifyDataSetChanged();
		}
	}
}
