package com.mediatek.ui.menu.commonview;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.mediatek.ui.R;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem;
import com.mediatek.ui.util.MtkLog;

/**
 * FactoryProgressView is used in factory. This view includes a text view (the
 * name) and the second text view(for number), it is one item of list view
 * items. you can press LEFT or RIGHT key to change the value
 * 
 * @author hs_yaoxiao
 */

public class FactoryProgressView extends ListViewItemView implements
		RespondedKeyEvent {

	private static final String TAG = "FactoryProgressView";
	// the item name and value of list view item
	private TextView mNameTextView;
	private TextView mValueView;
	private DataItem mParentDataItem;
	private int mParentInitValue;
	private int mPostion;
	private int mStepValue;

	public FactoryProgressView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		init();
	}

	public FactoryProgressView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		init();
	}

	public FactoryProgressView(Context context) {
		super(context);
		this.context = context;
		init();
	}

	public TextView getmNameTextView() {
		return mNameTextView;
	}

	public TextView getmValueView() {
		return mValueView;
	}

	private void setViewName(String viewName) {
		mNameTextView.setText(viewName);
	}

	private OnValueChangedListener mValueChangedListener;

	public void setValueChangedListener(
			OnValueChangedListener mValueChangedListener) {
		this.mValueChangedListener = mValueChangedListener;
	}

	// read current value
	public int getValue() {
		return mDataItem == null ? 0 : mDataItem.mInitValue;
	}

	private void init() {
//		this
//				.addView(inflate(context, R.layout.menu_factory_progress_view,
//						null));
		LinearLayout lv = (LinearLayout) inflate(mContext,
				R.layout.menu_factory_progress_view, null);
		LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
		addView(lv, params);

		mNameTextView = (TextView) findViewById(R.id.factory_progress_tv_itemname);
		mValueView = (TextView) findViewById(R.id.factory_progress_tv_itemvalue);
	}

	public void setAdapter(SetConfigListViewAdapter.DataItem mDataItem) {
		this.mDataItem = mDataItem;
		if (mDataItem.mParent != null) {
			mParentDataItem = mDataItem.mParent;
			mParentInitValue = mParentDataItem.userDefined;
		}
		mId = mDataItem.getmItemID();
		mPostion = mDataItem.getmInitValue();
		mStepValue = mDataItem.getmStepValue();
		showValue(mDataItem.getmInitValue());
		setViewName(mDataItem.getmName());
	}

	private void switchValuePrevious() {
		MtkLog.v(TAG, "initvalue========" + mDataItem.getmInitValue());
		if (mPostion > 0) {
			mPostion = mPostion - mStepValue;
			mDataItem.mInitValue = mPostion;
			showValue(mDataItem.mInitValue);
			if (mValueChangedListener != null) {
				mValueChangedListener.onValueChanged(this,
						(mDataItem.mInitValue));
			}
		}
	}

	private void switchValueNext() {
		MtkLog.v(TAG, "initvalue========" + mDataItem.getmInitValue());
		if (mPostion < mDataItem.getmEndValue()) {
			mPostion = mPostion + mStepValue;
			mDataItem.mInitValue = mPostion;
			showValue(mDataItem.mInitValue);
			if (mValueChangedListener != null) {
				mValueChangedListener
						.onValueChanged(this, mDataItem.mInitValue);
			}
		}
	}

	public void onKeyEnter() {
	}

	// press LEFT
	public void onKeyLeft() {
		switchValuePrevious();
	}

	// press RIGHT
	public void onKeyRight() {
		switchValueNext();
	}

	public void setValue(int mInitValue) {
		mPostion = mInitValue;
		if (mValueChangedListener != null) {
			mValueChangedListener.onValueChanged(this, mInitValue);
		}
		showValue(mInitValue);
	}

	/**
	 * item show value
	 */
	public void showValue(int value) {
		mValueView.setText("" + value);
		mDataItem.mInitValue = value;
		mPostion = value;
	}

}
