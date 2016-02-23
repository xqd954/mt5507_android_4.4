package com.mediatek.timeshift_pvr.controller;

import java.util.ArrayList;
import java.util.List;
import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnDismissListener;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.mediatek.timeshift_pvr.manager.Core;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.timeshift_pvr.ui.SpecialConfirmDialog;
import com.mediatek.ui.R;

public class SchedulePvrActivity extends Activity {

	private Context mContext;
	int taskID = 0;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_schedule_pvr);
		mContext = getApplicationContext();
		
		taskID = getIntent().getIntExtra(Core.ALARM_ACTION_TAG, -1);
		Util.showDLog("Core.ALARM_ACTION_TAG:" + taskID);
	}

	/*
 * 
 */
	@Override
	protected void onStart() {
		super.onStart();
		showReminderDialog();
	}

	/*
	 * 
	 */
	@Override
	protected void onStop() {
		// TODO Auto-generated method stub
		super.onStop();
		finish();
	}

	private void showReminderDialog() {
		final ScheduleItem item = Util.getScheduleItem(mContext,taskID);
		List<ScheduleItem> items = new ArrayList<ScheduleItem>();
		items.add(item);
		final SpecialConfirmDialog confirm = new SpecialConfirmDialog(
				SchedulePvrActivity.this, items);
		confirm.setPositiveButton(null);
		confirm.setNegativeButton(null);

		confirm.setPositiveButton(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				Toast.makeText(mContext,
						"Will start schedule PVR!!!!,ID: " + item.getTaskID(),
						Toast.LENGTH_LONG).show();
				
				Util.startPvrRecord(mContext,taskID);
				confirm.dismiss();
			}
		});
		confirm.setNegativeButton(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				confirm.dismiss();
			}
		});

		confirm.setOnDismissListener(new OnDismissListener() {
			@Override
			public void onDismiss(DialogInterface dialog) {
				finish();
			}
		});

		String title1 = mContext
				.getString(R.string.pvr_schedulelist_replace_line1);
		String title2 = mContext
				.getString(R.string.pvr_schedulelist_replace_line2);

		confirm.setTitle(title1, title2);
		confirm.show();
	}



}
