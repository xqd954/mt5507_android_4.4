package com.mediatek.ui.nav;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.menu.MenuMain;
import com.mediatek.ui.mmp.util.LogicManager;
import com.mediatek.ui.nav.commonview.CaptureSelectArea;
import com.mediatek.ui.nav.util.CaptureLogoImp;
import com.mediatek.ui.nav.util.NavSundryImplement;
import com.mediatek.ui.util.DestroyApp;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;
import com.mediatek.tvcm.TVStorage;

/**
 * this activity is used to show capture logo screen, when this activity start,
 * the TV screen will freeze firstly, and cancel freezing when stop this
 * activity. You can select full screen or special area as power on picture and
 * save this picture to special place.
 * 
 * @author MTK40462
 * 
 */
public class CaptureLogoActivity extends Activity {

	private static final String TAG = "CaptureLogoDialog";

	public static final String FROM_MMP = "mmp";

	public static final int MMP_PHOTO = 1;

	public static final int MMP_VIDEO = 2;

	// show save position view
	private LinearLayout saveLogoView = null;
	// capture logo dialog layout
	public static LinearLayout captureMain = null;

	private TextView mContextTextView;
	private TextView mSavePosition;
	private ProgressBar mCapturingProgressBar;
	private Button button1;
	private Button button2;
	private CaptureSelectArea mSelectArea;
	private CaptureLogoImp capImp;

	private View mRootView;

	// capture logo view dismiss time
	public static final int AUTO_DISMISS_TIME = 5000;
	public static final int MESSAGE_DISMISS = 1;

