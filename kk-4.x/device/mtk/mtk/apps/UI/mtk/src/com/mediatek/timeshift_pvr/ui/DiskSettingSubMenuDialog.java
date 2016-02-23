/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.ui;

import android.content.Context;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.mediatek.timeshift_pvr.controller.StateBase;
import com.mediatek.timeshift_pvr.controller.StateDiskSetting;
import com.mediatek.timeshift_pvr.controller.StateDiskSettingsCallback;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.ui.R;

/**
 *
 */
public class DiskSettingSubMenuDialog extends CommonDialog {

	private float wScale = 0.3f;
	private float hScale = 0.35f;

	private TextView mTitle1;
	private TextView mTitle2;
	private TextView mProgressValueTextview;
	private ProgressBar mProgressBar;
	private Button exitBtn;

	private final static int LAYOUT_FORMAT_CONFIRM = R.layout.pvr_tshift_confirmdialog;
	private final static int LAYOUT_PROGRESS = R.layout.pvr_tshift_dialog_layout;

	private final int FORMAT_CONFIRM_BTN_YES = R.id.confirm_btn_yes;
	private final int FORMAT_CONFIRM_BTN_NO = R.id.confirm_btn_no;
	private final int EXIT_BTN = R.id.pvr_tshift_device_diskop_exit;

	public enum UiType {
		FORMATCONFIRM, FORMAT_ING, FORMAT_DONE,FORMAT_FAIL, SPEEDTEST_ING, SPEEDTEST_DONE
	}

	private UiType mType;

	private StateDiskSettingsCallback callBack;
	private StateDiskSetting mState;

	public DiskSettingSubMenuDialog(Context context, StateBase state,
			UiType type) {
		super(context, LAYOUT_FORMAT_CONFIRM);

		this.getWindow().setLayout(
				(int) (state.getManager().getTVWidth() * wScale),
				(int) (state.getManager().getTVHeight() * hScale));
		this.mState = (StateDiskSetting) state;
		this.mType = type;

		setCancelable(false);
	}

	@Override
	public void initView() {
		super.initView();
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

		switch (keyCode) {
		case KeyEvent.KEYCODE_BACK:
            dismiss();
			break;

		default:
			break;
		}
		return super.onKeyDown(keyCode, event);
	}

	/*
	 * 
	 */
	@Override
	public void show() {
		// TODO Auto-generated method stub
		super.show();
		refreshUI(mType);
	}

	/**
	 * 
	 */
	private void initViewItem() {
		mTitle1 = (TextView) this.findViewById(R.id.diskop_title_line1);
		mTitle2 = (TextView) this.findViewById(R.id.diskop_title_line2);
		mProgressValueTextview = (TextView) this
				.findViewById(R.id.finishpercentage);
		mProgressBar = (ProgressBar) this.findViewById(R.id.progressbar);

		mProgressValueTextview.setText("0%");

		mProgressBar.setMax(100);
		mProgressBar.setProgress(0);

	}

	public void refreshProgres(int percent) {
		if (percent >= 100) {
			percent = 100;
			if(mType==UiType.SPEEDTEST_ING)
			{
				refreshUI(UiType.SPEEDTEST_DONE);	
			}
			else if(mType==UiType.FORMAT_ING)
			{
				refreshUI(UiType.FORMAT_DONE);	
			}
		}
		if (percent < 0) {
			percent = 0;
		}
		mProgressValueTextview.setText(String.format("%3.0f", percent * 1f)+"%");
		mProgressBar.setProgress(percent);
	}

