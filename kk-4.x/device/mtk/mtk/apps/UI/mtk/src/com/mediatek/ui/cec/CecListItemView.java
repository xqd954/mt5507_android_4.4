package com.mediatek.ui.cec;

import android.content.Context;
import android.util.AttributeSet;
import android.view.WindowManager;
import android.widget.ListView;
import android.widget.RelativeLayout;

import com.mediatek.ui.cec.CecListviewAdapter.CecDataItem;
import com.mediatek.ui.util.ScreenConstant;

public class CecListItemView extends RelativeLayout {
	protected Context context;
	protected String mId;
	protected CecDataItem mDataItem;

	public CecListItemView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
	}

	public CecListItemView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
	}

	public String getmId() {
		return mId;
	}

	public void setmId(String mId) {
		this.mId = mId;
	}

	public CecDataItem getmDataItem() {
		return mDataItem;
	}

	public void setmDataItem(CecDataItem mDataItem) {
		this.mDataItem = mDataItem;
	}

	public CecListItemView(Context context) {
		super(context);
		this.context = context;
		ListView.LayoutParams params = new ListView.LayoutParams(
                ListView.LayoutParams.MATCH_PARENT, getSize());
		setLayoutParams(params);
		setFocusable(true);
		setClickable(true);
		setEnabled(true);
	}

	public int getSize() {
		return ScreenConstant.SCREEN_HEIGHT / 20;
	}

}
