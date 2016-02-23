package com.mediatek.timeshift_pvr.ui;

import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.RelativeLayout.LayoutParams;

import com.mediatek.timeshift_pvr.manager.SaveValue;
import com.mediatek.timeshift_pvr.manager.SetConfigListViewAdapter;
import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.ui.R;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import android.os.SystemProperties;
import android.widget.ImageView;

/**
 * This view includes a text view (the name),the second text view(the current
 * value) and a ImageView(selected or not), it is one item of list view items.
 * you can press LEFT or RIGHT key to change value of the second text view
 * 
 * @author hs_yaoxiao
 */

public class OptionView extends ListViewItemView implements RespondedKeyEvent {

	private static final String TAG = "OptionView";
	// the item name, value of list view item
	private TextView mNameView;
	private TextView mValueView;
	private OnValueChangedListener mValueChangedListener;
	private static List<Timer> timers = new ArrayList<Timer>();
	private SaveValue saveV;
    private ImageView mRightImageIv;

	private class MyTimerTask extends TimerTask {

		@Override
		public void run() {
			// TODO Auto-generated method stub
			if (mValueChangedListener != null) {
				mValueChangedListener.onValueChanged(OptionView.this,
						mDataItem.mInitValue);
			}
		}
	}

	public OptionView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		init();
	}

	public OptionView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		init();
	}

	public OptionView(Context context) {
		super(context);
		this.context = context;
		init();
	}

	public int getValue() {
		return this.mDataItem.getmInitValue();
	}

	public void setValue(int mPositon) {
		this.mDataItem.setmInitValue(mPositon);
		mValueView.setText(this.mDataItem.getmOptionValue()[mPositon]);
		if (mValueChangedListener != null) {
			mValueChangedListener.onValueChanged(this, mPositon);
		}
	}

	private void setViewName(String viewName) {
		mNameView.setText(viewName);
	}

	public TextView getNameView() {
		return mNameView;
	}

	public TextView getValueView() {
		return mValueView;
	}

	public void setValueChangedListener(
			OnValueChangedListener mValueChangedListener) {
		this.mValueChangedListener = mValueChangedListener;
	}

	private void init() {
		LinearLayout lv = (LinearLayout) inflate(mContext,
				R.layout.menu_option_view, null);
		LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
		addView(lv, params);
		mNameView = (TextView) findViewById(R.id.common_tv_itemname);
		mValueView = (TextView) findViewById(R.id.common_tv_itemshow);
        mRightImageIv = (ImageView) findViewById(R.id.common_iv_itemimage);
		saveV = SaveValue.getInstance(context);
	}

	public void setAdapter(SetConfigListViewAdapter.DataItem mDataItem) {
		this.mDataItem = mDataItem;
		mId = mDataItem.mItemID;
		setViewName(this.mDataItem.getmName());
			mValueView
					.setText(this.mDataItem.mOptionValue[this.mDataItem.mInitValue]);

	}

	protected void switchValueNext() {
			if (mDataItem.mInitValue != mDataItem.mOptionValue.length - 1) {
				mValueView
						.setText(mDataItem.mOptionValue[++mDataItem.mInitValue]);
			} else {
				mDataItem.mInitValue = 0;
				mValueView
						.setText(mDataItem.mOptionValue[mDataItem.mInitValue]);
			}
		// if (mValueChangedListener != null
		// && !mId.equals(MenuConfigManager.NETWORK_INTERFACE)
		// && !mId.equals(MenuConfigManager.NETWORK_CONNECTION)) {
		mValueChangedListener.onValueChanged(this, mDataItem.mInitValue);
		
		if (TimeShiftManager.SCHEDULE_PVR_CHANNELLIST.equals(mDataItem.mItemID)) {
            TimeShiftManager.getInstance().getTvLogicManager().selectChannel(KeyMap.KEYCODE_MTKIR_CHUP);
        }


		MtkLog.v(TAG, "" + mDataItem.mInitValue);
	}

	protected void switchValuePrevious() {
			if (mDataItem.mInitValue != 0) {
				mValueView
						.setText(mDataItem.mOptionValue[--mDataItem.mInitValue]);
			} else {
				mDataItem.mInitValue = mDataItem.mOptionValue.length - 1;
				mValueView
						.setText(mDataItem.mOptionValue[mDataItem.mInitValue]);
			}
		// if (mValueChangedListener != null
		// && !mId.equals(MenuConfigManager.NETWORK_INTERFACE)) {
		mValueChangedListener.onValueChanged(this, mDataItem.mInitValue);
		
        if (TimeShiftManager.SCHEDULE_PVR_CHANNELLIST.equals(mDataItem.mItemID)) {
            TimeShiftManager.getInstance().getTvLogicManager().selectChannel(KeyMap.KEYCODE_MTKIR_CHDN);
        }
		// }
	}

	public void onKeyEnter() {
	}

	public void onKeyLeft() {
		if (mDataItem.isEnable) {
			// if ( mId.equals(MenuConfigManager.NETWORK_CONNECTION)) {
			// TimerTask tt = new MyTimerTask();
			// Timer timer = new Timer();
			// timer.schedule(tt, 500);
			// timers.add(timer);
			// MtkLog.v(TAG, "size:" + timers.size() + "********************");
			// for (int i = 0; i < timers.size() - 1; i++) {
			// timers.get(i).cancel();
			// }
			// }
			switchValuePrevious();
		}
	}

	public void onKeyRight() {
		if (mDataItem.isEnable) {
			// if (mId.equals(MenuConfigManager.NETWORK_INTERFACE)) {
			// TimerTask tt = new MyTimerTask();
			// Timer timer = new Timer();
			// timer.schedule(tt, 500);
			// timers.add(timer);
			// MtkLog.v(TAG, "size:" + timers.size() + "********************");
			// for (int i = 0; i < timers.size() - 1; i++) {
			// timers.get(i).cancel();
			// }
			// }
			switchValueNext();
		}
	}

	public void showValue(int value) {
		if (value < 0 || value > mDataItem.mOptionValue.length - 1) {
			throw new IllegalArgumentException("value is Illegal value");
		}
		mDataItem.mInitValue = value;
		mValueView.setText(mDataItem.mOptionValue[value]);
	}
	
    public void setRightImageSource(boolean isHighlight) {
		if (isHighlight) {
			mRightImageIv.setImageResource(R.drawable.menu_icon_select_hi);
		} else {
			mRightImageIv.setImageResource(R.drawable.menu_icon_select_nor);
		}
	}
}
