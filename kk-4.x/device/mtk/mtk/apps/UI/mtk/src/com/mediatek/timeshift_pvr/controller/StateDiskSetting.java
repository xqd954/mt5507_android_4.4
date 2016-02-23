/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.controller;

import android.R.integer;
import android.app.Activity;
import android.app.Service;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnDismissListener;
import android.hardware.usb.UsbManager;
import android.os.Environment;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.ServiceManager;
import android.os.storage.IMountService;
import android.os.storage.StorageResultCode;
import android.util.Log;

import com.mediatek.dm.MountPoint;
import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.timeshift_pvr.ui.DiskSettingDialog;
import com.mediatek.timeshift_pvr.ui.DiskSettingSubMenuDialog;
import com.mediatek.ui.R;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.math.BigDecimal;
import java.util.ArrayList;

/**
 * 
 */
public class StateDiskSetting extends StateBase{

	public final static int SHOW_1ST_MENU = 1;
	private final static int SHOW_3RD_MENU = 2;
	private final static int DissMiss_All = 10;

	private final static int SHOW_SET_PVR = 3;
	private final static int SHOW_FORMATDISK = 4;
	private final static int SHOW_SPEEDTEST = 5;
	private final static int UPDATE_PROGRESS = 6;
	private final static int Refresh_Device = 1226;
	private final static int SHOW_SET_TSHIFT = 7;

	private static StateDiskSetting mStateSelf;
	private MyHandler handler;

	private DiskSettingDialog mSettingDialog;
	private DiskSettingSubMenuDialog mSubSettingDialog;

	private int mOPProgress = 0;
	private float maxSpeed = 0.0f;

	private boolean speedTesting = true;
	private static final String TAG = "StateDiskSetting";

	public static class MyHandler extends Handler {
		WeakReference<Activity> mActivity;

		MyHandler(Activity activity) {
			mActivity = new WeakReference<Activity>(activity);
		}

		public void handleMessage(Message msg) {
			Util.showDLog(TAG,"handleMessage:"+msg.what);
			switch (msg.what) {
			case SHOW_1ST_MENU:
				mStateSelf.showDiskSettingDialog();
				break;

			case SHOW_3RD_MENU:

				break;

			case SHOW_SET_TSHIFT:
				//mStateSelf.setTSHIFT();
				break;
			case SHOW_SET_PVR:
				mStateSelf.setPVR();
				
				break;
			case SHOW_FORMATDISK:
				mStateSelf.hiddenBigBigDialog();
				mStateSelf.formatDisk();
				break;
			case SHOW_SPEEDTEST:
				mStateSelf.hiddenBigBigDialog();
				mStateSelf.speedTest();
				break;

			case UPDATE_PROGRESS:
				Util.showDLog("handle Msg:UPDATE_PROGRESS");
				mStateSelf.refreshSubMenuDialog();
				break;
            case DissMiss_All:
                mStateSelf.hiddenBigBigDialog();
				break;
			case Refresh_Device:
				mStateSelf.mSettingDialog.refreshList();
                break;
			default:
				break;
			}

			super.handleMessage(msg);
		}
	}
	
	   public static StateDiskSetting getInstance(){
	        if (mStateSelf!=null) {
	            return mStateSelf;
	        }else {
	            return null;
	        }
	        
	    }

	public StateDiskSetting(Context mContext, TimeShiftManager manager) {
		super(mContext, manager);
		setType(StatusType.DISKSETTING);
		mStateSelf = this;
		handler = new MyHandler(this.getActivity());
		showDiskSettingDialog();
		setRunning(true);
	}

	/*
	 * (non-Javadoc)
	 */
	@Override
	public boolean onKeyDown(int keycode) {
		Util.showDLog("onKeyDown::: state disk setting.java" + keycode);
        handler.removeMessages(DissMiss_All);
        handler.sendEmptyMessageDelayed(DissMiss_All, 15000);
		return false;
	}

	/*
	 * 
	 */
	@Override
	public void onResume() {
		// TODO Auto-generated method stub

	}

	/* 
	 */
	@Override
	public void onPause() {
		// TODO Auto-generated method stub
	}

	/* 
	 */
	@Override
	public void onStop() {
		// TODO Auto-generated method stub

	}

	/* 
	 */
	@Override
	public void onRelease() {
		handler.sendEmptyMessage(DissMiss_All);
		setRunning(false);
	}

	/**
	 * 
	 */
	public void showDiskSettingDialog() {
		mSettingDialog = new DiskSettingDialog(getActivity(),
				R.layout.pvr_timeshfit_deviceinfo, this);

		mSettingDialog.setCallback(new Callback());
		mSettingDialog.setOnDismissListener(new OnDismissListener() {

			@Override
			public void onDismiss(DialogInterface dialog) {
				getManager().restoreToDefault(mStateSelf);
				handler.removeMessages(DissMiss_All);
			}
		});

		mSettingDialog.show();
		handler.sendEmptyMessageDelayed(DissMiss_All, 15000);
	}

	public DiskSettingDialog getDialog() {
		return mSettingDialog;
	}

