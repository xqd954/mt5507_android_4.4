package com.mediatek.ui.menu.commonview;

import android.content.Context;
import android.graphics.Color;
import android.util.AttributeSet;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.mediatek.ui.R;
import com.mediatek.ui.menu.MenuMain;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.MenuDataContainer;

/**
 * This view includes a text view (the name), a progress bar(the current
 * progress) and the second text view(the current value), it is one item of list
 * view items. you can press LEFT or RIGHT key to adjust the progress, press
 * ENTER to enter the ChangeValueBarView interface.
 * 
 * author MTK40462
 */
public class ProgressView extends ListViewItemView implements RespondedKeyEvent {

	// the item name, progress and value of list view item
	private TextView mNameView;
	private MyProgressBar mProgressView;
	private TextView mValueView;
	private OnValueChangedListener mValueChangedListener;
	private int mPostion;
	private int mOffset;
	
	private MenuConfigManager mConfigManager;
	/*
	 * when press LEFT or RIGHT, the decrease or increase value every time
	 * default value is 1
	 */
	private int mStepValue;
	private int listPosition;

	public ProgressView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		init();
	}

	public ProgressView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		init();
	}

	public ProgressView(Context context) {
		super(context);
		this.context = context;
		init();
	}

	public TextView getNameView() {
		return mNameView;
	}

	public ProgressBar getProgressView() {
		return mProgressView;
	}

	public TextView getValueView() {
		return mValueView;
	}

	private void setViewName(String viewName) {
		mNameView.setText(viewName);
	}

	public void setValueChangedListener(
			OnValueChangedListener mValueChangedListener) {
		this.mValueChangedListener = mValueChangedListener;
	}

	// read current value
	public int getValue() {
		return mDataItem == null ? 0 : mDataItem.mInitValue;
	}

	private void init() {
		mConfigManager = MenuConfigManager.getInstance(context);
		LinearLayout lv = (LinearLayout) inflate(mContext,
				R.layout.menu_progress_view, null);
		LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
		addView(lv, params);

		
		mNameView = (TextView) findViewById(R.id.common_tv_itemname);
		mProgressView = (MyProgressBar) findViewById(R.id.common_ps_itemshow);
		mValueView = (TextView) findViewById(R.id.common_tv_itemvalue);
		mProgressView
				.setOnProgressBarChangeListener(new MyProgressBar.OnProgressBarChangeListener() {

					public void onStopTrackingTouch(MyProgressBar myProgressBar) {
						// TODO Auto-generated method stub
						((MenuMain) context).setIsMouse(true);
						((MenuMain) context).seekMouthFocus(listPosition, -1);
					}

					public void onStartTrackingTouch(MyProgressBar myProgressBar) {
						// TODO Auto-generated method stub
						listPosition = ((MenuMain) context)
								.getSelectedIndexByID(mId);
					}

					public void onProgressChanged(MyProgressBar myProgressBar,
							int progress, boolean fromUser) {						
						if (fromUser) {
							setValue(progress - mOffset);

							DataItem parentDataItem = mDataItem.mParent;
							if (parentDataItem != null
									&& parentDataItem.getmInitValue() != parentDataItem.userDefined) {
								MenuSetUpListView mListView = ((MenuSetUpListView) ProgressView.this.mParent);
								if (mListView != null) {
									EffectOptionView view = ((EffectOptionView) mListView
											.getChildAt(parentDataItem
													.getPosition()));
									view
											.showValue(MenuDataContainer
													.getInstance(mContext)
													.getValueById(
															parentDataItem
																	.getmItemID()));
									}
							}
						}
					}
				});
	}

	public void setAdapter(SetConfigListViewAdapter.DataItem mDataItem) {
		this.mDataItem = mDataItem;
		mId = mDataItem.getmItemID();
		mOffset = -mDataItem.getmStartValue();
		mProgressView.setMax(mDataItem.getmEndValue()
				- mDataItem.getmStartValue());
		mPostion = mDataItem.getmInitValue() + mOffset;
		mStepValue = mDataItem.getmStepValue();
		mProgressView.setProgress(mPostion);
		showValue(mDataItem.getmInitValue());
		setViewName(mDataItem.getmName());
		if(MenuConfigManager.TV_SINGLE_SCAN_SIGNAL_LEVEL.equals(mDataItem.mItemID)){
			mNameView.setTextColor(Color.WHITE);
			mValueView.setTextColor(Color.WHITE);
		}
	}

	private void switchValuePrevious() {
	        resetColorTempUser();
		if (mPostion > 0) {
			mPostion = mPostion - mStepValue;
			mDataItem.mInitValue = mPostion - mOffset;
			showValue(mDataItem.mInitValue);
			if (mValueChangedListener != null) {
				mValueChangedListener.onValueChanged(this,
						(mDataItem.mInitValue));
			}
		}
	}

	private void switchValueNext() {
	        resetColorTempUser();
		if (mPostion < mProgressView.getMax()) {
			mPostion = mPostion + mStepValue;
			mDataItem.mInitValue = mPostion - mOffset;
			showValue(mDataItem.mInitValue);
			if (mValueChangedListener != null) {
				mValueChangedListener
						.onValueChanged(this, mDataItem.mInitValue);
			}
		}
	}

	public void onKeyEnter() {
	}

	public void onKeyLeft() {
		switchValuePrevious();
	}

	public void onKeyRight() {
		switchValueNext();
	}

	public void setValue(int mInitValue) {
		mPostion = mInitValue + mOffset;
		mDataItem.mInitValue = mInitValue;
		showValue(mDataItem.mInitValue);
		if (mValueChangedListener != null) {
			mValueChangedListener.onValueChanged(this, mDataItem.mInitValue);
		}
	}

	public void showValue(int value) {
		mValueView.setText("" + value);
		mDataItem.mInitValue = value;
		mPostion = value + mOffset;
		mProgressView.setProgress(mPostion);
	}

	private void resetColorTempUser(){		
		if (mDataItem.mItemID.equals(MenuConfigManager.COLOR_G_R)
				|| mDataItem.mItemID.equals(MenuConfigManager.COLOR_G_G)
				|| mDataItem.mItemID.equals(MenuConfigManager.COLOR_G_B)) {
			if (mDataItem.mParentGroup.size() < 6) {//factory mode have six data item
			mConfigManager.setValue(MenuConfigManager.COLOR_TEMPERATURE, 0);		
		}
		}
	}

}