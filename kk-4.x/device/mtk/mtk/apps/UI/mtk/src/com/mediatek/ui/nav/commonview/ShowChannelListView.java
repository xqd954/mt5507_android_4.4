package com.mediatek.ui.nav.commonview;

import java.util.List;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnDismissListener;
import android.graphics.Rect;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;

import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.timeshift_pvr.ui.OnPVRDialogListener;
import com.mediatek.timeshift_pvr.ui.PvrDialog;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.commonview.CustListView;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.nav.adapter.ChannelAdapter;
import com.mediatek.ui.nav.util.CheckLockSignalChannelState;
import com.mediatek.ui.nav.util.ListData;
import com.mediatek.ui.nav.util.NavChannel;
import com.mediatek.ui.nav.util.NavChannel.IChannelListChangedListener;
import com.mediatek.ui.util.AnimationManager;
import com.mediatek.ui.util.BypassWindowManager;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

public class ShowChannelListView extends Dialog implements OnDismissListener {

	private Context mContext;
	private static BypassWindowManager mBypassWindowManager;
	private View mChannelListLayout;
	private TextView mChannelListTextView;
	private CustListView mChannelListView;
	private View mChannelListPageUpDownLayout;
	private View mChannelDetailLayout;
	private ShowFavoriteChannelListView mShowFavoriteChannelListView;
	private TextView mTitleTextView;
	private TextView mNavPageUpTextView;
	private TextView mNavPageDownTextView;
	private TextView mNavChannelSelectTextView;
	private TextView mNavExitTextView;
	private TextView mNavChannelDetailsTitleTextView;
	private TextView mNavChannelDetailsChannelInfoTextView;
	private String mChannelListTitle;
	private String mNavPageUpString;
	private String mNavPageDownString;
	private String mNavChannelSelectString;
	private String mNavExitString;
	private String mNavChannelDetailsTitleString;
	private String   mNavChannelDetailsChannelInfoString;
	// private View mChannelListLayout;
	// private ShowChannelTypeView mShowChannelTypeView;
	// private Handler mHandler;

	// private List<String> mChTypeArrayList;

	private static final int MSG_DISMISS = 1;
	private static final int MSG_DISMISS_DELAY = 10000;
	private static final String TAG = "ShowChannelListView";
	static final int CHANNEL_LIST_PAGE_MAX = 7;
	private static int CURRENT_CHANNEL_TYPE = 0;
	private static int position = 0;

	private List<TVChannel> list;
	private ChannelAdapter mChannelAdapter;
	private ListData<TVChannel> mChannelDataList;
	// private static NavIntegration mNavIntegration;
	private static NavChannel mNavIntegration;

	public ShowChannelListView(Context context, int theme) {
		super(context, theme);
	}

	public ShowChannelListView(Context context) {
		this(context, R.style.nav_dialog);
		MtkLog.d(TAG, "Constructor!");
		mContext = context;
		mBypassWindowManager = BypassWindowManager.getInstance(mContext);
	}

	public void setChannelType(int channelType) {
		CURRENT_CHANNEL_TYPE = channelType;
	}

	// public Handler getHandler() {
	// return mHandler;
	// }
	//
	// public void setHandler(Handler mHandler) {
	// this.mHandler = mHandler;
	// }

	private Handler mHandler = new Handler() {

		public void handleMessage(Message msg) {
			super.handleMessage(msg);

			switch (msg.what) {
			case MSG_DISMISS: {
				exit();
				  CheckLockSignalChannelState.getInstance(mContext)
						.checkLockedSignStateOrHasChannel(false);
				
				break;
			}
			default:
				break;
			}

		}

	};

