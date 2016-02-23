package com.mediatek.ui.menu.commonview;



import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem;

import android.content.Context;
import android.util.AttributeSet;

/**
 * This view includes a text view (the name),the second text view(the current
 * value) and a ImageView(selected or not), it is one item of list view items.
 * when it changes value,its effect items change value too.
 * you can press LEFT or RIGHT key to change value of the second text view
 * 
 * @author hs_yaoxiao
 */

public class EffectOptionView extends OptionView {
	public EffectOptionView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	public EffectOptionView(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	public EffectOptionView(Context context) {
		super(context);
	}

	public void setAdapter(DataItem mDataItem) {
		super.setAdapter(mDataItem);
		mDataItem.getmEffectGroup();
		mDataItem.getmHashMap();
	}

	protected void switchValueNext() {
		super.switchValueNext();
	}

	protected void switchValuePrevious() {
		super.switchValuePrevious();
	}

}
