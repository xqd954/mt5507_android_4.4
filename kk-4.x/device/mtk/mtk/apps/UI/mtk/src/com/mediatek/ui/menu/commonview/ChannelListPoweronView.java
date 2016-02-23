package com.mediatek.ui.menu.commonview;

import java.util.List;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVTimerManager;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter;
import com.mediatek.ui.menu.util.EditChannel;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.util.MtkLog;

/**
 * A view to show power on channel interface and block channel in parental
 * control
 * 
 * @author MTK40462
 * 
 */
public class ChannelListPoweronView extends ListViewItemView implements
		RespondedKeyEvent {

	private static final String TAG = "ChannelListPoweronView";
	private TextView num, isAnalog, name;
	private ImageView radio;

	// private int preSelectPosition = 1;
	private boolean isChannelBlock = false;

	private EditChannel editChannel;
	private ChannelListPoweronView preSelectView;

	private static int powerOn_picid = 0;

	public int getPowerOn_picid() {
		return powerOn_picid;
	}

	public void setPowerOn_picid(int powerOn_picid) {
		ChannelListPoweronView.powerOn_picid = powerOn_picid;
	}

	private List<SetConfigListViewAdapter.DataItem> mSubChildGroup;

	public List<SetConfigListViewAdapter.DataItem> getSubChildGroup() {
		return mSubChildGroup;
	}

	public void setmGroup(List<SetConfigListViewAdapter.DataItem> mSubChildGroup) {
		this.mSubChildGroup = mSubChildGroup;
	}

	public ChannelListPoweronView(Context context) {
		super(context);
		this.context = context;
		init();
	}

	public ChannelListPoweronView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		init();
	}

	public ChannelListPoweronView(Context context, AttributeSet attrs,
			int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		init();
	}

	public void init() {
		LinearLayout lv = (LinearLayout) inflate(mContext,
				R.layout.menu_channel_poweron_view, null);
		LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
		addView(lv, params);

		num = (TextView) findViewById(R.id.common_channelview_text_num);
		isAnalog = (TextView) findViewById(R.id.common_channelview_text_system);
		name = (TextView) findViewById(R.id.common_channelview_text_type);
		radio = (ImageView) findViewById(R.id.common_channel_poweron_radio);

		editChannel = EditChannel.getInstance(context);

	}

	public void changeBackGround(boolean isSelected) {
		if (isSelected) {
			radio.setImageResource(R.drawable.poweron_channel_selected);
		} else {
			radio.setImageResource(R.drawable.poweron_channel_normal);
		}
	}

	private String[] mData;// channel information array

	// setting position

	public void setAdapter(SetConfigListViewAdapter.DataItem mDataItem) {
		this.mDataItem = mDataItem;
		mData = mDataItem.getmOptionValue();
		setChannelData();
		mId = mDataItem.getmItemID();
		this.mSubChildGroup = mDataItem.getmSubChildGroup();

		if (mDataItem.mBrightBackGroundFlag == true) {
			radio.setImageResource(R.drawable.poweron_channel_selected);
		} else {
			radio.setImageResource(R.drawable.poweron_channel_normal);
		}
		if (mId.equals(MenuConfigManager.PARENTAL_CHANNEL_BLOCK_CHANNELLIST)) {
			short ch_num = (short) Integer.parseInt(mData[0]);
			if (editChannel.isChannelBlock(ch_num)) {
				MtkLog.d(TAG, ch_num + " channel is blocked");
				radio.setImageResource(R.drawable.poweron_channel_selected);
			}
		}
	}

	public void setChannelData() {
		num.setText(mData[0]);
		isAnalog.setText(mData[1]);
		name.setText(mData[4]);
	}

	public void onKeyRed() {

	}

	public void onKeyGreen() {

	}

	public int getValue() {
		return 0;
	}

	public void onKeyEnter() {
		// current options is the start channel
		if (getmId().equals(MenuConfigManager.SETUP_POWER_ONCHANNEL_LIST)) {
			short ch_num = (short) Integer.parseInt(mData[0]);
			TVContent tvContent = TVContent.getInstance(mContext);
			TVTimerManager mTVTimerManager = tvContent.getTimerManager();
			mTVTimerManager.setPowerOnChannel(ch_num);

		} else if (getmId().equals(
				MenuConfigManager.PARENTAL_CHANNEL_BLOCK_CHANNELLIST)) {
			// current options is the block channel

			short ch_num = (short) Integer.parseInt(mData[0]);

			// judge selected channel state whether block
			isChannelBlock = editChannel.isChannelBlock(ch_num);
			MtkLog.d(TAG, " channel block previous state: " + isChannelBlock);

			// selected channel is block state
			if (isChannelBlock) {
				editChannel.blockChannel(ch_num, false);
			} else {
				editChannel.blockChannel(ch_num, true);
			}
			// to judge the selected channel block state
			isChannelBlock = editChannel.isChannelBlock(ch_num);
			changeBackGround(isChannelBlock);
			MtkLog.d(TAG, "channel block current state: " + isChannelBlock);
		}

	}

	public void onKeyLeft() {
	}

	public void onKeyRight() {
	}

	public void setValue(int value) {
	}

	public void showValue(int value) {
	}

	public void setPreSelectView(ChannelListPoweronView v) {
		preSelectView = v;
	}

}
