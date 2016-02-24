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
	 * frameworks/base/core/java/android/widget/ProgressBar.java add function
	 * _setTclStyle
	 */
	public static final int TclStyle_Circle = R.style.TCLProgressBarC;
	public static final int TclStyle_Horizontal = R.style.TCLProgressBarH;
	public static final int TclStyle_HorizontalWhite = R.style.TCLProgressBarHW;

	public void setTclStyle(Context context, int _style) {
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
		}
	}

}

