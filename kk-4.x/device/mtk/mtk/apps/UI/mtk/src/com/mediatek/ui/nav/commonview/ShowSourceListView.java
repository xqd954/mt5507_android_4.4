package com.mediatek.ui.nav.commonview;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import android.R.integer;
import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnDismissListener;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;
import com.mediatek.ui.pippop.PippopUiLogic;
import com.mediatek.timeshift_pvr.controller.StateTimeShift;
import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.timeshift_pvr.ui.OnPVRDialogListener;
import com.mediatek.timeshift_pvr.ui.PvrDialog;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVInputManager;
import com.mediatek.tvcommon.TVCommonNative;
import com.mediatek.tvcommon.ITVCommon;
import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.commonview.CustListView;
import com.mediatek.ui.nav.adapter.SourceListAdapter;
import com.mediatek.ui.nav.util.CheckLockSignalChannelState;
import com.mediatek.ui.nav.util.NavChannel;
import com.mediatek.ui.util.AnimationManager;
import com.mediatek.ui.util.BypassWindowManager;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

public class ShowSourceListView extends Dialog implements OnDismissListener {

	private static Context mContext;
	private static BypassWindowManager mBypassWindowManager;
	private CustListView mSourceListView;
	private List<String> mSourceList = new ArrayList<String>();
	private List<String> mConflictList;
	private PippopUiLogic mPippopUiLogic; 
	private SourceListAdapter mSourceListAdapter;
	private static NavChannel mNavChannel;
	private static final String TAG = "SourceListView";
	private static final int MSG_DISMISS = 1;
	private static final int MSG_DISMISS_DELAY = 5000;
	private static int position = 0;
	// private Handler mHandler;
	private Drawable mSourceSelectedIcon, mSourceUnSelectedIcon, mConflictIcon;
	private static TVInputManager mTVInputManager;
	private BannerView mBannerView;
	private TextView mTitleTextView;
	private String mSourceTitle ;

	public ShowSourceListView(Context context, int theme) {
		super(context, theme);
	}

	public ShowSourceListView(Context context) {
		this(context, R.style.dialog);
		mContext = context;
		this.mBypassWindowManager = BypassWindowManager.getInstance(mContext);
		// mHandler = new Handler(context.getMainLooper());
	}

	/*
	 * public Handler getHandler() { return mHandler; }
	 * 
	 * public void setHandler(Handler mHandler) { this.mHandler = mHandler; }
	 */

