package com.mediatek.ui.menu.commonview;

import java.util.List;

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.AbsSeekBar;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.SeekBar.OnSeekBarChangeListener;

import com.mediatek.ui.R;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem.DataType;
import com.mediatek.ui.menu.commonview.MyProgressBar.OnProgressBarChangeListener;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.util.MtkLog;

/**
 * A view that includes a text view (the name), a progress bar or seek bar(the
 * current progress) and the second text view(the current value). you can press
 * left or right key to adjust the progress, press ENTER or EXIT to exit this
 * view. when you press ENTER key in selected progress view or position view,
 * this view will pop out.
 * 
 * author MTK40462
 */
public class ChangeValueBarView extends LinearLayout {
	private static final String TAG = "ChangeValueBarView";

	private ProChangeListener mListener;
	private Context mContext;
	private Handler mHandler;

	// the item name, progress and value of pop-out Progress or Position View
	private TextView mNameTextView;
	private AbsSeekBar mShowView;
	private TextView mValueView;
	private ImageView mImageView;

	private DataItem mData;

	private List<DataItem> mDataList;

	private int mPosition;
	private int mStepValue;
	private int mOffset;

	private Integer[] mImageIds = { R.drawable.slider_brightness_icon,
			R.drawable.slider_contrast_icon, R.drawable.slider_saturation_icon,
			R.drawable.slider_hue_icon };
	private String[] mItemNames = { MenuConfigManager.BRIGHTNESS,
			MenuConfigManager.CONTRAST, MenuConfigManager.SATURATION,
			MenuConfigManager.HUE };

	public void setHandler(Handler mHandler) {
		this.mHandler = mHandler;
	}

