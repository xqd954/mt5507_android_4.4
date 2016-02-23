package com.mediatek.ui.mmp.commonview;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.GridView;

import com.mediatek.ui.util.MtkLog;

public class MediaGridView extends GridView {
	public MediaGridView(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	public MediaGridView(Context context) {
		this(context, null);

	}

	protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		super.onMeasure(widthMeasureSpec, heightMeasureSpec);
		MtkLog.i("onMeasure", getWidth() + ":" + getHeight());
	}

	public void setNumColumns(int numColumns) {
		super.setNumColumns(numColumns);
	}

}