	public boolean onKeyDown(int keyCode, KeyEvent event) {

		if (mContext == null) {
			return true;
		}
		TurnkeyUiMainActivity.reshowFoucs();
		mHandler.removeMessages(MSG_DISMISS);
		mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);
		switch (event.getKeyCode()) {
		case KeyMap.KEYCODE_MENU:
//		case KeyMap.KEYCODE_MTKIR_SOURCE:
		case KeyMap.KEYCODE_BACK:
		case KeyMap.KEYCODE_VOLUME_UP:
		case KeyMap.KEYCODE_VOLUME_DOWN:
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
			mHandler.removeMessages(MSG_DISMISS);
			exit();
			break;
		
		case KeyMap.KEYCODE_MTKIR_PLAYPAUSE:
			mHandler.removeMessages(MSG_DISMISS);
			exit();
			break;
		case KeyMap.KEYCODE_MTKIR_YELLOW:
			// switch to channel_type
			// MtkLog.d(TAG, "KEYCODE_MTKIR_YELLOW");
			// mShowChannelTypeView = new ShowChannelTypeView(mContext);
			// mShowChannelTypeView.show();
			// mHandler.removeMessages(MSG_DISMISS);
			// dismiss();
			TVChannel selectedChannel = (TVChannel) mChannelListView
					.getSelectedItem();
			mNavIntegration.iSelectChannel(selectedChannel);
			mHandler.removeMessages(MSG_DISMISS);
			exit();
			return true;
		case KeyMap.KEYCODE_MTKIR_RED:
			MtkLog.d(TAG, "KEYCODE_MTKIR_RED");
			if (mChannelListView.getSelectedItemPosition() != 0) {
				mChannelListView.setSelection(0);
			} else {
				mChannelAdapter.updateData(mChannelDataList.getPrePageData());
				mChannelListView.setSelection(0);
			}
			return true;
		case KeyMap.KEYCODE_MTKIR_GREEN:
			MtkLog.d(TAG, "KEYCODE_MTKIR_GREEN");
			if (mChannelListView.getSelectedItemPosition() != ListData.COUNT_EACH_PAGE - 1) {
				mChannelListView.setSelection(ListData.COUNT_EACH_PAGE - 1);
			} else {
				mChannelAdapter.updateData(mChannelDataList.getNextPageData());
				mChannelListView.setSelection(ListData.COUNT_EACH_PAGE - 1);
			}

			return true;
		case KeyMap.KEYCODE_MTKIR_CHUP:			
		case KeyMap.KEYCODE_MTKIR_PRECH:
		case KeyMap.KEYCODE_MTKIR_CHDN:
			willChangeChannel(keyCode);
			return true;
//		case KeyMap.KEYCODE_MTKIR_EJECT:
//			mNavIntegration.isSetChannelFavorite(mNavIntegration
//					.iGetCurrentChannel());
//			return true;
		case KeyMap.KEYCODE_MTKIR_STOP:
		case KeyMap.KEYCODE_MTKIR_BLUE:
			mHandler.removeMessages(MSG_DISMISS);
			exit();
			return true;
			
		default:
			break;
		}

		if (null != mContext && mContext instanceof TurnkeyUiMainActivity) {
			MtkLog.d(TAG, "TurnkeyUiMainActivity");
			((TurnkeyUiMainActivity) mContext).onKeyDown(keyCode, event);
			if (keyCode == KeyMap.KEYCODE_MTKIR_MUTE) {
				return true;
			}
		}	

		if (keyCode == KeyMap.KEYCODE_VOLUME_UP
				|| keyCode == KeyMap.KEYCODE_VOLUME_DOWN) {
			return true;
		}

