/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.ui;

import android.content.Context;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnFocusChangeListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.RelativeLayout;

import com.mediatek.dm.MountPoint;
import com.mediatek.timeshift_pvr.controller.StateBase;
import com.mediatek.timeshift_pvr.controller.StateDiskSetting;
import com.mediatek.timeshift_pvr.controller.StateDiskSettingsCallback;
import com.mediatek.timeshift_pvr.controller.StateInitDiskItemAdapter;
import com.mediatek.timeshift_pvr.manager.Core;
import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.ui.R;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;

import java.util.ArrayList;

/**
 *
 */
public class DiskSettingDialog extends CommonDialog implements
		OnItemClickListener {

	private float wScale = 0.5f;
	private float hScale = 0.5f;
    private Button mSetShiftBtn;
	private Button mSetPVRBtn;
	private Button mFormatBtn;
	private Button mSpeedTestBtn;
	private RelativeLayout mainLayout;

	private ListView mUsbList;

	private StateDiskSettingsCallback mCallback;

	private LinearLayout mOpMenuList;
	private StateBase mState;
	public ArrayList<MountPoint> mountList = new ArrayList<MountPoint>();
	StateInitDiskItemAdapter adapter;

	/**
	 * @param context
	 * @param layoutID
	 */
	public DiskSettingDialog(Context context, int layoutID, StateBase state) {
		super(context, layoutID);
		// TODO Auto-generated constructor stub

		this.getWindow().setLayout(
				(int) (state.getManager().getTVWidth() * wScale),
				(int) (state.getManager().getTVHeight() * hScale));
		this.getWindow()
				.setBackgroundDrawableResource(R.drawable.tv_background);
		this.mState = state;
		mountList = intiView2();

		adapter = new StateInitDiskItemAdapter<MountPoint>(mContext, mountList);
		// intiView2();
		setAdapter(adapter);
	}

	@Override
	public void initView() {
		// TODO Auto-generated method stub
		super.initView();
		mainLayout = (RelativeLayout)findViewById(R.id.pvr_timeshift_deviceinfo);
		mSetShiftBtn = (Button) findViewById(R.id.disksetting_setshift);
		if(!TimeShiftManager.enableTshiftModule()){
			mSetShiftBtn.setVisibility(View.GONE);
		}

		
		mSetPVRBtn = (Button) findViewById(R.id.disksetting_setpvr);
		mFormatBtn = (Button) findViewById(R.id.disksetting_format);
		mSpeedTestBtn = (Button) findViewById(R.id.disksetting_speedtest);

		mOpMenuList = (LinearLayout) findViewById(R.id.device_info_sub_menu);

		mOpMenuList.setVisibility(View.INVISIBLE);
		mUsbList = (ListView) findViewById(R.id.device_info_list);
		mUsbList.setOnFocusChangeListener(new OnFocusChangeListener() {
			@Override
			public void onFocusChange(View v, boolean hasFocus) {
				if (hasFocus) {
//					hiddenOpMenuList();
				}
			}
		});
		mUsbList.setOnKeyListener(new android.view.View.OnKeyListener() {

			@Override
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				
				if (event.getAction() != KeyEvent.ACTION_DOWN) {
					return false;
				}

				switch (keyCode) {
				case KeyEvent.KEYCODE_MTKIR_GREEN:

					break;
				case KeyEvent.KEYCODE_MTKIR_BLUE:
					DiskSettingDialog.this.dismiss();
					break;

				case KeyEvent.KEYCODE_DPAD_CENTER:
				case KeyEvent.KEYCODE_DPAD_RIGHT:

					showOpMenuList();
					return false;
				}

				return false;
			}
		});

		mUsbList.setOnItemClickListener(new OnItemClickListener() {
			@Override
			public void onItemClick(AdapterView<?> parent, View view,
					int position, long id) {
				showOpMenuList();
			}
		});
		initListener();
	}
	
	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		if (null != StateDiskSetting.getInstance()) {
			StateDiskSetting.getInstance().updateDissmissTimer();
			
		}
		return super.onKeyUp(keyCode, event);
	}
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
	    // TODO Auto-generated method stub
	    if (null!=StateDiskSetting.getInstance()) {
            StateDiskSetting.getInstance().getHandler().removeMessages(10);
            StateDiskSetting.getInstance().getHandler().sendEmptyMessageDelayed(10, 15000);
        }
	    switch (keyCode) {
            case KeyEvent.KEYCODE_DPAD_LEFT:
                hiddenOpMenuList();
                
                break;
		case KeyMap.KEYCODE_BACK:
			// hiddenOpMenuList();
			dismiss();

			break;
		case KeyMap.KEYCODE_MTKIR_BLUE:
			DiskSettingDialog.this.dismiss();
			break;
            default:
                break;
        }
	    return super.onKeyDown(keyCode, event);
	}

	private ArrayList<MountPoint> intiView2() {
		ArrayList<MountPoint> list = mState.getManager().getDeviceList();
		MtkLog.e("list", "list:" + list);
		if (list == null || list.size() == 0) {
			list = new ArrayList<MountPoint>();
			MountPoint nullPoint = new MountPoint("", "", Core.NO_DEVICES);
			list.add(nullPoint);

		}
		return list;
	}

	public void refreshList(ListView view) {
		mountList = intiView2();
		adapter.setGroup(mountList);
		// setAdapter(adapter);
		view.setAdapter(adapter);
		
		if (null != StateDiskSetting.getInstance()) {
			StateDiskSetting.getInstance().updateDissmissTimer();
		}
	}

	public void refreshList() {
		mOpMenuList.setVisibility(View.INVISIBLE);
		if (null != StateDiskSetting.getInstance()) {
			StateDiskSetting.getInstance().getHandler().removeMessages(10);
			StateDiskSetting.getInstance().getHandler()
					.sendEmptyMessageDelayed(10, 15000);
		}
		mountList = intiView2();
		adapter.setGroup(mountList);
		setAdapter(adapter);
	}

	public ListView getListView() {
		return mUsbList;
	}

	void setAdapter(StateInitDiskItemAdapter adapter) {
		mUsbList.setAdapter(adapter);
	}
	public int getSelectedPosition() {
		return mUsbList.getSelectedItemPosition();
	}

	public int setSelectedPosition(int position) {
		mUsbList.setSelection(position);
		return 0;
	}

	/**
	 * 
	 */
	private void initListener() {
	    mSetShiftBtn.setOnClickListener(this);
		mSetPVRBtn.setOnClickListener(this);
		mFormatBtn.setOnClickListener(this);
		mSpeedTestBtn.setOnClickListener(this);

		mUsbList.setOnItemClickListener(this);
	}

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		if (mCallback == null) {
			return;
		}

		switch (v.getId()) {
		case R.id.disksetting_setshift:
			getmCallback().showSetTSHIFT();
	         break;
		case R.id.disksetting_setpvr:
			getmCallback().showSetPVR();
			break;
		case R.id.disksetting_format:
			getmCallback().showFormatDisk();
			break;
		case R.id.disksetting_speedtest:
			getmCallback().showSpeedTest();
			break;
		default:
			break;
		}
		super.onClick(v);
	}

	/*
	 * 
	 */
	@Override
	public void onItemClick(AdapterView<?> parent, View view, int position,
			long id) {

	}

	public void setCallback(StateDiskSettingsCallback callback) {
		mCallback = callback;
	}

	public StateDiskSettingsCallback getmCallback() {
		return mCallback;
	}

	public void hiddenOpMenuList() {
		mOpMenuList.setVisibility(View.GONE);
		// StateDiskSetting.getInstance().hiddenBig();
		// TimeShiftActivity.getInstance().getHandler().sendEmptyMessageDelayed(TimeShiftActivity.delayTime_Dialog,
		// 1000);
		StateDiskSetting.getInstance().getHandler().removeMessages(10);
		StateDiskSetting.getInstance().getHandler()
				.sendEmptyMessageDelayed(10, 15000);
	}

	private void showOpMenuList() {
		if (!("".equalsIgnoreCase(mountList.get(0).mMountPoint))) {
			mOpMenuList.setVisibility(View.VISIBLE);
		}
	}
	public MountPoint getSelectMountPoint(){
		if(mUsbList!=null && mountList!=null){
			try{
				return mountList.get(mUsbList.getSelectedItemPosition());
			}catch(Exception e){
				return null;
			}
		}
		return null;
	}
}
