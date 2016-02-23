package com.mediatek.ui.menu.commonview;

import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.SaveValue;
import com.mediatek.ui.util.MtkLog;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.TextView;

public class DateTimeView extends TextView {
	private String mDate;
	private char[] mDateChars;
	public final static int DATETYPE = 0;
	public final static int TIMETYPE = 1;
	private Context mContext;
	// the currently selected number in the string
	private int mCurrentSelectedPosition = -1;

	public int getCurrentSelectedPosition() {
		return mCurrentSelectedPosition;
	}

	public void setCurrentSelectedPosition(int mCurrentSelectedPosition) {
		this.mCurrentSelectedPosition = mCurrentSelectedPosition;
	}

	private float mWidth;
	private float mHeight;
	private float x;
	private float y;
	public int mType;

	public boolean flag = true;
	private Paint mPaint;
	private int mTextSize = 18;
	// skip the split line index
	private int mSplitIndex[];
	private UpdateTime updateTime;

	public int getmTextSize() {
		return mTextSize;
	}

	// private Handler mHandler =new Handler(){
	// public void handleMessage(Message msg) {
	// switch(msg.what){
	// case MessageType.MESSAGE_UPDATE_POWOFF_TIMER:
	// PowerOff mPowerOff =PowerOff.getInstance(mContext);
	// mPowerOff.updatePowerOff(mDate);
	// break;
	// default:
	// break;
	// }
	// };
	// };

	public DateTimeView(Context context, AttributeSet attrs) {
		super(context, attrs);
		mContext = context;
		// mPaint = new Paint();
	}

	public DateTimeView(Context context) {
		super(context);
		mContext = context;
	}

	protected void onFinishInflate() {
		// TODO Auto-generated method stub
		super.onFinishInflate();
	}