	public MyHandler getHandler() {
		return handler;
	}

	public void setHandler(MyHandler handler) {
		this.handler = handler;
	}

	public boolean setPVR() {
		// TODO Auto-generated method stub
		ArrayList<MountPoint> deviceList = mStateSelf.getManager()
				.getDeviceList();

		Util.showDLog("deviceList.size()::" + deviceList.size());

		for (MountPoint point : deviceList) {
			Util.showDLog("mMountPoint: " + point.mMountPoint);
			Util.showDLog("mVolumeLabel: " + point.mVolumeLabel);
			Util.showDLog("mDeviceName: " + point.mDeviceName);
			Util.showDLog("mFreeSize: " + point.mFreeSize);
			Util.showDLog("mTotalSize: " + point.mTotalSize);
		}

		Util.tempSetPVR(deviceList.get(mSettingDialog
				.getSelectedPosition()).mMountPoint);
		getManager().setPvrMP(deviceList.get(mSettingDialog
				.getSelectedPosition()));
		mStateSelf.mSettingDialog.hiddenOpMenuList();
		mStateSelf.mSettingDialog.refreshList();
		return false;
	}

	public boolean formatDisk() {
		mSubSettingDialog = new DiskSettingSubMenuDialog(
				mStateSelf.getActivity(), mStateSelf,
				DiskSettingSubMenuDialog.UiType.FORMATCONFIRM);
		initSubSettingDialog();
		mSubSettingDialog.show();

		return false;
	}

	public boolean speedTest() {
		mSubSettingDialog = new DiskSettingSubMenuDialog(
				mStateSelf.getActivity(), mStateSelf,
				DiskSettingSubMenuDialog.UiType.SPEEDTEST_ING);
		initSubSettingDialog();
		mSubSettingDialog.show();

		return false;
	}

	/**
	 * 
	 */
	private void initSubSettingDialog() {
		mSubSettingDialog.setCallBack(new SubPageCallback());
		mSubSettingDialog.setOnDismissListener(new OnDismissListener() {
			@Override
			public void onDismiss(DialogInterface dialog) {
				if (isRunning()) {
					getHandler().sendEmptyMessage(SHOW_1ST_MENU);
				}
			}
		});
	}

	public void hiddenBigBigDialog() {
		Util.printStackTrace();
		if(mSettingDialog!=null&&mSettingDialog.isShowing()){
			mSettingDialog.dismiss();
		}
		
		if(mSubSettingDialog!=null&&mSubSettingDialog.isShowing()){
			mSubSettingDialog.dismiss();
		}
		handler.removeMessages(DissMiss_All);
		//getManager().restoreToDefault();
	}
	
	   public void hiddenBig() {
	        mSettingDialog.dismiss();
		mSettingDialog = null;
	    }

	public int getmOPProgress() {
		return mOPProgress;
	}

	public void setmOPProgress(int mOPProgress) {
		this.mOPProgress = mOPProgress;
		mStateSelf.getHandler().sendEmptyMessage(UPDATE_PROGRESS);
	}

	/**
	 * 
	 */
	private void refreshSubMenuDialog() {
		mSubSettingDialog.refreshProgres(getmOPProgress());
	}

	/**
	 * @return the maxSpeed
	 */
	public float getMaxSpeed() {
		return maxSpeed;
	}

	/**
	 * @param maxSpeed
	 *            the maxSpeed to set
	 */
	public void setMaxSpeed(float maxSpeed) {
		this.maxSpeed = maxSpeed;
		getManager().setDiskSpeed(maxSpeed);
	}

	/**
	 * @return the speedTesting
	 */
	public boolean isSpeedTesting() {
		return speedTesting;
	}

	/**
	 * @param speedTesting
	 *            the speedTesting to set
	 */
	public void setSpeedTesting(boolean speedTesting) {
		this.speedTesting = speedTesting;
	}

	public void restoreState(){
		if(mSubSettingDialog.isShowing()){
			mSubSettingDialog.setOnDismissListener(null);
			mSubSettingDialog.dismiss();
			
			//cancel format,cancel speed test,
		}
		showDiskSettingDialog();
	}
	
	class Callback implements StateDiskSettingsCallback {

		/*
	 * 
	 */
		@Override
		public boolean showSetPVR() {
			getHandler().sendEmptyMessage(SHOW_SET_PVR);
			return false;
		}

		/*
	 * 
	 */
		@Override
		public boolean showFormatDisk() {
			getHandler().sendEmptyMessage(SHOW_FORMATDISK);
			return false;
		}

		/*
	 * 
	 */
		@Override
		public boolean showSpeedTest() {
			getHandler().sendEmptyMessage(SHOW_SPEEDTEST);
			return false;
		}

		/*
		 * 
		 */
		@Override
		public boolean cancelFormat() {
			// TODO Auto-generated method stub
			return false;
		}

		/*
		 * 
		 */
		@Override
		public boolean cancelSpeedTest() {
			// TODO Auto-generated method stub
			return false;
		}

		@Override
		public boolean showSetTSHIFT() {
			// TODO Auto-generated method stub
			getHandler().sendEmptyMessage(SHOW_SET_TSHIFT);
			return false;
		}
	}