	public ChangeValueBarView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.mContext = context;

	}

	public ChangeValueBarView(Context context) {
		super(context);
		this.mContext = context;
	}

	/*
	 * add this view to view group and show this view
	 */
	private void initUI() {
		LinearLayout view = (LinearLayout)inflate(mContext, R.layout.menu_change_value_bar, null);
		view.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT,LayoutParams.WRAP_CONTENT));
		addView(view);

		mNameTextView = (TextView) findViewById(R.id.menu_arg_title);
		mValueView = (TextView) findViewById(R.id.menu_arg_value);
		mImageView = (ImageView) findViewById(R.id.menu_arg_image);

		// if SeekBar or ProgressBar is not null, set it invisible
		if (mShowView != null) {
			mShowView.setVisibility(View.GONE);
		}
		if (mData.getDataType() == DataItem.DataType.POSITIONVIEW) {
			mShowView = (SeekBar) findViewById(R.id.men_arg_sb);

			((SeekBar) mShowView)
					.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

						public void onProgressChanged(SeekBar seekBar,
								int progress, boolean fromUser) {
							// TODO Auto-generated method stub
							setValue(progress);
						}

						public void onStartTrackingTouch(SeekBar seekBar) {
							// TODO Auto-generated method stub
							ChangeValueBarView.this.requestFocus();
							ChangeValueBarView.this.requestFocusFromTouch();
						}

						public void onStopTrackingTouch(SeekBar seekBar) {
							// TODO Auto-generated method stub
							ChangeValueBarView.this.requestFocus();
							ChangeValueBarView.this.requestFocusFromTouch();
						}

					});
		} else {
			mShowView = (MyProgressBar) findViewById(R.id.menu_arg_pb);
			((MyProgressBar) mShowView)
					.setOnProgressBarChangeListener(new OnProgressBarChangeListener() {

						public void onProgressChanged(
								MyProgressBar myProgressBar, int progress,
								boolean fromUser) {
							// TODO Auto-generated method stub
							setValue(progress);
						}

						public void onStartTrackingTouch(
								MyProgressBar myProgressBar) {
							// TODO Auto-generated method stub
							ChangeValueBarView.this.requestFocus();
							ChangeValueBarView.this.requestFocusFromTouch();
						}

						public void onStopTrackingTouch(
								MyProgressBar myProgressBar) {
							// TODO Auto-generated method stub
							ChangeValueBarView.this.requestFocus();
							ChangeValueBarView.this.requestFocusFromTouch();
						}

					});
		}
		mShowView.setVisibility(View.VISIBLE);
		initImageView(mData);
		initValue();
	}

	/*
	 * get progress view or position view initial value, offset value and max
	 * value, set current progress
	 */
	private void initValue() {
		mOffset = -mData.getmStartValue();
		mPosition = mData.getmInitValue() + mOffset;
		// mShowView.setMax(mData.getmEndValue() - mData.getmStartValue());
		mStepValue = mData.getmStepValue();
		mShowView.setProgress(mPosition);
		showValue(mData.mInitValue);
		setViewName(mData.getmName());
		mShowView.setMax(mData.getmEndValue() - mData.getmStartValue());
		mShowView.setVisibility(View.VISIBLE);
		if (mListener == null) {
			mListener = new ProChangeListener();
		}
	}

	public void setAdapter(DataItem mData) {
		this.mData = mData;
		initUI();
	}

	public void setAdapter(List<DataItem> mDataList) {
		this.mDataList = mDataList;
	}

	public void setViewName(String viewName) {
		mNameTextView.setText(viewName);
	}

	/*
	 * when press left or right key, the value will change larger or smaller
	 */
	private void changeValue(int keyCode) {
		if (keyCode == KeyEvent.KEYCODE_DPAD_LEFT && mPosition > 0) {
			mPosition = mPosition - mStepValue;
		} else if (keyCode == KeyEvent.KEYCODE_DPAD_RIGHT
				&& mPosition < mShowView.getMax()) {
			mPosition = mPosition + mStepValue;
		}
		if (mData.mInitValue != (mPosition - mOffset)) {
			mData.mInitValue = mPosition - mOffset;
			mShowView.setProgress(mPosition);
			showValue(mData.mInitValue);

			/*
			 * Register a value changed listener, when value is changed in UI,
			 * notice TV Manager do corresponding action
			 */
			if (mListener != null) {
				mListener.onValueChanged(this, mData.mInitValue);
			} else {
				MtkLog.d(TAG, "Change Value Bar View Lisenter is null");
			}
		}
	}

	private void setValue(int value) {
		if ((value - mOffset) < mData.getmStartValue()
				|| (value - mOffset) > mData.getmEndValue()) {
			return;
		}
		mPosition = value;
		if (mData.mInitValue != (mPosition - mOffset)) {
			mData.mInitValue = mPosition - mOffset;
			mShowView.setProgress(mPosition);
			showValue(mData.mInitValue);

			/*
			 * Register a value changed listener, when value is changed in UI,
			 * notice TV Manager do corresponding action
			 */
			if (mListener != null) {
				mListener.onValueChanged(this, mData.mInitValue);
			} else {
				MtkLog.d(TAG, "Change Value Bar View Lisenter is null");
			}
		}
	}

	public void showValue(int value) {
		mValueView.setText("" + value);
	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_DPAD_LEFT
				|| keyCode == KeyEvent.KEYCODE_DPAD_RIGHT) {
			this.mHandler.removeMessages(MessageType.WAIT_DELAY_MESSAGE);
			this.mHandler.sendEmptyMessageDelayed(
					MessageType.WAIT_DELAY_MESSAGE, 30000);
			changeValue(keyCode);
			return true;
		} else if (keyCode == KeyEvent.KEYCODE_BACK
				|| keyCode == KeyEvent.KEYCODE_DPAD_CENTER
				|| keyCode == KeyEvent.KEYCODE_ENTER) {
			this.mHandler.removeMessages(MessageType.WAIT_DELAY_MESSAGE);
			Message msg = new Message();
			msg.what = MessageType.MESSAGE1;
			msg.arg1 = getSelectPosition();
			MtkLog.d(TAG, "------ select item postion-------- " + msg.arg1);
			this.mHandler.sendMessage(msg);
			return true;
		} else if (keyCode == KeyEvent.KEYCODE_DPAD_DOWN
				|| keyCode == KeyEvent.KEYCODE_DPAD_UP) {
			this.mHandler.removeMessages(MessageType.WAIT_DELAY_MESSAGE);
			this.mHandler.sendEmptyMessageDelayed(
					MessageType.WAIT_DELAY_MESSAGE, 30000);
			changeOption(keyCode);
			return true;
		}
		return super.onKeyDown(keyCode, event);
	}

	public class ProChangeListener implements OnValueChangedListener {

		public void onValueChanged(View v, int value) {
			// TODO Auto-generated method stub
			ChangeValueBarView view = (ChangeValueBarView) v;

			// get current view id
			String selId = view.mData.mItemID;
			MtkLog.d(TAG, "Item name: " + selId + "   value: " + value);

			MenuConfigManager mg = MenuConfigManager.getInstance(mContext);
			if (selId != null) {
				if (selId.startsWith("UNDEFINE") || selId.startsWith("TVSCAN")) {

				} else if (selId.startsWith("SETUP")) {
					mg.setSetup(selId, value);
				} else {
					mg.setValue(selId, value);
				}
			} else {
				MtkLog.e(TAG, "Item " + selId + " return null");
			}
		}
	}

	private int getSelectPosition() {
		for (int i = 0; i < mDataList.size(); i++) {
			if (mData.mItemID.equals(mDataList.get(i).getmItemID())) {
				return i;
			}
		}
		return 0;
	}

	private void changeOption(int keyCode) {
		int position = getSelectPosition();
		if (keyCode == KeyEvent.KEYCODE_DPAD_UP) {
			while (true) {
				position--;
				if (position < 0) {
					position = mDataList.size() - 1;
				}
				if ((mDataList.get(position).getDataType() == DataType.POSITIONVIEW || mDataList
						.get(position).getDataType() == DataType.PROGRESSBAR)
						&& mDataList.get(position).isEnable) {
					break;
				}

			}

		} else if (keyCode == KeyEvent.KEYCODE_DPAD_DOWN) {
			while (true) {
				position++;
				if (position >= mDataList.size()) {
					position = 0;
				}
				if ((mDataList.get(position).getDataType() == DataType.POSITIONVIEW || mDataList
						.get(position).getDataType() == DataType.PROGRESSBAR)
						&& mDataList.get(position).isEnable) {
					break;
				}
			}
		}
		mData = mDataList.get(position);
		if (mData.getDataType() == DataItem.DataType.POSITIONVIEW) {
			mShowView.setVisibility(View.GONE);
			mShowView = (SeekBar) findViewById(R.id.men_arg_sb);
		} else if (mData.getDataType() == DataItem.DataType.PROGRESSBAR) {
			mShowView.setVisibility(View.GONE);
			mShowView = (MyProgressBar) findViewById(R.id.menu_arg_pb);
		}
		initImageView(mData);
		initValue();
	}

	/**
	 * show Data Item image in left
	 * 
	 * @param mData
	 */
	private void initImageView(DataItem mData) {
		int index = getImageIndex(mData);
		if (index >= 0) {
			mImageView.setImageResource(mImageIds[index]);
		} else {
			mImageView.setImageResource(R.drawable.transparent_background);
		}

	}

	private int getImageIndex(DataItem mData) {
		for (int i = 0; i < mItemNames.length; i++) {
			if (mItemNames[i].equals(mData.mItemID)) {
				return i;
			}
		}
		return -1;
	}
}
