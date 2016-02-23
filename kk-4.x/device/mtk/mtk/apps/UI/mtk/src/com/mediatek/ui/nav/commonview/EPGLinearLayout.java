package com.mediatek.ui.nav.commonview;

import java.util.List;

import android.content.Context;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.view.View;
import android.widget.LinearLayout;

import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVEvent;
import com.mediatek.ui.R;
import com.mediatek.ui.nav.epginfo.EPGProgramInfo;
import com.mediatek.ui.nav.util.DataReader;
import com.mediatek.ui.util.MtkLog;

public class EPGLinearLayout extends LinearLayout {
	private int mMax;
	private Context mContext;
	private static final String TAG = "EPGLinearLayout";
	private int mWidth;

	private int mCurrentSelectPosition = 0;
	private List<EPGProgramInfo> childViewData;

	public EPGLinearLayout(Context context, AttributeSet attrs) {
		super(context, attrs);
		mContext = context;
	}

	public EPGLinearLayout(Context context) {
		super(context);
		mContext = context;
	}

	public void setWidth(int mWidth) {
		this.mWidth = mWidth;
	}

	public int getmCurrentSelectPosition() {
		return mCurrentSelectPosition;
	}

	public void setmCurrentSelectPosition(int mCurrentSelectPosition) {
		this.mCurrentSelectPosition = mCurrentSelectPosition;
	}

