package com.mediatek.ui.nav.commonview;

import java.util.List;

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
import com.mediatek.ui.nav.util.NavChannel.FavoriteListListener;
import com.mediatek.ui.util.BypassWindowManager;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

public class ShowFavoriteChannelListView extends Dialog implements
		OnDismissListener {

	private Context mContext;
	private static BypassWindowManager mBypassWindowManager;
	private static CustListView mFavoriteListView;
	private View mChannelListPageUpDownLayout;
	// private Handler mHandler;

	private static final int MSG_DISMISS = 1;
	private static final int MSG_DISMISS_DELAY = 10000;
	private static final String TAG = "ShowFavoriteChannelListView";
	static final int CHANNEL_LIST_PAGE_MAX = 7;

	// private static NavIntegration mNavIntegration;
	// private static FavoriteAdapter mFavoriteAdapter;

	private List<String> list;
	private ChannelAdapter mChannelAdapter;
	private ListData<TVChannel> mFavChannelDataList;
	private static NavChannel mNavIntegration;
	private TextView mTitleTextView;
	private TextView mNavPageUpTextView;
	private TextView mNavPageDownTextView;
	private TextView mNavFavoriteSelectTextView;
	private TextView mNavExitTextView;
	private String mFavoriteChannelListTitle;
	private String mNavPageUpString;
	private String mNavPageDownString;
	private String mNavFavoriteSelectString;
	private String mNavExitString;

	public ShowFavoriteChannelListView(Context context, int theme) {
		super(context, theme);
	}

	public ShowFavoriteChannelListView(Context context) {
		this(context, R.style.nav_dialog);
		MtkLog.d(TAG, "Constructor!");
		mContext = context;
		this.mBypassWindowManager = BypassWindowManager.getInstance(mContext);
		// mHandler = new Handler(context.getMainLooper());
		// mNavIntegration = NavChannel.getInstance(mContext);
	}

	private Handler mHandler = new Handler() {

		public void handleMessage(Message msg) {
			super.handleMessage(msg);

			switch (msg.what) {
			case MSG_DISMISS: {
				dismiss();
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
		case KeyMap.KEYCODE_BACK:
		case KeyMap.KEYCODE_MTKIR_PLAYPAUSE:
			mHandler.removeMessages(MSG_DISMISS);
			dismiss();
			return true;
		case KeyMap.KEYCODE_VOLUME_UP:
		case KeyMap.KEYCODE_VOLUME_DOWN:
			mHandler.removeMessages(MSG_DISMISS);
			dismiss();
			break;
		case KeyMap.KEYCODE_DPAD_CENTER:
			MtkLog.d(TAG, "onKeyDown! KeyMap.KEYCODE_DPAD_CENTER");
			mNavIntegration.isSetChannelFavorite(favListChagedLsner,
					mNavIntegration.iGetCurrentChannel());
			((TurnkeyUiMainActivity)mContext).getBannerView().updateFavorite();
			return true;
		case KeyMap.KEYCODE_MTKIR_EJECT:
			MtkLog.e(TAG, "KEYCODE_MTKIR_EJECT");
			// mNavIntegration.isSetChannelFavorite(favListChagedLsner,
			// mNavIntegration.iGetCurrentChannel());
			TVChannel selectedChannel = (TVChannel) mFavoriteListView
					.getSelectedItem();

			// Fix CR: 588706
			if (mNavIntegration.iGetCurrentChannel() == null) {
				return true;
			}

			if (selectedChannel == null) {
				mNavIntegration.isSetChannelFavorite(favListChagedLsner,
						mNavIntegration.iGetCurrentChannel());
				((TurnkeyUiMainActivity)mContext).getBannerView().updateFavorite();
				return true;
			}

			if (selectedChannel.equals(mNavIntegration.iGetCurrentChannel())) {
				mNavIntegration.isSetChannelFavorite(favListChagedLsner,
						selectedChannel);
			} else {
				TVChannel mFavoriteChannel = mNavIntegration
						.iGetCurrentChannel();
				if (null != mFavoriteChannel && mFavoriteChannel.isFavorite()) {
					selectedChannel.setFavorite(false);
					updateFavList();
				} else {
					mNavIntegration.isSetChannelFavorite(favListChagedLsner,
							mNavIntegration.iGetCurrentChannel());
				}
			}
			break;
		case KeyMap.KEYCODE_MTKIR_RED:
			MtkLog.d(TAG, "KEYCODE_MTKIR_RED");
			if (mFavoriteListView.getSelectedItemPosition() != 0) {
				mFavoriteListView.setSelection(0);
			} else {
				mChannelAdapter
						.updateData(mFavChannelDataList.getPrePageData());
				mFavoriteListView.setSelection(0);
			}
			return true;
		case KeyMap.KEYCODE_MTKIR_GREEN:
			MtkLog.d(TAG, "KEYCODE_MTKIR_GREEN");
			if (mFavoriteListView.getSelectedItemPosition() != ListData.COUNT_EACH_PAGE - 1) {
				mFavoriteListView.setSelection(ListData.COUNT_EACH_PAGE - 1);
			} else {
				mChannelAdapter.updateData(mFavChannelDataList
						.getNextPageData());
				mFavoriteListView.setSelection(ListData.COUNT_EACH_PAGE - 1);
			}
			return true;
		case KeyMap.KEYCODE_MTKIR_STOP:
			// TODO
			if ((mFavChannelDataList.getCount() > 0)
					&& !NavIntegration.getInstance(mContext)
							.isCurrentSourceBlocked()) {
				// channel list and current channel not null
				mNavIntegration.iSelectFavChanUp();
				mChannelAdapter.updateData(mFavChannelDataList
						.getShowData(mNavIntegration.iGetCurrentChannel()));
				mFavoriteListView
						.setSelection(mFavChannelDataList
								.getIdxInShowList(mNavIntegration
										.iGetCurrentChannel()));
			}
			return true;
		case KeyMap.KEYCODE_MTKIR_CHUP:
		case KeyMap.KEYCODE_MTKIR_CHDN:
		case KeyMap.KEYCODE_MTKIR_PRECH:
			willChangeChannel(keyCode);
			return true;
		default:
			break;
		}

		if (null != mContext && mContext instanceof TurnkeyUiMainActivity) {
			MtkLog.d(TAG, "jump to TurnkeyUiMainActivity!!!!!!!");
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
			if (mFavoriteListView.getSelectedItemPosition() == 0) {
				// update adapter list data by get pro page datas
				mChannelAdapter
						.updateData(mFavChannelDataList.getPrePageData());
			} else {
				// update adapter list data by get next page datas
				mChannelAdapter.updateData(mFavChannelDataList
						.getNextPageData());
			}
			mFavoriteListView.setAdapter(mChannelAdapter);
		}
	};

	FavoriteListListener favListChagedLsner = new FavoriteListListener() {

		public void updateFavoriteList() {

			MtkLog.d(TAG, "FavoriteListListener favListChagedLsner!!!!");
			updateFavList();
		}
	};

	// private void mUpdateFavList() {
	// TVChannel curentCh = mNavIntegration.iGetCurrentChannel();
	// mFavChannelDataList.updateAllData(mNavIntegration.iGetFavoriteList());
	// mChannelAdapter.updateData(mFavChannelDataList.getShowData(curentCh));
	// mFavoriteListView.initData(mFavChannelDataList.getAllData(),
	// CHANNEL_LIST_PAGE_MAX, mChannelListUpDate);
	//
	// mChannelAdapter.notifyDataSetChanged();
	// int tmp = mFavChannelDataList.getIdxInShowList(curentCh);
	// tmp = tmp < 0 ? 0 : tmp;
	//
	// mFavoriteListView.setSelection(tmp);
	// }

	private void updateFavList() {

		MtkLog.e(TAG, "updateFavList");

		int selectPosition = mFavoriteListView.getSelectedItemPosition();
		MtkLog.d(TAG, "selectPosition = " + selectPosition);

		TVChannel curChannel = mNavIntegration.iGetCurrentChannel();

		List<TVChannel> mFavoriteChannels = mNavIntegration.iGetFavoriteList();

		if (null != mFavoriteChannels) {
			mFavChannelDataList = new ListData<TVChannel>(mFavoriteChannels,
					curChannel);
			mFavChannelDataList.updateAllData(mFavoriteChannels);
		}
		mChannelAdapter = new ChannelAdapter(mContext,
				mFavChannelDataList.getShowData(mNavIntegration
						.iGetCurrentChannel()));
		mChannelAdapter.updateData(mFavChannelDataList.getShowData(curChannel));
		mFavoriteListView.setAdapter(mChannelAdapter);

		final int tmp1 = mFavChannelDataList.getIdxInShowList(mNavIntegration
				.iGetCurrentChannel());
		MtkLog.d(TAG, "tmp1 = " + tmp1);
		mFavoriteListView.initData(mFavChannelDataList.getAllData(),
				CHANNEL_LIST_PAGE_MAX, mChannelListUpDate);
		mChannelAdapter.updateData(mFavChannelDataList.getShowData(
				mNavIntegration.iGetCurrentChannel(), tmp1));
		mFavoriteListView.setAdapter(mChannelAdapter);
		mChannelAdapter.notifyDataSetChanged();

		int tmp = mFavChannelDataList.getIdxInShowList(curChannel);
		MtkLog.d(TAG, "mFavChannelDataList.getIdxInShowList(curChannel) = "
				+ tmp);

		tmp = tmp < 0 ? 0 : tmp;
		mFavoriteListView.setSelection(tmp);
		showPageUpDownView();

	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		MtkLog.d(TAG, "onCreate");
		setContentView(R.layout.nav_favoritelist);
		setWindowPosition();
		findViews();
		register();
		init();

		mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);
		mFavoriteListView.setFocusable(true);
		mFavoriteListView.requestFocus();
		mFavoriteListView.setSelection(0);
		updateFavList();
		showPageUpDownView();
	}

	protected void onStart() {
		MtkLog.d(TAG, "onStart");
		mFavoriteChannelListTitle = mContext.getResources().getString(
				R.string.nav_favorite_list);
		mTitleTextView = (TextView) findViewById(R.id.nav_favorite_list_title);
		mTitleTextView.setText(mFavoriteChannelListTitle);
		mNavPageUpTextView = (TextView) findViewById(R.id.fav_nav_page_up);
		mNavPageUpString = mContext.getResources().getString(
				R.string.nav_page_up);
		mNavPageUpTextView.setText(mNavPageUpString);
		mNavPageDownTextView = (TextView) findViewById(R.id.fav_nav_page_down);
		;
		mNavPageDownString = mContext.getResources().getString(
				R.string.nav_page_down);
		mNavPageDownTextView.setText(mNavPageDownString);
		mNavFavoriteSelectTextView = (TextView) findViewById(R.id.fav_nav_favorite_select);
		mNavFavoriteSelectString = mContext.getResources().getString(
				R.string.nav_favorite_select);
		mNavFavoriteSelectTextView.setText(mNavFavoriteSelectString);
		mNavExitTextView = (TextView) findViewById(R.id.fav_nav_favorite_exit);
		mNavExitString = mContext.getResources().getString(R.string.nav_exit);
		mNavExitTextView.setText(mNavExitString);
		setWindowPosition();
		updateFavList();
		showPageUpDownView();
		mHandler.removeMessages(MSG_DISMISS);
		mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);
	}

	private void findViews() {
		// mChannelListTextView =
		// (TextView)findViewById(R.id.nav_channel_list_title);
		mFavoriteListView = (CustListView) findViewById(R.id.nav_favorite_listview);
		mChannelListPageUpDownLayout = findViewById(R.id.nav_fav_page_up_down);
	}

	private void register() {
		mFavoriteListView.setOnKeyListener(new ChannelListOnKey());
	}

	private void init() {
		mNavIntegration = NavChannel.getInstance(mContext);
		TVChannel curChannel = mNavIntegration.iGetCurrentChannel();

		List<TVChannel> mFavoriteChannels = mNavIntegration.iGetFavoriteList();

		if (null != mFavoriteChannels) {
			mFavChannelDataList = new ListData<TVChannel>(mFavoriteChannels,
					curChannel);
			mFavChannelDataList.updateAllData(mFavoriteChannels);
		}

		mChannelAdapter = new ChannelAdapter(mContext,
				mFavChannelDataList.getShowData(mNavIntegration
						.iGetCurrentChannel()));
		mChannelAdapter.updateData(mFavChannelDataList.getShowData(curChannel));
		mFavoriteListView.setAdapter(mChannelAdapter);
	}

	private void showPageUpDownView() {
		if (mFavChannelDataList.getCount() <= 0) {
			mHandler.removeMessages(MSG_DISMISS);
			mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);
		} else if (mFavChannelDataList.getCount() > CHANNEL_LIST_PAGE_MAX) {
			mChannelListPageUpDownLayout.setVisibility(View.VISIBLE);
		} else {
			mChannelListPageUpDownLayout.setVisibility(View.INVISIBLE);
		}

	}

	public static int marginX;
	public static int marginY;
	public static int menuWidth = 494 + 329;// 514
	public static int menuHeight = 420;
	public int x = 0;
	public int y = 0;

	public void setWindowPosition() {
		WindowManager m = getWindow().getWindowManager();
		Display display = m.getDefaultDisplay();
		Window window = getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();

		marginY = (int) (display.getHeight() * 0.0139);
		marginX = (int) (display.getWidth() * 0.48);
		menuWidth = (int) (display.getWidth() * 0.32);
		menuHeight = (int) (display.getHeight() * 0.56);
		lp.width = menuWidth;
		lp.height = menuHeight;

		x = display.getWidth() / 2 - menuWidth / 2 - marginX;
		y = (int) (display.getHeight() / 2) - marginY - menuHeight / 2;
		lp.x = x;
		lp.y = y;
		window.setAttributes(lp);
	}

	public int dip2px(Context context, int dp) {
		final float scale = context.getResources().getDisplayMetrics().density;
		return (int) (dp / scale + 0.5f);
	}

	public void onDismiss(DialogInterface dialog) {
		mContext = null;
	}

	@Override
	public void dismiss() {
		mHandler.removeMessages(MSG_DISMISS);
		mBypassWindowManager.setBypassWindow(false,
				TurnkeyUiMainActivity.mShowFavoriteChannelListWindowId,
				new Rect(0, 0, 0, 0));
		super.dismiss();
	}

	class ChannelListOnKey implements View.OnKeyListener {
		public boolean onKey(View v, int keyCode, KeyEvent event) {
			TurnkeyUiMainActivity.reshowFoucs();
			// TODO
			// int selectPosition = mFavoriteListView.getSelectedItemPosition();
			if (event.getAction() == KeyEvent.ACTION_DOWN) {
				switch (keyCode) {
				case KeyEvent.KEYCODE_DPAD_CENTER:
					MtkLog.d(TAG, "come in ChannelListOnKey,KeyEvent.KEYCODE_DPAD_CENTER");
					willChangeChannel(keyCode);
					return true;
				case KeyEvent.KEYCODE_DPAD_DOWN:
					MtkLog.d(TAG, "KEYCODE_DPAD_DOWN!!!!!");
					mHandler.removeMessages(MSG_DISMISS);
					mHandler.sendEmptyMessageDelayed(MSG_DISMISS,
							MSG_DISMISS_DELAY);
					return false;
				case KeyEvent.KEYCODE_DPAD_UP:
					MtkLog.d(TAG, "KEYCODE_DPAD_UP!!!!!");
					mHandler.removeMessages(MSG_DISMISS);
					mHandler.sendEmptyMessageDelayed(MSG_DISMISS,
							MSG_DISMISS_DELAY);
					return false;
				default:
					return false;
				}
			}
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
			mNavIntegration.iSelectFavChanUp();
			mChannelAdapter.updateData(mFavChannelDataList
					.getShowData(mNavIntegration.iGetCurrentChannel()));
			mFavoriteListView.setSelection(mFavChannelDataList
					.getIdxInShowList(mNavIntegration.iGetCurrentChannel()));
			break;
		case KeyMap.KEYCODE_MTKIR_CHDN:
			mNavIntegration.iSelectFavChanDown();
			mChannelAdapter.updateData(mFavChannelDataList
					.getShowData(mNavIntegration.iGetCurrentChannel()));
			mFavoriteListView.setSelection(mFavChannelDataList
					.getIdxInShowList(mNavIntegration.iGetCurrentChannel()));
			break;
		case KeyMap.KEYCODE_MTKIR_PRECH:
			mNavIntegration.iSetChannel(NavIntegration.CHANNEL_PRE);
			mChannelAdapter.updateData(mFavChannelDataList
					.getShowData(mNavIntegration.iGetCurrentChannel()));
			mFavoriteListView.setSelection(mFavChannelDataList
					.getIdxInShowList(mNavIntegration.iGetCurrentChannel()));
			break;
		case KeyEvent.KEYCODE_DPAD_CENTER:
			mHandler.removeMessages(MSG_DISMISS);
			mHandler.sendEmptyMessageDelayed(MSG_DISMISS,
					MSG_DISMISS_DELAY);

			MtkLog.v(
					TAG,
					"mChannelItemKeyLsner*********** selectPosition"
							+ mFavoriteListView
									.getSelectedItemPosition());

			TVChannel selectedChannel = (TVChannel) mFavoriteListView
					.getSelectedItem();
			if (!selectedChannel.equals(mNavIntegration
					.iGetCurrentChannel())) {
				mNavIntegration.iSelectChannel(selectedChannel);
			} else {
				mNavIntegration.isSetChannelFavorite(selectedChannel);
				updateFavList();
				if(null != mContext){
					((TurnkeyUiMainActivity)mContext).getBannerView().updateFavorite();
				}
			}
			break;
		}
	}
}