	public void refreshUI(UiType type) {
		Util.showDLog("refreshUI(),"+type.name());
		mType=type;
		switch (type) {
		case FORMATCONFIRM:
			setContentView(LAYOUT_FORMAT_CONFIRM);
			TextView mTV1 = (TextView) findViewById(R.id.diskop_title_line1);
			mTV1.setText(mState.getResource().getString(
					R.string.format_confirm_dialog_line1));

			TextView mTV2 = (TextView) findViewById(R.id.diskop_title_line2);
			mTV2.setText(mState.getResource().getString(
					R.string.format_confirm_dialog_line2));

			Button btn = ((Button) findViewById(R.id.confirm_btn_yes));
			btn.setOnClickListener(new Button.OnClickListener() {

				@Override
				public void onClick(View v) {
					Util.showDLog("onClick??" + "FORMAT_CONFIRM_BTN_YES");
					refreshUI(UiType.FORMAT_ING);
				}
			});

			Button cancelBTN = ((Button) findViewById(R.id.confirm_btn_no));
			cancelBTN.setOnClickListener(new Button.OnClickListener() {

				@Override
				public void onClick(View v) {
					DiskSettingSubMenuDialog.this.dismiss();
				    StateDiskSetting.getInstance().showDiskSettingDialog();
				}
			});

			break;
		case FORMAT_ING:
			setContentView(LAYOUT_PROGRESS);
			initViewItem();

			mTitle1.setText(mState.getResource().getString(
					R.string.disk_setting_formating));
			mTitle2.setText(mState.getResource().getString(
					R.string.disk_setting_unplugdevice_warnning));

			getCallBack().showFormatDisk();

			exitBtn = ((Button) findViewById(R.id.pvr_tshift_device_diskop_exit));
			exitBtn.setVisibility(View.INVISIBLE);
			
			break;
		case FORMAT_DONE:
			mTitle1.setText("");
			mTitle2.setText(mState.getResource().getString(
					R.string.disk_setting_format_done));
			mProgressValueTextview.setText("100%");
			exitBtn = ((Button) findViewById(R.id.pvr_tshift_device_diskop_exit));
			exitBtn.setOnClickListener(new Button.OnClickListener() {
	
				@Override
				public void onClick(View v) {
					if (mProgressBar.getProgress()==100) {
						DiskSettingSubMenuDialog.this.dismiss();
						StateDiskSetting.getInstance().showDiskSettingDialog();
					}else {
						Toast.makeText(mContext, "Testing !Please wait!", Toast.LENGTH_LONG).show();
					}
	//				DiskSettingSubMenuDialog.this.dismiss();
					
				}
			});
			exitBtn.setVisibility(View.VISIBLE);
			exitBtn.requestFocus();
			break;
		case SPEEDTEST_ING:
			setContentView(LAYOUT_PROGRESS);
			initViewItem();

			mTitle1.setText(mState.getResource().getString(
					R.string.disk_setting_speedtesting));
			mTitle2.setText(mState.getResource().getString(
					R.string.disk_setting_unplugdevice_warnning));
			
			getCallBack().showSpeedTest();
			exitBtn = ((Button) findViewById(R.id.pvr_tshift_device_diskop_exit));
			exitBtn.setVisibility(View.INVISIBLE);
			break;
		case SPEEDTEST_DONE:
			if(mState.getMaxSpeed()==0.0f){
				mState.getHandler().sendEmptyMessageAtTime(mState.SHOW_1ST_MENU, 50);
				this.dismiss();
				break;
			}
			mTitle1.setText(mState.getResource().getString(
					R.string.disk_setting_speedtest_done));
			
			String speedStr=String.format(mState.getResource().getString(
					R.string.disk_setting_speedtest_maxspeed), mState.getMaxSpeed());
			mTitle2.setText(speedStr);
			
			mProgressValueTextview.setText("100%");
						
			exitBtn = ((Button) findViewById(R.id.pvr_tshift_device_diskop_exit));
			exitBtn.setOnClickListener(new Button.OnClickListener() {
	
				@Override
				public void onClick(View v) {
					if (mProgressBar.getProgress()==100) {
						DiskSettingSubMenuDialog.this.dismiss();
						StateDiskSetting.getInstance().showDiskSettingDialog();
					}else {
						Toast.makeText(mContext, "Testing !Please wait!", Toast.LENGTH_LONG).show();
					}
	//				DiskSettingSubMenuDialog.this.dismiss();
					
				}
			});
			exitBtn.setVisibility(View.VISIBLE);
			exitBtn.requestFocus();
			break;
		case FORMAT_FAIL:
			mTitle1.setText("");
			mTitle2.setText(mState.getResource().getString(
					R.string.disk_setting_format_fail));
			
//			mTitle2.setText(mState.getResource().getString(
//					R.string.disk_setting_format_fail));
			mProgressValueTextview.setText("100%");
			
			exitBtn = ((Button) findViewById(R.id.pvr_tshift_device_diskop_exit));
			exitBtn.setOnClickListener(new Button.OnClickListener() {
	
				@Override
				public void onClick(View v) {
					if (mProgressBar.getProgress()==100) {
						DiskSettingSubMenuDialog.this.dismiss();
						StateDiskSetting.getInstance().showDiskSettingDialog();
					}else {
						Toast.makeText(mContext, "Testing !Please wait!", Toast.LENGTH_LONG).show();
					}
	//				DiskSettingSubMenuDialog.this.dismiss();
					
				}
			});
			exitBtn.setVisibility(View.VISIBLE);
			exitBtn.requestFocus();
			break;
		default:
			break;
		}
	}

	@Override
	public void onClick(View v) {

		if (callBack == null || v == null) {
			Util.showDLog("onClick??" + "callBack == null||v==null");
			return;
		}

		switch (v.getId()) {
		case FORMAT_CONFIRM_BTN_YES:
			Util.showDLog("onClick??" + "FORMAT_CONFIRM_BTN_YES");
			refreshUI(UiType.FORMAT_ING);
			break;
		case FORMAT_CONFIRM_BTN_NO:
			Util.showDLog("onClick??" + "FORMAT_CONFIRM_BTN_NO");
			dismiss();
			break;
		case EXIT_BTN:
			Util.showDLog("onClick??" + "EXIT_BTN");
			
			mState.setSpeedTesting(false);
			dismiss();
			break;
		default:
			break;

		}

		// mCallback.setPVR();

		super.onClick(v);
	}

	public StateDiskSettingsCallback getCallBack() {
		return callBack;
	}

	public void setCallBack(StateDiskSettingsCallback callBack) {
		this.callBack = callBack;
	}
}
