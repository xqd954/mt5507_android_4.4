/*
 * Copyright (C) 2006 The Android Open Source Project
 * 
 * ©2010-2013 TCL CORPORATION All Rights Reserved.
 */

package android.tclwidget;

import com.android.internal.R;
import android.content.Context;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.widget.ProgressBar;
import android.content.res.TypedArray;

import android.util.Log;

public class TCLProgressBar extends ProgressBar {
	// {{lvh@tcl , for remove id from public.xml
	/*
	 * @author TCL TVOS Team
	 */
	private static final int TCLProgressBarC=0x01030318;
	private static final int TCLProgressBarH=0x01030317;
	private static final int TCLProgressBarHW=0x01030319;

	public static final int TclStyle_Circle = TCLProgressBarC;
	public static final int TclStyle_Horizontal = TCLProgressBarH;
	public static final int TclStyle_HorizontalWhite = TCLProgressBarHW;

	/**
     * Sets the MediaPlayer of MediaPlayerControl.
	 * @author TCL TVOS Team
     * @param context  The context to use. 
	 * @param _style TclStyle_Circle Circle Style
					 TclStyle_Horizontal Horizontal Style
					 TclStyle_HorizontalWhite White Horizontal Style
     */
	public void setTclStyle(Context context, int _style) {
		switch(_style) {
			case TclStyle_Circle:
				_style = R.style.TCLProgressBarC;
				break;
			case TclStyle_Horizontal:
				_style = R.style.TCLProgressBarH;
				break;
			case TclStyle_HorizontalWhite:
				_style = R.style.TCLProgressBarHW;
				break;
			default:
				return;

		}
		TypedArray a = context.obtainStyledAttributes(_style,
				R.styleable.ProgressBar);

		if (a != null) {

			Drawable drawable = a
					.getDrawable(R.styleable.ProgressBar_progressDrawable);
			/*
			 * debug Log.d("TCLProgressBar", "progressDrawable = " + drawable);
			 * Log.d("TCLProgressBar", "indeterminateOnly = " +
			 * a.getBoolean(R.styleable.ProgressBar_indeterminateOnly,false));
			 * Log.d("TCLProgressBar", "mMinWidth = " +
			 * a.getDimensionPixelSize(R.styleable.ProgressBar_minWidth, 123));
			 * Log.d("TCLProgressBar", "mMaxWidth = " +
			 * a.getDimensionPixelSize(R.styleable.ProgressBar_maxWidth, 123));
			 * Log.d("TCLProgressBar", "mMinHeight = " +
			 * a.getDimensionPixelSize(R.styleable.ProgressBar_minHeight, 19));
			 * Log.d("TCLProgressBar", "mMaxHeight = " +
			 * a.getDimensionPixelSize(R.styleable.ProgressBar_maxHeight, 19));
			 * Log.d("TCLProgressBar", "indeterminateDrawable = " +
			 * a.getDrawable(R.styleable.ProgressBar_indeterminateDrawable));
			 */
			_setTclStyle(context, a);
			if (drawable != null) {
				setIndeterminate(false);
			}
			a.recycle();
		}
	}
	// }}

	public TCLProgressBar(Context context) {
		super(context);
		setTclStyle(context, TclStyle_Circle);
	}

	public TCLProgressBar(Context context, AttributeSet attrs) {
		super(context, attrs);
		drawableOfStyleCircle(context, attrs); // add by liuyan 20120524
	}

	public TCLProgressBar(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		drawableOfStyleCircle(context, attrs); // add by liuyan 20120524
	}

	private void drawableOfStyleCircle(Context context, AttributeSet attrs) {
		TypedArray a = context.obtainStyledAttributes(attrs,
				R.styleable.ProgressBar,
				com.android.internal.R.attr.progressBarStyle, 0);
		boolean mOnlyIndeterminate = false;
		Drawable drawable = a
				.getDrawable(R.styleable.ProgressBar_progressDrawable);
		mOnlyIndeterminate = a.getBoolean(
				R.styleable.ProgressBar_indeterminateOnly, mOnlyIndeterminate);
		if (drawable == null && mOnlyIndeterminate) {
			drawable = a
					.getDrawable(R.styleable.ProgressBar_indeterminateDrawable);
			if (drawable != null) {
				setIndeterminateDrawable(drawable);
			}
		}
		a.recycle();
		// lvh@tcl
		if (attrs.getStyleAttribute() == 0) {
			setTclStyle(context, TclStyle_Circle);
		} else { // 2013.9.29 , fixed xml style , for old apk
			String attr_style = attrs.getAttributeValue(null, "style"); 
			if(attr_style != null){
				if(attr_style.indexOf("android:attr/tclprogressBarStyleHorizontal") != -1) {
					setTclStyle(context, TclStyle_Horizontal);
				}else if(attr_style.indexOf("android:attr/tclprogressBarStyleCircle") != -1){
					setTclStyle(context, TclStyle_Circle);
				}else if(attr_style.indexOf("android:attr/tclprogressBarStyleHorizontalWhite") != -1){
					setTclStyle(context, TclStyle_HorizontalWhite);
				}
			}
		}
	}

}


