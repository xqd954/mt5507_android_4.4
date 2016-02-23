package com.mediatek.ui.mmp.multimedia;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Observable;
import java.util.concurrent.ConcurrentHashMap;

import android.app.Dialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.IntentFilter;
import android.content.DialogInterface.OnCancelListener;
import android.content.DialogInterface.OnDismissListener;
import android.content.DialogInterface.OnKeyListener;
import android.content.Intent;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.graphics.drawable.Drawable;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.NetworkInfo.State;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.MessageQueue.IdleHandler;
import android.view.Display;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.TextView;

import com.mediatek.mmpcm.audioimpl.AudioConst;
import com.mediatek.mmpcm.audioimpl.PlaybackService;
import com.mediatek.mmpcm.device.DevManager;
import com.mediatek.mmpcm.fileimpl.MtkFile;
import com.mediatek.mmpcm.fileimpl.UsbFileOperater;
import com.mediatek.mmpcm.fileimpl.UsbFileOperater.OnUsbCopyProgressListener;
import com.mediatek.mmpcm.mmcimpl.Const;
import com.mediatek.mmpcm.videoimpl.Thumbnail;
import com.mediatek.mmpcm.videoimpl.VideoConst;
import com.mediatek.tvcm.TVContent;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.SaveValue;
import com.mediatek.ui.mmp.commonview.ConfirmDeleteDialog;
import com.mediatek.ui.mmp.commonview.ConfirmDeleteDialog.OnConfirmDeleteListener;
import com.mediatek.ui.mmp.commonview.ConfirmReplaceDialog;
import com.mediatek.ui.mmp.commonview.ConfirmReplaceDialog.OnConfirmReplaceListener;
import com.mediatek.ui.mmp.commonview.KeyboardDialog;
import com.mediatek.ui.mmp.commonview.KeyboardDialog.OnPressedListener;
import com.mediatek.ui.mmp.commonview.LoginDialog;
import com.mediatek.ui.mmp.commonview.LoginDialog.OnKeyClickListener;
import com.mediatek.ui.mmp.commonview.MenuListView;
import com.mediatek.ui.mmp.commonview.MusicPlayInfoView;
import com.mediatek.ui.mmp.commonview.ProgressDialog;
import com.mediatek.ui.mmp.commonview.TipsDialog;
import com.mediatek.ui.mmp.gamekit.filebrowse.MtkFileFlimBrowseActivity;
import com.mediatek.ui.mmp.model.FileAdapter;
import com.mediatek.ui.mmp.model.FilesManager;
import com.mediatek.ui.mmp.model.MultiFilesManager;
import com.mediatek.ui.mmp.util.AsyncLoader;
import com.mediatek.ui.mmp.util.AsyncLoader.LoadWork;
import com.mediatek.ui.mmp.util.GetDataImp;
import com.mediatek.ui.mmp.util.LogicManager;
import com.mediatek.ui.mmp.util.LogicManager.UnmountLisenter;
import com.mediatek.ui.mmp.util.MenuFatherObject;
import com.mediatek.ui.mmp.util.MultiMediaConstant;
import com.mediatek.ui.mmp.util.Util;
import com.mediatek.ui.util.DestroyApp;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

public class MtkFilesBaseListActivity extends FilesListActivity<FileAdapter> {

	/**
	 * Log tag
	 */
	private static final String TAG = "MtkFilesBaseListActivity";

	public static final String INTENT_NAME_PATH = "Path";

	public static final String INTENT_NAME_SELECTION = "Position";

	public static final String INTENT_NAME_COPYED_FILES = "CopyedFiles";
	public static final String USBDISK = "/mnt/usbdisk";

	public static final int MODE_NORMAL = 1;

	public static final int MODE_RECURSIVE = 2;

	protected static final int DIALOG_EMPTY_TIP = 0;

	protected static final int DIALOG_MENU = 1;

	protected static final int DIALOG_REU_TIP = 2;

	protected static final int DIALOG_LOGIN = 3;

	protected static final int DIALOG_KEYBOARD = 4;

	protected static final int DIALOG_LOGIN_FAILED_TIP = 5;

	protected static final int DIALOG_PROGRESS = 6;

	protected static final int DIALOG_CONFIRM_DELETE = 7;

	protected static final int DIALOG_CONFIRM_REPLACE = 8;

	protected static final int DIALOG_LOADING = 9;

	private static final int MSG_RECURSIVE_PARSER = 0;

	private static final int MSG_PHOTO_FRAME = 1;

	private static final int MSG_NOSUPPORT_SHOW = 10;

	private static final int MSG_NOSUPPORT_DISMISS = 11;

	public static final int KEY_DURATION = 400;

	protected long mLastKeyDownTime;

	private ProgressDialog vProgressDialog;

	private LoginDialog vLoginDialog;

	private TipsDialog vTipDialog;

	private ConfirmDeleteDialog vConfirmDeleteDialog;

	protected Resources mResources;

	// Added by Dan for fix bug DTV00379203
	protected ArrayList<String> mSelectedFiles;

	protected ArrayList<String> mCopyedFiles;

	protected String[] mContentTypeNames;

	protected Drawable[] mContentTypeIcons;

	public int mThumbnailSize;

	protected int mPlayMode;

	protected int mCurrentPage;

	protected int mPageSize;

	protected int mPageCount;

	private boolean mPasteCanceled;

	private long mPasteSizeCount;

	protected PopupWindow mPopView;

	protected MusicPlayInfoView vMusicView;

	protected List<FileAdapter> mRecursiveFiles;

	private boolean mYes;

	private boolean mYesToAll;

	public static int mMode = MODE_NORMAL;

	private boolean isCancel = false;

	private Thread mRecursThread;

	public static boolean mFlag = false;

	protected TipsDialog mTipsDialog;
	protected AsyncLoader<String> mInfoLoader;
	protected ConcurrentHashMap<String, LoadInfo> mLoadInfoWorks;
	
	public int currentPosition = 0;
	public boolean isFirstRequest = true;
	
