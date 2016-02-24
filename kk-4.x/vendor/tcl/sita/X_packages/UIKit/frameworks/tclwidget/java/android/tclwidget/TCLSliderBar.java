/*
 * Copyright (C) 2006 The Android Open Source Project
 * 
 * ©2010-2013 TCL CORPORATION All Rights Reserved.
 */

package android.tclwidget;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;

public class TCLSliderBar extends View {
	private Context mContext;
	
	Drawable mdrawbale, mbgdrawbale, mbthumbrawbale;
	int width, height;
	private int mMax = 100,mlinewidth=18;
	private int mProgress = 0;
	private String TAG = "ScrollBar";
	
	public TCLSliderBar(Context context) {
		super(context);
		mContext = context;
		init();
	}

	public TCLSliderBar(Context context, AttributeSet attrs) {
		super(context, attrs);
		mContext = context;
		init();
	}

	public TCLSliderBar(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		mContext = context;
		init();
	}
	
	private void init() {
		mbgdrawbale = mContext.getResources().getDrawable(com.android.internal.R.drawable.tcl_slider_bg);
		mbthumbrawbale = mContext.getResources().getDrawable(com.android.internal.R.drawable.tcl_slider_thumb);
	}
	
    /**
     * <p>Set the range of the slider bar to 0...<tt>max</tt>.</p>
     * @author TCL TVOS Team
     * @param max the upper range of this slider bar
     * @see #getMax()
     */
	public void setMax(int max) {
		if (max > 0)
			mMax = max;
	}
	
    /**
     * <p>Set the width of the slider bar with <tt>width</tt>, the range: 18~22.</p>
     * @author TCL TVOS Team
     * @param width the width of this slider bar
     * @see #getMax()
     */
	public void setSliderWidth(int width) {
		if ((width>=18)/*&& (width<=22)*/)
			mlinewidth = width;
	}

    /**
     * <p>Set the current progress to the specified value.</p>
     * @author TCL TVOS Team
     * @param progress the new sliderbar, between 0 and {@link #getMax()}
     * @see #getProgress()
     */
	public void setProgress(int progress) {
		if (progress > mMax) 
			mProgress = mMax;
		else
			mProgress = progress;
		invalidate();
	}
	
    /**
     * <p>Return the upper limit of this slider bar's range.</p>
     * @author TCL TVOS Team
     * @return int a positive integer
     * @see #setMax(int)
     * @see #getProgress()
     */
	public int getMax() {
		return mMax;
	}

    /**
     * <p>Get the specified value of the slider bar's current progress .</p>
     * @author TCL TVOS Team
     * @return int the width of this slider bar
     * @see #getProgress()
     */
	public int getSliderWidth() {
		return mlinewidth;
	}
	
	/**
     * <p>Get the slider bar's current level of progress.</p>
     * @author TCL TVOS Team
     * @return the current progress, between 0 and {@link #getMax()}
     * @see #setProgress(int)
     * @see #setMax(int)
     * @see #getMax()
     */
	public int getProgress() {
		return mProgress;
	}
	
	@Override
	protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		// TODO Auto-generated method stub
		int heightSpec = MeasureSpec.getSize(heightMeasureSpec);
		int heightMode = MeasureSpec.getMode(heightMeasureSpec);
		
		width = 40;
		if (heightMode == MeasureSpec.EXACTLY)
			height = heightSpec;
		else
			height = 100;
		setMeasuredDimension(width, height);
	}

	@Override
	protected void onDraw(Canvas canvas) {
		int mid = width/2;
		int top = 0;
		int scrollbar = 100;
		int progressHeight = ((height-top-scrollbar)*mProgress)/mMax;
		
		Log.d(TAG, String.format("--> %d, %d", mMax, mProgress));
		
		mbgdrawbale.setBounds(mid - mlinewidth/2, top,mid + mlinewidth/2, height);
		mbgdrawbale.draw(canvas);		
	//	mdrawbale.setBounds(mid - linewidth/2, top, mid + linewidth/2, progressHeight + top);
	//	mdrawbale.draw(canvas);
		mbthumbrawbale.setBounds(mid - mlinewidth/2,  progressHeight+top, mid + mlinewidth/2,  progressHeight+top+scrollbar);
		mbthumbrawbale.draw(canvas);
	}
}

