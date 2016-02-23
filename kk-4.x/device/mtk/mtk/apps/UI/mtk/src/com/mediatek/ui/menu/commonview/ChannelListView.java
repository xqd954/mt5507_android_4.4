package com.mediatek.ui.menu.commonview;

import java.util.List;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.mediatek.ui.R;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter;
import com.mediatek.ui.util.MtkLog;

public class ChannelListView extends ListViewItemView implements
		RespondedKeyEvent {

	private static final String TAG = "ChannelListView";
	private TextView num, type, freq, system, name;
	// private ImageView skip

	private ImageView skip;

	private List<SetConfigListViewAdapter.DataItem> mSubChildGroup;

	public List<SetConfigListViewAdapter.DataItem> getSubChildGroup() {
		return mSubChildGroup;
	}

	public void setmGroup(List<SetConfigListViewAdapter.DataItem> mSubChildGroup) {
		this.mSubChildGroup = mSubChildGroup;
	}

	public ChannelListView(Context context) {
		super(context);
		this.context = context;
		init();
	}

	public ChannelListView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		init();
	}

	public ChannelListView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		init();
	}

	public void init() {
		LinearLayout lv = (LinearLayout) inflate(mContext,
				R.layout.menu_channel_view, null);
		LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
		addView(lv, params);

		num = (TextView) findViewById(R.id.common_channelview_text_num);
		type = (TextView) findViewById(R.id.common_channelview_text_type);
		freq = (TextView) findViewById(R.id.common_channelview_text_freq);
		system = (TextView) findViewById(R.id.common_channelview_text_system);
		name = (TextView) findViewById(R.id.common_channelview_text_name);
		skip = (ImageView) findViewById(R.id.common_channelview_pic_skip);
	}

	/**
	 * data
	 * 
	 * @param mDataItem
	 */
	private String[] mData;// channel information array

	// setting position
	public void setAdapter(SetConfigListViewAdapter.DataItem mDataItem) {
		this.mDataItem = mDataItem;
		mData = mDataItem.getmOptionValue();
		setChannelData();
		mId = mDataItem.getmItemID();
		this.mSubChildGroup = mDataItem.getmSubChildGroup();
		changeBackGround();
	}

	public void setChannelData() {
		num.setText(mData[0]);
		type.setText(mData[1]);
		freq.setText(mData[2]);
		system.setText(mData[3]);
		MtkLog.v(TAG, ":" + mData[4]);
		if (mData[4].length() == 0) {
			name.setText("________");
		} else {
			name.setText(mData[4]);
		}
	}

	public void changeBackGround() {
		if (mDataItem.mBrightBackGroundFlag) {
			skip.setImageResource(R.drawable.poweron_channel_selected);
		} else {
			skip.setImageResource(R.drawable.poweron_channel_normal);
		}
	}

	// mobile channel
	public void onKeyRed() {
	}

	// insertion of channel
	public void onKeyGreen() {
	}

	public int getValue() {
		// TODO Auto-generated method stub
		return 0;
	}

	public void onKeyEnter() {
		// TODO Auto-generated method stub
	}

	public void onKeyLeft() {
		// TODO Auto-generated method stub
	}

	public void onKeyRight() {
		// TODO Auto-generated method stub
	}

	public void setValue(int value) {
		// TODO Auto-generated method stub
	}

	public void showValue(int value) {
		// TODO Auto-generated method stub
	}

}