	private MtkNetWorkRecevier netRecevier;
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		//mInfoLoader = new AsyncLoader<String>(1);
		netRecevier = new MtkNetWorkRecevier();
		IntentFilter intentFilter = new  IntentFilter("android.net.conn.CONNECTIVITY_CHANGE");
		registerReceiver(netRecevier, intentFilter);
		mInfoLoader = AsyncLoader.getInstance(1);
		mLoadInfoWorks =new  ConcurrentHashMap<String, LoadInfo>();
		LogicManager.getInstance(this).registerUnMountLisenter(unmountLisenter);
	}


	private UnmountLisenter unmountLisenter = new UnmountLisenter(){
			public void onUnmount(){
				stopMusicView();
			}
		}; 

	private Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			super.handleMessage(msg);

			try {
				dismissDialog(DIALOG_REU_TIP);
			} catch (IllegalArgumentException e) {
//			     e.printStackTrace();
				MtkLog.d(TAG,"dismissDialog DIALOG_REU_TIP exception~~");
			}
			switch (msg.what) {
			case MSG_RECURSIVE_PARSER:
				try {
					dismissDialog(DIALOG_EMPTY_TIP);
				} catch (IllegalArgumentException e) {
//				    e.printStackTrace();
					MtkLog.d(TAG,"dismissDialog DIALOG_EMPTY_TIP exception~~");
				}
				if (mRecursiveFiles != null && mRecursiveFiles.size() > 0) {
					MultiFilesManager
							.getInstance(MtkFilesBaseListActivity.this)
							.setFiles(mRecursiveFiles);
					mMode = MODE_RECURSIVE;
					refreshListView(mRecursiveFiles);
					//setCurrentSelection();
					//setListSelection(mFilesManager.getPositionInParent());
				}else{
					mMode = MODE_RECURSIVE;
					refreshListView(null);


				}
				break;
			case MSG_PHOTO_FRAME:
				if (mRecursiveFiles != null && mRecursiveFiles.size() > 0) {
					((MultiFilesManager) mFilesManager).getPlayList(
							mRecursiveFiles, 0, FilesManager.CONTENT_PHOTO,
							MultiFilesManager.SOURCE_LOCAL);

					Intent intent = null;
					if(Util.PHOTO_4K2K_ON){
						intent = new Intent(MtkFilesBaseListActivity.this,
							Photo4K2KPlayActivity.class);
					}else{
						intent = new Intent(MtkFilesBaseListActivity.this,
							PhotoPlayActivity.class);
					}
					intent.putExtra("PlayMode", mPlayMode);
					startActivity(intent);
					//finish();
				}
				break;
			case MSG_NOSUPPORT_SHOW:

				onNotSuppsort(getString(R.string.mmp_file_notsupport));
				mHandler.sendEmptyMessageDelayed(MSG_NOSUPPORT_DISMISS, 1000);
				break;
			case MSG_NOSUPPORT_DISMISS:
				dismissNotSupprot();
				break;
			default:
				break;
			}
		}
	};
	protected ConcurrentHashMap<String, String> mInforCache = new ConcurrentHashMap<String, String>();
	
	protected class LoadInfo implements LoadWork<String> {
		private FileAdapter mFile;
		
		private TextView mView;

		public LoadInfo(FileAdapter file,TextView view) {
			mFile = file;
			mView=view;
		}

		public String load() {
			if (null == mFile) {
				return "";
			}
			MtkLog.d(TAG,"LoadInfo load mFile ="+mFile.getAbsolutePath()+"name = "+mFile.getName());

			String path = mFile.getAbsolutePath();
			String info = mFile.getInfo();
			if (null != info) {
				mInforCache.put(path, info);
			}
			return info;
		}

		public void loaded(final String result) {
			mView.post(new Runnable() {
				
				public void run() {
					mView.setText(result);
				}
			});
		}

	}

	private ListView.OnItemClickListener mListener = new OnItemClickListener() {
		public void onItemClick(AdapterView<?> parent, View view, int position,
				long id) {
			TextView tvTextView = (TextView) view
					.findViewById(R.id.mmp_menulist_tv);
			String content = tvTextView.getText().toString();
			controlState(content);
		}
	};

	/**
	 * {@inheritDoc}
	 */
	protected void init() {
		mResources = getResources();
		mSelectedFiles = new ArrayList<String>();
		mCopyedFiles = new ArrayList<String>();
	
		Intent intent = getIntent();
		
		
		//add for ThumbnailSize bug
		
		int value = intent.getIntExtra("mThumbnailSize", 1);		
		try {
			String m = TVContent.getInstance(this).getStorage().get(
					MultiMediaConstant.THUMBNAIL_SIZE);
			if (null != m && m.length()>0) {
				value = Integer.parseInt(m);
			}
		} catch (Exception e) {
		    e.printStackTrace();
		}
		
		if(value == MultiMediaConstant.MEDIUM){
			mThumbnailSize = MultiMediaConstant.MEDIUM;
		}
		else if(value == MultiMediaConstant.LARGE){
			mThumbnailSize = MultiMediaConstant.LARGE;
		}
		else{
			mThumbnailSize = MultiMediaConstant.SMALL;
		}
						
	
		findViews();
		prepareResources();

		DestroyApp des = (DestroyApp) this.getApplication();
		des.add(this);
	}

	/**
	 * Set current selection focus
	 * 
	 * @return the selected item position
	 */
	protected int setCurrentSelection() {
		if (null == mAdapter || mAdapter.getCount() == 0) {
			return 0;
		}
		int sourceType = MultiFilesManager.getInstance(this)
				.getCurrentSourceType();
		// if Connection mode is SMB or DLNA path choosing the name else path is
		// the absolute path
		if ( sourceType == MultiFilesManager.SOURCE_DLNA) {
			for (int position = 0; position < mAdapter.getCount(); position++) {
				FileAdapter adapter = mAdapter.getItem(position);
				if (null != adapter) {
					String path = LogicManager.getInstance(this)
							.getCurrentPath(getMediaFilter());
					if(null == path) {
						path = mFilesManager.getCurrentPath();
					}
					if (null != path && path.equals(adapter.getName())) {
						setListSelection(position);
						return position;
					}
				}
			}
			return 0;
		} else {
			for (int position = 0; position < mAdapter.getCount(); position++) {
				FileAdapter adapter = mAdapter.getItem(position);
				if (null != adapter) {
					String path = LogicManager.getInstance(this)
							.getCurrentPath(getMediaFilter());
					if(null == path) {
						path = mFilesManager.getCurrentPath();
					}
					if (null != path && path.equals(adapter.getAbsolutePath())) {
						setListSelection(position);
						return position;
					}
				}
			}
			return 0;
		}
	}

	protected int getMediaFilter() {
		switch (getListContentType()) {
		case MultiMediaConstant.AUDIO:
			return Const.FILTER_AUDIO;
		case MultiMediaConstant.THRD_PHOTO:
		case MultiMediaConstant.PHOTO:
			return Const.FILTER_IMAGE;
		case MultiMediaConstant.VIDEO:
			return Const.FILTER_VIDEO;
		case MultiMediaConstant.TEXT:
			return Const.FILTER_TEXT;
		default:
			return -1;
		}
	}

	protected void initMusicView() {

	}

	/**
	 * {@inheritDoc}
	 */
	protected void onPause() {
		super.onPause();
		if (null != vMusicView) {
			vMusicView.removeMessage();
		}
	}

	public static void reSetModel() {
		mMode = MODE_NORMAL;
	}

	/**
	 * {@inheritDoc}
	 */
	protected void onDestroy() {
		//mFilesManager.deleteObserver(this);
		this.unregisterReceiver(netRecevier);
		cancelRecursive();
		try {
			dismissDialog(DIALOG_MENU);
			dismissDialog(DIALOG_EMPTY_TIP);
		} catch (IllegalArgumentException e) {
//		    e.printStackTrace();
			MtkLog.d(TAG,"dismissDialog DIALOG_MENU&&DIALOG_EMPTY_TIP exception~~");
		}

		if (null != mPopView && mPopView.isShowing()) {
			mPopView.dismiss();
		}

		((DestroyApp) getApplication()).remove(this);
		super.onDestroy();
	}

	/**
	 * Show music play view
	 */
	protected void showMusicView() {
		if (vMusicView != null) {
			vMusicView.initControl();
		} else {
			initMusicView();
		}

		if (null != mPopView && null != vMusicView) {
			vMusicView.init(this);
			return;
		}
	}

	protected void stopMusicView() {
		runOnUiThread(new Runnable(){
			@Override
			public void run(){
				if (null != mPopView) {
					if (mPopView.isShowing()) {
						mPopView.dismiss();
					}
				}
			}
		});
				
		
	}

	protected boolean isValid() {
		long currentTime = System.currentTimeMillis();
		if ((currentTime - mLastKeyDownTime) >= KEY_DURATION) {
			mLastKeyDownTime = currentTime;
			return true;
		} else {
			MtkLog.i(TAG, " key down duration :"
					+ (currentTime - mLastKeyDownTime) + "< 400 mmm");
			mLastKeyDownTime = currentTime;
			return false;
		}
	}

	/**
	 * {@inheritDoc}
	 */
	public boolean dispatchKeyEvent(KeyEvent event) {
		int keyCode = event.getKeyCode();
		MtkLog.d(TAG, "Key : " + keyCode);
		switch (keyCode) {
		case KeyMap.KEYCODE_VOLUME_UP:
		case KeyMap.KEYCODE_VOLUME_DOWN:
		case KeyMap.KEYCODE_MTKIR_CHDN:
		case KeyMap.KEYCODE_MTKIR_CHUP:
			if (event.getAction() == KeyEvent.ACTION_DOWN) {
				onKeyDown(keyCode, event);
			}
			return true;
		default:
			break;
		}

		return super.dispatchKeyEvent(event);
	}

	/**
	 * show not support tips dialog
	 * 
	 * @param title
	 *            the tips dialog content
	 */
	protected void onNotSuppsort(String title) {

		MtkLog.i(TAG, "onNotSuppsort  :" + title);
		if (null == mTipsDialog) {
			mTipsDialog = new TipsDialog(this);
			mTipsDialog.setText(title);
			mTipsDialog.show();
			mTipsDialog.setBackground(R.drawable.toolbar_playerbar_test_bg);
			Drawable drawable = this.getResources().getDrawable(
					R.drawable.toolbar_playerbar_test_bg);

			int weight = (int) (drawable.getIntrinsicWidth() * 0.6);
			int height = drawable.getIntrinsicHeight();
			mTipsDialog.setDialogParams(weight, height);

		
			WindowManager m = mTipsDialog.getWindow().getWindowManager();
			Display display = m.getDefaultDisplay();

			int x = -((int) (ScreenConstant.SCREEN_WIDTH / 2) - weight / 2)
					+ (int) (ScreenConstant.SCREEN_WIDTH / 10);
			int y = (int) (ScreenConstant.SCREEN_HEIGHT * 3 / 8) - 114 - height / 2;
		    mTipsDialog.setWindowPosition(x, y);
			
		} else {
			mTipsDialog.setText(title);
			mTipsDialog.show();
		}

	}

	/**
	 * Dismiss not support tips dialog
	 */
	protected void dismissNotSupprot() {

		if (null != mTipsDialog && mTipsDialog.isShowing()) {
			mTipsDialog.dismiss();
		}
	}

	/**
	 * {@inheritDoc}
	 */
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		LogicManager lm=null;
		switch (keyCode) {
		case KeyMap.KEYCODE_MENU:
			showDialog(DIALOG_MENU);
			break;
		case KeyMap.KEYCODE_MTKIR_ANGLE:
//			LogicManager.getInstance(this).restoreVideoResource();
//			LogicManager.getInstance(this).finishAudioService();
//			LogicManager.getInstance(this).finishVideo();
//			DestroyApp destroyApp = (DestroyApp) getApplicationContext();
//			destroyApp.finishAll();
			if (isValid()) {
				cancelRecursive();
				mAdapter.cancel();
				Thumbnail thumbnail = Thumbnail.getInstance();
				if (thumbnail.isLoadThumanil()) {
					thumbnail.stopThumbnail();
				}
				Util.exitMmpActivity(this);
			}
			break;
		case KeyMap.KEYCODE_VOLUME_UP: {
			if (mPopView != null && mPopView.isShowing()) {
				vMusicView.setVolumeUp();
			}else{
				
				lm=LogicManager.getInstance(this);
				if (lm.isMute()) {
					lm.setMute();
				}else{
					int max=lm.getMaxVolume();
					int curVol=lm.getVolume();
					curVol+=1;
					if(curVol<max){
						lm.setVolume(curVol);
					}else{
						lm.setVolume(max);
					}
				}
			}
			return true;
		}
		case KeyMap.KEYCODE_VOLUME_DOWN: {
			if (mPopView != null && mPopView.isShowing()) {
				vMusicView.setVolumeDown();
			}else{
				lm=LogicManager.getInstance(this);
				if (lm.isMute()) {
					lm.setMute();
				}else{
					int curVol=lm.getVolume();
					curVol-=1;
					if(curVol>0){
						lm.setVolume(curVol);
					}else{
						lm.setVolume(0);
					}
				}
			}
			return true;
		}
		case KeyMap.KEYCODE_MTKIR_REPEAT: {
			if (mPopView != null && mPopView.isShowing()) {
				vMusicView.onRepeatClick();
			}
			return true;
		}
		case KeyMap.KEYCODE_MTKIR_PLAYPAUSE: {
			if (mPopView != null && mPopView.isShowing()) {
				vMusicView.onPauseOrPlay();
			}
			return true;
		}
		case KeyMap.KEYCODE_MTKIR_STOP: {
			if (mPopView == null || !mPopView.isShowing()) {
				return true;
			}
			vMusicView.onStop();
			if (mPopView.isShowing()) {
				mPopView.dismiss();
			}
			return true;
		}
		case KeyMap.KEYCODE_MTKIR_MUTE: {
			if (mPopView != null && mPopView.isShowing()) {
				vMusicView.setMute();
			}else{
				LogicManager.getInstance(this).setMute();
			}
			return true;
		}
		case KeyMap.KEYCODE_MTKIR_NEXT: {
			if (mPopView != null && mPopView.isShowing()) {
				LogicManager.getInstance(this).playNextAudio();
			}
			return true;
		}
		case KeyMap.KEYCODE_MTKIR_PREVIOUS: {
			if (mPopView != null && mPopView.isShowing()) {
				LogicManager.getInstance(this).playPrevAudio();
			}
			return true;
		}
		case KeyMap.KEYCODE_MTKIR_FASTFORWARD: {
			if (null == mPopView || !mPopView.isShowing()
					|| LogicManager.getInstance(this).isAudioStoped()) {
				return true;
			}
			try {
				LogicManager.getInstance(this).fastForwardAudio();
				vMusicView.onFast();
			} catch (IllegalStateException e) {

				mHandler.sendEmptyMessage(MSG_NOSUPPORT_SHOW);

			}

			return true;
		}
		case KeyMap.KEYCODE_MTKIR_REWIND: {
			if (null == mPopView || !mPopView.isShowing()
					|| LogicManager.getInstance(this).isAudioStoped()) {
				return true;
			}
			try {
				LogicManager.getInstance(this).fastRewindAudio();
				vMusicView.onFast();
			} catch (IllegalStateException e) {
				mHandler.sendEmptyMessage(MSG_NOSUPPORT_SHOW);

			}

			return true;
		}
		case KeyMap.KEYCODE_MTKIR_CHDN:
		case KeyMap.KEYCODE_MTKIR_CHUP:
			return true;
		case KeyMap.KEYCODE_MTKIR_GUIDE: {
			if (isValid()) {
				mAdapter.cancel();
				Thumbnail thumbnail = Thumbnail.getInstance();
				if (thumbnail.isLoadThumanil()) {
					thumbnail.stopThumbnail();
				}
				Util.startEPGActivity(this);
				
			}
			break;
		}
		// Added by Dan for fix bug DTV00379203
		case KeyMap.KEYCODE_MTKIR_EJECT:
			select();
			break;
		default:
			break;
		}

		return super.onKeyDown(keyCode, event);
	}

	// Added by Dan for fix bug DTV00379203
	private void select() {
	   FileAdapter file = getListSelectedItem();
	   if(null != file){
          if(file.isFile()){
			  return;
		  }
		  String selectedFile = file.getAbsolutePath();
				   if (mSelectedFiles.size() > 0) {
					  String oneSelectedFile = mSelectedFiles.get(0);
					  if (!mFilesManager.isInSameFolder(oneSelectedFile, selectedFile)) {
							 mSelectedFiles.clear();
					  }
				   }
		  
				   if (!mSelectedFiles.contains(selectedFile)) {
					  mSelectedFiles.add(selectedFile);
				   } else {
					  mSelectedFiles.remove(selectedFile);
				   }
		  
				   cancelLoadFiles();
				   mAdapter.notifyDataSetChanged();

		  
	   }
	}

	/**
	 * {@inheritDoc}
	 */
	protected Dialog onCreateDialog(int id) {
		Dialog dialog = null;

		switch (id) {
		case DIALOG_EMPTY_TIP:
			TipsDialog emptyTip = new TipsDialog(this);
			emptyTip.setWindowPosition();
			emptyTip.setOnKeyListener(new OnKeyListener() {
				public boolean onKey(DialogInterface dialog, int keyCode,
						KeyEvent event) {
					switch (keyCode) {
					case KeyMap.KEYCODE_MENU:
						dismissDialog(DIALOG_EMPTY_TIP);
						showDialog(DIALOG_MENU);
						return true;
					case KeyMap.KEYCODE_BACK:
						dismissDialog(DIALOG_EMPTY_TIP);
						onBackPressed();
						return true;
					default:
						break;
					}
					return false;
				}
			});
			dialog = emptyTip;
			vTipDialog = emptyTip;
			break;
		case DIALOG_LOGIN_FAILED_TIP:
			TipsDialog failedTip = new TipsDialog(this);
			failedTip.setText(mResources
					.getString(R.string.mmp_login_failed_tip));
			failedTip.setWindowPosition();

			dialog = failedTip;
			break;
		case DIALOG_MENU:
			MtkLog.i("dialog","onCreateDialog");
			int enable_id = R.array.mmp_menu_filelist_enable;
			final MenuListView menu = new MenuListView(this, GetDataImp
					.getInstance().getComMenu(this, R.array.mmp_menu_filelist,
							enable_id,
							R.array.mmp_menu_filelist_hasnext), mListener, null);
			menu.setOnDismissListener(new OnDismissListener() {
				public void onDismiss(DialogInterface dialog) {
					if (menu.isContain(mResources
							.getString(R.string.mmp_menu_mount_iso))) {
						menu.removeItem(menu.getCount() - 1);
					}

					if (menu.isContain(mResources
							.getString(R.string.mmp_menu_unmount_iso))) {
						menu.removeItem(menu.getCount() - 1);
					}

					if (getListItemsCount() == 0) {
						onListEmpty();
					}
				}
			});
			customMenu(menu);

			dialog = menu;
			break;
		case DIALOG_REU_TIP:
			TipsDialog reuTip = new TipsDialog(this);
			reuTip.setText(mResources.getString(R.string.mmp_reutip));
			reuTip.setWindowPosition();

			dialog = reuTip;
			reuTip.setOnKeyListener(new OnKeyListener() {
				public boolean onKey(DialogInterface dialog, int keyCode,
						KeyEvent event) {
					if (event.getAction() == KeyEvent.ACTION_DOWN) {
						if (keyCode == KeyMap.KEYCODE_BACK) {
							isCancel = true;
							mMode = MODE_RECURSIVE;
							if (null != mRecursThread) {
								try {
									MtkLog.i(TAG, "thread isAlive:"
											+ mRecursThread.isAlive());
									if (mRecursThread.isAlive()) {
										mRecursThread.interrupt();
									}
								} catch (Exception e) {
									MtkLog.d(TAG, e.getMessage());
								}
							}
							finish();
						}
					}
					return false;
				}
			});
			break;
		case DIALOG_LOGIN:
			final LoginDialog login = new LoginDialog(this);
			login.setOnKeyClickListener(new OnKeyClickListener() {
				public void onInputClicked() {
					String prefill = login.getPrefill();
					boolean isPassword = login.isPassword();
					Bundle data = new Bundle();
					data.putString("Prefill", prefill);
					data.putBoolean("IsPassword", isPassword);
					dismissDialog(DIALOG_LOGIN);
					showDialog(DIALOG_KEYBOARD, data);
				}

				public void onConfirmClicked(String userName, String password) {
					dismissDialog(DIALOG_LOGIN);
					MtkLog.d(TAG, "UserName : " + userName);
					MtkLog.d(TAG, "Password : " + password);

					boolean success = ((MultiFilesManager) mFilesManager)
							.login(getListCurrentPath(), userName, password);
					if (success) {
						MtkLog.i(TAG, "Login Success!!");
						mFilesManager.popOpenedHistory();
						openDir(getListCurrentPath());
					} else {
						MtkLog.w(TAG, "Login Failed!!");
						showDialog(DIALOG_LOGIN_FAILED_TIP);
					}
				}
			});
			dialog = login;
			vLoginDialog = login;
			break;
		case DIALOG_KEYBOARD:
			KeyboardDialog keyboard = new KeyboardDialog(this);
			keyboard.setOnPressedListener(new OnPressedListener() {
				public void onPositivePressed(String input) {
					Bundle data = new Bundle();
					data.putString("Input", input);
					dismissDialog(DIALOG_KEYBOARD);
					showDialog(DIALOG_LOGIN, data);
				}

				public void onNegativePressed() {
					dismissDialog(DIALOG_KEYBOARD);
					showDialog(DIALOG_LOGIN);
				}
			});

			keyboard.setOnDismissListener(new OnDismissListener() {
				public void onDismiss(DialogInterface dialog) {
					showDialog(DIALOG_LOGIN);
				}
			});

			dialog = keyboard;
			break;
		case DIALOG_PROGRESS:
			ProgressDialog progress = new ProgressDialog(this);
			progress.setTitle(R.string.mmp_title_copy);
			progress.setCancelable(false);
			dialog = progress;
			vProgressDialog = progress;
			break;
		case DIALOG_CONFIRM_DELETE:
			ConfirmDeleteDialog confirmDelete = new ConfirmDeleteDialog(this);
			confirmDelete.setTitle(R.string.mmp_title_confirm_delete);
			confirmDelete.setMessage(R.string.mmp_message_confirm_delete);
			dialog = confirmDelete;
			vConfirmDeleteDialog = confirmDelete;
			break;
		case DIALOG_LOADING:
			TipsDialog loadTip = new TipsDialog(this);
			loadTip.setText(mResources.getString(R.string.mmp_loading_tip));
			loadTip.setWindowPosition();
			loadTip.setOnKeyListener(new OnKeyListener() {
				public boolean onKey(DialogInterface dialog, int keyCode,
						KeyEvent event) {
					if (keyCode == KeyMap.KEYCODE_BACK) {
						dismissDialog(DIALOG_LOADING);
						onBackPressed();
					}

					return true;
				}
			});

			dialog = loadTip;
			break;
		case DIALOG_CONFIRM_REPLACE:
			ConfirmReplaceDialog replaceDialog = new ConfirmReplaceDialog(this);
			replaceDialog.setCancelable(false);
			replaceDialog.setTitle(R.string.mmp_title_confirm_replace);
			replaceDialog
					.setOnConfirmReplaceListener(new OnConfirmReplaceListener() {
						public void onYesToAllPressed() {
							MtkLog.d(TAG, "Replace All!!");
							dismissDialog(DIALOG_CONFIRM_REPLACE);
							showDialog(DIALOG_PROGRESS);
							mYesToAll = true;
							synchronized (mCopyedFiles) {
								mCopyedFiles.notifyAll();
							}
						}

						public void onYesPressed() {
							dismissDialog(DIALOG_CONFIRM_REPLACE);
							showDialog(DIALOG_PROGRESS);
							MtkLog.d(TAG, "Replace One!!");
							mYes = true;
							synchronized (mCopyedFiles) {
								mCopyedFiles.notifyAll();
							}
						}

						public void onNoPressed() {
							dismissDialog(DIALOG_CONFIRM_REPLACE);
							showDialog(DIALOG_PROGRESS);
							MtkLog.d(TAG, "Replace No!!");
							synchronized (mCopyedFiles) {
								mCopyedFiles.notifyAll();
							}
						}

						public void onCancelPressed() {
							dismissDialog(DIALOG_CONFIRM_REPLACE);
							MtkLog.d(TAG, "Replace Cancel!!");
							UsbFileOperater.getInstance().setCopyAbort(true);
							mPasteCanceled = true;
							synchronized (mCopyedFiles) {
								mCopyedFiles.notifyAll();
							}
						}
					});

			dialog = replaceDialog;
			break;
		default:
			break;
		}

		return dialog;
	}

	protected void customMenu(MenuListView menu) {
	}

	/**
	 * {@inheritDoc}
	 */
	protected void onPrepareDialog(int id, Dialog dialog, Bundle args) {
		switch (id) {
		case DIALOG_MENU:
			MenuListView menu = (MenuListView) dialog;

			menu.setMediaType(getListContentType());
			FileAdapter file = getListSelectedItem();
			//for 3d browse start
			if(MultiFilesManager.ROOT_PATH.equals(getListCurrentPath())){
				
				menu.setItemEnabled(0,false);
			}else{
				menu.setItemEnabled(0,true);
				
			}
			int sType = 0;
			if (mFilesManager instanceof MultiFilesManager) {
				sType = ((MultiFilesManager) mFilesManager).getCurrentSourceType();
			}
			if(MultiFilesManager.SOURCE_DLNA ==sType){
				if (menu.isContain(mResources.getString(R.string.mmp_menu_3d_photo)) ){
					menu.setItemEnabled(menu.getCount()-1, false);
				}
			}else{
				if (menu.isContain(mResources.getString(R.string.mmp_menu_3d_photo)) ){
					menu.setItemEnabled(menu.getCount()-1, true);
				}
				
			}
			if (mFilesManager.getContentType() == FilesManager.CONTENT_TEXT ||
					mFilesManager.getContentType() == FilesManager.CONTENT_VIDEO || 
					sType== MultiFilesManager.SOURCE_DLNA ||
					sType == MultiFilesManager.SOURCE_SMB ||
					mMode != MODE_RECURSIVE ||
					file == null){
				if (menu.isContain(mResources.getString(R.string.mmp_menu_3d_browse)) ){
					menu.setItemEnabled(menu.getCount()-2, false);
				}
			}else {
				if (menu.isContain(mResources.getString(R.string.mmp_menu_3d_browse)) ){
					menu.setItemEnabled(menu.getCount()-2, true);
				}
			}
			//for 3d browse end.
			if (null != file
					&& DevManager.getInstance().isVirtualDev(
							file.getAbsolutePath())) {
				// TODO unmount
				String menuContent = mResources
						.getString(R.string.mmp_menu_unmount_iso);
				if (!menu.isContain(menuContent)) {
					MenuFatherObject obj = new MenuFatherObject();
					obj.content = menuContent;
					obj.enable = true;
					obj.hasnext = false;
					obj.enter = false;
					menu.addItem(menu.getCount(), obj);
				}

			} else if (file == null || file.isDirectory()) {
				menu.showCopy(false);
				menu.showDelete(false);
			} else if (file.isIsoFile()) {
				// TODO ISO mount menu
				String menuContent = mResources
						.getString(R.string.mmp_menu_mount_iso);
				if (!menu.isContain(menuContent)) {
					MenuFatherObject obj = new MenuFatherObject();
					obj.content = menuContent;
					obj.enable = true;
					// Add by Dan for fix bug DTV00374299
					if (((MultiFilesManager) mFilesManager)
							.getMountedIsoCount() >= 8) {
						obj.enable = false;
					}
					obj.hasnext = false;
					obj.enter = false;
					menu.addItem(menu.getCount(), obj);
				}
			} else {
				menu.showCopy(true);
				menu.showDelete(true);
			}

			if (mCopyedFiles.size() > 0) {
				String oneCopyedFile = mCopyedFiles.get(0);
				if (mFilesManager.canPaste(oneCopyedFile)) {
					menu.showPaste(true);
				}else{
					menu.showPaste(false);
				}
			} else {
				menu.showPaste(false);
			}

			int sourceType = MultiFilesManager.getInstance(this)
					.getCurrentSourceType();

			int index = 6;
			// Removed by Dan for fix bug DTV00379191
			// if
			// (menu.isContain(mResources.getString(R.string.mmp_menu_thumb))) {
			// index = 6;
			// } else {
			// index = 5;
			// }

			MenuFatherObject obj = menu.getItem(index);
			if (sourceType == MultiFilesManager.SOURCE_LOCAL) {
				obj.enable = true;
			} else {
				obj.enable = false;
			}

			menu.updateItem(index, obj);

			if (mMode == MODE_NORMAL) {
				menu.updateItem(index,
						getResources().getString(R.string.mmp_menu_recur));
			} else if (mMode == MODE_RECURSIVE) {
				menu.updateItem(index,
						getResources().getString(R.string.mmp_menu_not_recur));
				// Added by Dan for fix bug DTV00376269
				menu.showCopy(false);
				menu.showPaste(false);
			}
			if(sourceType == MultiFilesManager.SOURCE_DLNA || sourceType == MultiFilesManager.SOURCE_SMB){
				menu.showCopy(false);
				menu.showPaste(false);
				menu.showDelete(false);
			}
			break;
		case DIALOG_LOGIN:
			if (args != null) {
				String input = args.getString("Input");
				if (input != null) {
					((LoginDialog) dialog).setInput(input);
				}

				boolean init = args.getBoolean("Init");
				if (init) {
					((LoginDialog) dialog).init();
				}
			}
			break;
		case DIALOG_KEYBOARD:
			if (args != null) {
				String prefill = args.getString("Prefill");
				boolean isPassword = args.getBoolean("IsPassword");
				((KeyboardDialog) dialog).setPrefill(prefill);
				((KeyboardDialog) dialog).isPassword(isPassword);
			} else {
				((KeyboardDialog) dialog).setPrefill("");
				((KeyboardDialog) dialog).isPassword(false);
			}
			break;
		case DIALOG_PROGRESS:
			if (args != null) {
				final ProgressDialog progress = (ProgressDialog) dialog;
				int max = args.getInt("Max");
				if (max > 0) {
					progress.setMax(max);
				}

				String message = args.getString("Message");
				if (message != null) {
					progress.setMessage(message);
				}
			}

			break;
		case DIALOG_CONFIRM_REPLACE:
			if (args != null) {
				ConfirmReplaceDialog confirmReplace = (ConfirmReplaceDialog) dialog;
				String message = args.getString("Message");
				if (message != null) {
					confirmReplace.setMessage(message);
				}
			}
			break;
		case DIALOG_EMPTY_TIP: {
			if (mFlag) {
				((TipsDialog) dialog).setText(getString(R.string.mmp_nodevice));
			} else {
				((TipsDialog) dialog)
						.setText(getString(R.string.mmp_emptyfile));

			}
			mFlag = false;
			break;
		}
		default:
			break;
		}
	}

	private void cancelRecursive(){
		isCancel = true;
		try {
			dismissDialog(DIALOG_REU_TIP);
		} catch (IllegalArgumentException e) {
//				 e.printStackTrace();
			MtkLog.d(TAG,"dismissDialog DIALOG_REU_TIP exception~~");
		}
		if (null != mRecursThread) {
			try {
				MtkLog.i(TAG, "thread isAlive:"
						+ mRecursThread.isAlive());
				if (mRecursThread.isAlive()) {
					mRecursThread.interrupt();
				}
			} catch (Exception e) {
				MtkLog.d(TAG, e.getMessage());
			}
		}



	}

	private void controlState(String content) {
		dismissDialog(DIALOG_MENU);

		if (mResources.getString(R.string.mmp_menu_name).equals(content)) {
			setListSortType(FilesManager.SORT_BY_NAME);
			openDir(getListCurrentPath());
		} else if (mResources.getString(R.string.mmp_menu_date).equals(content)) {
			setListSortType(FilesManager.SORT_BY_DATE);
			openDir(getListCurrentPath());
		} else if (mResources.getString(R.string.mmp_menu_album)
				.equals(content)) {
			setListSortType(FilesManager.SORT_BY_ALBUM);
			openDir(getListCurrentPath());
		} else if (mResources.getString(R.string.mmp_menu_artist).equals(
				content)) {
			setListSortType(FilesManager.SORT_BY_ARTIST);
			openDir(getListCurrentPath());
		} else if (mResources.getString(R.string.mmp_menu_genre)
				.equals(content)) {
			setListSortType(FilesManager.SORT_BY_GENRE);
			openDir(getListCurrentPath());
		} else if (mResources.getString(R.string.mmp_menu_type).equals(content)) {
			setListSortType(FilesManager.SORT_BY_TYPE);
			openDir(getListCurrentPath());
		}

		else if (mResources.getString(R.string.mmp_menu_small).equals(content)) {
			setThumbnailSize(MultiMediaConstant.SMALL);
		} else if (mResources.getString(R.string.mmp_menu_medium).equals(
				content)) {
			MtkLog.d(TAG, "Medium!!");
			setThumbnailSize(MultiMediaConstant.MEDIUM);
		} else if (mResources.getString(R.string.mmp_menu_large)
				.equals(content)) {
			MtkLog.d(TAG, "Large!!");
			setThumbnailSize(MultiMediaConstant.LARGE);
		} else if (content.equals(mResources
				.getString(R.string.mmp_menu_phototext))) {
			setListContentType(FilesManager.CONTENT_PHOTO);
			openDir(getListCurrentPath());
		} else if (content
				.equals(mResources.getString(R.string.mmp_menu_music))) {
			setListContentType(FilesManager.CONTENT_AUDIO);
			openDir(getListCurrentPath());
		} else if (content
				.equals(mResources.getString(R.string.mmp_menu_video))) {
			setListContentType(FilesManager.CONTENT_VIDEO);
			LogicManager.getInstance(this).finishAudioService();
			stopMusicView();
			openDir(getListCurrentPath());
		} else if (content.equals(mResources.getString(R.string.mmp_menu_text))) {
			setListContentType(FilesManager.CONTENT_TEXT);
			openDir(getListCurrentPath());
		} else if (content
				.equals(mResources.getString(R.string.mmp_menu_recur))) {
			recursive();

		} else if (content.equals(mResources
				.getString(R.string.mmp_menu_not_recur))) {
			cancelRecursive();
			mMode = MODE_NORMAL;
			openDir(getListCurrentPath());
		} else if (mResources.getString(R.string.mmp_menu_copy).equals(content)) {
			copy();
		} else if (mResources.getString(R.string.mmp_menu_paste)
				.equals(content)) {
			if (mCopyedFiles.size() > 0) {
				paste();
			}
		} else if (mResources.getString(R.string.mmp_menu_delete).equals(
				content)) {
			delete();
		} else if (mResources.getString(R.string.mmp_menu_onephotomode).equals(
				content)) {
			framePhoto(1);

		} else if (content.equals(MultiFilesManager.getInstance(
				MtkFilesBaseListActivity.this).getCurDevName())) {
			framePhoto(2);
		}
		// TODO mount
		else if (mResources.getString(R.string.mmp_menu_mount_iso).equals(
				content)) {
			DevManager.getInstance().mountISOFile(
					getListSelectedItem().getAbsolutePath());
			// Log.v("mout", getListSelectedItem().getAbsolutePath());
		} else if (mResources.getString(R.string.mmp_menu_unmount_iso).equals(
				content)) {
			DevManager.getInstance().umoutISOFile(
					getListSelectedItem().getAbsolutePath());
		} else if (mResources.getString(R.string.mmp_menu_3d_photo).equals(
				content)) {
			setListContentType(FilesManager.CONTENT_THRDPHOTO);
			openDir(getListCurrentPath());
		} else if (mResources.getString(R.string.mmp_menu_3d_browse).equals(
				content)) {
			//For 3D Browse start
			stopVideoListMode();
			setListContentType(getListContentType());
			// openDir(getListCurrentPath());
			Intent intent = new Intent(this, MtkFileFlimBrowseActivity.class);
			intent.putExtra(MtkFileFlimBrowseActivity.MEDIA_PATH, getListCurrentPath());
			intent.putExtra(MtkFileFlimBrowseActivity.RECURSER_MODE, mMode);
			MtkLog.d(TAG, "chang to 3D mode =" + mMode);
			if (this instanceof MtkFilesGridActivity) {
				intent.putExtra(INTENT_NAME_SELECTION, getListSelectedItemPosition());
				intent.putExtra(MtkFileFlimBrowseActivity.CURRENT_BROWSE_STATUS, 
						MtkFileFlimBrowseActivity.BrowseStatus.FROM_GRID_STATUS);				
			}else{
				if (mMode == MODE_RECURSIVE) {
					intent.putExtra(INTENT_NAME_SELECTION, getListSelectedItemPosition());
				} else {
					intent.putExtra(INTENT_NAME_SELECTION, getListSelectedItemPosition() /*+ 1*/);
				}
				intent.putExtra(MtkFileFlimBrowseActivity.CURRENT_BROWSE_STATUS, 
						MtkFileFlimBrowseActivity.BrowseStatus.FROM_LIST_STATUS);
			}
			startActivity(intent);
			finish();
			//For 3D Browse end
		}
	}

	protected void stopVideoListMode() {

	}
	private void recursive() {
		MtkLog.d(TAG,"recursive~~~");
		showDialog(DIALOG_REU_TIP);

		try {
			dismissDialog(DIALOG_EMPTY_TIP);
		} catch (IllegalArgumentException e) {
//		    e.printStackTrace();
			MtkLog.d(TAG,"dismissDialog DIALOG_EMPTY_TIP exception~~");
		}
		mRecursThread = new Thread() {
			public void run() {
				isCancel = false;
				mRecursiveFiles = ((MultiFilesManager) mFilesManager)
						.listRecursiveFiles(getListContentType());
				if (isCancel) {
					mRecursiveFiles.clear();
				} else {
					mHandler.sendEmptyMessage(MSG_RECURSIVE_PARSER);
				}
			}
		};
		mRecursThread.start();
	}

	private void framePhoto(int model) {
		MtkLog.d(TAG,"framePhoto  model = "+model);
		if (model == 1) {
			mPlayMode = 1;

			if (mPopView != null && mPopView.isShowing()) {
				vMusicView.removeMessage();
				mPopView.dismiss();
			}
			Intent intent = new Intent();
			Bundle bundle = new Bundle();
			if(Util.PHOTO_4K2K_ON){
				intent.setClass(this, Photo4K2KPlayActivity.class);
			}else{
				intent.setClass(this, PhotoPlayActivity.class);
			}
			bundle.putInt("PlayMode", mPlayMode);
			intent.putExtras(bundle);
			startActivity(intent);
			//finish();
		} else if (model == 2) {
			mPlayMode = 2;
			showDialog(DIALOG_REU_TIP);
			new Thread() {
				public void run() {
					mRecursiveFiles = ((MultiFilesManager) mFilesManager)
							.listRecursiveFiles(FilesManager.CONTENT_PHOTO,
									MultiFilesManager.SOURCE_LOCAL, false);
					mHandler.sendEmptyMessage(MSG_PHOTO_FRAME);
				}
			}.start();
		}

	}

	private void setThumbnailSize(int newSize) {
		if (mThumbnailSize != newSize) {
			mPageSize = 0;
			mThumbnailSize = newSize;
			mAdapter.updateThumbnail();
			// openDir(getListCurrentPath(), getListSelectedItemPosition());
			TVContent.getInstance(this).getStorage().set(MultiMediaConstant.THUMBNAIL_SIZE,
					newSize+"");
		}
	}

	private void copy() {
		// Modified by Dan for fix bug DTV00379203
		FileAdapter file = getListSelectedItem();
		if(null != file){
		String copyedFile = file.getAbsolutePath();
		if (mCopyedFiles.size() > 0) {
			String oneCopyedFile = mCopyedFiles.get(0);
			if (!mFilesManager.isInSameFolder(oneCopyedFile, copyedFile)) {
				mSelectedFiles.clear();
				mCopyedFiles.clear();
			}
		}

		if (mSelectedFiles.size() > 0) {
			for (String path : mSelectedFiles) {
				if (!mCopyedFiles.contains(path)) {
					mCopyedFiles.add(path);
				}
			}
			mSelectedFiles.clear();
		}

		if (!mCopyedFiles.contains(copyedFile)) {
			mCopyedFiles.add(copyedFile);
		}

		cancelLoadFiles();
		// vList.setAdapter(mAdapter);
		mAdapter.notifyDataSetChanged();		
		}
	}

	private void delete() {
		showDialog(DIALOG_CONFIRM_DELETE);
		if (vConfirmDeleteDialog != null) {
			vConfirmDeleteDialog
					.setOnConfirmDeleteListener(new OnConfirmDeleteListener() {
						public void onPositivePressed() {
							new Thread(new Runnable() {
								public void run() {
									deleteSelectedFiles();
									runOnUiThread(new Runnable() {
										public void run() {
											// Modified by Dan for fix bug DTV00379203
											openDir(getListCurrentPath());
										}
									});
									dismissDialog(DIALOG_CONFIRM_DELETE);
								}
							}).start();
						}

						public void onNegativePressed() {
							dismissDialog(DIALOG_CONFIRM_DELETE);
						}
					});
		}
	}

	private void deleteSelectedFiles() {
		// Modified by Dan for fix bug DTV00379203
		FileAdapter file = getListSelectedItem();
		if(null != file){
		String selectedFile = file.getAbsolutePath();
		if (!mSelectedFiles.contains(selectedFile)) {
			mSelectedFiles.add(selectedFile);
		}

		for (String deleteFile : mSelectedFiles) {
			if ((getListContentType() == MultiMediaConstant.AUDIO)) {
				LogicManager logicManager = LogicManager.getInstance(this);
				String path = logicManager.getCurrentPath(Const.FILTER_AUDIO);
				MtkLog.i(TAG, "deleteSelectedFile  file.getPath():"
						+ deleteFile + "\t logicManager.getCurrentPath:" + path);
				if (null != path && path.equals(deleteFile)) {
					if (null != logicManager.getAudioPlaybackService()) {
						logicManager.finishAudioService();
						stopMusicView();
					}
				}
			}
			new MtkFile(deleteFile).delete();

			cancelCopyIfDeleted(deleteFile);
		}

		mSelectedFiles.clear();
	 }
	}

	private void cancelCopyIfDeleted(String file) {
		if (mCopyedFiles.size() > 0 && mCopyedFiles.contains(file)) {
			mCopyedFiles.remove(file);
		}
	}

	private void paste() {
		long totalSize = 0;
		for (String path : mCopyedFiles) {
			MtkFile file = new MtkFile(path);
			totalSize += file.length();
		}

		final MtkFile dst = new MtkFile(getListCurrentPath());
		Bundle data = new Bundle();
		int max = (int) (totalSize / 2048);
		data.putInt("Max", max);
		showDialog(DIALOG_PROGRESS, data);

		OnUsbCopyProgressListener listener = new OnUsbCopyProgressListener() {
			public void onSetProgress(long len) {
				if (vProgressDialog != null) {
					long size = mPasteSizeCount + len;
					int progress = (int) (size / 2048);
					MtkLog.d(TAG,"OnUsbCopyProgressListener progress ="+progress);
					vProgressDialog.setProgress(progress);
				}
			}
		};

		final UsbFileOperater operator = UsbFileOperater.getInstance();
		operator.setOnUsbCopyProgressListener(listener);

		if (vProgressDialog != null) {
			vProgressDialog.setOnCancelListener(new OnCancelListener() {
				public void onCancel(DialogInterface dialog) {
					MtkLog.d(TAG, "Paste Cancel!!");
					operator.setCopyAbort(true);
					mPasteCanceled = true;
				}
			});
		}

		new Thread(new Runnable() {
			public void run() {
				try {
					for (String path : mCopyedFiles) {
						MtkFile file = new MtkFile(path);
						if (vProgressDialog != null) {
							String msg = mResources.getString(
									R.string.mmp_message_copy, file.getName(),
									file.getParent(), getListCurrentPath());
							vProgressDialog.setMessage(msg);
						}
						setListItems(mFilesManager.getCurrentFiles());
						List<FileAdapter> items = mLoadFiles;
						for (final FileAdapter item : items) {
							if (item.getName().equals(new File(path).getName())) {
								MtkLog.d(TAG, "Replace !!");
								if (!mYesToAll) {
									runOnUiThread(new Runnable() {
										public void run() {
											dismissDialog(DIALOG_PROGRESS);

											Bundle data = new Bundle();
											String msg = mResources
													.getString(
															R.string.mmp_message_confirm_replace,
															item.getName());
											data.putString("Message", msg);
											showDialog(DIALOG_CONFIRM_REPLACE,
													data);
										}
									});

									synchronized (mCopyedFiles) {
										mCopyedFiles.wait();
									}
								}

								if (mPasteCanceled) {
									break;
								}

								if (mYesToAll || mYes) {
									item.delete();
									mYes = false;
								}
							}
						}

						if (mPasteCanceled) {
							break;
						}
						operator.copyFile2Dir(file, dst);
						mPasteSizeCount += file.length();
					}
				} catch (Exception e) {
					MtkLog.d(TAG, "Paste File Error!!" + e.toString());
				}

				dismissDialog(DIALOG_PROGRESS);
				mCopyedFiles.clear();
				mPasteCanceled = false;
				mYesToAll = false;
				mPasteSizeCount = 0;

				openDir(getListCurrentPath());
			}
		}).start();
	}

	private void prepareResources() {
		mContentTypeNames = mResources
				.getStringArray(R.array.mmp_menu_item_names);
		TypedArray iocns = mResources
				.obtainTypedArray(R.array.mmp_menu_item_resources);

		mContentTypeIcons = new Drawable[mContentTypeNames.length];
		for (int i = 0; i < mContentTypeIcons.length; i++) {
			mContentTypeIcons[i] = iocns.getDrawable(i);
		}
	}

	protected void findViews() {
	}

	protected void bindData(Intent intent) {
		getIntentData(intent);
		setupHeader();
		refreshListView(null);
		int sourceType = MultiFilesManager.getInstance(this)
				.getCurrentSourceType();
		if (mMode == MODE_RECURSIVE && !"/".equals(getListCurrentPath())
				&& sourceType == MultiFilesManager.SOURCE_LOCAL) {
			cancelLoadFiles();
			recursive();
			return;
		}
		loadFiles(getListCurrentPath());
	}

	/**
	 * {@inheritDoc}
	 */
	protected void onPreLoadFiles() {
		showDialog(DIALOG_LOADING);
	}

	/**
	 * {@inheritDoc}
	 */
	protected void onLoadedFiles() {
		try{
			dismissDialog(DIALOG_LOADING);
		}catch(Exception e){
				
		}
	}

	/**
	 * {@inheritDoc}
	 */
	protected void onListEmpty() {
		super.onListEmpty();
		if ((vLoginDialog == null || !vLoginDialog.isShowing())
				&& (vProgressDialog == null || !vProgressDialog.isShowing())
				&&(vTipDialog == null || !vTipDialog.isShowing())) {
			try {
				showDialog(DIALOG_EMPTY_TIP);
			} catch (Exception e) {
				MtkLog.d(TAG,"dismissDialog DIALOG_EMPTY_TIP exception~~");
			}			
		}
		//stopMusicView();
	}

	/**
	 * {@inheritDoc}
	 */
	protected void onListNotEmpty() {
		super.onListNotEmpty();
		try {
			dismissDialog(DIALOG_EMPTY_TIP);
		} catch (IllegalArgumentException e) {
			MtkLog.w(TAG, "No Such Dialog!!");
		}
	}

	/**
	 * {@inheritDoc}
	 */
	
	protected FilesManager<FileAdapter> setupFilesManager() {
		SaveValue pref = SaveValue.getInstance(this);
		boolean smbAvailable = pref.readValue(MenuConfigManager.MY_NET_PLACE) == 0 ? false
				: true;
		MtkLog.d(TAG, "Samba Available : " + smbAvailable);
		boolean dlnaAvailable = pref.readValue(MenuConfigManager.DLNA) == 0 ? false
				: true;
		MtkLog.d(TAG, "Dlna Available : " + dlnaAvailable);

		MultiFilesManager mManager = MultiFilesManager.getInstance(this,
				smbAvailable, dlnaAvailable);
		//manager.initDevices();
		//manager.initFilesManager(this);
		mManager.deleteObservers();
		mManager.addObserver(this);
		//manager.initDevicesManager();

		return mManager;
	}

	protected void getIntentData(Intent intent) {
		if (intent != null) {
			String path = intent.getStringExtra(INTENT_NAME_PATH);
			setListCurrentPath(path);

			int contentType = intent.getIntExtra(MultiMediaConstant.MEDIAKEY,
					-1);
			if (contentType >= 0) {
				setListContentType(contentType);
				if (contentType == MultiMediaConstant.VIDEO) {
					LogicManager.getInstance(this).finishAudioService();
				}
			}

			int currentSelection = intent
					.getIntExtra(INTENT_NAME_SELECTION, -1);
			mFilesManager.setPositionInParent(currentSelection);
			getListView().requestFocusFromTouch();
			getListView().setSelection(currentSelection);
			ArrayList<String> copyedFiles = intent.getStringArrayListExtra(INTENT_NAME_COPYED_FILES);
			if (copyedFiles != null && copyedFiles.size() > 0) {
				for(String copyedPath : copyedFiles){
					mCopyedFiles.add(copyedPath);
				}
			}
		}
	}

	protected void setupHeader() {

	}

	protected void onListItemSelected(AbsListView l, View v, int position,
			long id) {
	}

	protected void onListItemClick(AbsListView l, View v, int position, long id) {
		FileAdapter file = getListItem(position);
		getListView().requestFocusFromTouch();
		//getListView().setSelection(position);
		mAdapter.cancel();
		if (null != file && file.isDirectory()) {
			// open directory
			mFilesManager.pushOpenedHistory(position);
			currentPosition = position;
			isFirstRequest = true;
			openDir(file.getAbsolutePath(), 0);

		} else if (null != file && file.isIsoFile()) {
			// TODO ISO mount
		} else if(null != file){
			// play multi media file
			//int pos = 0;
			/*pos = getListSelectedItemPosition();
			if (pos < 0) {
				return;
			}*/
			int pos = position;
			if (this instanceof MtkFilesListActivity) {			
				pos -= 1;
			} else{
				try{
					file.stopThumbnail();
				}catch(Exception ex){
					ex.printStackTrace();
				}



			}
			if (pos < 0){
				return;
			}// support mouse click by lei modif.
			
			((MultiFilesManager) mFilesManager).getPlayList(pos);
		//	int contentType = getListContentType();
		//	if (contentType == FilesManager.CONTENT_PHOTO || 
		//			contentType == FilesManager.CONTENT_THRDPHOTO) {
				//file.stopDecode();
				
//				file.stopThumbnail();
		//	} 
			playFile(file.getAbsolutePath());
		}
	}

	/**
	 * {@inheritDoc}
	 */
	protected void openDir(String path, int selection) {
		super.openDir(path, selection);
		if (path != null) {
			Intent intent = new Intent(this, this.getClass());
			intent.putExtra(INTENT_NAME_PATH, path);
			intent.putExtra(INTENT_NAME_SELECTION, selection);
			intent.putExtra(MultiMediaConstant.MEDIAKEY, getListContentType());
			startActivity(intent);
		} else {
			onReachRoot(selection);
		}
	};

	/**
	 * Reach root
	 * 
	 * @param selection
	 */
	protected void onReachRoot(int selection) {		
		mFilesManager.deleteObserver(this);
		mFilesManager.destroy();
	}

	protected void destroyManger(){
		mFilesManager.destroyManager();
	}
	/**
	 * {@inheritDoc}
	 */
	protected void playFile(String path) {
		super.playFile(path);

		Intent intent = new Intent();
		Bundle bundle = new Bundle();

		int contentType = getListContentType();
		if (contentType == FilesManager.CONTENT_PHOTO) {
			if(Util.PHOTO_4K2K_ON){
				intent.setClass(this, Photo4K2KPlayActivity.class);
			}else{
				intent.setClass(this, PhotoPlayActivity.class);
			}
			bundle.putInt("PlayMode", mPlayMode);
		} else if (contentType == FilesManager.CONTENT_THRDPHOTO) {
			intent.setClass(this, Photo3DPlayActivity.class);
		} else if (contentType == FilesManager.CONTENT_AUDIO) {
			intent.setClass(this, MusicPlayActivity.class);
			intent.putExtras(bundle);
			startActivityForResult(intent, 0);
			return;

		} else if (contentType == FilesManager.CONTENT_VIDEO) {
			intent.setClass(this, VideoPlayActivity.class);
		} else if (contentType == FilesManager.CONTENT_TEXT) {
			intent.setClass(this, TextPlayActivity.class);
		}

		intent.putExtras(bundle);
		startActivity(intent);
	}

	/**
	 * {@inheritDoc}
	 */
	protected void refreshListView(List<FileAdapter> files) {
		super.refreshListView(files);

		mCurrentPage = 1;
		mPageCount = 1;

		setListSelection(mFilesManager.getPositionInParent());
	}

	/**
	 * Observer change,
	 */
	public void update(Observable observable, Object data) {
		final int request = (Integer) data;
		MtkLog.d(TAG,"update ~~ "+ " request ="+request);
		runOnUiThread(new Runnable() {
			public void run() {
				switch (request) {
				case FilesManager.REQUEST_REFRESH: {
					
					if ((mFilesManager instanceof MultiFilesManager) 
						&&(((MultiFilesManager) mFilesManager)
								.getCurrentSourceType() == MultiFilesManager.SOURCE_DLNA) 
						&& (mFilesManager.getFilesCount() == 0) ){
								onListEmpty();
						
					}else{

						try {
							dismissDialog(DIALOG_EMPTY_TIP);
						} catch (IllegalArgumentException e) {
							MtkLog.w(TAG, "No Such Dialog!!");
						}
					}
					MtkLog.d(TAG,"update ~~ this ="+MtkFilesBaseListActivity.this );
					refreshListView();
					break;
				}

				case FilesManager.REQUEST_SUB_DIRECTORY: {
					MultiFilesManager multiFileManager = MultiFilesManager
							.getInstance(MtkFilesBaseListActivity.this);
					String path = multiFileManager.getDeviceMountPoint();
					MtkLog.d("USB", path);
					MtkLog.d("USB", mFilesManager.getCurrentPath());
					if (!mFilesManager.getCurrentPath().equals(path)) {
						mFilesManager.pushOpenedHistory(0);
						openDir(path, 0);
					}

					break;
				}
				case FilesManager.REQUEST_DEVICE_LEFT: {
					try {
						dismissDialog(DIALOG_EMPTY_TIP);
					} catch (IllegalArgumentException e) {
						MtkLog.w(TAG, "No Such Dialog!!");
					}
					Intent intent = new Intent();
					intent.putExtra(INTENT_NAME_PATH, "/");
					intent.putExtra(MultiMediaConstant.MEDIAKEY,
							mFilesManager.getContentType());
					bindData(intent);

					break;
				}
				case FilesManager.REQUEST_MMP_MENU: {
					openDir(null, 0);
					break;
				}
				case FilesManager.REQUEST_LOGIN:
					Looper.myQueue().addIdleHandler(new IdleHandler() {
						public boolean queueIdle() {
							try {
								dismissDialog(DIALOG_EMPTY_TIP);
							} catch (IllegalArgumentException e) {
								MtkLog.w(TAG, "No Such Dialog!!");
							}
							if(!isFirstRequest){
								mFilesManager.pushOpenedHistory(currentPosition);
							}
							if(isFirstRequest){
								isFirstRequest = false;
							}							
							Bundle data = new Bundle();
							data.putBoolean("Init", true);
							showDialog(DIALOG_LOGIN, data);
							return false;
						}
					});

					break;

				case FilesManager.REQUEST_BACK_TO_ROOT: {
					mFlag = true;
					openDir(getListRootPath());
					break;
				}
				default:
					break;
				}
			}
		});
	}
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if(requestCode==0){
			   switch (resultCode) {
			   case 100:
			    PlaybackService mAudioPlayback =LogicManager.getInstance(this).getAudioPlaybackService();
				if(null != mAudioPlayback){
			    	int playStatus = mAudioPlayback.getPlayStatus();
			    	if (playStatus == AudioConst.PLAY_STATUS_FF 
			     	|| playStatus == AudioConst.PLAY_STATUS_FR
			     	|| playStatus == AudioConst.PLAY_STATUS_SF){
			     	mAudioPlayback.play();
			    }
				}
			    break;
			 
			   default:
			    break;
			   }
			  }

		super.onActivityResult(requestCode, resultCode, data);
	}
	class MtkNetWorkRecevier extends BroadcastReceiver{

		@Override
		public void onReceive(Context context, Intent intent) {
			ConnectivityManager connectivityManager = (ConnectivityManager)
					context.getSystemService(Context.CONNECTIVITY_SERVICE);
			if(null != connectivityManager){
				NetworkInfo [] netWorkInfos = connectivityManager.getAllNetworkInfo();
				for(int i = 0; i<netWorkInfos.length;i++){
					State state = netWorkInfos[i].getState();
					if(NetworkInfo.State.CONNECTED==state){
						MtkLog.e(TAG, "network ok.....");
						return  ;
					}
				}
			}
			
			MtkLog.e(TAG, "network error.....");
			MultiFilesManager.getInstance(MtkFilesBaseListActivity.this).netWorkError();
			
		}
		
		
	}
}
