package com.mediatek.ui.nav.commonview;

import com.mediatek.ui.R;
import com.mediatek.ui.nav.epginfo.EPGProgramInfo;

import android.content.Context;
import android.graphics.Color;
import android.util.AttributeSet;
import android.view.Gravity;
import android.widget.TextView;
import android.text.TextUtils;

public class EPGTextView extends TextView {
	private EPGProgramInfo tvProgramInfo;

	public EPGTextView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		// TODO Auto-generated constructor stub
	}

	public EPGTextView(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
	}

	public EPGTextView(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}

	public EPGTextView(Context context, EPGProgramInfo mTVProgramInfo) {
		super(context);
		this.tvProgramInfo = mTVProgramInfo;
		this.setSingleLine(true);
		this.setGravity(Gravity.CENTER);
		this.setTextSize(14);
		this.setTextColor(Color.WHITE);
		this.setEllipsize(TextUtils.TruncateAt.END);
	}

	public void setBackground(boolean isDrawLeftIcon,
			boolean isDrawRightwardIcon, boolean selected) {
		if (isDrawRightwardIcon && isDrawLeftIcon) {
			if (selected) {
				setBackgroundResource(R.drawable.epg_left_right_hi);
			} else {
				setBackgroundResource(R.drawable.epg_left_right_nor);
			}
		} else if (isDrawLeftIcon) {
			if (selected) {
				setBackgroundResource(R.drawable.epg_left_high);
			} else {
				setBackgroundResource(R.drawable.epg_left_nor);
			}
		} else if (isDrawRightwardIcon) {
			if (selected) {
				setBackgroundResource(R.drawable.epg_right_high);
			} else {
				setBackgroundResource(R.drawable.epg_right_nor);
			}
		} else {
			if (selected) {
				setBackgroundResource(R.drawable.epg_textviewselected);
			} else {
				setBackgroundResource(R.drawable.epg_textviewnormal);
			}
		}
	}

	public String getShowTitle() {
		return tvProgramInfo != null ? tvProgramInfo.getTitle() : null;

	}

	// @Override
	// protected void onLayout(boolean changed, int left, int top, int right,
	// int bottom) {
	// super.onLayout(changed, left, top, right, bottom);
	// }
	//
	// @Override
	// protected void onSizeChanged(int w, int h, int oldw, int oldh) {
	// // TODO Auto-generated method stub
	// super.onSizeChanged(w, h, oldw, oldh);
	//
	// }

	//
	// @Override
	// protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
	// super.onMeasure(widthMeasureSpec, heightMeasureSpec);
	// }

}
