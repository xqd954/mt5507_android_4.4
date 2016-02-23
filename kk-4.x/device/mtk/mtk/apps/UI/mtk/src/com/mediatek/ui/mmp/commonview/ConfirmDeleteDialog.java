package com.mediatek.ui.mmp.commonview;

import com.mediatek.ui.R;

import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnShowListener;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class ConfirmDeleteDialog extends AlertDialog implements
		android.view.View.OnClickListener, OnShowListener {
	private Button vPositive;
	private Button vNegative;
	private OnConfirmDeleteListener mListener;

	public ConfirmDeleteDialog(Context context, int theme) {
		super(context, theme);
	}

	public ConfirmDeleteDialog(Context context) {
		super(context);
	}

	
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		vPositive = (Button) findViewById(R.id.mmp_confirm);
		vNegative = (Button) findViewById(R.id.mmp_cancel);

		vPositive.setOnClickListener(this);
		vNegative.setOnClickListener(this);

		setOnShowListener(this);
	}

	
	protected int getContentResId() {
		return R.layout.mmp_confirm_delete_dialog;
	}

	public void setOnConfirmDeleteListener(OnConfirmDeleteListener listener) {
		mListener = listener;
	}

	public interface OnConfirmDeleteListener {
		void onPositivePressed();

		void onNegativePressed();
	}

	public void onClick(View v) {
		int id = v.getId();
		switch (id) {
		case R.id.mmp_confirm:
			if (mListener != null) {
				mListener.onPositivePressed();
			}
			break;
		case R.id.mmp_cancel:
			if (mListener != null) {
				mListener.onNegativePressed();
			}
			break;
		default:
			break;
		}
	}

	public void onShow(DialogInterface dialog) {
		vPositive.requestFocus();
	}
}
