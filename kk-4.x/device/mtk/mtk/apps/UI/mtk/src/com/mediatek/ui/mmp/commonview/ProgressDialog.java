package com.mediatek.ui.mmp.commonview;

import java.text.NumberFormat;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.style.StyleSpan;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.mediatek.ui.R;

public class ProgressDialog extends AlertDialog {
	private ProgressBar vProgress;
	private TextView vProgressPercent;
	private TextView vProgressNumber;
	private Button vCancel;

	private String mProgressNumberFormat;
	private NumberFormat mProgressPercentFormat;

	private int mMax;
	private int mProgressVal;
	private int mSecondaryProgressVal;
	private int mIncrementBy;
	private int mIncrementSecondaryBy;
	private Drawable mProgressDrawable;
	private Drawable mIndeterminateDrawable;
	private boolean mIndeterminate;

	private boolean mHasStarted;
	private Handler mViewUpdateHandler;

	public ProgressDialog(Context context, int theme) {
		super(context, theme);
	}

	public ProgressDialog(Context context) {
		this(context, R.style.dialog);
	}

	public static AlertDialog show(Context context, CharSequence title,
			CharSequence message) {
		return show(context, title, message, false);
	}

	public static AlertDialog show(Context context, CharSequence title,
			CharSequence message, boolean indeterminate) {
		return show(context, title, message, indeterminate, false, null);
	}

	public static AlertDialog show(Context context, CharSequence title,
			CharSequence message, boolean indeterminate, boolean cancelable) {
		return show(context, title, message, indeterminate, cancelable, null);
	}

	public static AlertDialog show(Context context, CharSequence title,
			CharSequence message, boolean indeterminate, boolean cancelable,
			OnCancelListener cancelListener) {
		ProgressDialog dialog = new ProgressDialog(context);
		dialog.setTitle(title);
		dialog.setMessage(message);
		dialog.setIndeterminate(indeterminate);
		dialog.setCancelable(cancelable);
		dialog.setOnCancelListener(cancelListener);
		dialog.show();
		return dialog;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		mViewUpdateHandler = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				super.handleMessage(msg);
				int what = msg.what;
				if (what == 0) {
					/* Update the number and percent */
					int progress = vProgress.getProgress();
					int max = vProgress.getMax();
					double percent = (double) progress / (double) max;
					String format = mProgressNumberFormat;
					vProgressNumber.setText(String
							.format(format, progress, max));
					SpannableString tmp = new SpannableString(
							mProgressPercentFormat.format(percent));
					tmp.setSpan(new StyleSpan(android.graphics.Typeface.BOLD),
							0, tmp.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
					vProgressPercent.setText(tmp);
				} else {
					vMessage.setText(mMessage);
				}
			}
		};

		vProgress = (ProgressBar) findViewById(R.id.progress);
		vProgressNumber = (TextView) findViewById(R.id.progress_number);
		mProgressNumberFormat = "%d/%d";
		vProgressPercent = (TextView) findViewById(R.id.progress_percent);
		mProgressPercentFormat = NumberFormat.getPercentInstance();
		mProgressPercentFormat.setMaximumFractionDigits(0);

		vCancel = (Button) findViewById(R.id.mmp_cancel);
		vCancel.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				cancel();
			}
		});

		if (mMax > 0) {
			setMax(mMax);
		}
		if (mProgressVal > 0) {
			setProgress(mProgressVal);
		}
		if (mSecondaryProgressVal > 0) {
			setSecondaryProgress(mSecondaryProgressVal);
		}
		if (mIncrementBy > 0) {
			incrementProgressBy(mIncrementBy);
		}
		if (mIncrementSecondaryBy > 0) {
			incrementSecondaryProgressBy(mIncrementSecondaryBy);
		}
		if (mProgressDrawable != null) {
			setProgressDrawable(mProgressDrawable);
		}
		if (mIndeterminateDrawable != null) {
			setIndeterminateDrawable(mIndeterminateDrawable);
		}

		setIndeterminate(mIndeterminate);
		onProgressChanged();
	}

	@Override
	protected int getContentResId() {
		return R.layout.mmp_progress_dialog;
	}

	@Override
	public void onStart() {
		super.onStart();
		mHasStarted = true;
	}

	@Override
	protected void onStop() {
		super.onStop();
		mHasStarted = false;
	}

	public void setProgress(int value) {
		if (mHasStarted) {
			vProgress.setProgress(value);
			onProgressChanged();
		} else {
			mProgressVal = value;
		}
	}

	public void setSecondaryProgress(int secondaryProgress) {
		if (vProgress != null) {
			vProgress.setSecondaryProgress(secondaryProgress);
			onProgressChanged();
		} else {
			mSecondaryProgressVal = secondaryProgress;
		}
	}

	public int getProgress() {
		if (vProgress != null) {
			return vProgress.getProgress();
		}
		return mProgressVal;
	}

	public int getSecondaryProgress() {
		if (vProgress != null) {
			return vProgress.getSecondaryProgress();
		}
		return mSecondaryProgressVal;
	}

	public int getMax() {
		if (vProgress != null) {
			return vProgress.getMax();
		}
		return mMax;
	}

	public void setMax(int max) {
		if (vProgress != null) {
			vProgress.setMax(max);
			onProgressChanged();
		} else {
			mMax = max;
		}
	}

	public void incrementProgressBy(int diff) {
		if (vProgress != null) {
			vProgress.incrementProgressBy(diff);
			onProgressChanged();
		} else {
			mIncrementBy += diff;
		}
	}

	public void incrementSecondaryProgressBy(int diff) {
		if (vProgress != null) {
			vProgress.incrementSecondaryProgressBy(diff);
			onProgressChanged();
		} else {
			mIncrementSecondaryBy += diff;
		}
	}

	public void setProgressDrawable(Drawable d) {
		if (vProgress != null) {
			vProgress.setProgressDrawable(d);
		} else {
			mProgressDrawable = d;
		}
	}

	public void setIndeterminateDrawable(Drawable d) {
		if (vProgress != null) {
			vProgress.setIndeterminateDrawable(d);
		} else {
			mIndeterminateDrawable = d;
		}
	}

	public void setIndeterminate(boolean indeterminate) {
		if (vProgress != null) {
			vProgress.setIndeterminate(indeterminate);
		} else {
			mIndeterminate = indeterminate;
		}
	}

	public boolean isIndeterminate() {
		if (vProgress != null) {
			return vProgress.isIndeterminate();
		}
		return mIndeterminate;
	}

	public void setProgressNumberFormat(String format) {
		mProgressNumberFormat = format;
	}

	private void onProgressChanged() {
		mViewUpdateHandler.sendEmptyMessage(0);
	}

	@Override
	public void setMessage(CharSequence message) {
		mMessage = message;
		mViewUpdateHandler.sendEmptyMessage(1);
	}
}
