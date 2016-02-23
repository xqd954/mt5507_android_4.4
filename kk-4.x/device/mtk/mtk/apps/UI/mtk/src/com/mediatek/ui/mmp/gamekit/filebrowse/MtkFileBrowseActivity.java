package com.mediatek.ui.mmp.gamekit.filebrowse;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Observable;
import java.util.Observer;

import com.mediatek.gamekit.GKView;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.SaveValue;
import com.mediatek.ui.mmp.gamekit.content.MutilMediaConst;
import com.mediatek.ui.mmp.gamekit.filebrowse.MtkFileFlimBrowseActivity.BrowseStatus;
import com.mediatek.ui.mmp.gamekit.media.MtkGLES2TextureInfo;
import com.mediatek.ui.mmp.gamekit.media.MtkThumbnailManager;
import com.mediatek.ui.mmp.gamekit.util.AsyncLoader;
import com.mediatek.ui.mmp.gamekit.util.BrowseList;
import com.mediatek.ui.mmp.gamekit.util.TextureMananger;
import com.mediatek.ui.mmp.model.FileAdapter;
import com.mediatek.ui.mmp.model.FilesManager;
import com.mediatek.ui.mmp.model.LocalFileAdapter;
import com.mediatek.ui.mmp.model.MultiFilesManager;
import com.mediatek.ui.mmp.multimedia.MtkFilesBaseListActivity;
import com.mediatek.ui.mmp.util.BitmapCache;
import com.mediatek.ui.util.DestroyApp;
import com.mediatek.ui.util.MtkLog;

import android.app.Activity;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.os.AsyncTask;
import android.os.Bundle;
import com.mediatek.ui.util.ScreenConstant;




public class MtkFileBrowseActivity extends Activity  implements
Observer {

	protected MultiFilesManager mManager;
	private String TAG = "MtkFileBrowseActivity";
	protected BitmapDrawable mFolder;
	protected BitmapDrawable mDefault;
	protected Resources mResources;
	protected String mFromPath;
	protected BrowseStatus mFromStatus = BrowseStatus.NORMAL_STATUS;
	protected BrowseStatus mBackStatus = BrowseStatus.NORMAL_STATUS;
	protected int mFromType;
	protected int mFromPostion;
	protected int mRecuseMode;
	protected BitmapDrawable mPhotoIcon;
	protected BitmapDrawable mVideoIcon;
	protected BitmapDrawable mAudioIcon;
	protected BitmapDrawable mTextIcon;
	protected BrowseList mBrowseList;
	protected TextureMananger mTexutreManager;
	protected GKView mEngineView;
	protected boolean mIsSupportNetMode = false;
	protected MtkThumbnailManager mThumbnailManager = MtkThumbnailManager.getInstance();

	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		DestroyApp des = (DestroyApp) this.getApplication();
		des.add(this);
	}

	@Override
	protected void onResume() {
		super.onResume();

	}

	protected void initDefaultIcon() {
		mResources = getResources();
		mFolder = (BitmapDrawable) mResources
				.getDrawable(R.drawable.mmp_thumbnail_icon_folder_big);
		mDefault = (BitmapDrawable) mResources
				.getDrawable(R.drawable.defalut_bg);
		mVideoIcon = (BitmapDrawable) mResources
				.getDrawable(R.drawable.mmp_thumbnail_icon_video_big);
		mPhotoIcon = (BitmapDrawable) mResources
				.getDrawable(R.drawable.mmp_thumbnail_icon_photo_big);
		mAudioIcon = (BitmapDrawable) mResources
				.getDrawable(R.drawable.mmp_thumbnail_icon_audio_big);
		mTextIcon = (BitmapDrawable) mResources
				.getDrawable(R.drawable.mmp_thumbnail_icon_text_big);
	}

	protected void initView() {
		initDefaultIcon();
		mFolder = (BitmapDrawable) mResources
				.getDrawable(R.drawable.mmp_thumbnail_icon_folder_big);
		mDefault = (BitmapDrawable) mResources
				.getDrawable(R.drawable.defalut_bg);
		
		mCurPath = null;
	}

	protected void initFilesManager(){
		SaveValue pref = SaveValue.getInstance(this);
		boolean smbAvailable = pref.readValue(MenuConfigManager.MY_NET_PLACE) == 0 ? false
				: true;
		MtkLog.d(TAG, "Samba Available : " + smbAvailable);
		boolean dlnaAvailable = pref.readValue(MenuConfigManager.DLNA) == 0 ? false
				: true;
		MtkLog.d(TAG, "Dlna Available : " + dlnaAvailable);
		mManager = MultiFilesManager.getInstance(getApplicationContext(),
				smbAvailable, dlnaAvailable);
		
		if (mIsSupportNetMode){
			mManager.addObserver(this);
		}
		
		MtkLog.d(TAG, "mRecuseMode = " + mRecuseMode);
		if (mRecuseMode == MtkFilesBaseListActivity.MODE_RECURSIVE) {
			recursive();			
		} else {
			if (mFromPath == null) {
				executeLoadFiles(mManager.getRootPath());
			} else {
				executeLoadFiles(mFromPath);
			}
		}
	}
	
