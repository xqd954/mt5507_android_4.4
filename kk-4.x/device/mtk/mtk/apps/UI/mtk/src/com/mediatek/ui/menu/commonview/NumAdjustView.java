package com.mediatek.ui.menu.commonview;

import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.mediatek.ui.R;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter;
import com.mediatek.ui.util.MtkLog;

public class NumAdjustView extends ListViewItemView implements
		RespondedKeyEvent {

	private static final String TAG = "NumAdjustView";
	// the item name, value of list view item
	private TextView mNameView;
	private TextView mValueView;
	private OnValueChangedListener mValueChangedListener;

	DelayController delayController = new DelayController();

	private class MyTimerTask extends TimerTask {

		@Override
		public void run() {
			// TODO Auto-generated method stub
			Log.v(TAG, "hahaha");
			if (mValueChangedListener != null) {
				mValueChangedListener.onValueChanged(NumAdjustView.this,
						mDataItem.mInitValue);
			}
		}
	}

	private class DelayController {

		private List<Timer> timers = new ArrayList<Timer>();

		public void start() {
			TimerTask tt = new MyTimerTask();
			Timer timer = new Timer();
			timers.add(timer);
			timer.schedule(tt, 2000);
			for (int i = 0; i < timers.size() - 1; i++) {
				timers.get(i).cancel();
			}
		}

	}

	public NumAdjustView(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
		this.context = context;
		init();
	}

	public NumAdjustView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		init();
	}

	public NumAdjustView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		init();
	}

	public int getValue() {
		return this.mDataItem.getmInitValue();
	}

	public void setValue(int value) {
		this.mDataItem.setmInitValue(value);
		mValueView.setText(value + "");
		if (mValueChangedListener != null) {
			mValueChangedListener.onValueChanged(this, value);
		}
	}

	private void setViewName(String viewName) {
		mNameView.setText(viewName);
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
	}

	public void setAdapter(SetConfigListViewAdapter.DataItem mDataItem) {
		this.mDataItem = mDataItem;
		mId = mDataItem.mItemID;
		setViewName(this.mDataItem.getmName());
		mValueView.setText(mDataItem.mInitValue + "");
	}

	public TextView getNameView() {
		return mNameView;
	}

	public TextView getValueView() {
		return mValueView;
	}

	protected void switchValueNext() {

		mDataItem.mInitValue += mDataItem.getmStepValue();
		if (mDataItem.mInitValue >= mDataItem.getmEndValue()) {
			mDataItem.mInitValue = mDataItem.getmStartValue();
		}
		mValueView.setText(mDataItem.mInitValue + "");
		MtkLog.v(TAG, "" + mDataItem.mInitValue);
	}

	protected void switchValuePrevious() {

		mDataItem.mInitValue -= mDataItem.getmStepValue();
		if (mDataItem.mInitValue <= mDataItem.getmStartValue()) {
			mDataItem.mInitValue = mDataItem.getmEndValue();
		}
		mValueView.setText(mDataItem.mInitValue + "");
		MtkLog.v(TAG, "" + mDataItem.mInitValue);
	}

	public void onKeyEnter() {
		// TODO Auto-generated method stub

	}

	public void onKeyLeft() {
		// TODO Auto-generated method stub
		delayController.start();
		if (mDataItem.isEnable) {
			switchValuePrevious();
		}
	}

	public void onKeyRight() {
		// TODO Auto-generated method stub
		delayController.start();
		if (mDataItem.isEnable) {
			switchValueNext();
		}
	}

	public void showValue(int value) {
		// TODO Auto-generated method stub
		if (value < mDataItem.getmStartValue()
				|| value > mDataItem.getmEndValue()) {
			throw new IllegalArgumentException("value is Illegal value");
		}
		mDataItem.mInitValue = value;
		mValueView.setText(value + "");
	}
}
