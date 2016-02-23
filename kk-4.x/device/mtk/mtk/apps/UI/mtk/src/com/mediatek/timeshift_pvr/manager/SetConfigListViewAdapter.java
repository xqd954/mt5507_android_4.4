package com.mediatek.timeshift_pvr.manager;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

import android.app.AlarmManager;
import android.content.Context;
import android.graphics.Color;
import android.os.SystemClock;

import com.mediatek.timeshift_pvr.ui.DateTimeInputView;
import com.mediatek.timeshift_pvr.ui.OnValueChangedListener;
import com.mediatek.timeshift_pvr.ui.OptionView;
import com.mediatek.ui.util.MtkLog;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnFocusChangeListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;

import com.mediatek.ui.R;
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
    private int mSelectPos = -1;
	// store the data of page which is shown on screen
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
		return mGroup == null ? null : mGroup.get(position);
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
		final ViewHolder hodler = new ViewHolder();

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
				}
				hodler.mOptionView.setAdapter(dataItem);
				hodler.mOptionView.setmId(dataItem.mItemID);
				hodler.mOptionView
						.setValueChangedListener(new OnValueChangedListener() {
							public void onValueChanged(View v, int value) {
								OptionView v1 = (OptionView) v;
								String selId = v1.getmId();
								 if (selId.equals(TimeShiftManager.DTV_TSHIFT_OPTION))
									{
										TimeShiftManager.getInstance().setValueToPrefer(TimeShiftManager.DTV_TSHIFT_OPTION,value);
								} else {
//									mg
//											.setValue(selId, value
//													+ mg.getMin(selId));
								}
							}
						});
				if (position == mSelectPos && isEnabled(position)) {
					hodler.mOptionView.setRightImageSource(true);
				} else {
					hodler.mOptionView.setRightImageSource(false);
				}
				return hodler.mOptionView;
			}








			/**
			 * FactoryProgressView
			 */


			if (dataItem.getDataType() == DataItem.DataType.DATETIMEVIEW) {
				if (hodler.mDateTimeInputView == null) {
					hodler.mDateTimeInputView = new DateTimeInputView(mContext);
				}
				if (isEnabled(position)) {
					hodler.mDateTimeInputView.getmDateTimeView().flag = false;
				} else {
					hodler.mDateTimeInputView.getmTextViewName().setTextColor(
							Color.GRAY);
					// hodler.mDateTimeInputView.getmDateTimeView().getmPaint().setColor(Color.GRAY);
					hodler.mDateTimeInputView.getmDateTimeView().flag = false;
				}
				//Log.d("timestring","come in hodler.mDateTimeInputView.setAdapter ="+dataItem.mItemID);
				hodler.mDateTimeInputView.setAdapter(dataItem);
				hodler.mDateTimeInputView.setmId(dataItem.mItemID);
				convertView.setOnFocusChangeListener(new OnFocusChangeListener(){

					@Override
					public void onFocusChange(View v, boolean hasFocus) {						
						hodler.mDateTimeInputView.setCurrentSelectedPosition(-1);
					}
				});
				
				
				return hodler.mDateTimeInputView;
			}


		}
		return convertView;
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
		DateTimeInputView mDateTimeInputView;
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

	public void setSelectPos(int position) {
		mSelectPos = position;
		//notifyDataSetChanged();
	}

}