	private Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			switch (msg.what) {
			case MSG_DISMISS: {
				exit();
				//if (!(mBannerView.getTotalVisibility() == View.VISIBLE)) {
					if (!(mPippopUiLogic.isPipOrPopState())) {
					CheckLockSignalChannelState.getInstance(mContext)
							.checkLockedSignStateOrHasChannel(false);
					}
					break;
				//}
			}
			default:
				break;
			}
		}
	};
	
	public void setBannerView(BannerView mBannerView){
		this.mBannerView=mBannerView;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		MtkLog.d(TAG, "oncreate");
		super.onCreate(savedInstanceState);
		setContentView(R.layout.nav_source_list);
		setWindowPosition();
		init();
		initData();
//		mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);
//		mSourceListView.setFocusable(true);
//		mSourceListView.requestFocus();
//		mSourceListView.setSelection(position);
	}

	protected void onStart() {
		MtkLog.d(TAG, "onStart");
		mTitleTextView = (TextView) findViewById(R.id.nav_tv_source_title);
		if (("sub").equals(mPippopUiLogic.getOutputFocus())) {
			mSourceTitle = mContext.getResources().getString(
					R.string.nav_source_sub_title);
		} else {
			mSourceTitle = mContext.getResources().getString(
					R.string.nav_source_main_title);
		}
		mTitleTextView.setText(mSourceTitle);
		initData();
		int selectedItemPosition = mSourceList.indexOf(mNavChannel
				.iGetCurrentSouce());
		if (selectedItemPosition < 0) {
			selectedItemPosition = 0;
		}
		mSourceListView.setFocusable(true);
		mSourceListView.requestFocus();
		mSourceListView.setSelection(selectedItemPosition);
		mHandler.removeMessages(MSG_DISMISS);
		mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);
		
		if(AnimationManager.getInstance().getIsAnimation()){
			AnimationManager.getInstance().channelListEnterAnimation(findViewById(R.id.nav_ll_sourceList_model));
		}
	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (mContext == null) {
			return true;
		}
		TurnkeyUiMainActivity.reshowFoucs();
		mHandler.removeMessages(MSG_DISMISS);
		mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);
		MtkLog.d(TAG,
				"ShowSourceListView============onKeyDown====================");

		switch (event.getKeyCode()) {
		case KeyMap.KEYCODE_MENU:
	    case KeyMap.KEYCODE_MTKIR_ANGLE:
		case KeyMap.KEYCODE_MTKIR_GUIDE:
			exit();
			mBypassWindowManager.setBypassWindow(false,TurnkeyUiMainActivity.mShowSourceListWindowId,new Rect(0,0,0,0));
			break;
		case KeyMap.KEYCODE_BACK:
			exit();
			mBypassWindowManager.setBypassWindow(false,TurnkeyUiMainActivity.mShowSourceListWindowId,new Rect(0,0,0,0));
			if(mBannerView.getTotalVisibility()==View.VISIBLE){				
				return true;
			}else{
				break;
			}			
		case KeyMap.KEYCODE_MTKIR_SOURCE:
			

			if (StateTimeShift.getInstance() != null
					&& StateTimeShift.getInstance().isRunning()) {
				PvrDialog conDialog = new PvrDialog(TimeShiftManager
						.getInstance().getActivity(),
						PvrDialog.TYPE_Confirm,
						PvrDialog.TYPE_Change_Source_By_Src_Key, this,
						PvrDialog.TYPE_Timeshift);
				conDialog.setOnPVRDialogListener(new OnPVRDialogListener(){

					@Override
					public void onPVRDialogListener(int keyCode) {
						// TODO Auto-generated method stub
						handleSourceKey();
					}
				});
				conDialog.show();
				return true;
			} else {
				/*// Fix CR:DTV00585773
				if (TimeShiftManager.getInstance().pvrIsRecording()) {
					PvrDialog conDialog = new PvrDialog(TimeShiftManager
							.getInstance().getActivity(),
							PvrDialog.TYPE_Confirm,
							PvrDialog.TYPE_Change_Source_By_Src_Key, this,
							PvrDialog.TYPE_Record);
					conDialog.setOnPVRDialogListener(new OnPVRDialogListener(){

						@Override
						public void onPVRDialogListener(int keyCode) {
							// TODO Auto-generated method stub
							handleSourceKey();
						}
					});
					conDialog.show();
					return true;
				}*/
			}
			handleSourceKey();
			return true;
		case KeyMap.KEYCODE_DPAD_DOWN:
		case KeyMap.KEYCODE_DPAD_UP:
		case KeyMap.KEYCODE_VOLUME_DOWN:
		case KeyMap.KEYCODE_VOLUME_UP:
		case KeyMap.KEYCODE_0:
		case KeyMap.KEYCODE_1:
		case KeyMap.KEYCODE_2:
		case KeyMap.KEYCODE_3:
		case KeyMap.KEYCODE_4:
		case KeyMap.KEYCODE_5:
		case KeyMap.KEYCODE_6:
		case KeyMap.KEYCODE_7:
		case KeyMap.KEYCODE_8:
		case KeyMap.KEYCODE_9:
		case KeyMap.KEYCODE_MTKIR_INFO:
		case KeyMap.KEYCODE_MTKIR_MUTE:
			return true;
		default:
			break;
		}
		if (null != mContext && mContext instanceof TurnkeyUiMainActivity) {
			MtkLog.d(TAG,
					"ShowSourceListView=====TurnkeyUiMainActivity=========");
			((TurnkeyUiMainActivity) mContext).onKeyDown(keyCode, event);
		}
		return super.onKeyDown(keyCode, event);

	}

	private void handleSourceKey() {
		//			int selectedItemPosition = mSourceList.indexOf(mNavChannel
					int selectedItemPosition;
					if(("sub").equals(mPippopUiLogic.getOutputFocus())){
						selectedItemPosition = mSourceList.indexOf(mPippopUiLogic.getSubOutputSourceName());
					}else{
						selectedItemPosition = mSourceList.indexOf(mPippopUiLogic.getMainOutputSourceName());
					}
					if (selectedItemPosition < 0) {
						selectedItemPosition = 0;
					}
					/*[KK]
					if(null != mSourceListView.getChildAt(selectedItemPosition)) {
					mSourceListView.getChildAt(selectedItemPosition).setFocusable(true);
					if (!mSourceListView.getChildAt(selectedItemPosition).isFocused()) {
						mSourceListView.getChildAt(selectedItemPosition).requestFocus();
						}
					}
					*/
					TextView tv = (TextView) mSourceListView
							.getChildAt(selectedItemPosition);
					if (tv != null) {
						tv
								.setCompoundDrawables(mSourceUnSelectedIcon, null,
										null, null);
					}
					MtkLog.d(TAG,
							"ShowSourceListView========selectedItemPosition============"
									+ selectedItemPosition);
		
					// mTVInputManager.changeNextInputSource();
					try {
						ITVCommon gDefault = null;
						gDefault = TVCommonNative.getDefault(null);
						if(null != gDefault){
					  gDefault.changeNextInputSource();
					}
					} catch (RemoteException e) {
						e.printStackTrace();
					}
					if(("sub").equals(mPippopUiLogic.getOutputFocus())){
						selectedItemPosition = mSourceList.indexOf(mPippopUiLogic.getSubOutputSourceName());
					}else{
						selectedItemPosition = mSourceList.indexOf(mPippopUiLogic.getMainOutputSourceName());
					}
					if (selectedItemPosition < 0) {
						selectedItemPosition = 0;
					}
					mSourceListView.setSelection(selectedItemPosition);
		
					TextView select = (TextView) mSourceListView
							.getChildAt(selectedItemPosition);
					if (select != null) {
						select.setCompoundDrawables(mSourceSelectedIcon, null, null,
								null);
					}
	};

	private void init() {
		mTVInputManager = TVContent.getInstance(mContext).getInputManager();
		mSourceListView = (CustListView) findViewById(R.id.nav_source_listview);
		mSourceSelectedIcon = mContext.getResources().getDrawable(
				R.drawable.nav_source_list_select_icon);
		mSourceSelectedIcon.setBounds(0, 0, mSourceSelectedIcon
				.getMinimumWidth(), mSourceSelectedIcon.getMinimumWidth());
		mSourceUnSelectedIcon = mContext.getResources().getDrawable(
				R.drawable.translucent_background);
		mSourceUnSelectedIcon.setBounds(0, 0, mSourceSelectedIcon
				.getMinimumWidth(), mSourceSelectedIcon.getMinimumWidth());
		mConflictIcon = mContext.getResources().getDrawable(
				R.drawable.nav_source_pip_disable_icon);
		mConflictIcon.setBounds(0, 0, mSourceSelectedIcon.getMinimumWidth(),
				mSourceSelectedIcon.getMinimumWidth());
		mSourceListView.setOnKeyListener(new SourceListOnKey());
		mNavChannel = NavChannel.getInstance(mContext);
		mPippopUiLogic = PippopUiLogic.getInstance(mContext);
	}

	private void initData() {
		// Source List
		MtkLog.d(TAG, "initData");
		MtkLog.d(TAG, "mTVInputManager.getInputSourceArray().length========="
				+ mTVInputManager.getInputSourceArray().length);
		mSourceList = Arrays.asList(mTVInputManager.getInputSourceArray());
		mConflictList = mPippopUiLogic.getConflictInputs();
		if (mSourceList != null) {
			MtkLog.d(TAG, "mSourceList != null");
			MtkLog.d(TAG, "mSourceList.size()===" + mSourceList.size());
			mSourceListView.initData(mSourceList, mSourceList.size(),
					mSoureListUpDate);
			setSourceListData();
		}
	}

	public void onDismiss(DialogInterface dialog) {
		// mView = null;
	}

	class SourceListOnKey implements View.OnKeyListener {

		public boolean onKey(View v, int keyCode, KeyEvent event) {
			TurnkeyUiMainActivity.reshowFoucs();
			int selectedPosition = mSourceListView.getSelectedItemPosition();
			if (selectedPosition < 0) {
				selectedPosition = 0;
			}
			/* [KK]
			if(null != mSourceListView.getChildAt(selectedPosition)) {
			mSourceListView.getChildAt(selectedPosition).setFocusable(true);
			if (!mSourceListView.getChildAt(selectedPosition).isFocused()) {
				mSourceListView.getChildAt(selectedPosition).requestFocus();
				}
			}
			*/
			if (event.getAction() == KeyEvent.ACTION_DOWN) {
				mHandler.removeMessages(MSG_DISMISS);
				mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);
				switch (keyCode) {
				case KeyEvent.KEYCODE_DPAD_CENTER:
					int selectedSourcePosition = mSourceListView
							.getSelectedItemPosition();
					if (selectedSourcePosition < 0) {
						selectedSourcePosition = 0;
					}
					if (StateTimeShift.getInstance() != null
							&& StateTimeShift.getInstance().isRunning()) {
						PvrDialog conDialog = new PvrDialog(TimeShiftManager
								.getInstance().getActivity(),
								PvrDialog.TYPE_Confirm,
								PvrDialog.TYPE_Change_Source, ShowSourceListView.this,
								PvrDialog.TYPE_Timeshift);
						conDialog.show();
					} else {
						if (!mConflictList.contains(mSourceList
								.get(selectedSourcePosition))) {
							//ComponentStatusListener.getInstance().updateStatus(
							//		ComponentStatusListener.NAV_KEY_OCCUR, keyCode);
						} else {
							if (TimeShiftManager.getInstance().pvrIsRecording()) {
								PvrDialog conDialog = new PvrDialog(
										TimeShiftManager.getInstance()
												.getActivity(),
										PvrDialog.TYPE_Confirm,
										PvrDialog.TYPE_Change_Source, ShowSourceListView.this,
										PvrDialog.TYPE_Record);
								conDialog.show();
								break;
							}
						}
						handleCenterKey(selectedPosition);
	                }
					break;
				case KeyEvent.KEYCODE_DPAD_DOWN:
				case KeyEvent.KEYCODE_DPAD_UP:
					int selectedItemPosition1 = mSourceList.indexOf(mNavChannel
							.iGetCurrentSouce());
					if (selectedItemPosition1 < 0) {
						selectedItemPosition1 = 0;
					}
					if(null != mSourceListView.getChildAt(selectedItemPosition1)) {
					mSourceListView.getChildAt(selectedItemPosition1)
							.requestFocusFromTouch();
					}
					return false;
				default:
					return false;
				}
			}
			return false;
		}

		private void handleCenterKey(int selectedPosition) {
			if (("sub").equals(mPippopUiLogic.getOutputFocus())) {
				if (selectedPosition != mSourceList
						.indexOf(mPippopUiLogic
								.getSubOutputSourceName())
						&& !(mConflictList.contains(mSourceList
								.get(selectedPosition)))) {
				mBannerView.hideAllBanner();
				if (selectedPosition < 0) {
					selectedPosition = 0;
				}
					TextView tv = (TextView) mSourceListView
							.getChildAt(mSourceList
									.indexOf(mPippopUiLogic
											.getSubOutputSourceName()));
				if (tv != null) {
					tv.setCompoundDrawables(mSourceUnSelectedIcon,
							null, null, null);
				}
					mTVInputManager.changeInputSource("sub",
							mSourceList.get(selectedPosition));
					selectedPosition = mSourceList
							.indexOf(mPippopUiLogic
									.getSubOutputSourceName());
				TextView select = (TextView) mSourceListView
						.getChildAt(selectedPosition);
				if (select != null) {
						select.setCompoundDrawables(
								mSourceSelectedIcon, null, null, null);
					}
				}
			} else {
				if (selectedPosition != mSourceList
						.indexOf(mPippopUiLogic
								.getMainOutputSourceName())
						&& !(mConflictList.contains(mSourceList
								.get(selectedPosition)))) {
					mBannerView.hideAllBanner();
					if (selectedPosition < 0) {
						selectedPosition = 0;
					}
					TextView tv = (TextView) mSourceListView
							.getChildAt(mSourceList
									.indexOf(mPippopUiLogic
											.getMainOutputSourceName())); 
					if (tv != null) {
						tv.setCompoundDrawables(mSourceUnSelectedIcon,
								null, null, null);
					}
					mTVInputManager.changeInputSource("main",
							mSourceList.get(selectedPosition));
					selectedPosition = mSourceList
							.indexOf(mPippopUiLogic
									.getMainOutputSourceName());
					TextView select = (TextView) mSourceListView
							.getChildAt(selectedPosition);
					if (select != null) {
						select.setCompoundDrawables(
								mSourceSelectedIcon, null, null, null);
					}
				}
			}
		}
	}

	private CustListView.UpDateListView mSoureListUpDate = new CustListView.UpDateListView() {
		public void updata() {
			setSourceListData();
		}
	};

	private void setSourceListData() {
		mSourceList = (List<String>) mSourceListView.getCurrentList();
		mConflictList = mPippopUiLogic.getConflictInputs();  
		mSourceListAdapter = new SourceListAdapter(mContext, mSourceList,
				mConflictList, mSourceSelectedIcon, mSourceUnSelectedIcon,
				mConflictIcon);
		mSourceListView.setAdapter(mSourceListAdapter);
	}

	public static int marginX;
	public static int marginY;
	public static int menuWidth = 343;
	public static int menuHeight = 400;
	public int x = 0;
	public int y = 0;

	private void setWindowPosition() {
		WindowManager m = getWindow().getWindowManager();
		Display display = m.getDefaultDisplay();
		Window window = getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();
		marginX = (int)(display.getWidth()*0.52);
		marginY = (int)(display.getHeight()*0.0139);
		menuWidth = (int)(display.getWidth()*0.27);
		menuHeight = (int)(display.getHeight()*0.56);
		lp.width = menuWidth;
		lp.height = menuHeight;
		
		x = display.getWidth() / 2 - menuWidth / 2 - marginX;
		y = display.getHeight() / 2 - marginY - menuHeight / 2;
		lp.x = x;
		lp.y = y;
		window.setAttributes(lp);
	}
	
	public void exit(){
		if(AnimationManager.getInstance().getIsAnimation()){
		AnimationManager.getInstance().channelListExitAnimation(findViewById(R.id.nav_ll_sourceList_model) , new AnimatorListenerAdapter() {
			@Override
			public void onAnimationEnd(Animator animation) {
				super.onAnimationEnd(animation);
				dismiss();
			}
		});
		}else{
			dismiss();
		}
	}

	@Override
	public void dismiss() {
		mHandler.removeMessages(MSG_DISMISS);
                mBypassWindowManager.setBypassWindow(false,TurnkeyUiMainActivity.mShowSourceListWindowId,new Rect(0,0,0,0));
		super.dismiss();
	}

}