	public void setDateStr(String mDateStr, UpdateTime updateTime) {
		mDate = mDateStr;
		mDateChars = mDate.toCharArray();
		setmSplitIndex(mDateChars);
		this.updateTime = updateTime;
	}

	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);
		if (mDate != null) {
			mPaint = new Paint();
			mWidth = this.getWidth();
			mHeight = this.getHeight();
			x = (mWidth - mPaint.measureText(mDate)) / 2;
			// y = mHeight / 2;
			y = (mHeight + mPaint.measureText(mDateChars, 0, 1) * 1.2f) / 2;
			mPaint.setTextSize(mTextSize);
			mPaint.setFlags(Paint.ANTI_ALIAS_FLAG);
			mPaint.setAntiAlias(true);
			if (flag) {
				mPaint.setColor(Color.WHITE);
			} else {
				mPaint.setColor(Color.GRAY);
			}
			mPaint.setAlpha(220);
			mPaint.setTextSize(mTextSize);
			mPaint.setStyle(Paint.Style.FILL);
			canvas.drawText(mDate, x, y, mPaint);
			if (mCurrentSelectedPosition != -1) {
				if (flag) {
					mPaint.setColor(Color.YELLOW);
				} else {
					mPaint.setColor(Color.GRAY);
				}
				if (mCurrentSelectedPosition == 0) {
					canvas.drawText(mDate
							.substring(mCurrentSelectedPosition, 1), x, y,
							mPaint);
				} else {
					x = x
							+ mPaint.measureText(mDate.substring(0,
									mCurrentSelectedPosition));
					canvas.drawText(mDate.substring(mCurrentSelectedPosition,
							mCurrentSelectedPosition + 1), x, y, mPaint);

				}
			}
		}
	}

	public void onKeyLeft() {
		if (mCurrentSelectedPosition != 0) {
			--mCurrentSelectedPosition;
		}
		for (int index : mSplitIndex) {
			if (index == mCurrentSelectedPosition)
				--mCurrentSelectedPosition;
		}
		this.postInvalidate();
	}

	public void onKeyRight() {
		if (mCurrentSelectedPosition != mDate.length() - 1) {
			++mCurrentSelectedPosition;
		}
		for (int index : mSplitIndex) {
			if (index == mCurrentSelectedPosition)
				++mCurrentSelectedPosition;
		}
		this.postInvalidate();
	}

	public void input(char ch, DataItem mDataItem) {
		mDateChars = mDate.toCharArray();
		mDateChars[mCurrentSelectedPosition] = ch;
		String tempStr = String.valueOf(mDateChars);
		if (validate(tempStr, mCurrentSelectedPosition, ch, mType)) {
			mDate = tempStr;
			updateTime.onTimeModified(mDate);

			if (mType == DATETYPE) {
				if ((mCurrentSelectedPosition == 2 && Integer.valueOf(mDate
						.substring(2, 4)) > 37)
						|| (mCurrentSelectedPosition == 3
								&& mDateChars[2] == '3' && mDateChars[mCurrentSelectedPosition] > '7')
						|| ((mCurrentSelectedPosition == 2
								|| mCurrentSelectedPosition == 3
								|| mCurrentSelectedPosition == 5 || mCurrentSelectedPosition == 6)
								&& Integer.valueOf(mDate.substring(0, 4)) == 2000 && Integer
								.valueOf(mDate.substring(5, 7)) == 1)
						&& Integer.valueOf(mDate.substring(8, 10)) < 2) {
					changeNotValidate();
				}
			}
			if (mType == DATETYPE || mType == TIMETYPE) {
				SaveValue mSaveValue = SaveValue.getInstance(mContext);
				if (mSaveValue.readValue(MenuConfigManager.POWER_ON_TIMER) != 0) {
					String poweronTime = mSaveValue
							.readStrValue(MenuConfigManager.TIMER1);
					MTKPowerManager mMtkPowerManager = MTKPowerManager
							.getInstance(mContext);
					mMtkPowerManager.updatePowerOn(poweronTime);
				}
				if (mSaveValue.readValue(MenuConfigManager.POWER_OFF_TIMER) != 0) {
					String powerOffTime = mSaveValue
							.readStrValue(MenuConfigManager.TIMER2);
					MTKPowerManager mMtkPowerManager = MTKPowerManager
							.getInstance(mContext);
					mMtkPowerManager.updatePowerOff(powerOffTime);
				}
			}
		} else {
			changeNotValidate();
		}
		if (mDataItem.getmItemID().equals(MenuConfigManager.TIMER1)) {
			SaveValue.getInstance(mContext).saveStrValue(
					MenuConfigManager.TIMER1, mDate);
			MTKPowerManager mMTKPowerManager = MTKPowerManager
					.getInstance(mContext);
			mMTKPowerManager.updatePowerOn(mDate);
		}
		if (mDataItem.getmItemID().equals(MenuConfigManager.TIMER2)) {
			SaveValue.getInstance(mContext).saveStrValue(
					MenuConfigManager.TIMER2, mDate);
			// Message message = Message.obtain();
			// mHandler.removeMessages(MessageType.MESSAGE_UPDATE_POWOFF_TIMER);
			// mHandler.sendEmptyMessageDelayed(MessageType.MESSAGE_UPDATE_POWOFF_TIMER,
			// MessageType.delayMillis4);
			MTKPowerManager mMTKPowerManager = MTKPowerManager
					.getInstance(mContext);
			mMTKPowerManager.updatePowerOff(mDate);
		}
		onKeyRight();
	}

	public void setPaint(Paint mPaint) {
		this.mPaint = mPaint;
	}

	public int[] getmSplitIndex() {
		return mSplitIndex;
	}

	// Extraction of an array of characters of digital element subscript
	// position, can consider to use the regular representation of improvement
	public void setmSplitIndex(char[] mDataChar) {
		int[] temp = new int[mDataChar.length];
		int j = 0;
		int i = 0;
		for (; i < mDataChar.length; i++) {
			if (mDataChar[i] < '0' || mDataChar[i] > '9') {
				temp[j++] = i;
			}
		}
		i = 0;
		while (temp[i] != 0) {
			++i;
		}
		mSplitIndex = new int[i];
		for (j = 0; j < i; j++) {
			mSplitIndex[j] = temp[j];
		}
		temp = null;
	}

	public void setTextSize(int mTextSize) {
		this.mTextSize = mTextSize;
	}

	protected boolean isDateValidate(String time, int index, char value) {
		if (value > '9' || value < '0') {
			return false;
		}
		time = time.replace(time.charAt(index), value);
		String parten = "^(((("
				+ "|[2-9]\\d)\\d{2})/(0?[13578]|1[02])/(0?[1-9]|[12]\\d|3[01]))|(((1[6-9]|[2-9]\\d)\\d{2})/(0?[13456789]|1[012])/(0?[1-9]|[12]\\d|30))|(((1[6-9]|[2-9]\\d)\\d{2})/0?2/(0?[1-9]|1\\d|2[0-8]))|(((1[6-9]|[2-9]\\d)(0[48]|[2468][048]|[13579][26])|((16|[2468][048]|[3579][26])00))/0?2/29/))$";
		return time.matches(parten);
	}

	protected boolean isTimeValidate(String time, int index, char value) {
		if (value > '9' || value < '0') {
			return false;
		}
		time = time.replace(time.charAt(index), value);
		String parten = "(20|21|22|23|[0-1]?\\d):[0-5]?\\d:[0-5]?\\d$";
		return time.matches(parten);
	}

	protected boolean validate(String str, int mCurrentSelectIndex, char value,
			int type) {
		if (type == TIMETYPE) {
			return isTimeValidate(str, mCurrentSelectIndex, value);
		}
		if (type == DATETYPE) {
			return isDateValidate(str, mCurrentSelectIndex, value);
		}
		return false;
	}

	public Paint getmPaint() {
		return mPaint;
	}

	public void setmPaint(Paint mPaint) {
		this.mPaint = mPaint;
	}

	public String getmDate() {
		return mDate;
	}

	public void setmDate(String mDate) {
		this.mDate = mDate;
	}

	private void changeNotValidate() {
		String tempStr = String.valueOf(mDateChars);
		if (mType == DATETYPE) {
			if (mCurrentSelectedPosition == 2 || mCurrentSelectedPosition == 3) {
				if (Integer.valueOf(tempStr.substring(0, 4)) > 2037) {
					mDateChars[2] = '3';
					mDateChars[3] = '7';
				} else if (Integer.valueOf(tempStr.substring(0, 4)) < 2000) {
					mDateChars[0] = '2';
					mDateChars[1] = '0';
					mDateChars[2] = '0';
					mDateChars[3] = '0';
				} else if (Integer.valueOf(tempStr.substring(0, 4)) == 2000
						&& Integer.valueOf(tempStr.substring(5, 7)) == 1
						&& Integer.valueOf(tempStr.substring(8, 10)) < 2) {
					mDateChars[8] = '0';
					mDateChars[9] = '2';
				} else {
					if (Integer.valueOf(tempStr.substring(5, 7)) == 2) {

						if ((Integer.valueOf(tempStr.substring(0, 4)) % 4 == 0 && Integer
								.valueOf(tempStr.substring(0, 4)) % 100 != 0)
								|| (Integer.valueOf(tempStr.substring(0, 4)) % 400 == 0)) {

							if (Integer.valueOf(tempStr.substring(8, 10)) > 29) {

								mDateChars[8] = '2';
								mDateChars[9] = '9';
								mDateChars[5] = '0';
								mDateChars[6] = '2';
							}
						} else {
							if (Integer.valueOf(tempStr.substring(8, 10)) > 28) {
								mDateChars[8] = '2';
								mDateChars[9] = '8';
								mDateChars[5] = '0';
								mDateChars[6] = '2';
							}
						}
					}
				}

			} else if (mCurrentSelectedPosition == 5
					|| mCurrentSelectedPosition == 6) {

				if (Integer.valueOf(tempStr.substring(5, 7)) > 12) {
					mDateChars[5] = '1';
					mDateChars[6] = '2';
				} else if (Integer.valueOf(tempStr.substring(5, 7)) < 1) {
					mDateChars[5] = '0';
					mDateChars[6] = '1';

				} else if (Integer.valueOf(tempStr.substring(0, 4)) == 2000
						&& Integer.valueOf(tempStr.substring(5, 7)) == 1
						&& Integer.valueOf(tempStr.substring(8, 10)) < 2) {
					mDateChars[8] = '0';
					mDateChars[9] = '2';
				} else {
					if (Integer.valueOf(tempStr.substring(5, 7)) == 2) {

						if ((Integer.valueOf(tempStr.substring(0, 4)) % 4 == 0 && Integer
								.valueOf(tempStr.substring(0, 4)) % 100 != 0)
								|| (Integer.valueOf(tempStr.substring(0, 4)) % 400 == 0)) {

							if (Integer.valueOf(tempStr.substring(8, 10)) > 29) {

								mDateChars[8] = '2';
								mDateChars[9] = '9';
								mDateChars[5] = '0';
								mDateChars[6] = '2';
							}
						} else {
							if (Integer.valueOf(tempStr.substring(8, 10)) > 28) {
								mDateChars[8] = '2';
								mDateChars[9] = '8';
								mDateChars[5] = '0';
								mDateChars[6] = '2';

							}
						}
					} else {
						switch (Integer.valueOf(tempStr.substring(5, 7))) {
						case 3:
							mDateChars[5] = '0';
							mDateChars[6] = '3';
							break;
						case 6:
							mDateChars[5] = '0';
							mDateChars[6] = '6';
							break;
						case 9:
							mDateChars[5] = '0';
							mDateChars[6] = '9';
							break;
						case 11:
							mDateChars[5] = '1';
							mDateChars[6] = '1';
							break;
						default:
							break;

						}
						if (Integer.valueOf(tempStr.substring(8, 10)) > 30) {
							mDateChars[8] = '3';
							mDateChars[9] = '0';

						}
					}
				}

			} else if (mCurrentSelectedPosition == 8
					|| mCurrentSelectedPosition == 9) {
				if (Integer.valueOf(tempStr.substring(0, 4)) == 2000
						&& Integer.valueOf(tempStr.substring(5, 7)) == 1
						&& Integer.valueOf(tempStr.substring(8, 10)) < 2) {
					mDateChars[8] = '0';
					mDateChars[9] = '2';
				} else if (Integer.valueOf(tempStr.substring(8, 10)) < 1) {
					mDateChars[8] = '0';
					mDateChars[9] = '1';
				} else {
					if (Integer.valueOf(mDate.substring(5, 7)) == 2) {
						if ((Integer.valueOf(mDate.substring(0, 4)) % 4 == 0 && Integer
								.valueOf(mDate.substring(0, 4)) % 100 != 0)
								|| Integer.valueOf(mDate.substring(0, 4)) % 400 == 0) {
							mDateChars[8] = '2';
							mDateChars[9] = '9';
						} else {
							mDateChars[8] = '2';
							mDateChars[9] = '8';
						}
					} else {
						switch (Integer.valueOf(mDate.substring(5, 7))) {
						case 1:
						case 3:
						case 5:
						case 7:
						case 8:
						case 10:
						case 12:
							mDateChars[8] = '3';
							mDateChars[9] = '1';
							break;
						case 2:
						case 4:
						case 6:
						case 9:
						case 11:
							mDateChars[8] = '3';
							mDateChars[9] = '0';
							break;
						default:
							break;
						}
					}
				}
			}
		}
		if (mType == TIMETYPE) {

			if (mCurrentSelectedPosition == 0) {
				mDateChars[0] = '2';
				mDateChars[1] = '3';
			} else if (mCurrentSelectedPosition == 1) {
				mDateChars[1] = '3';
			} else if (mCurrentSelectedPosition == 3) {
				mDateChars[3] = '5';
			} else if (mCurrentSelectedPosition == 6) {
				mDateChars[6] = '5';
			}
		}
		String mFinalString = String.valueOf(mDateChars);
		mDate = mFinalString;
		updateTime.onTimeModified(mDate);

		if (mType == DATETYPE || mType == TIMETYPE) {
			SaveValue mSaveValue = SaveValue.getInstance(mContext);
			if (mSaveValue.readValue(MenuConfigManager.POWER_ON_TIMER) != 0) {
				String poweronTime = mSaveValue
						.readStrValue(MenuConfigManager.TIMER1);
				MTKPowerManager mMtkPowerManager = MTKPowerManager
						.getInstance(mContext);
				mMtkPowerManager.updatePowerOn(poweronTime);
			}
			if (mSaveValue.readValue(MenuConfigManager.POWER_OFF_TIMER) != 0) {
				String powerOffTime = mSaveValue
						.readStrValue(MenuConfigManager.TIMER2);
				MTKPowerManager mMtkPowerManager = MTKPowerManager
						.getInstance(mContext);
				mMtkPowerManager.updatePowerOff(powerOffTime);
			}
		}
	}

}
