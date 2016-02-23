package com.mediatek.ui.menu.commonview;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;

import com.mediatek.ui.R;
import com.mediatek.ui.menu.MenuMain;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem;
import com.mediatek.ui.menu.util.MenuDataContainer;

/**
 * This view includes a text view (the name), a seek bar(the current progress)
 * and the second text view(the current value), it is one item of list view
 * items. you can press LEFT or RIGHT key to adjust the progress, press ENTER to
 * enter the ChangeValueBarView interface.
 * 
 * @author hs_yaoxiao
 */

public class PositionView extends ListViewItemView implements RespondedKeyEvent {

	// the item name, seek bar and value of list view item
	private TextView mNameView;
	private SeekBar mSeekbarView;
	private TextView mValueView;
	private DataItem mParentDataItem;
	private int mParentInitValue;
	private int mPostion;
	private int mStepValue;
	private OnValueChangedListener mValueChangedListener;
	private int mOffset;
	private int listPosition;

	public PositionView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		init();
	}

	public PositionView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		init();
	}

	public PositionView(Context context) {
		super(context);
		this.context = context;
		init();
	}

	public int getValue() {
		return mDataItem.mInitValue;
	}

	private void setViewName(String viewName) {
		mNameView.setText(viewName);
	}

	public void setValue(int mValue) {
		mPostion = mValue + mOffset;
		mDataItem.mInitValue = mValue;
		mSeekbarView.setProgress(mPostion);
		if (mValueChangedListener != null) {
			mValueChangedListener.onValueChanged(this, mDataItem.mInitValue);
		}
		showValue(mDataItem.mInitValue);
	}

	public void setValueChangedListener(
			OnValueChangedListener mValueChangedListener) {
		this.mValueChangedListener = mValueChangedListener;
	}

	private void init() {
		LinearLayout lv = (LinearLayout) inflate(mContext,
				R.layout.menu_position_view, null);
		LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
		addView(lv, params);

		mNameView = (TextView) findViewById(R.id.common_tv_itemname);
		mSeekbarView = (SeekBar) findViewById(R.id.common_sb_itemshow);
		mValueView = (TextView) findViewById(R.id.common_tv_itemvalue);
	}

	public void setAdapter(final SetConfigListViewAdapter.DataItem mDataItem) {
		this.mDataItem = mDataItem;
		if (mDataItem.mParent != null) {
			mParentDataItem = mDataItem.mParent;
			mParentInitValue = mParentDataItem.userDefined;
		}
		mId = mDataItem.getmItemID();
		mOffset = -mDataItem.getmStartValue();
		mSeekbarView.setMax(mDataItem.getmEndValue()
				- mDataItem.getmStartValue());
		mStepValue = mDataItem.getmStepValue();
		mPostion = mOffset + mDataItem.getmInitValue();
		mSeekbarView.setProgress(mPostion);
		showValue(mDataItem.mInitValue);
		setViewName(mDataItem.getmName());

		mSeekbarView.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				// TODO Auto-generated method stub
				if (fromUser) {
					setValue(progress - mOffset);

					DataItem parentDataItem = mDataItem.mParent;
					if (parentDataItem != null
							&& parentDataItem.getmInitValue() != parentDataItem.userDefined) {
						MenuSetUpListView mListView = ((MenuSetUpListView) PositionView.this.mParent);
						if (mListView != null) {
							EffectOptionView view = ((EffectOptionView) mListView
									.getChildAt(parentDataItem.getPosition()));
							view.showValue(MenuDataContainer.getInstance(
									mContext).getValueById(
									parentDataItem.getmItemID()));
						}
					}
				}				
			}

			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
				listPosition = ((MenuMain) context).getSelectedIndexByID(mId);
			}

			public void onStopTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
				((MenuMain) context).setIsMouse(true);
				((MenuMain) context).seekMouthFocus(listPosition, -1);
			}

		});
	}

	private void switchValuePrevious() {
		if (mPostion != 0) {
			mPostion = mPostion - mStepValue;
			mDataItem.mInitValue = mPostion - mOffset;
			showValue(mDataItem.mInitValue);
			if (mValueChangedListener != null) {
				mValueChangedListener
						.onValueChanged(this, mDataItem.mInitValue);
			}
		}
	}

	private void switchValueNext() {
		if (mPostion != mSeekbarView.getMax()) {
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

	public void showValue(int value) {
		mValueView.setText("" + value);
		mDataItem.mInitValue = value;
		mPostion = value + mOffset;
		mSeekbarView.setProgress(mPostion);
	}

	public void setSeekBarSelect(boolean b) {
		if (b) {
			mSeekbarView.setThumb(getResources().getDrawable(R.drawable.seekbar_shape_selected));
		} else {
			mSeekbarView.setThumb(getResources().getDrawable(R.drawable.seekbar_shape_default));
		}
	}

}
