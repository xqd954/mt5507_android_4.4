package com.mediatek.timeshift_pvr.ui;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageParser.NewPermissionInfo;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnFocusChangeListener;
import android.view.View.OnTouchListener;
import android.widget.Button;
import android.widget.TextView;

import com.mediatek.timeshift_pvr.controller.StatePVR;
import com.mediatek.timeshift_pvr.manager.PvrConstant;
import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.mmp.MeidaMainActivity;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.nav.commonview.BannerView;
import com.mediatek.ui.nav.commonview.ShowSourceListView;
import com.mediatek.ui.setup.Loading;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

/**
 * dialog factory,get types of dialogs here
 * 
 * @author hs_haosun
 * 
 */

public class PvrDialog extends Dialog implements Runnable {
	private TextView titleView, textView, waitView;
	private Loading loading;
	private Button buttonYes, buttonNo;
	// dialog title
	private String title;
	// dialog display information
	private String message;
	private String buttonYesName;
	private String buttonNoName;
	private String buttonOKName;
	private int xOff;
	private int yOff;
	private int focusedButton = 0;
	private String TAG = "TurnkeyCommDialog";
	Window window;
	WindowManager.LayoutParams lp;
	public int width = 0;
	public int height = 0;
	private int type = 0;
	private int keyCode = 0;
	public static final int TYPE_BGM = 0;
	public static final int TYPE_Confirm = 1;
	public static final int TYPE_Normal = 2;
	public static final int TYPE_Record = 1;
	public static final int TYPE_Timeshift = 2;
	public static final int TYPE_STOP_PVR_PLAYING_ENTER_MMP = 3;
	public static final int TYPE_Change_Source = 11234;
	private static final int TYPE_ChangeChanel_UP = 11235;
	private static final int TYPE_ChangeChanel_Down = 11236;
	private static final int TYPE_ChangeChanel_Pre = 11237;
	public static final int TYPE_Change_ChannelNum = 11238;
	private static final int TYPE_PIPPOP = 11239;
	public static final int TYPE_Confirm_From_ChannelList = 0xA001;
	private static final int Msg_ID_Change_Channel = 0xA002;
	
	public static final int KEYCODE_FROM_FAV = 0xA002;
	
	public static final int TYPE_Change_Source_By_Src_Key = 0xA003;
	
	private OnPVRDialogListener onPVRDialogListener;
	
	private Context mContext;
	private Activity mActivity;
	ShowSourceListView dialog;
	BannerView bannerView;
	int modeValue;
	Handler mHandler = new Handler() {
		public void handleMessage(android.os.Message msg) {
			switch (msg.what) {
			case 123:
				getOnPVRDialogListener().onPVRDialogListener(KEYCODE_FROM_FAV);
				break;
			case TYPE_Change_Source_By_Src_Key:
				if(getOnPVRDialogListener()!=null){
					getOnPVRDialogListener().onPVRDialogListener(KeyMap.KEYCODE_MTKIR_SOURCE);
				}
				break;
			case TYPE_ChangeChanel_UP:
				if(getOnPVRDialogListener()!=null){
					getOnPVRDialogListener().onPVRDialogListener(KeyMap.KEYCODE_MTKIR_CHUP);
				}
				break;
			case TYPE_ChangeChanel_Down:
				if(getOnPVRDialogListener()!=null){
					getOnPVRDialogListener().onPVRDialogListener(KeyMap.KEYCODE_MTKIR_CHDN);
				}
				break;
			case TYPE_ChangeChanel_Pre:
				if(getOnPVRDialogListener()!=null){
					getOnPVRDialogListener().onPVRDialogListener(KeyMap.KEYCODE_MTKIR_PRECH);
				}
				break;
			case TYPE_PIPPOP:
				MtkLog.e("pippop", "pippop:");
				TurnkeyUiMainActivity.getInstance().onKeyDown(KeyMap.KEYCODE_MTKIR_PIPPOP, new KeyEvent(KeyEvent.ACTION_DOWN,KeyMap.KEYCODE_MTKIR_PIPPOP));
				break;
			
			case Msg_ID_Change_Channel:
				MtkLog.e("FAV", "Msg_ID_Change_Channel:");
				if(getOnPVRDialogListener()!=null){
					getOnPVRDialogListener().onPVRDialogListener(msg.arg1);
				}
				break;
			default:
				break;
			}
		};
	};