	private Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case MESSAGE_DISMISS:
				CaptureLogoActivity.this.finish();
				MtkLog.d(TAG, " -------receive message----------");
				break;
			}
		}
	};

	// the selected save position
	String[] saveIds = { "0", "1" };
	static int selectSaveId = 0;

	// identify which screen is shown
	int status = -1;

	private int mContentViewId;
	private String content;
	private String bt_left;
	private String bt_right;

	private static final int CAPTURELOGO_CANCEL = -1;

	private int mType = -1;

	private boolean isFreezing = false;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.nav_capture_logo_main_view);

		capImp = CaptureLogoImp.getInstance(this);
		isFreezing = NavSundryImplement.getInsNavSundryImplement(this)
				.isFreeze();
		if (!isFreezing) {
			capImp.freezeScreen(true);
		}
		mHandler.sendEmptyMessageDelayed(MESSAGE_DISMISS, AUTO_DISMISS_TIME);
		initUI();
		initData();
	}

	@Override
	protected void onPause() {
		super.onPause();
		if (!isFreezing) {
			capImp.freezeScreen(false);
		}
	}

	public int getContentViewId() {
		return mContentViewId;
	}

	public void setContentViewId(int mContentViewId) {
		this.mContentViewId = mContentViewId;
	}

	/**
	 * init
	 */
	private void initUI() {
		mContextTextView = (TextView) findViewById(R.id.cap_logo_msg);
		captureMain = (LinearLayout) findViewById(R.id.capture_main);
		LayoutParams mLayoutParams = new LinearLayout.LayoutParams((int)(ScreenConstant.SCREEN_WIDTH*0.43), (int)(ScreenConstant.SCREEN_HEIGHT *0.417));
		captureMain.setLayoutParams(mLayoutParams);
		button1 = (Button) findViewById(R.id.bt_left);
		button2 = (Button) findViewById(R.id.bt_right);
		mCapturingProgressBar = (ProgressBar) findViewById(R.id.cap_logo_progressbar);
		saveLogoView = (LinearLayout) findViewById(R.id.savePositionView);
		mSavePosition = (TextView) findViewById(R.id.save_position_select);

		mRootView = findViewById(R.id.capturelogo_bg);

	}

	/**
	 * set the progress bar visibility
	 * 
	 * @param v
	 *            Visible or Invisible
	 * 
	 */
	public void setMyProgressBarVisibility(int v) {
		mCapturingProgressBar.setVisibility(v);
	}

	/**
	 * set select save position view visibility
	 * 
	 * @param v
	 *            Visible or Invisible
	 */
	public void setSelectSavePositionView(int v) {
		saveLogoView.setVisibility(v);
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		mHandler.removeMessages(MESSAGE_DISMISS);
		mHandler.sendEmptyMessageDelayed(MESSAGE_DISMISS, AUTO_DISMISS_TIME);

		switch (keyCode) {

		case KeyMap.KEYCODE_DPAD_UP:
			if (status == DialogId.SELECT_SAVE_POSITION) {
				mSavePosition.setBackgroundResource(R.drawable.selectbg_nor);
			}
			break;
		case KeyMap.KEYCODE_DPAD_DOWN:
			if (status == DialogId.SELECT_SAVE_POSITION) {
				mSavePosition.setBackgroundResource(R.drawable.selectbg_gray);
				button1.requestFocus();
			}
			break;
		case KeyMap.KEYCODE_DPAD_LEFT:
			if (status == DialogId.SELECT_SAVE_POSITION && !button1.hasFocus()
					&& !button2.hasFocus()) {
				if (selectSaveId > 0) {
					selectSaveId--;
				} else {
					selectSaveId = saveIds.length - 1;
				}
				mSavePosition.setText(saveIds[selectSaveId]);
			}
			break;
		case KeyMap.KEYCODE_DPAD_RIGHT:
			if (status == DialogId.SELECT_SAVE_POSITION && !button1.hasFocus()
					&& !button2.hasFocus()) {
				if (selectSaveId < saveIds.length - 1) {
					selectSaveId++;
				} else {
					selectSaveId = 0;
				}
				mSavePosition.setText(saveIds[selectSaveId]);

			}
			break;
		// Added by Dan 20111118 for fix bug DTV00373196
		case KeyMap.KEYCODE_MTKIR_RECORD:
		case KeyMap.KEYCODE_MTKIR_YELLOW:
			MtkLog.d(TAG, "------------------ pressed YELLOW KEY");
			// Added by Dan 20120227 for fix CR DTV00399663
			if (keyCode == KeyMap.KEYCODE_MTKIR_YELLOW 
					&& (mType == MMP_VIDEO || mType == MMP_PHOTO)) {
				break;
			}
			
			if (status == DialogId.CAPTURING) {
				capImp.removeLogoCaptureListener(getCaptureSourceType());
			}
			this.finish();
			break;

		case KeyMap.KEYCODE_DPAD_CENTER:
			if (status == DialogId.CAPTURE_RESULT) {
				this.finish();
				break;
			}
			break;
		case KeyEvent.KEYCODE_MENU:
			// Added by Dan 20111118 for fix bug DTV00373178&DTV00375599
			if (mType == MMP_VIDEO || mType == MMP_PHOTO) {
				break;
			}

			if (status == DialogId.CAPTURING) {
				capImp.removeLogoCaptureListener(getCaptureSourceType());
			}
			Intent intent = new Intent(CaptureLogoActivity.this, MenuMain.class);
			startActivity(intent);
			finish();
			break;
		case KeyMap.KEYCODE_MTKIR_MUTE: {
			if (mType == MMP_VIDEO || mType == MMP_PHOTO) {
				return true;
			} else {
				TurnkeyUiMainActivity.switchMute();
				return true;
			}
		}//end
		// Added by Dan 20111118 for fix bug DTV00373187
		case KeyMap.KEYCODE_MTKIR_GUIDE:
			((DestroyApp) getApplication()).finishAll();
			if (status == DialogId.CAPTURING) {
				capImp.removeLogoCaptureListener(getCaptureSourceType());
			}
			intent = new Intent(CaptureLogoActivity.this, EPGActivity.class);
			startActivity(intent);
			finish();
			break;
		// added by keke 1.2 for DTV00386546
		case KeyMap.KEYCODE_MTKIR_ANGLE: {
			if (mType == MMP_PHOTO || mType == MMP_VIDEO) {
				mHandler.removeMessages(MESSAGE_DISMISS);
				((DestroyApp) getApplication()).finishAll();
				if (status == DialogId.CAPTURING) {
					capImp.removeLogoCaptureListener(getCaptureSourceType());
				}
				LogicManager.getInstance(this).finishAudioService();
				LogicManager.getInstance(this).finishVideo();
				finish();
			}
			break;
		}
		//end
		}

		return super.onKeyDown(keyCode, event);
	}

	public void createDialog(int id) {
		switch (id) {
		case DialogId.CAPTURE_THIS_SCREEN:
			status = DialogId.CAPTURE_THIS_SCREEN;
			content = getResources().getString(R.string.cplogo_msg_this_screen);
			bt_left = getResources().getString(R.string.cplogo_bt_ok);
			bt_right = getResources().getString(R.string.cplogo_bt_cancel);
			button1.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) {
					createDialog(DialogId.SELECT_SAVE_POSITION);
				}
			});
			button2.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) {
					finish();
				}
			});
			break;
		case DialogId.ADJUST_POSITION:
			status = DialogId.ADJUST_POSITION;
			content = getResources().getString(
					R.string.cplogo_msg_adjust_position);
			bt_left = getResources().getString(R.string.cplogo_bt_no);
			bt_right = getResources().getString(R.string.cplogo_bt_adjust);
			button1.setOnClickListener(new View.OnClickListener() {

				public void onClick(View v) {
					if (mSelectArea == null) {
						mSelectArea = new CaptureSelectArea(
								CaptureLogoActivity.this);
						mSelectArea.setHandler(mHandler);
					}
					capImp.setSpecialArea(mSelectArea.getCaptureArea());
					mSelectArea.setVisibility(View.INVISIBLE);
					createDialog(DialogId.SELECT_SAVE_POSITION);
				}
			});
			button2.setOnClickListener(new View.OnClickListener() {

				public void onClick(View v) {
					captureMain.setVisibility(View.INVISIBLE);
					showSelectAreaView();
				}
			});
			break;
		case DialogId.SELECT_SAVE_POSITION:
			status = DialogId.SELECT_SAVE_POSITION;
			content = getResources().getString(
					R.string.cplogo_msg_select_position);
			setSelectSavePositionView(View.VISIBLE);
			mSavePosition.setText("0");
			bt_left = getResources().getString(R.string.cplogo_bt_ok);
			bt_right = getResources().getString(R.string.cplogo_bt_cancel);
			button1.setOnClickListener(new View.OnClickListener() {

				public void onClick(View v) {
					capImp.setSavePosition(Integer
							.valueOf(saveIds[selectSaveId]));
					setSelectSavePositionView(View.GONE);
					createDialog(DialogId.CAPTURING);
				}
			});

			button2.setOnClickListener(new View.OnClickListener() {
				public void onClick(View v) {
					finish();
				}
			});
			break;
		case DialogId.CAPTURING:
			mHandler.removeMessages(MESSAGE_DISMISS);
			mHandler.sendEmptyMessageDelayed(MESSAGE_DISMISS, 60000);
			status = DialogId.CAPTURING;
			content = getResources().getString(R.string.cplogo_msg_capturing);
			bt_right = getResources().getString(R.string.cplogo_bt_cancel);

			int type = getCaptureSourceType();
			if (type == TVStorage.CAP_LOGO_MM_IMAGE) {
				LogicManager manager = LogicManager.getInstance(this);
				capImp.setLogoCaptureListener(mCaptureListener, type, manager
						.getNativeBitmap(), manager.getWidth(), manager
						.getHeight(), manager.getPitch(), manager.getMode());
			} else {
				capImp.setLogoCaptureListener(mCaptureListener, type);
			}
			setMyProgressBarVisibility(View.VISIBLE);
			button1.setVisibility(View.INVISIBLE);
			button2.setOnClickListener(new View.OnClickListener() {

				public void onClick(View v) {
					capImp.removeLogoCaptureListener(getCaptureSourceType());
					createDialog(DialogId.CAPTURE_RESULT, CAPTURELOGO_CANCEL);
				}
			});
			break;

		default:
			break;
		}
		mContextTextView.setText(content);
		button1.setText(bt_left);
		button2.setText(bt_right);
	}

	private TVStorage.LogoCaptureListener mCaptureListener = new TVStorage.LogoCaptureListener() {

		public void onEvent(int event) {
			mHandler.removeMessages(MESSAGE_DISMISS);
			mHandler
					.sendEmptyMessageDelayed(MESSAGE_DISMISS, AUTO_DISMISS_TIME);
			createDialog(DialogId.CAPTURE_RESULT, event);
		}
	};

	private int getCaptureSourceType() {
		int type = TVStorage.CAP_LOGO_TV;

		if (mType == MMP_PHOTO) {
			type = TVStorage.CAP_LOGO_MM_IMAGE;
			// Modified by Dan 20120227 : type => mType
		} else if (mType == MMP_VIDEO) {
			type = TVStorage.CAP_LOGO_MM_VIDEO;
		}

		return type;
	}

	/**
	 * show capture result interface
	 * 
	 * @param id
	 *            the screen which to show id
	 * @param result
	 *            the capture result
	 */
	protected void createDialog(int id, int result) {
		MtkLog.d(TAG, "capturing result : " + result);
		if (DialogId.CAPTURE_RESULT == id) {
			status = DialogId.CAPTURE_RESULT;
			if (result == TVStorage.LogoCaptureListener.CAP_COMPLETE) {
				content = getResources().getString(
						R.string.cplogo_msg_res_success);
			} else if (result == TVStorage.LogoCaptureListener.CAP_FAIL) {
				content = getResources()
						.getString(R.string.cplogo_msg_res_fail);
			} else {
				content = getResources().getString(
						R.string.cplogo_capture_res_cancel);
			}

			capImp.finishLogoCaputer(getCaptureSourceType());
			button1.setVisibility(View.INVISIBLE);
//			button2.setVisibility(View.INVISIBLE);
			setMyProgressBarVisibility(View.INVISIBLE);
			mContextTextView.setText(content);
			button2.setVisibility(View.VISIBLE);
			MtkLog.d(TAG, "button2.setVisibility(View.VISIBLE);");
			button2.setText(getResources()
					.getString(R.string.nav_exit));
			button2.setOnClickListener(new OnClickListener(){

				@Override
				public void onClick(View view) {
					// TODO Auto-generated method stub
					finish();
				}
				
			});

		}
	}

	public void showSelectAreaView() {
		if (mSelectArea == null) {
			MtkLog.d(TAG, "showSelectAreaView------>mSelectArea == null");
			mSelectArea = new CaptureSelectArea(this);
			mSelectArea.setFocusable(true);
			mSelectArea.setHandler(mHandler);
			addContentView(mSelectArea, new LayoutParams(
					LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
			mSelectArea.requestFocus();
			mHandler.removeMessages(MESSAGE_DISMISS);
			mHandler.sendEmptyMessageDelayed(
					CaptureLogoActivity.MESSAGE_DISMISS,
					CaptureLogoActivity.AUTO_DISMISS_TIME);
		} else {
			MtkLog.d(TAG, "showSelectAreaView------>mSelectArea =!null");
			mSelectArea.setVisibility(View.VISIBLE);
			mSelectArea.requestFocus();
			mHandler.removeMessages(MESSAGE_DISMISS);
			mHandler.sendEmptyMessageDelayed(
					CaptureLogoActivity.MESSAGE_DISMISS,
					CaptureLogoActivity.AUTO_DISMISS_TIME);
		}

		MtkLog.d(TAG, "select area has focus: " + mSelectArea.hasFocus());
	}

	public void initData() {

		Bundle bundle = getIntent().getExtras();
		if (null != bundle) {
			mType = bundle.getInt(FROM_MMP, -1);
			if (mType == MMP_PHOTO || mType == MMP_VIDEO) {
				createDialog(DialogId.CAPTURE_THIS_SCREEN);
				mRootView.setAlpha(0.7f);
				if (mType == MMP_PHOTO) {
					mRootView
							.setBackgroundResource(R.drawable.translucent_background);
				}
				return;
			}
		}
		if (!NavIntegration.getInstance(this).isCurrentSourceTv()) {
			createDialog(DialogId.CAPTURE_THIS_SCREEN);
			return;
		}
		status = DialogId.WHICH_AREA;

		if (mContextTextView != null) {
			mContextTextView.setText(getResources().getString(
					R.string.cplogo_msg_which_screen));
		}

		button1.setText(getResources()
				.getString(R.string.cplogo_bt_full_screen));
		button1.setOnClickListener(new View.OnClickListener() {

			public void onClick(View v) {
				createDialog(DialogId.CAPTURE_THIS_SCREEN);
				capImp.setSpecialArea(null);
			}
		});
		button2.setText(getResources().getString(
				R.string.cplogo_bt_special_area));
		button2.setOnClickListener(new View.OnClickListener() {

			public void onClick(View v) {
				captureMain.setVisibility(View.INVISIBLE);
				showSelectAreaView();
			}
		});
	}

	public class DialogId {
		public static final int WHICH_AREA = 1;
		public static final int CAPTURE_THIS_SCREEN = 2;
		public static final int SELECT_SAVE_POSITION = 3;
		public static final int CAPTURING = 4;
		public static final int CAPTURE_RESULT = 5;
		public static final int ADJUST_POSITION = 6;
	}
}