//	private Thread mRecursThread;
	private List<FileAdapter> mRecursiveFiles;
	protected void recursive() {

		/*mRecursThread = new Thread() {
			public void run() {
				mRecursiveFiles = mManager.getCurrentFiles();
				//mRecursiveFiles = mManager.listRecursiveFiles(mManager.getContentType());
				refreshBoardView(mRecursiveFiles);			
			}
		};
		
		mRecursThread.start();*/
		
		mRecursiveFiles = mManager.getCurrentFiles();
		refreshBoardView(mRecursiveFiles);	
	}
	
	protected void reloadView(int idx) {

		int browse_size = mBrowseList.getNum();
		int board_size = mTexutreManager.getSize();
		int bd_idx = 0;
		int tmp_idx = idx;
		int count = -1;
		if (browse_size <= 0) {
			return;
		}

		resetDefaultTexture();
		tmp_idx = idx - 1;
		for (bd_idx = board_size / 2 - 1; bd_idx >= 0 && tmp_idx >= 0; bd_idx--, tmp_idx--) {
			initBoardView(mBrowseList.getSpecial(tmp_idx), bd_idx);
		}
		initBoardView(mBrowseList.getSpecial(idx), board_size / 2);
		tmp_idx = idx + 1;
		for (bd_idx = board_size / 2 + 1; bd_idx < board_size
				&& tmp_idx < browse_size; bd_idx++, tmp_idx++) {
			initBoardView(mBrowseList.getSpecial(tmp_idx), bd_idx);
		}

		count = tmp_idx - 1;
		
		BrowseList.getList().setCurrentIndex(count);
		BrowseList.getList().setCurrentGetMode(BrowseList.NEXT);
	}
	
	private boolean FilterNetAdapater(FileAdapter f ){
		if (mIsSupportNetMode) {
			return true;
		}else {
			return (f instanceof LocalFileAdapter);
		}
	}
	
	protected void removePasteBitmapMsgs(){
		
	}
	
	private void refreshBoardView(List<FileAdapter> fileList) {
		if (fileList == null) return;
		ArrayList<FileAdapter> pathList = new ArrayList<FileAdapter>();
		MtkLog.d(TAG, "refreshBoardView.... :::" + mCurPath);
		int i = 0;
		for (FileAdapter f : fileList) {
			MtkLog.d(TAG, "refreshBoardView filelist file :" + i++ + ":" + f.getPath() );
			//only support local mode.
			if ( FilterNetAdapater(f))
				pathList.add(f);
		}
		mBrowseList.addFiles(pathList);
		// TextureMananger.getTextureManager(this).reset();
		//BitmapCache.createCache(true);
		AsyncLoader.getInstance().clearQueue();
		
		pathList = null;
		int idx = BrowseList.getList().getSpecailIndx(mCurPath);
		//from extra photo or music play mode .
		if (mFromStatus == BrowseStatus.FROM_PLAY_STATUS && 
			mFromType != MutilMediaConst.CONTENT_UNKNOW ){
			reloadContent();
			mFromType = MutilMediaConst.CONTENT_UNKNOW;
			mFromStatus = BrowseStatus.NORMAL_STATUS;
			return;
			//from exra list mode or grid mode
		} else if (mFromStatus == BrowseStatus.FROM_GRID_STATUS || 
				mFromStatus == BrowseStatus.FROM_LIST_STATUS){
			reloadView(mFromPostion);
			mBackStatus = mFromStatus;
			mFromStatus = BrowseStatus.NORMAL_STATUS;
			return;
			//normal enter folder or leave folder
		} else if (mFromStatus == BrowseStatus.NORMAL_STATUS) {
			if (idx >= 0) {
				reloadView(idx);
				return;
			} else {
				reloadView(0);
				return;
			}
		}
	}

	protected void reloadContent(){
		
	}
	
	protected void initBoardView(FileAdapter f, int index) {

	}

	protected void resetDefaultTexture() {
		int idx = 0;
		for (idx = 0; idx < mTexutreManager.getSize(); idx++) {
			setDefaultTexture(mTexutreManager.getId(idx));
		}
	}
	
	protected void setDefaultTexture(final int textureId){
				Bitmap bm = mDefault.getBitmap();
				String name = getString(textureId);
				MtkGLES2TextureInfo textureInfo = mTexutreManager
						.getTextureInfo(name);
				if (textureInfo != null) {
					textureInfo.setPath(null);
					textureInfo.setFileAdapter(null);
					if (!bm.equals(textureInfo.getBitmap()))
					textureInfo.uploadImage(mEngineView, bm);
				}
	}
	
	protected void luaStopVideo(){
		
	}

	private class LoadFilesTask extends
			AsyncTask<String, Void, List<FileAdapter>> {

		protected void onPreExecute() {

		}

		protected List<FileAdapter> doInBackground(String... params) {
			List<FileAdapter> files = loadListFiles(params[0]);

			return files;
		}

		protected void onPostExecute(List<FileAdapter> result) {

			if (result != null && result.size() > 0) {
				refreshBoardView(result);
			} else if (result.size() == 0){
				//clean last record.
				mBrowseList.cleanList();
				MtkLog.e(TAG, "error: result = 0");
			}
			
		}

	}

	protected void executeLoadFiles(String path) {
		MtkLog.i(TAG, "Start Load Files : " + path);
		new LoadFilesTask().execute(path);
	}

	protected List<FileAdapter> loadListFiles(String path) {
		List<FileAdapter> fileList = mManager.listAllFiles(path);
		MtkLog.d(TAG, "loadListFiles = " + path);
		if (fileList == null) {
			MtkLog.e(TAG, "error: loadListFiles return null");
		} else {
			MtkLog.d(TAG, "loadListFiles Files size = " + fileList.size());
		}
		return fileList;
	}

	protected void openDir(String path) {
		MtkLog.d(TAG, "open dir path = " + path);
		removePasteBitmapMsgs();
		mManager.setCurrentPath(path);
		executeLoadFiles(path);

	}

	protected void playFile(FileAdapter fAdapter) {

	}

	protected void exitAPP() {
		if (mIsSupportNetMode) {
			if (mManager != null) {
				mManager.deleteObserver(this);
			}
		}
	}

	private String mCurPath = null;

	public void onBackPressed() {
		//when recusive, directly exit.
		if (mRecuseMode == MtkFilesBaseListActivity.MODE_RECURSIVE){
			exitAPP();
			return;
		}
		if (mManager == null){
			return ;
		}
		
		String path = mManager.getBackPath();
		mCurPath = mManager.getCurrentPath();
		if ("/".equals(mCurPath)){
			path = null;
		}
		MtkLog.i(TAG, "onBackPressed current path :" + mManager.getCurrentPath());
		MtkLog.i(TAG, "onBackPressed:" + path);
		// TODO: mutil mount devices?
		if (null == path || path.equals("/") ){
			// super.onBackPressed();
			exitAPP();
		} else if (("/mnt".equals(path)) && ( mManager.getAllDevicesNum() > 1)){
			path = "/";
			luaStopVideo();
			resetDefaultTexture();
			openDir(path);
		}else if (("/mnt".equals(path)) && ( mManager.getAllDevicesNum() <= 1)){
			exitAPP();
		}else{
			luaStopVideo();
			resetDefaultTexture();
			openDir(path);
		}
	}

	@Override
	public void update(Observable observable, Object data) {
		// TODO Auto-generated method stub
		if (!mIsSupportNetMode) return;
		final int request = (Integer) data;
		runOnUiThread(new Runnable() {
			public void run() {
				// TODO Auto-generated method stub
				switch (request) {
				case FilesManager.REQUEST_REFRESH:{
					refreshBoardView(mManager.getCurrentFiles());
				}
					break;
				default:
					break;
				}
			}
		});
	}
}
