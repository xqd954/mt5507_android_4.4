package com.mediatek.timeshift_pvr.ui;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.ListView;
import android.widget.RelativeLayout;

import com.mediatek.timeshift_pvr.manager.SetConfigListViewAdapter.DataItem;
import com.mediatek.ui.util.ScreenConstant;

public class ListViewItemView extends RelativeLayout {
	protected Context context;
	protected String mId;
	protected DataItem mDataItem;

	public ListViewItemView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
	}

	public ListViewItemView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
	}

	public String getmId() {
		return mId;
	}

	public void setmId(String mId) {
		this.mId = mId;
	}

	public DataItem getmDataItem() {
		return mDataItem;
	}

	public void setmDataItem(DataItem mDataItem) {
		this.mDataItem = mDataItem;
	}

	public ListViewItemView(Context context) {
		super(context);
		this.context = context;
		setLayoutParams(new ListView.LayoutParams(
				ListView.LayoutParams.MATCH_PARENT, getSize()));
		setFocusable(true);
		setClickable(true);
		setEnabled(true);
	}

	public int getSize() {
		return ScreenConstant.SCREEN_HEIGHT / 13;
	}

}