		return super.onKeyDown(keyCode, event);

	};

	/**
	 * implements data update interface
	 */
	private CustListView.UpDateListView mChannelListUpDate = new CustListView.UpDateListView() {

		public void updata() {
			MtkLog.d(TAG, "mChannelListUpDate");
			// gain data
			if (mChannelListView.getSelectedItemPosition() == 0) {
				// update adapter list data by get pro page datas
				mChannelAdapter.updateData(mChannelDataList.getPrePageData());
			} else {
				// update adapter list data by get next page datas
				mChannelAdapter.updateData(mChannelDataList.getNextPageData());
			}
			mChannelListView.setAdapter(mChannelAdapter);
		}
	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		MtkLog.d(TAG, "onCreate");
		// TODO
		setContentView(R.layout.nav_channellist);
		setWindowPosition();
		findViews();
		init();

		mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);

		mChannelListView.setFocusable(true);
		mChannelListView.requestFocus();
		mChannelListView.setSelection(position);
		updateChList(CURRENT_CHANNEL_TYPE);
		showPageUpDownView();
		mChannelDetailLayout.setVisibility(View.INVISIBLE);
		register();
	}

	protected void onStart() {
		MtkLog.d(TAG, "onStart");
		mTitleTextView = (TextView) findViewById(R.id.nav_channel_list_title);
		mChannelListTitle = mContext.getResources().getString(
				R.string.nav_channel_list);
		mTitleTextView.setText(mChannelListTitle);
	
		mNavPageUpTextView = (TextView) findViewById(R.id.channel_nav_page_up);
		mNavPageUpString = mContext.getResources().getString(R.string.nav_page_up);
		mNavPageUpTextView.setText(mNavPageUpString);
		mNavPageDownTextView = (TextView) findViewById(R.id.channel_nav_page_down);;
		mNavPageDownString = mContext.getResources().getString(R.string.nav_page_down);
		mNavPageDownTextView.setText(mNavPageDownString);
		mNavChannelSelectTextView =(TextView) findViewById(R.id.channel_nav_select_list);
		mNavChannelSelectString = mContext.getResources().getString(R.string.nav_favorite_select);
		mNavChannelSelectTextView.setText(mNavChannelSelectString);
		mNavExitTextView = (TextView)findViewById(R.id.channel_nav_exit);
		mNavExitString = mContext.getResources().getString(R.string.nav_exit);
		mNavExitTextView.setText(mNavExitString);
		mNavChannelDetailsTitleTextView = (TextView) findViewById(R.id.nav_channel_details_title);
		mNavChannelDetailsTitleString = mContext.getResources().getString(R.string.nav_channel_details);
		mNavChannelDetailsTitleTextView.setText(mNavChannelDetailsTitleString);
		mNavChannelDetailsChannelInfoTextView = (TextView) findViewById(R.id.nav_channel_details_channel_info);
		mNavChannelDetailsChannelInfoString = mContext.getResources().getString(R.string.nav_channel_infoNoProgram);
		mNavChannelDetailsChannelInfoTextView.setText(mNavChannelDetailsChannelInfoString);
		 updateChList(CURRENT_CHANNEL_TYPE);
		// setWindowPosition();
		showPageUpDownView();
		mChannelDetailLayout.setVisibility(View.INVISIBLE);
		mHandler.removeMessages(MSG_DISMISS);
		mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);
		// register();
		
		if(AnimationManager.getInstance().getIsAnimation()){
			AnimationManager.getInstance().channelListEnterAnimation(mChannelListLayout);
		}

	}

	private void init() {
		mNavIntegration = NavChannel.getInstance(mContext);
		mShowFavoriteChannelListView = new ShowFavoriteChannelListView(mContext);
		List<TVChannel> channelList = produceList();
		mChannelDataList = new ListData<TVChannel>(channelList, mNavIntegration
				.iGetCurrentChannel());
		mChannelAdapter = new ChannelAdapter(mContext, channelList);
		mChannelAdapter.updateData(mChannelDataList.getShowData(mNavIntegration
				.iGetCurrentChannel()));
		mChannelListView.setAdapter(mChannelAdapter);
	}

	private List<TVChannel> produceList() {
		list = mNavIntegration.iGetChannelList(CURRENT_CHANNEL_TYPE);
		return list;
	}

	private void findViews() {
		mChannelListLayout = findViewById(R.id.nav_channellist);
		mChannelListTextView = (TextView) findViewById(R.id.nav_channel_list_title);
		mChannelListView = (CustListView) findViewById(R.id.nav_channel_listview);
		mChannelListPageUpDownLayout = findViewById(R.id.nav_page_up_down);
		mChannelDetailLayout = findViewById(R.id.nav_channel_details_layout);
	}

	private void register() {
		mChannelListView.setOnKeyListener(new ChannelListOnKey());
		mNavIntegration.iRegisteClcListener(mChListChageLsner);
	}

	private void showPageUpDownView() {
		if (mChannelDataList.getCount() <= 0) {
			mHandler.removeMessages(MSG_DISMISS);
			mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);
		} else if (mChannelDataList.getCount() > CHANNEL_LIST_PAGE_MAX) {
			MtkLog.d(TAG, "mChannelDataList.getCount() = "
					+ mChannelDataList.getCount());
			mChannelListPageUpDownLayout.setVisibility(View.VISIBLE);
		} else {
			mChannelListPageUpDownLayout.setVisibility(View.INVISIBLE);
		}

	}

	public static int marginX;
	public static int marginY;
	public static int menuWidth = 494 + 329;
	public static int menuHeight = 420;
	public int x = 0;
	public int y = 0;

	public void setWindowPosition() {
		WindowManager m = getWindow().getWindowManager();
		Display display = m.getDefaultDisplay();
		Window window = getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();
		marginY = (int) (display.getHeight() * 0.0139);
		marginX = (int) (display.getWidth() * 0.27);
		menuWidth = (int) (display.getWidth() * 0.52);
		menuHeight = (int) (display.getHeight() * 0.56);
		lp.width = menuWidth;
		lp.height = menuHeight;
		// lp.x = marginX;
		// lp.y = display.getHeight() - marginY - menuHeight;
		x = display.getWidth() / 2 - menuWidth / 2 - marginX;
		y = (int) (display.getHeight() / 2) - marginY - menuHeight / 2;
		lp.x = x;
		lp.y = y;

		window.setAttributes(lp);
	}

	public void onDismiss(DialogInterface dialog) {
		// TODO Auto-generated method stub
		// mView = null;
		MtkLog.d(TAG, "onDismiss!!!!!!!!!");
		mContext = null;

		
	}
	
	public void exit(){
		if(AnimationManager.getInstance().getIsAnimation()){
		AnimationManager.getInstance().channelListExitAnimation(mChannelListLayout , new AnimatorListenerAdapter() {
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
		MtkLog.d(TAG, "dismiss!!!!!!!");
		mHandler.removeMessages(MSG_DISMISS);
		mBypassWindowManager.setBypassWindow(false,
				TurnkeyUiMainActivity.mShowChannelListWindowId,
				new Rect(0, 0, 0, 0));
		super.dismiss();
	}

	class ChannelListOnKey implements View.OnKeyListener {
		public boolean onKey(View v, int keyCode, KeyEvent event) {
			TurnkeyUiMainActivity.reshowFoucs();
			// TODO Auto-generated method stub
			int slectPosition = mChannelListView.getSelectedItemPosition();
			if (slectPosition < 0) {
				slectPosition = 0;
			}
			/* [KK]
			mChannelListView.getChildAt(slectPosition).setFocusable(true);
			*/
			if (event.getAction() == KeyEvent.ACTION_DOWN) {
				switch (keyCode) {
				case KeyEvent.KEYCODE_DPAD_CENTER:

					mHandler.removeMessages(MSG_DISMISS);
					mHandler.sendEmptyMessageDelayed(MSG_DISMISS,
							MSG_DISMISS_DELAY);

					MtkLog.v(TAG,
							"mChannelItemKeyLsner*********** slectPosition = "
									+ mChannelListView
											.getSelectedItemPosition());

					willChangeChannel(KeyEvent.KEYCODE_DPAD_CENTER);
					return true;
				case KeyEvent.KEYCODE_DPAD_DOWN:
					MtkLog.d(TAG, "KEYCODE_DPAD_DOWN!!!!!");
					if (null != mChannelListView.getChildAt(slectPosition)) {
						mChannelListView.getChildAt(slectPosition)
								.requestFocusFromTouch();
					}
					mChannelDetailLayout.setVisibility(View.INVISIBLE);
					mHandler.removeMessages(MSG_DISMISS);
					mHandler.sendEmptyMessageDelayed(MSG_DISMISS,
							MSG_DISMISS_DELAY);
					return false;
				case KeyEvent.KEYCODE_DPAD_UP:
					MtkLog.d(TAG, "KEYCODE_DPAD_UP!!!!!");
					if (null != mChannelListView.getChildAt(slectPosition)) {
						mChannelListView.getChildAt(slectPosition)
								.requestFocusFromTouch();
					}
					mChannelDetailLayout.setVisibility(View.INVISIBLE);
					mHandler.removeMessages(MSG_DISMISS);
					mHandler.sendEmptyMessageDelayed(MSG_DISMISS,
							MSG_DISMISS_DELAY);
					return false;
				default:
					return false;
				}
			}else{
				mLastKeyDownTime = 0;
			}
			return false;
		}
	}

	private void updateChList(int curChType) {
		// mChTypeArrayList = Arrays.asList(mContext.getResources()
		// .getStringArray(R.array.nav_channel_types));
		// mChannelListTextView.setText(mContext
		// .getString(R.string.nav_channel_list_title_str)
		// + mChTypeArrayList.get(curChType));

		List<TVChannel> channelList = produceList();

//		mChannelDataList = new ListData<TVChannel>(channelList, mNavIntegration
//				.iGetCurrentChannel());
//		mChannelAdapter = new ChannelAdapter(mContext, channelList);
//		mChannelAdapter.updateData(mChannelDataList.getShowData(mNavIntegration
//				.iGetCurrentChannel(), position));
//		mChannelListView.setAdapter(mChannelAdapter);

		final int tmp1 = mChannelDataList.getIdxInShowList(mNavIntegration
				.iGetCurrentChannel());
		MtkLog.v(TAG, "tmp1 = " + tmp1);
		mChannelDataList = new ListData<TVChannel>(channelList, mNavIntegration
				.iGetCurrentChannel());
		mChannelListView.initData(mChannelDataList.getAllData(),
				CHANNEL_LIST_PAGE_MAX, mChannelListUpDate);
		mChannelAdapter.updateData(mChannelDataList.getShowData(mNavIntegration
				.iGetCurrentChannel(), tmp1));
		mChannelListView.setAdapter(mChannelAdapter);

		final int tmp = mChannelDataList.getIdxInShowList(mNavIntegration
				.iGetCurrentChannel());
		mChannelListView.setSelection(tmp);

	}

	IChannelListChangedListener mChListChageLsner = new IChannelListChangedListener() {
		public void updateChannelList() {

			MtkLog.d(TAG,
					"IChannelListChangedListener mChListChageLsner!!!!!!!!!!");

//			 mChannelDataList.updateAllData(mNavIntegration.iGetChannelList());
//			 showPageUpDownView();
//			 mChannelListView.setAdapter(mChannelAdapter);
			updateChList(CURRENT_CHANNEL_TYPE);
		}
	};

	// List<TVChannel> channelList = mNavIntegration
	// .iGetChannelList(curChType);
	//
	// final int tmp1 = mChannelData.getIdxInShowList(mNavIntegration
	// .iGetCurrentChannel());
	// MtkLog.v(TAG, "***************************tmp1=" + tmp1);
	// mChannelData = new ListData<TVChannel>(channelList, mNavIntegration
	// .iGetCurrentChannel());
	// mChannelListView.initData(mChannelData.getAllData(),
	// CHANNEL_LIST_PAGE_MAX, mChannelListUpDate);
	// mChannelAdapter.updateData(mChannelData.getShowData(mNavIntegration
	// .iGetCurrentChannel(), tmp1));
	// mChannelListView.setAdapter(mChannelAdapter);
	//
	// final int tmp = mChannelData.getIdxInShowList(mNavIntegration
	// .iGetCurrentChannel());
	//
	// mChannelListView.setSelection(tmp);

	private static long mLastKeyDownTime;
	private final long KEY_DURATION = 800;
	private boolean isValid() {
		long currentTime = System.currentTimeMillis();
		if ((currentTime - mLastKeyDownTime) >= KEY_DURATION) {
			mLastKeyDownTime = currentTime;
			return true;
		} else {
			return false;
		}
	}
	public void willChangeChannel(int keyCode) {
		// TODO Auto-generated method stub
		if (TimeShiftManager.getInstance().pvrIsRecording()) {
			PvrDialog conDialog = new PvrDialog(TimeShiftManager.getInstance()
					.getActivity(), PvrDialog.TYPE_Confirm,
					PvrDialog.KEYCODE_FROM_FAV, PvrDialog.TYPE_Record);
			conDialog.setFavChannel(keyCode);
			conDialog.setOnPVRDialogListener(new OnPVRDialogListener(){
				@Override
				public void onPVRDialogListener(int keyCode) {
					// TODO Auto-generated method stub
					changeChannel(keyCode);
				}
			});
			conDialog.show();
			return;
		}
		changeChannel(keyCode);
	}

	public void changeChannel(int keyCode) {
		// TODO Auto-generated method stub
		switch (keyCode) {
		case KeyMap.KEYCODE_MTKIR_CHUP:			
			mNavIntegration.iSetChannel(NavChannel.CHANNEL_UP);
			mChannelAdapter.updateData(mChannelDataList
					.getShowData(mNavIntegration.iGetCurrentChannel()));
			mChannelListView.setSelection(mChannelDataList
					.getIdxInShowList(mNavIntegration.iGetCurrentChannel()));
			break;
		case KeyMap.KEYCODE_MTKIR_PRECH:
			mNavIntegration.iSetChannel(NavChannel.CHANNEL_PRE);
			mChannelAdapter.updateData(mChannelDataList
					.getShowData(mNavIntegration.iGetCurrentChannel()));
			mChannelListView.setSelection(mChannelDataList
					.getIdxInShowList(mNavIntegration.iGetCurrentChannel()));
			break;
		case KeyMap.KEYCODE_MTKIR_CHDN:
			mNavIntegration.iSetChannel(NavChannel.CHANNEL_DOWN);
			mChannelAdapter.updateData(mChannelDataList
					.getShowData(mNavIntegration.iGetCurrentChannel()));
			mChannelListView.setSelection(mChannelDataList
					.getIdxInShowList(mNavIntegration.iGetCurrentChannel()));
			break;
		case KeyMap.KEYCODE_DPAD_CENTER:
			onEnterKeyPressed();
			break;
		}
	}
	
	private void onEnterKeyPressed() {
		TVChannel selectedChannel = (TVChannel) mChannelListView
				.getSelectedItem();

		if (!selectedChannel.equals(mNavIntegration
				.iGetCurrentChannel())) {
			// mHandler.removeMessages(MSG_DISMISS);
			// dismiss();
			mChannelDetailLayout.setVisibility(View.INVISIBLE);
			mNavIntegration.iSelectChannel(selectedChannel);
		} else {
			if (!mNavIntegration.iCurrentInputSourceHasSignal()
					|| !mNavIntegration.iCurrentChannelHasSignal()) {
				mChannelDetailLayout.setVisibility(View.INVISIBLE);
			} else if ((mChannelDetailLayout.getVisibility() == View.VISIBLE)
					&& isValid()) {
				mChannelDetailLayout.setVisibility(View.INVISIBLE);
			} else if ((mChannelDetailLayout.getVisibility() == View.INVISIBLE)
					&& isValid()) {
				mChannelDetailLayout.setVisibility(View.VISIBLE);
			}
		}
	}
}