	public PvrDialog(Context context, int type) {
		super(context, R.style.Theme_pvr_Dialog);
		window = getWindow();
		lp = window.getAttributes();
		this.type = type;
		mContext = context;
		mSelectedChannelID = -1;
	}

	public PvrDialog(Activity context, int type, int keyCode, int mode) {
		super(context, R.style.Theme_pvr_Dialog);
		window = getWindow();
		lp = window.getAttributes();
		this.type = type;
		this.keyCode = keyCode;
		mContext = context;
		mActivity = context;
		modeValue = mode;
	}

	public PvrDialog(Activity context, int type, int keyCode,
			ShowSourceListView dialog, int mode) {
		super(context, R.style.Theme_pvr_Dialog);
		window = getWindow();
		lp = window.getAttributes();
		this.type = type;
		this.keyCode = keyCode;
		mContext = context;
		mActivity = context;
		this.dialog = dialog;
		modeValue = mode;
	}

	public PvrDialog(Activity context, int type, int keyCode,
			BannerView dialog, int mode) {
		super(context, R.style.Theme_pvr_Dialog);
		window = getWindow();
		lp = window.getAttributes();
		this.type = type;
		this.keyCode = keyCode;
		mContext = context;
		mActivity = context;
		this.bannerView = dialog;
		modeValue = mode;
	}

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.pvr_dialog_one_button);
		initview(type);
		setPositon(0, 0);
	}

	private void initview(int type) {
		titleView = (TextView) findViewById(R.id.comm_dialog_title);
		textView = (TextView) findViewById(R.id.comm_dialog_text);
		buttonYes = (Button) findViewById(R.id.comm_dialog_buttonYes);
		buttonNo = (Button) findViewById(R.id.comm_dialog_buttonNo);
		
		buttonNo.setVisibility(View.VISIBLE);
		switch (type) {
		case TYPE_BGM:
			if (StatePVR.getInstance() != null
					&& StatePVR.getInstance().isRunning()) {
				title = "warning:";
				message = "Do you want to just shut down the screen with the PVR running or Not?";
				titleView.setText(title);
				textView.setText(message);
				buttonNo.setVisibility(View.VISIBLE);

				buttonYes.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
						//BGMManager.getInstance().setWakeupReason(98);
						// shut down the screen
						//BGMManager.getInstance().setScreenOff();
					}
				});

				buttonNo.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
						//BGMManager.getInstance().setWakeupReasonNormal();
						//BGMManager.getInstance().powerOFF();
					}
				});

			} else {
				title = "warning:";
				message = "please cancel the BGM state!";
				buttonNo.setVisibility(View.GONE);
				titleView.setText(title);
				textView.setText(message);
				buttonYes.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
					}
				});
			}
			break;
		case TYPE_Confirm:
			title = "warning:";
			titleView.setText(title);
			buttonNo.setVisibility(View.VISIBLE);

			switch (keyCode) {
			case KeyEvent.KEYCODE_GUIDE:
			case KeyMap.KEYCODE_MTKIR_GUIDE:
				if (modeValue == TYPE_Record) {
					message = "Do you want to stop the record process,\n and enter EPG?";
				} else if (modeValue == TYPE_Timeshift) {
					message = "Do you want to stop the timeshift,\n and enter EPG?";
				}

				textView.setText(message);

				buttonYes.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
						TimeShiftManager.getInstance().stopAllRunning();
						TimeShiftManager
								.getInstance()
								.getTopHandler()
								.removeMessages(
										PvrConstant.RECORD_FINISH_AND_SAVE);

						if (NavIntegration.getInstance(mContext).isCurrentSourceTv()) {
							//EPGManager.getInstance(mActivity).startEpg(
							//		mActivity, NavBasic.NAV_REQUEST_CODE);
							TimeShiftManager
									.getInstance()
									.getTopHandler()
									.removeMessages(
											PvrConstant.RECORD_FINISH_AND_SAVE);

						} else {

						}
					}
				});

				buttonNo.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
					}
				});
				break;

			case KeyEvent.KEYCODE_CHANNEL_UP:
			case KeyMap.KEYCODE_MTKIR_CHUP:
				if (modeValue == TYPE_Record) {
					message = "Do you want to stop the record process,\n and change channel?";
				} else if (modeValue == TYPE_Timeshift) {
					message = "Do you want to stop the timeshift,\n and change channel?";
				}

				textView.setText(message);

				buttonYes.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
						TimeShiftManager.getInstance().stopAllRunning();
						TimeShiftManager
								.getInstance()
								.getTopHandler()
								.removeMessages(
										PvrConstant.RECORD_FINISH_AND_SAVE);
						mHandler.sendEmptyMessageDelayed(TYPE_ChangeChanel_UP,
								500);

					}
				});

				buttonNo.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
					}
				});
				break;
			case KeyMap.KEYCODE_MTKIR_CHDN:
			case KeyEvent.KEYCODE_CHANNEL_DOWN:
				if (modeValue == TYPE_Record) {
					message = "Do you want to stop the record process,\n and change channel?";
				} else if (modeValue == TYPE_Timeshift) {
					message = "Do you want to stop the timeshift,\n and change channel?";
				}
				textView.setText(message);

				buttonYes.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
						TimeShiftManager.getInstance().stopAllRunning();
						TimeShiftManager
								.getInstance()
								.getTopHandler()
								.removeMessages(
										PvrConstant.RECORD_FINISH_AND_SAVE);
						mHandler.sendEmptyMessageDelayed(
								TYPE_ChangeChanel_Down, 500);
					}
				});

				buttonNo.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
					}
				});
				break;
			case KeyMap.KEYCODE_MTKIR_PRECH:
				if (modeValue == TYPE_Record) {
					message = "Do you want to stop the record process,\n and change channel?";
				} else if (modeValue == TYPE_Timeshift) {
					message = "Do you want to stop the timeshift,\n and change channel?";
				}
				textView.setText(message);

				buttonYes.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
						TimeShiftManager.getInstance().stopAllRunning();
						TimeShiftManager
								.getInstance()
								.getTopHandler()
								.removeMessages(
										PvrConstant.RECORD_FINISH_AND_SAVE);
						mHandler.sendEmptyMessageDelayed(TYPE_ChangeChanel_Pre,
								500);
					}
				});

				buttonNo.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
					}
				});
				break;
			case PvrDialog.TYPE_Change_ChannelNum:
				if (modeValue == TYPE_Record) {
					message = "Do you want to stop the record process,\n and change channel?";
				} else if (modeValue == TYPE_Timeshift) {
					message = "Do you want to stop the timeshift,\n and change channel?";
				}
				textView.setText(message);

				buttonYes.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
						TimeShiftManager.getInstance().stopAllRunning();
						TimeShiftManager
								.getInstance()
								.getTopHandler()
								.removeMessages(
										PvrConstant.RECORD_FINISH_AND_SAVE);

						//mark
						/*if (mSelectedChannelID != -1) {
							Message msg = TimeShiftManager.getInstance()
									.getTopHandler().obtainMessage();
							msg.what = TimeShiftManager.CHANGE_CHANNEL;
							msg.arg1 = mSelectedChannelID;
							msg.arg2 = keyCode;
							Util.showDLog("msg.arg2=keyCode:," + keyCode);
							TimeShiftManager.getInstance().getTopHandler()
									.sendMessageDelayed(msg, 1500);
						} else {
							bannerView.pvrChangeNum(mSelectedChannelNumString);
						}*/

					}
				});

				buttonNo.setVisibility(View.VISIBLE);
				buttonNo.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
					}
				});
				break;
			case KeyMap.KEYCODE_MTKIR_RED:
				if (modeValue == TYPE_Record) {
					message = "Do you want to stop the record process,\n and enter Launcher?";
				} else if (modeValue == TYPE_Timeshift) {
					message = "Do you want to stop the timeshift,\n and enter Launcher?";
				} 
				textView.setText(message);

				buttonYes.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						if(getOnPVRDialogListener()!=null){
							getOnPVRDialogListener().onPVRDialogListener(KeyMap.KEYCODE_MTKIR_RED);
						}else if (TurnkeyUiMainActivity.getInstance() != null) {
							TurnkeyUiMainActivity.getInstance().startLauncher();
						}
						PvrDialog.this.dismiss();
					}
				});

				buttonNo.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
					}
				});
				break;
			case KeyMap.KEYCODE_MTKIR_ANGLE:
				if (modeValue == TYPE_Record) {
					message = "Do you want to stop the record process,\n and enter MMP?";
				} else if (modeValue == TYPE_Timeshift) {
					message = "Do you want to stop the timeshift,\n and enter MMP?";
				} else if (modeValue == TYPE_STOP_PVR_PLAYING_ENTER_MMP) {
					message = "Do you want to Enter MMP and leave PVR\n Play?";
				}
				textView.setText(message);

				buttonYes.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
						TimeShiftManager.getInstance().stopAllRunning();
						TimeShiftManager.getInstance().getTopHandler()
								.removeMessages(311);
						TimeShiftManager
								.getInstance()
								.getTopHandler()
								.removeMessages(
										PvrConstant.RECORD_FINISH_AND_SAVE);
						TimeShiftManager
								.getInstance()
								.getTopHandler()
								.sendEmptyMessage(PvrConstant.Dissmiss_Info_Bar);
						
						if(getOnPVRDialogListener()!=null){
							getOnPVRDialogListener().onPVRDialogListener(KeyMap.KEYCODE_MTKIR_ANGLE);
						}else if (TurnkeyUiMainActivity.getInstance() != null) {
							TurnkeyUiMainActivity.getInstance().enterMMP();
						}
					}
				});

				buttonNo.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
					}
				});
				break;
			case KeyMap.KEYCODE_MTKIR_PIPPOP:
				if (modeValue == TYPE_Record) {
					message = "Do you want to stop the record process,\n and enter PIP/POP?";
				} else if (modeValue == TYPE_Timeshift) {
					message = "Do you want to stop the timeshift,\n and enter PIP/POP?";
				}
				textView.setText(message);

				buttonYes.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
						TimeShiftManager.getInstance().stopAllRunning();
						TimeShiftManager
								.getInstance()
								.getTopHandler()
								.removeMessages(
										PvrConstant.RECORD_FINISH_AND_SAVE);
						mHandler.sendEmptyMessageDelayed(TYPE_PIPPOP, 1500);
					}
				});

				buttonNo.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
					}
				});
				break;
			case TYPE_Change_Source:
				if (modeValue == TYPE_Record) {
					message = "Do you want to stop the record process,\n and change source?";
				} else if (modeValue == TYPE_Timeshift) {
					message = "Do you want to stop the timeshift,\n and change source?";
				}
				textView.setText(message);

				buttonYes.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
						TimeShiftManager.getInstance().stopAllRunning();
						TimeShiftManager
								.getInstance()
								.getTopHandler()
								.removeMessages(
										PvrConstant.RECORD_FINISH_AND_SAVE);
						mHandler.sendEmptyMessageDelayed(123, 100);
					}
				});

				buttonNo.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
					}
				});
				break;
			
			case TYPE_Change_Source_By_Src_Key:
				if (modeValue == TYPE_Record) {
					message = "Do you want to stop the record process,\n and change source?";
				} else if (modeValue == TYPE_Timeshift) {
					message = "Do you want to stop the timeshift,\n and change source?";
				}
				textView.setText(message);

				buttonYes.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
						TimeShiftManager.getInstance().stopAllRunning();
						TimeShiftManager
								.getInstance()
								.getTopHandler()
								.removeMessages(
										PvrConstant.RECORD_FINISH_AND_SAVE);
						mHandler.sendEmptyMessageDelayed(TYPE_Change_Source_By_Src_Key, 100);
					}
				});

				buttonNo.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
					}
				});
				break;
			case PvrDialog.KEYCODE_FROM_FAV:
				if (modeValue == TYPE_Record) {
					message = "Do you want to stop the record process,\n and change channel?";
				} else if (modeValue == TYPE_Timeshift) {
					message = "Do you want to stop the timeshift,\n and change channel?";
				}
				textView.setText(message);

				buttonYes.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
						TimeShiftManager.getInstance().stopAllRunning();
						TimeShiftManager
								.getInstance()
								.getTopHandler()
								.removeMessages(
										PvrConstant.RECORD_FINISH_AND_SAVE);
						
						Message msg=mHandler.obtainMessage();
						msg.what=Msg_ID_Change_Channel;
						msg.arg1=getFavChannel();
						mHandler.sendMessageDelayed(msg, 100);
					}
				});

				buttonNo.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
					}
				});
				break;
			default:
				break;
			}

			break;
		case TYPE_Normal:

			break;
		case TYPE_Confirm_From_ChannelList:
			switch (keyCode) {
			
			case KeyEvent.KEYCODE_CHANNEL_UP:
			case KeyMap.KEYCODE_MTKIR_CHUP:
				
			case KeyMap.KEYCODE_MTKIR_CHDN:
			case KeyEvent.KEYCODE_CHANNEL_DOWN:
				
			case KeyMap.KEYCODE_MTKIR_PRECH:
			case KeyMap.KEYCODE_DPAD_CENTER:
			case PvrDialog.TYPE_Change_ChannelNum:
				if (modeValue == TYPE_Record) {
					message = "Do you want to stop the record process,\n and change channel?";
				} else if (modeValue == TYPE_Timeshift) {
					message = "Do you want to stop the timeshift,\n and change channel?";
				}
				textView.setText(message);
				buttonNo.setVisibility(View.VISIBLE);

				buttonYes.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
						TimeShiftManager.getInstance().stopAllRunning();
						TimeShiftManager
								.getInstance()
								.getTopHandler()
								.removeMessages(
										PvrConstant.RECORD_FINISH_AND_SAVE);

						/*if (mSelectedChannelID != -1) {
							Message msg = TimeShiftManager.getInstance()
									.getTopHandler().obtainMessage();
							msg.what = TimeShiftManager.CHANGE_CHANNEL;
							msg.arg1 = mSelectedChannelID;
							msg.arg2 = keyCode;
							Util.showDLog("msg.arg2=keyCode:," + keyCode);
							TimeShiftManager.getInstance().getTopHandler()
									.sendMessageDelayed(msg, 1500);
						} else {
							bannerView.pvrChangeNum(mSelectedChannelNumString);
						}*/
						if(getOnPVRDialogListener()!=null){
							getOnPVRDialogListener().onPVRDialogListener(keyCode);
						}
					}
				});

				buttonNo.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
					}
				});
				break;
			case KeyMap.KEYCODE_MTKIR_ANGLE:
				if (modeValue == TYPE_Record) {
					message = "Do you want to stop the record process,\n and enter MMP?";
				} else if (modeValue == TYPE_Timeshift) {
					message = "Do you want to stop the timeshift,\n and enter MMP?";
				} else if (modeValue == TYPE_STOP_PVR_PLAYING_ENTER_MMP) {
					message = "Do you want to Enter MMP and leave PVR\n Play?";
				}
				textView.setText(message);

				buttonYes.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
						TimeShiftManager.getInstance().stopAllRunning();
						TimeShiftManager.getInstance().getTopHandler()
								.removeMessages(311);
						TimeShiftManager
								.getInstance()
								.getTopHandler()
								.removeMessages(
										PvrConstant.RECORD_FINISH_AND_SAVE);
						TimeShiftManager
								.getInstance()
								.getTopHandler()
								.sendEmptyMessage(PvrConstant.Dissmiss_Info_Bar);
						if(getOnPVRDialogListener()!=null){
							getOnPVRDialogListener().onPVRDialogListener(KeyMap.KEYCODE_MTKIR_ANGLE);
						}else if (TurnkeyUiMainActivity.getInstance() != null) {
							TurnkeyUiMainActivity.getInstance().enterMMP();
						}
					}
				});

				buttonNo.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
					}
				});
				break;
			case KeyMap.KEYCODE_MTKIR_PIPPOP:
				if (modeValue == TYPE_Record) {
					message = "Do you want to stop the record process,\n and enter PIP/POP?";
				} else if (modeValue == TYPE_Timeshift) {
					message = "Do you want to stop the timeshift,\n and enter PIP/POP?";
				}
				textView.setText(message);

				buttonYes.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
						TimeShiftManager.getInstance().stopAllRunning();
						TimeShiftManager
								.getInstance()
								.getTopHandler()
								.removeMessages(
										PvrConstant.RECORD_FINISH_AND_SAVE);

						mHandler.sendEmptyMessageDelayed(TYPE_PIPPOP, 1500);
						// KeyDispatch.getInstance().passKeyToNative(keyCode,
						// null);
						// ComponentStatusListener.getInstance().updateStatus(
						// ComponentStatusListener.NAV_KEY_OCCUR, keyCode);
					}
				});

				buttonNo.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
					}
				});
				break;
			case TYPE_Change_Source:
				if (modeValue == TYPE_Record) {
					message = "Do you want to stop the record process,\n and change source?";
				} else if (modeValue == TYPE_Timeshift) {
					message = "Do you want to stop the timeshift,\n and change source?";
				}
				textView.setText(message);

				buttonYes.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
						TimeShiftManager.getInstance().stopAllRunning();
						TimeShiftManager
								.getInstance()
								.getTopHandler()
								.removeMessages(
										PvrConstant.RECORD_FINISH_AND_SAVE);
						mHandler.sendEmptyMessageDelayed(123, 100);

					}
				});

				buttonNo.setOnClickListener(new View.OnClickListener() {

					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						PvrDialog.this.dismiss();
					}
				});
				break;
			default:
				break;
			}

			break;
		default:
			break;
		}

		buttonYes.setFocusable(true);
		buttonYes.requestFocus();

	}

	/**
	 * Set the dialog's position relative to the (0,0)
	 */
	public void setPositon(int xoff, int yoff) {
		Window window = PvrDialog.this.getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();
		lp.x = xoff;
		lp.y = yoff;
		this.xOff = xoff;
		this.yOff = yoff;
		window.setAttributes(lp);
	}

	@Override
	public void dismiss() {
		// TODO Auto-generated method stub
		super.dismiss();
		//TimeShiftManager.getInstance().setBGMState(false);
	}

	public Button getButtonYes() {
		return buttonYes;
	}

	public Button getButtonNo() {
		return buttonNo;
	}

	public void setButtonYesName(String buttonYesName) {
		this.buttonYesName = buttonYesName;
	}

	public void setButtonNoName(String buttonNoName) {
		this.buttonNoName = buttonNoName;
	}

	public void setMessage(String info) {
		this.message = info;
	}

	public void run() {
	}

	public TextView getTextView() {
		return textView;
	}

	public TextView getWaitView() {
		return waitView;
	}

	public Loading getLoading() {
		return loading;
	}

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		return true;
	}

	// for volume down/up
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		switch (keyCode) {
		case KeyMap.KEYCODE_BACK:
			this.dismiss();
			break;
		case KeyMap.KEYCODE_VOLUME_UP:
			return true;

		default:
			break;
		}
		return super.onKeyDown(keyCode, event);
	}

	private String mSelectedChannelNumString = "0";
	private int mSelectedChannelID = -1;

	private int mFavChannelKeyCode;
	
	public void setChangeChannelNum(String value) {
		mSelectedChannelNumString = value;
	}

	public void setMtkTvChannelInfoBase(int channelID) {
		mSelectedChannelID = channelID;
	}

	public void setFavChannel(int keyCode) {
		// TODO Auto-generated method stub
		mFavChannelKeyCode=keyCode;
	}
	public int getFavChannel() {
		// TODO Auto-generated method stub
		return mFavChannelKeyCode;
	}

	public OnPVRDialogListener getOnPVRDialogListener() {
		return onPVRDialogListener;
	}

	public void setOnPVRDialogListener(OnPVRDialogListener onPVRDialogListener) {
		this.onPVRDialogListener = onPVRDialogListener;
	}

}