	public void setAdpter(List<EPGProgramInfo> mChildViewData, boolean flag) {
		this.childViewData = mChildViewData;
		final int width = mWidth;
		for (int i = 0; i < mChildViewData.size(); i++) {

			EPGTextView textView = new EPGTextView(mContext, mChildViewData
					.get(i));
			int childWidth = (int) (width * mChildViewData.get(i).getmScale());

			LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
					childWidth, LayoutParams.MATCH_PARENT);
			layoutParams.leftMargin = (int) (mChildViewData.get(i)
					.getLeftMargin()*width);
			MtkLog.d(TAG, "setAdpter-----layoutParams.leftMargin---->"
					+ layoutParams.leftMargin);
			if (i == mCurrentSelectPosition && flag == true) {
				textView.setBackground(childViewData.get(i).isDrawLeftIcon(),
						childViewData.get(i).isDrawRightwardIcon(), true);
			} else {
				textView.setBackground(childViewData.get(i).isDrawLeftIcon(),
						childViewData.get(i).isDrawRightwardIcon(), false);
			}
			textView.setText(textView.getShowTitle());

			addView(textView, i, layoutParams);
		}
	}

	public void setSelectedPosition(int index) {
		View childView;
		if (mCurrentSelectPosition != -1) {
			childView = getChildAt(mCurrentSelectPosition);
			if (childView != null
					&& mCurrentSelectPosition < childViewData.size()) {
				EPGProgramInfo childProgramInfo = childViewData
						.get(mCurrentSelectPosition);
				((EPGTextView) childView).setBackground(childProgramInfo
						.isDrawLeftIcon(), childProgramInfo
						.isDrawRightwardIcon(), false);
			}
		}
		childView = getChildAt(index);
		if (childView != null) {
			EPGProgramInfo childProgramInfo = childViewData.get(index);
			((EPGTextView) childView).setBackground(childProgramInfo
					.isDrawLeftIcon(), childProgramInfo.isDrawRightwardIcon(),
					true);
			mCurrentSelectPosition = index;
		}
	}

	public void clearSelected() {
		View childView;
		if (mCurrentSelectPosition != -1) {
			childView = getChildAt(mCurrentSelectPosition);
			if (childView != null
					&& mCurrentSelectPosition < childViewData.size()) {
				EPGProgramInfo childProgramInfo = childViewData
						.get(mCurrentSelectPosition);
				((EPGTextView) childView).setBackground(childProgramInfo
						.isDrawLeftIcon(), childProgramInfo
						.isDrawRightwardIcon(), false);
			}
			mCurrentSelectPosition = -1;
		}
	}

	public boolean onKeyLeft() {
		View childView;
		if (mCurrentSelectPosition != -1) {
			childView = getChildAt(mCurrentSelectPosition);
			if (childView != null
					&& mCurrentSelectPosition < childViewData.size()) {
				EPGProgramInfo childProgramInfo = childViewData
						.get(mCurrentSelectPosition);
				((EPGTextView) childView).setBackground(childProgramInfo
						.isDrawLeftIcon(), childProgramInfo
						.isDrawRightwardIcon(), false);
			} else {
				return false;
			}

			if (mCurrentSelectPosition > 0) {
				--mCurrentSelectPosition;
				childView = getChildAt(mCurrentSelectPosition);
				if (childView != null
						&& mCurrentSelectPosition < childViewData.size()) {
					EPGProgramInfo childProgramInfo = childViewData
							.get(mCurrentSelectPosition);
					((EPGTextView) childView).setBackground(childProgramInfo
							.isDrawLeftIcon(), childProgramInfo
							.isDrawRightwardIcon(), true);
					return true;
				} else {
					return false;
				}
			} else {
				return false;
			}
		}
		return true;
	}

	public boolean onKeyRight() {
		View childView;
		final int count = getChildCount();
		if (mCurrentSelectPosition != -1) {
			childView = getChildAt(mCurrentSelectPosition);
			if (childView != null
					&& mCurrentSelectPosition < childViewData.size()) {
				EPGProgramInfo childProgramInfo = childViewData
						.get(mCurrentSelectPosition);
				((EPGTextView) childView).setBackground(childProgramInfo
						.isDrawLeftIcon(), childProgramInfo
						.isDrawRightwardIcon(), false);
			} else {
				return false;
			}
			if (mCurrentSelectPosition < count - 1
					&& mCurrentSelectPosition < childViewData.size() - 1) {
				MtkLog.d(TAG, "=======onKeyRight=============count========="
						+ count);
				++mCurrentSelectPosition;
				childView = getChildAt(mCurrentSelectPosition);
				if (childView != null) {
					EPGProgramInfo childProgramInfo = childViewData
							.get(mCurrentSelectPosition);
					((EPGTextView) childView).setBackground(childProgramInfo
							.isDrawLeftIcon(), childProgramInfo
							.isDrawRightwardIcon(), true);
				} else {
					return false;
				}
			} else {
				return false;
			}
			return true;
		}
		return true;
	}

	@Override
	protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		super.onMeasure(widthMeasureSpec, heightMeasureSpec);
	}

	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh) {
		// TODO Auto-generated method stub
		super.onSizeChanged(w, h, oldw, oldh);

	}

	@Override
	protected void onLayout(boolean changed, int l, int t, int r, int b) {
		super.onLayout(changed, l, t, r, b);

	}

	public int getMax() {
		return mMax;
	}

	public void setMax(int mMax) {
		this.mMax = mMax;
	}

	@Override
	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);
	}

	public String getProgramType() {
		String type = mContext.getResources().getString(
				R.string.epg_info_program_type);
		DataReader dtReader = DataReader.getInstance(mContext);
		TVEvent curEvent = null;
		TVChannel curCh = dtReader.getCurrentPlayChannel();
		TVEvent[] curChTvEvents = dtReader.getTvEvents(curCh);
		if (curCh != null && curChTvEvents != null) {
			if (mCurrentSelectPosition >= 0
					&& mCurrentSelectPosition < curChTvEvents.length) {
				curEvent = curChTvEvents[mCurrentSelectPosition];
			}
		}
		int index = 0;
		if (curEvent != null) {
			index = curEvent.findFirstMainCategary();
			MtkLog.d(TAG, "------------------index: " + index);
			String[] mainType = mContext.getResources().getStringArray(
					R.array.nav_epg_filter_type);
			if (index >= 0 && index < mainType.length) {
				MtkLog.d(TAG, "index: " + index + "   main type: "
						+ mainType[index]);
				return mainType[index];
			}
			return type;
		}
		return null;

	}

}
