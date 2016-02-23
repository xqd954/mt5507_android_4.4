package com.mediatek.ui.menu.commonview;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.RelativeLayout.LayoutParams;

import com.mediatek.ui.R;
import com.mediatek.ui.menu.MenuMain;
import com.mediatek.ui.menu.MenuMain.IChannelNameChange;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem;
import com.mediatek.ui.menu.util.MenuConfigManager;

public class TextCommView extends ListViewItemView implements RespondedKeyEvent, IChannelNameChange {

	private TextView mNameTextView;
	private TextView textContent;
	private String mNum = "0000";
	private String mFreq = "";
	private String temp1 = "";
	private String temp2 = "";
	String TAG = "TextCommView";

	public String getmId() {
		return mId;
	}

	public void setmId(String mId) {
		this.mId = mId;
	}

	public TextCommView(Context context) {
		super(context);
		this.context = context;
		init();
	}

	public TextCommView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		init();
	}

	public TextCommView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		init();
	}

	public DataItem getmDataItem() {
		return mDataItem;
	}

	public void init() {
		LinearLayout lv = (LinearLayout) inflate(mContext,
				R.layout.menu_text_view, null);
		LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
		addView(lv, params);
		mNameTextView = (TextView) findViewById(R.id.common_text_itemname);
		textContent = (TextView) findViewById(R.id.common_text_content);
	}

	// time array
	private String[] mData;
	// set value position
	private int mPositon;

	public void setAdapter(SetConfigListViewAdapter.DataItem mDataItem) {
		this.mDataItem = mDataItem;
		mData = mDataItem.getmOptionValue();
		setViewName(mDataItem.getmName());
		mId = mDataItem.getmItemID();
		mPositon = mDataItem.getmInitValue();
		if (mPositon < 0) {
			if (("Signal Quality").equals(mId)) {
				textContent.setText("");
				return;
			}
		}
		if(mDataItem.getmItemID() == MenuConfigManager.TV_CHANNEL_NAME) {
			((MenuMain)this.context).setIChannelNameChangeListener(this);
			if(mData[mPositon] == null || mData[mPositon].length() == 0) {
				textContent.setText("________");
				return ;
			}
			
		}
		textContent.setText(mData[mPositon]);
	}

	public void inputNum(String ch) {
		textContent.setText(mNum);
		mNum = mNum + ch;
		mNum = mNum.substring(1, 5);
		textContent.setText(mNum);
		mData[mPositon] = mNum;
	}

	public void inputFreq(String ch) {
		mFreq = mFreq + ch;
		if (mFreq.length() > 3) {
			int c = 0;
			if (ch.equals(".")) {
				c = mFreq.indexOf(".");
				mFreq = mFreq.substring(0, c);
				mFreq = mFreq + ch;
			} else if (!mFreq.contains(".")) {
				mFreq = mFreq.substring(1, 4);
			} else if (mFreq.contains(".")) {
				if (mFreq.length() > 8) {
					temp1 = mFreq.substring(0, 4);
					temp2 = mFreq.substring(5, 9);
					mFreq = temp1 + temp2;
				}
			}
		}
		textContent.setText(mFreq);
		mData[mPositon] = mFreq;
	}

	public void setViewName(String name) {
		mNameTextView.setText(name);
	}

	public int getValue() {
		return 0;
	}

	public void onKeyEnter() {

	}

	public void onKeyLeft() {

	}

	public void onKeyRight() {
	}

	public void setValue(int value) {
	}

	public TextView getTextContent() {
		return textContent;
	}

	public void setTextContent(TextView textContent) {
		this.textContent = textContent;
	}

	public void showValue(int value) {
		if (value < mDataItem.mStartValue || value > mDataItem.mEndValue) {
			Log.v(TAG, "invalid value");
		} else {
			mDataItem.mInitValue = value;
			textContent.setText(mDataItem.mOptionValue[value]);
		}
	}

	public TextView getTextName() {
		return mNameTextView;
	}

	@Override
	public void nameChanged(String name) {
		if(mDataItem.getmItemID() == MenuConfigManager.TV_CHANNEL_NAME) {
			mData[mPositon] = name;
		}
	}
}
