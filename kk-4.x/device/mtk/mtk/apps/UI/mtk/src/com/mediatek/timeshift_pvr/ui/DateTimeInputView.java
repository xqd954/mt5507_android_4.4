package com.mediatek.timeshift_pvr.ui;

import java.util.Date;

import android.content.Context;
import android.content.Intent;
import android.graphics.Canvas;
import android.graphics.Color;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.text.format.Time;
import android.util.AttributeSet;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.mediatek.timeshift_pvr.manager.SetConfigListViewAdapter;
import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.commonview.UpdateTime;
import com.mediatek.ui.util.MtkLog;

public class DateTimeInputView extends ListViewItemView implements
		RespondedKeyEvent {
	public int mMinutes;
	public int mHour;
	public int second = 0;
	public Handler mHandler;
	// private Thread mClockThread
	private TextView mTextViewName;
	private DateTimeView mDateTimeView;
	// private DateTimeView dateView;
	// private DateTimeView timeView;
	public final int DATETYPE = 0x00000000;
	public final int TIMETYPE = 0x00000001;

	// private DataItem mDataItem
	public UpdateTime updateProcess;
	private OnValueChangedListener mValueChangedListener;

	public OnValueChangedListener getValueChangedListener() {
		return mValueChangedListener;
	}

	public void setValueChangedListener(
			OnValueChangedListener mValueChangedListener) {
		this.mValueChangedListener = mValueChangedListener;
	}

	public DateTimeInputView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		init();
	}

	public DateTimeInputView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		init();
	}

	public DateTimeInputView(Context context) {
		super(context);
		this.context = context;
		init();
	}

	public void setAdapter(SetConfigListViewAdapter.DataItem mDataItem) {
		this.mDataItem = mDataItem;
		mTextViewName.setText(mDataItem.getmName());
		//MtkLog.d("timestring","come in set TIME_TIME");
		if (mDataItem.isAutoUpdate()) {
		      if ((TimeShiftManager.TIME_TIME).equals(mDataItem.getmItemID())) {
		            // final DateTimeView timeView = (DateTimeView)
		            // findViewById(R.id.common_datetimeview);
		            final UpdateTime updateProcess = new UpdateTime(
		                    TimeShiftManager.TIME_TIME);
		            updateProcess.startprocess(new UpdateTime.UpdateListener() {
		                public void update(String mString) {
		                //MtkLog.d("timestring","come in set TIME_TIME end");
		                    mDateTimeView.setDateStr(mString, updateProcess);
		                    mDateTimeView.postInvalidate();
		                }
		            },context);
		        } else if ((TimeShiftManager.TIME_DATE).equals(mDataItem.getmItemID())) {
		            // final DateTimeView dateView = (DateTimeView)
		            // findViewById(R.id.common_datetimeview);
		            final UpdateTime updateProcess = new UpdateTime(
		                    TimeShiftManager.TIME_DATE);
		            updateProcess.startprocess(new UpdateTime.UpdateListener() {
		                public void update(String mString) {
		                //MtkLog.d("timestring","come in set TIME_TIME end");
		                    // TODO Auto-generated method stub
		                    mDateTimeView.setDateStr(mString, updateProcess);
		                    mDateTimeView.postInvalidate();
		                }
		            },context);

		        } 
		} else {
            this.mDateTimeView.setDateStr(mDataItem.getmDateTimeStr(),
                    new UpdateTime());
            DateTimeInputView.this.mDateTimeView.postInvalidate();
        }

		this.mDateTimeView.mType = mDataItem.getmDateTimeType();

	}

	private void init() {
		LinearLayout lv = (LinearLayout) inflate(mContext,
				R.layout.menu_datetime_input_view, null);
		LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
		addView(lv, params);

		mTextViewName = (TextView) findViewById(R.id.common_itemname);
		mDateTimeView = (DateTimeView) findViewById(R.id.common_datetimeview);
	}
	
	public void setWhiteColor(){
	    mDateTimeView.setDrawDone(true);
	    mDateTimeView.postInvalidate();
	}
	
	public void setFlag(){
		mDateTimeView.flag=true;
		mDateTimeView.setDrawDone(false);
	    mDateTimeView.postInvalidate();
	}

	public int getValue() {
		// TODO Auto-generated method stub
		return 0;
	}

	public void onKeyEnter() {

	}

	public void onKeyLeft() {
		if (mDateTimeView != null) {
			mDateTimeView.onKeyLeft();
		}
	}

	public void onKeyRight() {
		if (mDateTimeView != null) {
			mDateTimeView.onKeyRight();
		}
	}

	protected void onDraw(Canvas canvas) {
		mDateTimeView.postInvalidate();
		super.onDraw(canvas);
	}

	public void setCurrentSelectedPosition(int mCurrentSelectedPosition) {
		mDateTimeView.setCurrentSelectedPosition(mCurrentSelectedPosition);
	}

	public void setValue(int value) {
		// TODO Auto-generated method stub

	}

	public DateTimeView getmDateTimeView() {
		return mDateTimeView;
	}

	public void setmDateTimeView(DateTimeView mDateTimeView) {
		this.mDateTimeView = mDateTimeView;
	}

	public void showValue(int value) {
		// TODO Auto-generated method stub

	}

	public TextView getmTextViewName() {
		return mTextViewName;
	}

	public void setmTextViewName(TextView mTextViewName) {
		this.mTextViewName = mTextViewName;
	}

	class LooperThread extends Thread {
		public void run() {
			super.run();
			try {
				do {// get the time of system interval one second
					Time t = new Time();
					t.setToNow();
					// int year = t.year;
					// int month = t.month;
					// int date = t.monthDay
					mHour = t.hour; // 0-23
					mMinutes = t.minute;
					second = t.second;
					Thread.sleep(1000);

					Message m = new Message();
					m.what = 1;
					DateTimeInputView.this.mHandler.sendMessage(m);
				} while (true);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
}