	class SubPageCallback implements StateDiskSettingsCallback {

		@Override
		public boolean showSetTSHIFT() {
			// TODO Auto-generated method stub
			return false;
		}
		/*
		 * 
		 */
		@Override
		public boolean showSetPVR() {
			// TODO Auto-generated method stub
			return false;
		}

		/*
		 * 
		 */
		@Override
		public boolean showFormatDisk() {
		   
			new Thread((new Runnable() {
				@Override
				public void run() {
					// start format,and refresh UI.
					if(mSettingDialog.getSelectMountPoint()!=null){
						Util.fomatDisk(mSettingDialog.getSelectMountPoint());
					}
					for (int i = 0; i < 101; i++) {
						try {
							Thread.sleep(30);
							mStateSelf.setmOPProgress(i);
						} catch (Exception e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}

				}

			})).start();
			return false;
		}

		/*
		 * 
		 */
		@Override
		public boolean showSpeedTest() {
			new Thread((new Runnable() {
				@Override
				public void run() {
					// start format,and refresh UI.
					// for (int i = 0; i < 101; i++) {
					// try {
					// Thread.sleep(300);
					// setmOPProgress(i);
					// } catch (InterruptedException e) {
					// e.printStackTrace();
					// }
					// }
					// Util.speedTest(getManager());
					// setMaxSpeed(8.6f);

					// -------------------------------------------------------

					Util.showDLog("showSpeedTest()");
					
					setSpeedTesting(true);

					TimeShiftManager manager = mStateSelf.getManager();
					if (!manager.hasRemovableDisk()) {
						setSpeedTesting(false);
						return;
					}
					int index = (int) (Math.random() * 1000);

					if (manager.getPvrMP() == null) {
						manager.setPvrMP(manager.getDeviceList().get(0));

					}
					String path = String.format(
							manager.getPvrMP().mMountPoint + "/"
									+ "speedTest%d.dat", index);
					Util.showDLog("path:" + path);
					File testFile = new File(path);

					float maxSpeed = 0.0f;
					setMaxSpeed(maxSpeed);

					Long MinTime = Long.MAX_VALUE;

					if (testFile.exists()) {
						testFile.delete();
					}
					try {
						testFile.createNewFile();
					} catch (IOException e) {
						e.printStackTrace();
						setSpeedTesting(false);
						setMaxSpeed(0.0f);

						setmOPProgress(100);
						return;
					}

					int bufferSize = 1024 * 120; // 7.7
					final float defaultCount = 300L;
					float counts = defaultCount;

					byte[] writeByte = new byte[(int) (bufferSize)];

					FileOutputStream fis = null;
					Long startTime = 0L;
					Long endTime;
					try {
						fis = new FileOutputStream(testFile);
					} catch (FileNotFoundException e1) {
						setSpeedTesting(false);
						setMaxSpeed(0.0f);

						setmOPProgress(100);
						return;
					}
					startTime = System.currentTimeMillis();
					Long startTime1 = 0L;
					Long startTime2 = 0L;

					int progress = 0;
					try {

						while (counts > 0) {
							if (!isSpeedTesting()) {
								return;
							}
							startTime1 = System.currentTimeMillis();
							fis.write(writeByte);
							startTime2 = System.currentTimeMillis();

							if (MinTime > startTime2 - startTime1
									&& (startTime2 - startTime1) > 0) {
								MinTime = startTime2 - startTime1;
							}

							counts--;
							progress = (int) (((defaultCount - counts) / defaultCount) * 100);
							//Util.showDLog("Progress:" + progress);
							if (progress != 100) {
								setmOPProgress(progress);
							}
						}
						fis.close();
					} catch (FileNotFoundException e) {
						e.printStackTrace();
						setSpeedTesting(false);
						bufferSize=0;
					} catch (IOException e) {
						e.printStackTrace();
						setSpeedTesting(false);
						bufferSize=0;
					} finally {
						testFile.delete();
					}

					endTime = System.currentTimeMillis();

					maxSpeed = (float) (bufferSize * 1000f / MinTime / 1024 / 1024);
					maxSpeed = (new BigDecimal(maxSpeed).setScale(1,
							BigDecimal.ROUND_HALF_UP)).floatValue();

					float average = bufferSize * defaultCount * 1000L
							/ (endTime - startTime) / 1024 / 1024;
					Util.showDLog(String.format("Speed:%3.1f MB/s ",
							average));
					setSpeedTesting(false);
					setMaxSpeed(average);

					setmOPProgress(100);

				}
			})).start();
			return false;
		}

		/*
		 * 
		 */
		@Override
		public boolean cancelFormat() {
			// TODO Auto-generated method stub
			return false;
		}

		/*
		 * 
		 */
		@Override
		public boolean cancelSpeedTest() {
			// TODO Auto-generated method stub
			return false;
		}
	}
	public void updateDissmissTimer(){
		handler.removeMessages(DissMiss_All);
		handler.sendEmptyMessageDelayed(DissMiss_All, 15000);
	}
}
