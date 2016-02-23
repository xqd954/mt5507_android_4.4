package com.mediatek.ui.menu.commonview;

import java.util.Date;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.TextView;

import com.mediatek.tv.model.DtmbScanRF;
import com.mediatek.tvcm.TVChannelSelector;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVScanner;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.MenuMain;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.util.MtkLog;

/**
 * used for the TextView which can be modified by press left or right key,
 * 
 * @author hs_haosun
 * 
 */
public class NumView extends ListViewItemView implements RespondedKeyEvent {
	// show the name on the left
	private TextView mNameTextView;
	private TextView mShowView;
	private TVChannel tvCh;
	private TVChannelSelector chSel;
	private OnValueChangedListener mValueChangedListener;
	private String numString = "";
	private Long mPreTime = (long) 0;
	private boolean inputFlag = false;
    private TVScanner mTVScanner;
	public int getValue() {
		return this.mDataItem.getmInitValue();
	}

	public void setValue(int mPositon) {
		this.mDataItem.setmInitValue(mPositon);
		mShowView.setText(this.mDataItem.getmOptionValue()[mPositon]);
		if (mValueChangedListener != null) {
			mValueChangedListener.onValueChanged(this, mPositon);
		}
	}

	public NumView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		init();
	}

	public NumView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		init();
	}

	public NumView(Context context) {
		super(context);
		this.context = context;
		init();
	}

	private void init() {
		this.addView(inflate(context, R.layout.menu_num_view, null));
		mTVScanner =TVContent.getInstance(mContext).getScanner();
		mNameTextView = (TextView) findViewById(R.id.common_tv_itemname);
		mShowView = (TextView) findViewById(R.id.common_tv_itemshow);
	}

	public void setAdapter(SetConfigListViewAdapter.DataItem mDataItem) {
		this.mDataItem = mDataItem;
		setViewName(this.mDataItem.getmName());
		chSel = TVContent.getInstance(context).getChannelSelector();
		tvCh = chSel.getCurrentChannel();
		if (mDataItem.mItemID
				.equals(MenuConfigManager.TV_CHANNEL_START_FREQUENCY)) {
//			if ((tvCh != null) ) {
//				mDataItem.mInitValue = tvCh.getFreq() / 1000000;
//			} else {
//				mDataItem.mInitValue = 44;
//			}
			MenuMain.setFreFrom(mDataItem.mInitValue * 1000000);
			mShowView.setText(mDataItem.mInitValue + ".000");
		} else if (mDataItem.mItemID
				.equals(MenuConfigManager.TV_CHANNEL_END_FREQUENCY)) {

//			mDataItem.mInitValue = 865;
			MenuMain.setFreTo(mDataItem.mInitValue * 1000000);
			mShowView.setText(mDataItem.mInitValue + ".000");
		} else if (mDataItem.mItemID.equals(MenuConfigManager.SYM_RATE)) {
		    MenuMain.setSysrate(mDataItem.mInitValue);
			mShowView.setText(mDataItem.mInitValue + "");
		} else if(MenuConfigManager.TV_SINGLE_SCAN_SIGNAL_QUALITY.equals(mDataItem.mItemID)){
			mShowView.setText(mDataItem.mOptionValue[mDataItem.mInitValue]);
		}else if(MenuConfigManager.TV_SINGLE_SCAN_RF_CHANNEL.equals(mDataItem.mItemID)){
			String 	mCurrentDtmbRFChannel = mTVScanner.getCurrentDtmbRFChannel();
			if(mCurrentDtmbRFChannel !=null && !"".equals(mCurrentDtmbRFChannel)){
				mShowView.setText(mCurrentDtmbRFChannel);
				 MenuMain.setRfchanel(Integer.parseInt(mCurrentDtmbRFChannel));
			}
			
		}else if ((MenuConfigManager.TV_CHANNEL_FREQUENCY).equals(mDataItem.mItemID)) {
		    MenuMain.setRfchanel(mDataItem.mInitValue);
		    MenuMain.setFreFrom(mDataItem.mInitValue * 1000000);
            mShowView.setText(mDataItem.mInitValue + "");
        } else {
			MenuMain.setFreFrom(mDataItem.mInitValue * 1000000);
			mShowView.setText(mDataItem.mInitValue + "");
		}

	}
	
	public TextView getNameView(){
		return mNameTextView;
	}

	public void setViewName(String viewName) {
		mNameTextView.setText(viewName);
	}

	protected void switchValueNext() {
		if (mDataItem.mItemID
				.equals(MenuConfigManager.TV_CHANNEL_START_FREQUENCY)
				|| mDataItem.mItemID
						.equals(MenuConfigManager.TV_CHANNEL_END_FREQUENCY)) {
			mDataItem.mInitValue += 5;
			int max = mDataItem.getmEndValue();
			if (mDataItem.mInitValue > max) {
				mDataItem.mInitValue = max;
			}
			mShowView.setText(mDataItem.mInitValue + ".000");
			if (mValueChangedListener != null) {
				mValueChangedListener
						.onValueChanged(this, mDataItem.mInitValue);
			}
		}
	  if(MenuConfigManager.TV_SINGLE_SCAN_RF_CHANNEL.equals(mDataItem.mItemID)){
		  String mDtmbRFChannel = mTVScanner.getNextDtmbRFChannel();
			if(mDtmbRFChannel !=null&&!"".equals(mDtmbRFChannel)){
				mShowView.setText(mDtmbRFChannel);
				 MenuMain.setRfchanel(Integer.parseInt(mDtmbRFChannel));
			}
	    }
	}

	protected void switchValuePrevious() {
		if (mDataItem.mItemID
				.equals(MenuConfigManager.TV_CHANNEL_START_FREQUENCY)
				|| mDataItem.mItemID
						.equals(MenuConfigManager.TV_CHANNEL_END_FREQUENCY)) {
			int min = mDataItem.getmStartValue();
			mDataItem.mInitValue -= 5;
			if (mDataItem.mInitValue < min) {
				mDataItem.mInitValue = min;
			}
			mShowView.setText(mDataItem.mInitValue + ".000");
			if (mValueChangedListener != null) {
				mValueChangedListener
						.onValueChanged(this, mDataItem.mInitValue);
			}
		}
		  if(MenuConfigManager.TV_SINGLE_SCAN_RF_CHANNEL.equals(mDataItem.mItemID)){
				String  mDtmbRFChannel= mTVScanner.getPrevDtmbRFChannel();
				if(mDtmbRFChannel !=null && !"".equals(mDtmbRFChannel)){
					mShowView.setText(mDtmbRFChannel);
					 MenuMain.setRfchanel(Integer.parseInt(mDtmbRFChannel));
			}
		}
	}

	public void input(String num) {
		MtkLog.d("test", "--numString.length()----" + numString.length());
		Date currentDate = new Date(System.currentTimeMillis());
		Long mCurrentTime = currentDate.getTime();
		if (mCurrentTime - mPreTime > 2000) {
			numString = "";
			numString += num;

		} else {
			if (mDataItem.mItemID.equals(MenuConfigManager.SYM_RATE)) {
				if (numString.length() < 4) {
					numString += num;
				} else {
					numString = numString.substring(1, 4) + num;
				}
				MenuMain.setSysrate(Integer.parseInt(numString));
			} else {
				if (numString.length() < 3) {
					numString += num;
				} else {

					numString = numString.substring(1, 3) + num;

				}
			}
		}
		mPreTime = mCurrentTime;
		MtkLog.d("test", "--numString.length()--11--" + numString.length());
		if (mDataItem.mItemID
				.equals(MenuConfigManager.TV_CHANNEL_START_FREQUENCY)
				|| mDataItem.mItemID
						.equals(MenuConfigManager.TV_CHANNEL_END_FREQUENCY)) {
			mShowView.setText(numString + ".000");
		} else if(MenuConfigManager.TV_SINGLE_SCAN_RF_CHANNEL.equals(mDataItem.mItemID)){
		} else {
			mShowView.setText(numString + "");
			mDataItem.mInitValue = Integer.parseInt(numString);
			if (mValueChangedListener != null) {
				mValueChangedListener
						.onValueChanged(this, mDataItem.mInitValue);
			}
		}
		inputFlag = true;
	}

	public void onKeyEnter() {
		if ((MenuConfigManager.TV_CHANNEL_END_FREQUENCY).equals(mDataItem.mItemID)
				||(MenuConfigManager.TV_CHANNEL_START_FREQUENCY).equals(mDataItem.mItemID)
				||(MenuConfigManager.TV_CHANNEL_FREQUENCY).equals(mDataItem.mItemID)) {
			if (inputFlag) {
				int max = mDataItem.getmEndValue();
				int min = mDataItem.getmStartValue();
				if (Integer.parseInt(numString) < min) {
					mDataItem.mInitValue = min;
				} else if (Integer.parseInt(numString) > max) {
					mDataItem.mInitValue = max;
				} else {
					mDataItem.mInitValue = Integer.parseInt(numString);
				}
				if ((MenuConfigManager.TV_CHANNEL_FREQUENCY).equals(mDataItem.mItemID)) {
					mShowView.setText(mDataItem.mInitValue + "");
					MenuMain.setRfchanel(mDataItem.mInitValue);
				} else {
					mShowView.setText(mDataItem.mInitValue + ".000");
				}

				if (mValueChangedListener != null) {
					mValueChangedListener.onValueChanged(this,
							mDataItem.mInitValue);
				}
				inputFlag = false;
			}
		}
	}

	public void changeValueLister(OnValueChangedListener changedListener,
			DataItem mItem) {

		if (changedListener != null) {
			changedListener.onValueChanged(this, mItem.mInitValue);
		}
	}

	public void setNameText(String name) {
		mShowView.setText(name);
	}

	public void onKeyLeft() {
		switchValuePrevious();
	}

	public void onKeyRight() {
		switchValueNext();
	}

	public void showValue(int value) {
		if(mDataItem.mOptionValue !=null &&value<mDataItem.mOptionValue.length){
			this.mDataItem.setmInitValue(value);
			mShowView.setText(this.mDataItem.mOptionValue[value]);
		}
	}

	public TextView getmShowView() {
		return mShowView;
	}

	public void setmShowView(TextView mShowView) {
		this.mShowView = mShowView;
	}

	public OnValueChangedListener getValuedListener() {
		return mValueChangedListener;
	}

	public void setValueChangedListener(
			OnValueChangedListener mValueChangedListener) {
		this.mValueChangedListener = mValueChangedListener;
	}
}
