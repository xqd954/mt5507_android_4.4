package com.mediatek.ui.menu.commonview;

import java.util.List;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.mediatek.ui.R;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter;

/**
 * This view includes a text view (the name) and a image view(selected or not),
 * it is one item of list view items. you can press ENTER or RIGHT key to show
 * its sub view
 * 
 * @author hs_yaoxiao
 */

public class HaveSubPageView extends ListViewItemView implements
		RespondedKeyEvent {

	private TextView mNameView;
	private List<SetConfigListViewAdapter.DataItem> mSubChildGroup;

	public HaveSubPageView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		init();
	}

	public HaveSubPageView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		init();
	}

	public HaveSubPageView(Context context) {
		super(context);
		this.context = context;
		init();
	}

	public void setValue(int value) {
	}

	public int getValue() {
		return 0;
	}

	public void showValue(int value) {
	}

	public TextView getNameView() {
		return mNameView;
	}

	private void setViewName(String viewName) {
		mNameView.setText(viewName);
	}

	private void init() {
//		this.addView(inflate(context, R.layout.menu_hava_subpage_view, null));
		LinearLayout lv = (LinearLayout) inflate(mContext,
				R.layout.menu_hava_subpage_view, null);
		LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
		addView(lv, params);

		mNameView = (TextView) findViewById(R.id.common_itemname);
	}

	public void setAdapter(SetConfigListViewAdapter.DataItem mDataItem) {
		this.mDataItem = mDataItem;
		setViewName(mDataItem.getmName());
		this.mSubChildGroup = mDataItem.getmSubChildGroup();
	}

	public void onKeyEnter() {
	}

	public void onKeyLeft() {
	}

	public void onKeyRight() {
	}

}
