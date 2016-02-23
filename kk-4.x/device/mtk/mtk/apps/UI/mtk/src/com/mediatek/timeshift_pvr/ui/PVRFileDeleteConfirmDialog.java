/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.ui;

import android.content.Context;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.mediatek.timeshift_pvr.controller.StateBase;
import com.mediatek.timeshift_pvr.controller.StateFileList;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.ui.R;

/**
 *
 */
public class PVRFileDeleteConfirmDialog extends CommonDialog {

	private float wScale = 0.3f;
	private float hScale = 0.35f;

	private StateFileList mState;
	
	public PVRFileDeleteConfirmDialog(Context context, StateBase state) {
		super(context, R.layout.pvr_tshift_confirmdialog);

		this.getWindow().setLayout(
				(int) (state.getManager().getTVWidth() * wScale),
				(int) (state.getManager().getTVHeight() * hScale));
		this.mState = (StateFileList) state;

		setCancelable(true);
		initView2();
	}

	private void initView2() {
		TextView mTV1 = (TextView) findViewById(R.id.diskop_title_line1);
		mTV1.setText(mState.getResource().getString(
				R.string.delete_pvr_file_dialog_confirm1));

		TextView mTV2 = (TextView) findViewById(R.id.diskop_title_line2);
		mTV2.setText(mState.getResource().getString(
				R.string.delete_pvr_file_dialog_confirm2));

		Button btn = ((Button) findViewById(R.id.confirm_btn_yes));
		btn.setOnClickListener(new Button.OnClickListener() {

			@Override
			public void onClick(View v) {
				((StateFileList)mState).deletePvrFile();
				PVRFileDeleteConfirmDialog.this.dismiss();
			}
		});

		Button cancelBTN = ((Button) findViewById(R.id.confirm_btn_no));
		cancelBTN.setOnClickListener(new Button.OnClickListener() {

			@Override
			public void onClick(View v) {
				PVRFileDeleteConfirmDialog.this.dismiss();
			}
		});
	}

	/*
	 * 
	 */
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {

		Util.showDLog("onKeyDown:::" + keyCode);
		if (keyCode == KeyEvent.KEYCODE_DPAD_CENTER) {
			onClick(getCurrentFocus());
		}
		return super.onKeyDown(keyCode, event);
	}
}
