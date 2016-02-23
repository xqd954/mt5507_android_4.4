/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.ui;

import java.util.List;

import android.content.Context;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;

import com.mediatek.timeshift_pvr.controller.ScheduleItem;
import com.mediatek.timeshift_pvr.controller.StateBase;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.ui.R;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

/**
 *
 */
public class SpecialConfirmDialog extends CommonDialog {

	private float wScale = 0.55f;
	private float hScale = 0.35f;

	public StateBase mState;

	private TextView mTV1;
	private TextView mTV2;

	private Button positiveBtn;
	private Button negativeBtn;

	private List<ScheduleItem> mItems;

	public SpecialConfirmDialog(Context context, StateBase state) {
		super(context, R.layout.pvr_tshift_special_confirmdialog);

		this.getWindow().setLayout(
				(int) (state.getManager().getTVWidth() * wScale),
				(int) (state.getManager().getTVHeight() * hScale));
		this.mState = state;

		setCancelable(true);
		initView2();
	}

	public SpecialConfirmDialog(Context context, StateBase state,
			List<ScheduleItem> item) {
		super(context, R.layout.pvr_tshift_special_confirmdialog);

		this.getWindow().setLayout(
				(int) (state.getManager().getTVWidth() * wScale),
				(int) (state.getManager().getTVHeight() * hScale));
		this.mState = state;
		this.mItems = item;

		setCancelable(true);
		initView2();
	}

	public SpecialConfirmDialog(Context context, List<ScheduleItem> item) {
		super(context, R.layout.pvr_tshift_special_confirmdialog);

		WindowManager windowManager = (WindowManager) context
				.getSystemService(Context.WINDOW_SERVICE);

		int tvWidth = ScreenConstant.SCREEN_WIDTH;//windowManager.getDefaultDisplay().getRawWidth();
		int tvHeight = ScreenConstant.SCREEN_HEIGHT;//windowManager.getDefaultDisplay().getRawHeight();

		this.getWindow().setLayout((int) (tvWidth * wScale),
				(int) (tvHeight * hScale));
		this.mItems = item;

		setCancelable(true);
		initView2();
	}

	private void initView2() {
		mTV1 = (TextView) findViewById(R.id.diskop_title_line1);
		mTV1.setText("");

		mTV2 = (TextView) findViewById(R.id.diskop_title_line2);
		mTV2.setText("");

		positiveBtn = ((Button) findViewById(R.id.confirm_btn_yes));
		negativeBtn = ((Button) findViewById(R.id.confirm_btn_no));

		setItemValue(mItems);
	}

	/**
	 * id:1001365643949250
	 * 
	 * @param view
	 * @param item
	 */
	private void setItemValue(List<ScheduleItem> items) {

		TableLayout rootView = (TableLayout)findViewById(R.id.device_info);
		MtkLog.e("chengcl","size=="+items.size());
		if (items == null && items.size()<1) {
			Util.showDLog("specialConifrmDilog.setItemValue():"
					+ "ScheduleItem==null");
			rootView.setVisibility(View.INVISIBLE);
			return;
		}

		
		for(ScheduleItem item :items){
			MtkLog.e("chengcl", "coming .........");
			TableRow row = new TableRow(mContext);
			TextView label = new TextView(mContext);
			label.setTextColor(mContext.getResources().getColor(R.color.yellow));
			label.setText("CH" + item.getChannelName());
			row.addView(label);
			
			TextView size = new TextView(mContext);
			size.setTextColor(mContext.getResources().getColor(R.color.yellow));
			size.setText(Util.dateToStringYMD3(item.getStartTime()));
			row.addView(size);
			
			TextView duration = new TextView(mContext);
			duration.setTextColor(mContext.getResources().getColor(R.color.yellow));
			duration.setText(Util.longToHrMin(item.getDuration()));
			row.addView(duration);
			
			TextView repeatType = new TextView(mContext);			
			repeatType.setTextColor(mContext.getResources().getColor(R.color.yellow));
			String[] repeat = mContext.getResources().getStringArray(
					R.array.pvr_tshift_repeat_type);
			repeatType.setText(repeat[item.getRepeatType()]);
			row.addView(repeatType);
			
			TextView scheduleType = new TextView(mContext);
			scheduleType.setTextColor(mContext.getResources().getColor(R.color.yellow));
			String[] schedule = mContext.getResources().getStringArray(
					R.array.pvr_tshift_schedule_type);
			String type = schedule[item.getRemindType()];
			scheduleType.setText(type);
			row.addView(scheduleType);
			
			rootView.addView(row);
			
		}

	}

	public void setPositiveButton(Button.OnClickListener listener) {
		positiveBtn.setOnClickListener(listener);
	}

	public void setNegativeButton(Button.OnClickListener listener) {
		negativeBtn.setOnClickListener(listener);
	}

	public void setTitle(String line1, String line2) {
		mTV1.setText(line1);
		mTV2.setText(line2);
	}

	/*
	 * 
	 */
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {

		Util.showDLog("CommonConfirmDialog.onKeyDown()" + keyCode);
		if (keyCode == KeyEvent.KEYCODE_DPAD_CENTER) {
			View view = getCurrentFocus();
			switch (view.getId()) {
			case R.id.confirm_btn_yes:
			case R.id.confirm_btn_no:
				onClick(view);
				break;
			default:
				Util.showDLog("Current Focus !=Confirm Button.");
				break;
			}
		}
		return super.onKeyDown(keyCode, event);
	}

}
