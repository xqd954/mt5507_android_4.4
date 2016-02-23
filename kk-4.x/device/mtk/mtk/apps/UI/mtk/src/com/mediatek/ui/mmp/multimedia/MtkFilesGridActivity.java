
package com.mediatek.ui.mmp.multimedia;

import java.lang.ref.SoftReference;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.MessageQueue.IdleHandler;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.Animation.AnimationListener;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.PopupWindow;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.mediatek.mmpcm.videoimpl.Thumbnail;
import com.mediatek.mmpcm.videoimpl.VideoConst;
import com.mediatek.netcm.dlna.FileSuffixConst;
import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.mmp.MeidaMainActivity;
import com.mediatek.ui.mmp.commonview.MediaGridView;
import com.mediatek.ui.mmp.commonview.MusicPlayInfoView;
import com.mediatek.ui.mmp.model.FileAdapter;
import com.mediatek.ui.mmp.model.FilesManager;
import com.mediatek.ui.mmp.model.MultiFilesManager;
import com.mediatek.ui.mmp.util.AsyncLoader;
import com.mediatek.ui.mmp.util.AsyncLoader.LoadWork;
import com.mediatek.ui.mmp.util.BitmapCache;
import com.mediatek.ui.mmp.util.LogicManager;
import com.mediatek.ui.mmp.util.MultiMediaConstant;
import com.mediatek.ui.mmp.util.Util;
import com.mediatek.ui.util.AnimationManager;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

public class MtkFilesGridActivity extends MtkFilesBaseListActivity {

	private static final String TAG = "MtkFilesGridActivity";

	private ImageView vLeftTopImg;

	private TextView vLeftTopTv;

	private TextView vLeftMidTv;

	private TextView vTopPath;

	private TextView vTopRight;

	private int mGridViewH = (int)(ScreenConstant.SCREEN_HEIGHT*11/13);

	private int mGridViewW = (int)(ScreenConstant.SCREEN_WIDTH*0.75);

	private int mVerticalSpacing = ScreenConstant.SCREEN_HEIGHT/36;

	private int mHorizontalSpacing = ScreenConstant.SCREEN_WIDTH/64;

	private int mItemWidth;

	private int mItemHeight;
	
	/* if turn list mode from grid mode, true, else false, 
	 * for clear info and thumbnail array not in onstop function,
	 * but in press bule key. avoid clear array item when add in list mode 
	 * Help me?*/
	private boolean mIsListMode=false;

	/* if turn page, true, and will add Blank item, else false. */
	private boolean mTurnPage = false;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		if(AnimationManager.getInstance().getIsAnimation()){
			AnimationManager.getInstance().startActivityEnterAnimation(this, findViewById(R.id.mmp_files_grid_layout), null);
		}

	}
	@Override
	protected void onResume() {
		super.onResume();

		if ((getListContentType() != MultiMediaConstant.VIDEO)
				&& LogicManager.getInstance(this).isAudioStarted()) {
			showMusicView();
		}
		mPlayMode = 0;
	}

	@Override
	protected void onRestart() {
		super.onRestart();
		setCurrentSelection();
		mAdapter.notifyDataSetChanged();
	}

	@Override
	protected void initMusicView() {
		View contentView = LayoutInflater.from(this).inflate(
				R.layout.mmp_musicbackplay, null);
//		int width = getWindowManager().getDefaultDisplay().getRawWidth();
//		int height = getWindowManager().getDefaultDisplay().getRawHeight();
		mPopView = new PopupWindow(contentView, ScreenConstant.SCREEN_WIDTH / 4 - (int)(ScreenConstant.SCREEN_WIDTH/64), ScreenConstant.SCREEN_HEIGHT / 2);
		vMusicView = new MusicPlayInfoView(this, contentView, 0,
				mPopView);
		
	}
	/**
	 * {@inheritDoc}
	 */
	protected void onNewIntent(Intent intent) {
		super.onNewIntent(intent);
		MtkLog.d(TAG, "onNewIntent call mAdapter.clearWork");
		mAdapter.clearWork();
	}
/*	private OnCompletionListener mCompletionListener = new OnCompletionListener() {
		public void onCompletion(CHMtkMediaPlayer arg0) {
			// TODO
			Log.e("JF", "grid oncompletion");
			//if (flag == 1) {
				vMusicView.removeMessage();
				mPopView.dismiss();
			//}
			//return false;
		}
	};*/

	@Override
	protected void showMusicView() {
		super.showMusicView();

		Looper.myQueue().addIdleHandler(new IdleHandler() {
			public boolean queueIdle() {
				mPopView.showAtLocation(vLeftMidTv, Gravity.LEFT, (int)(ScreenConstant.SCREEN_WIDTH/128), (int)(ScreenConstant.SCREEN_HEIGHT/9));
				vMusicView.init(MtkFilesGridActivity.this);
				return false;
			}
		});
	}

	@Override
	protected int setupContentView() {
		return R.layout.mmp_files_grid;
	}

	@Override
	protected FilesAdapter getAdapter() {
		return new MtkFilesGridAdapter(this);
	}

	protected void findViews() {
		vLeftTopImg = (ImageView) findViewById(R.id.multimedia_showinfo_img);
		vLeftMidTv = (TextView) findViewById(R.id.multimedia_showinfo_left);
		vLeftTopTv = (TextView) findViewById(R.id.multimedia_showinfo_tv);
		vTopPath = (TextView) findViewById(R.id.multimedia_tv_filepath);
		vTopRight = (TextView) findViewById(R.id.multimedia_tv_right);
	}

	@Override
	protected void setupHeader() {
		int contentType = getListContentType();
		if (contentType >= 4 || contentType < 0){
			contentType = 0;
		}
			vLeftTopImg.setImageDrawable(mContentTypeIcons[contentType]);
			vLeftTopTv.setText(mContentTypeNames[contentType]);

		vTopPath.setText(getListCurrentPath());
		vTopRight.setText("");
		vLeftMidTv.setText("");
	}

	public void onBackPressed() {
		if (!MeidaMainActivity.isValid(300)) {
			return;
		}
		LogicManager mLogicManager = LogicManager.getInstance(this);

		MultiFilesManager multiFileManager = MultiFilesManager
				.getInstance(this);
		String path = multiFileManager.getDeviceMountPoint();
		int sourceType = multiFileManager.getCurrentSourceType();
		
		String parentPath = getListParentPath();

		MtkLog.d(TAG, "onBackPressed ParentPath :" + parentPath +"cur path = "+getListCurrentPath());

		if ((null != parentPath && parentPath.equals("/mnt")) || MultiFilesManager.ROOT_PATH.equals(getListCurrentPath())) {
			onReachRoot(0);
			return;
		}

		
		if (multiFileManager.getAllDevicesNum() == MultiFilesManager.ONE_DEVICES) {
			if (getListCurrentPath().equals(path) ||  mMode == MODE_RECURSIVE) {
				onReachRoot(0);
				return;
			}
		}

		

		if (mMode == MODE_RECURSIVE) {
			openDir(MultiFilesManager.ROOT_PATH,0);
		} else {
			
			openDir(getListParentPath(), mFilesManager.popOpenedHistory());
			
		}
		if (null != mLogicManager) {
			mLogicManager.stopDecode();
		}
	}

	@Override
	protected void onReachRoot(int selection) {
		if (selection == 0){
			
			mAdapter.cancel();
			Thumbnail thumbnail = Thumbnail.getInstance();
			if (thumbnail.isLoadThumanil()) {
				thumbnail.stopThumbnail();
			}
		}
			//Don't remove listener by jianfang.
		   //super.onReachRoot(selection);
		  //	destroyManger();
		
		exit();
	}

	@Override
	protected void playFile(String path) {
		if (mPopView != null && mPopView.isShowing()) {
			vMusicView.removeMessage();
			mPopView.dismiss();
		}

		super.playFile(path);
	}

	protected void moveTo(int selection) {
		// mAdapter.cancel();
		// setListAdapter(mAdapter);
		setListSelection(selection);
		mAdapter.notifyDataSetChanged();
		MtkLog.d(TAG, "Selection : " + selection);
		MtkLog.d(TAG, "CurrentPage : " + mCurrentPage);
	}

	@Override
	public boolean dispatchKeyEvent(KeyEvent event) {
		if (event.getAction() == KeyEvent.ACTION_DOWN) {
			int keyCode = event.getKeyCode();
			int position = getListSelectedItemPosition();
			int count = getListItemsCount();
			int rowSize = getNumberColum();
			switch (keyCode) {
			case KeyMap.KEYCODE_DPAD_UP: {
				mTurnPage = false;
				if (position >= rowSize) {
					position -= rowSize;
					mCurrentPage = computeCurrentPage(position);
				} else {
					position = 0;
					mCurrentPage = computeCurrentPage(position);
					setListSelection(position);
					return true;
				}
				break;
			}
			case KeyMap.KEYCODE_DPAD_DOWN: {
				mTurnPage = false;
				if ((position + rowSize) < count) {
					position += rowSize;
					mCurrentPage = computeCurrentPage(position);
				} else {
					position = count - 1;
					mCurrentPage = computeCurrentPage(position);
					setListSelection(position);
					return true;
				}
				break;
			}
			case KeyMap.KEYCODE_DPAD_LEFT: {
				if (position >= 1) {
					position -= 1;
					mCurrentPage = computeCurrentPage(position);
					setListSelection(position);
					return true;
				}
				return true;
			}
			case KeyMap.KEYCODE_DPAD_RIGHT: {
				if ((position + 1) < count) {
					position += 1;
					mCurrentPage = computeCurrentPage(position);
					setListSelection(position);
					return true;
				}
				return true;
			}
			case KeyMap.KEYCODE_MTKIR_CHUP: {
				if ((position - mPageSize) >= 0) {
					position -= mPageSize;
				} else {
					position = 0;
				}
				mTurnPage = true;
				mCurrentPage = computeCurrentPage(position);
				setListSelection(position);
				return true;
			}
			case KeyMap.KEYCODE_MTKIR_CHDN: {
				if ((position + mPageSize) < count) {
					position += mPageSize;
				} else {
					position = count - 1;
				}
				mCurrentPage = computeCurrentPage(position);
				if (mCurrentPage == mPageCount) {
					mTurnPage = true;
					setListAdapter(mAdapter);
				}
				setListSelection(position);
				return true;
			}
			default:
				break;
			}
		}

		return super.dispatchKeyEvent(event);
	}

	 void scrollTo(int count) {
		int scrollY;
		int marginRows = getMarginRows(count);
		if (marginRows > 0) {
			scrollY = marginRows * (mItemHeight + mVerticalSpacing)
					+ mVerticalSpacing;
			vList.scrollTo(vList.getScrollX(), scrollY);
		}
	}

	private int getMarginRows(int count) {
		int marginRows = 0;
		if (count % mPageSize > 0) {
			marginRows = (mPageSize - count % mPageSize) / getNumberColum();
		}
		return marginRows;

	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyMap.KEYCODE_MTKIR_BLUE:
			if(!MeidaMainActivity.isValid(400)){
				break;
			}
			/*by lei add for stop video and audio get thumbnail*/
			int contentType = getListContentType();
			//((MtkFilesGridAdapter) mAdapter).stop();

            mAdapter.stop();
			if (contentType == FilesManager.CONTENT_AUDIO
					|| contentType == FilesManager.CONTENT_VIDEO) {
				((MtkFilesGridAdapter) mAdapter).stopFileDecode();
			}
			
			mIsListMode = true;
			if (null != mInfoLoader) {
				mInfoLoader.clearQueue();
			}
			
			Intent intent = new Intent(this, MtkFilesListActivity.class);
			intent.putExtra(INTENT_NAME_PATH, getListCurrentPath());
			intent.putExtra(INTENT_NAME_SELECTION,
					getListSelectedItemPosition() + 1);
			intent.putExtra("Mode", 0);
			intent.putStringArrayListExtra(INTENT_NAME_COPYED_FILES,
					mCopyedFiles);
			//add for ThumbnailSize bug
			intent.putExtra("mThumbnailSize", mThumbnailSize);
			
			startActivity(intent);
			finish();
			break;
		default:
			break;
		}

		return super.onKeyDown(keyCode, event);
	}

	@Override
	protected void onListItemSelected(AbsListView l, View v, int position,
			long id) {
			Log.d(TAG,"onListItemSelected~~~");
		FileAdapter file = getListItem(position);


		if (mPageCount == 0) {
			vTopRight.setText("");
		} else {
			mCurrentPage = computeCurrentPage(position);
				Log.d(TAG,"onListItemSelected aft mCurrentPage"+mCurrentPage);
			vTopRight.setText(mCurrentPage + "/" + mPageCount);
		}

		
		if (file == null) {
			return;
		}
		if (file.isIsoFile()) {
			// TODO mount iso file
			return;
		}
		if (file.isFile()) {
			int type = mFilesManager.getContentType();
			String info = "";
			if (type == MultiMediaConstant.AUDIO || type == MultiMediaConstant.VIDEO 
				|| type ==MultiMediaConstant.PHOTO || type == MultiMediaConstant.THRD_PHOTO) {
				info = mInforCache.get(file.getAbsolutePath());
				if(info != null){
					vLeftMidTv.setText(info);
					return;
				} else {
					vLeftMidTv.setText("");
				}
				String suffix = file.getSuffix();
				if(null!=suffix&&suffix.startsWith(".")){
					suffix=suffix.substring(1);
				}
				if ("pcm".equalsIgnoreCase(suffix)
						|| "lpcm".equalsIgnoreCase(suffix)) {
					MtkLog.w(TAG, "pcm file:" + file.getAbsolutePath());
					vLeftMidTv.setText(file.getName()+file.getSuffix());
					return;
				}
				// fix 
				if(mInfoLoader != null){
					//mInfoLoader.clearQueue();
					LoadInfo loadinfo = mLoadInfoWorks.get(file.getAbsolutePath());
					if (null == loadinfo) {
						loadinfo = new LoadInfo(file, vLeftMidTv);
					}
					mInfoLoader.addSelectedInfoWork(loadinfo);
				}else{
					
					mInfoLoader = AsyncLoader.getInstance(1);
					if(null==mLoadInfoWorks){
						mLoadInfoWorks =new  ConcurrentHashMap<String, LoadInfo>();				  
					}
					LoadInfo loadinfo = new LoadInfo(file, vLeftMidTv);
					mInfoLoader.addSelectedInfoWork(loadinfo);
						
				}
				//info = mInforCache.get(file.getAbsolutePath());
			}else{
				info = file.getInfo();
				vLeftMidTv.setText(info);
				return;
			}
		} else {
			vLeftMidTv.setText("");
		}




		
	}






	@Override
	public void onScrollStateChanged(AbsListView view, int scrollState){
		MtkLog.d(TAG,"onScrollStateChanged scrollState ="+scrollState);
	
	};

	private int lastFirstVisibleItem = 0;

	@Override
	public void onScroll(AbsListView view, int firstVisibleItem, int visibleItemCount,
			   int totalItemCount){

		if(firstVisibleItem != lastFirstVisibleItem){
			//cancelLoadFiles();
			mAdapter.cancel();
			lastFirstVisibleItem = firstVisibleItem;
			vList.invalidateViews();
		}
			   
		MtkLog.d(TAG,"onScroll firstVisibleItem ="+firstVisibleItem +"visibleItemCount ="+visibleItemCount +
			"totalItemCount = "+totalItemCount);


		


	};


	@Override
	public void onNothingSelected(AdapterView<?> parent) {

		vLeftMidTv.setText("");
	}

	protected int computePageSize() {
		mPageSize = getRowNumber() * getNumberColum();
		mItemWidth = getColumnWidth();
		mItemHeight = getColumnHeight();
		int filesCount = getListItemsCount();
		mPageCount = ceilPage(filesCount - 1, mPageSize);
		return mPageSize;
	}

	private int ceilNumber(int count, int number) {
		if (number == 0) {
			return 0;
		}
		if (count % number == 0) {
			return count / number;
		} else {
			return (count / number) + 1;
		}

	}

	public int ceilPage(int position, int pageSize) {

		if (pageSize == 0) {
			return 0;
		}
		// index begin from 0
		position += 1;
		if (position == 1) {
			return 1;
		} else if (position % pageSize == 0) {
			return position / pageSize;
		} else {
			return position / pageSize + 1;
		}

	}

	protected int computeCurrentPage(int position) {
		MtkLog.d(TAG, "Selected Pos : " + position);
		int page = mCurrentPage;
		if (mPageSize > 0 && position >= 0 && position < getListItemsCount()) {
			page = ceilPage(position, mPageSize);
		}

		return page;
	}

	@Override
	protected void refreshListView() {
		mPageSize = computePageSize();
		mCurrentPage = computeCurrentPage(mFilesManager.getPositionInParent());
		if(mPageCount==0){
			vTopRight.setText("0/0");
		}else{
			vTopRight.setText(mCurrentPage + "/" + mPageCount);
		}
		super.refreshListView();
		setListSelection(mFilesManager.getPositionInParent());
	}

	public int getRowNumber() {
		switch (mThumbnailSize) {
		case MultiMediaConstant.SMALL: {
			return 5;
		}
		case MultiMediaConstant.MEDIUM: {
			return 4;
		}
		case MultiMediaConstant.LARGE: {
			return 3;
		}
		default:
			return 5;
		}
	}

	public int getColumnHeight() {
		int rowNumber = getRowNumber();
		int height = (mGridViewH - mVerticalSpacing * (rowNumber + 1))
				/ rowNumber;
		MtkLog.i(TAG, "getColumnHeight:" + height);
		return height;
	}

	public int getNumberColum() {
		switch (mThumbnailSize) {
		case MultiMediaConstant.SMALL: {
			return 7;
		}
		case MultiMediaConstant.MEDIUM: {
			return 6;
		}
		case MultiMediaConstant.LARGE: {
			return 5;
		}
		default:
			return 5;
		}
	}

	public int getColumnWidth() {
		int columnNumber = getNumberColum();
		int width = (mGridViewW - mHorizontalSpacing * (columnNumber + 1))
				/ columnNumber;
		MtkLog.i(TAG, "getColumnWidth:" + width);
		return width;
	}

	@Override
	protected void refreshListView(List<FileAdapter> files) {
		((MediaGridView) vList).setNumColumns(getNumberColum());

		super.refreshListView(files);

		if (files != null) {
			mPageSize = computePageSize();
			mCurrentPage = computeCurrentPage(mFilesManager
					.getPositionInParent());
			if(mPageCount==0){
				vTopRight.setText("0/0");
			}else{
				vTopRight.setText(mCurrentPage + "/" + mPageCount);
			}
		}
	}

	@Override
	protected void cancelLoadFiles() {
		if(!mIsListMode){
			mAdapter.cancel();
		}
		mIsListMode = false;	
		super.cancelLoadFiles();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
	}
	
	@Override
	protected void onStop() {
		super.onStop();
	}
	
	public void exit() {
		if(AnimationManager.getInstance().getIsAnimation()){
			AnimationManager.getInstance().startActivityEndAnimation(this, findViewById(R.id.mmp_files_grid_layout), null);
		}else{
			finish();
		}
	}

	private class MtkFilesGridAdapter extends FilesAdapter {
		private static final String TAG = "MtkFilesGridAdapter";
		private static final int MAX_NUM_RNUUNABLE = 300;

		private Drawable mVideoDefault;
		private Drawable mAudioDefault;
		private Drawable mPhotoDefault;
		private Drawable mTextDefault;
		private Drawable mSmbFolder;
		private Drawable mDlnaFolder;
		private Drawable mFolder;
		private Drawable mFailed;

		private Handler mBindHandler;
		private BitmapCache mCache;
		private AsyncLoader<Bitmap> mLoader;
		private ConcurrentHashMap<View, LoadBitmap> mWorks;
		private ConcurrentHashMap<View, Runnable> mRunnables;


		public void clearWork(){
			//mWorks.clear();
			if(mRunnables.size() > MAX_NUM_RNUUNABLE)
			mRunnables.clear();
		}
		// private PhotoLoader mPhotoLoader;	
		public MtkFilesGridAdapter(Context context) {
			super(context);
			mBindHandler = new Handler();
			mCache = BitmapCache.createCache(false);
//			mLoader = new AsyncLoader<Bitmap>(1);
			mLoader = AsyncLoader.getInstance(1);
			mWorks = new ConcurrentHashMap<View, LoadBitmap>();
			mRunnables = new ConcurrentHashMap<View, Runnable>();

			// mPhotoLoader = new PhotoLoader(context,
			// R.drawable.mmp_thumbnail_loading_failed_samll);

			prepareDefaultThumbnails(mThumbnailSize);
		}

		private void prepareDefaultThumbnails(int size) {
			if (size == MultiMediaConstant.SMALL) {
				mVideoDefault = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_video_small);
				mAudioDefault = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_audio_samll);
				mPhotoDefault = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_photo_samll);
				mTextDefault = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_text_samll);
				mFolder = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_folder_samll);
				mSmbFolder = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_folder_smb_small);
				mDlnaFolder = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_folder_dlna_small);
				mFailed = mResources
						.getDrawable(R.drawable.mmp_thumbnail_loading_failed_samll);
			} else if (size == MultiMediaConstant.MEDIUM) {
				mVideoDefault = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_video_middle);
				mAudioDefault = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_audio_middle);
				mPhotoDefault = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_photo_middle);
				mTextDefault = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_text_middle);
				mFolder = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_folder_middle);
				mSmbFolder = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_folder_smb_middle);
				mDlnaFolder = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_folder_dlna_middle);
				mFailed = mResources
						.getDrawable(R.drawable.mmp_thumbnail_loading_failed_mid);
			} else if (size == MultiMediaConstant.LARGE) {
				mVideoDefault = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_video_big);
				mAudioDefault = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_audio_big);
				mPhotoDefault = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_photo_big);
				mTextDefault = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_text_big);
				mFolder = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_folder_big);
				mSmbFolder = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_folder_smb_big);
				mDlnaFolder = mResources
						.getDrawable(R.drawable.mmp_thumbnail_icon_folder_dlna_big);
				mFailed = mResources
						.getDrawable(R.drawable.mmp_thumbnail_loading_failed_big);
			}
		}

		@Override
		public int getCount() {
			if (mDataList == null) {
				return 0;
			}
			// TODO fixed bug may be bug
			if (mTurnPage) {
				if (mPageSize > 0 && (getListItemsCount() > mPageSize)
						&& getListItemsCount() % mPageSize > 0) {
					return super.getCount()
							+ (mPageSize - getListItemsCount() % mPageSize);
				}
			}
			MtkLog.i(TAG, "getCount count =files size");
			return super.getCount();
		}

		@Override
		public FileAdapter getItem(int position) {
			if (position >= getListItemsCount()) {
				return null;
			}

			return super.getItem(position);
		}

		@Override
		public boolean areAllItemsEnabled() {
			return false;
		}

		@Override
		public boolean isEnabled(int position) {
			// return false;
			if (position >= getListItemsCount()) {
				return false;
			}

			return super.isEnabled(position);
		}

		@Override
		protected void updateThumbnail() {
			mCache.clear();
			prepareDefaultThumbnails(mThumbnailSize);
			computePageSize();
			int selection = getListSelectedItemPosition();
			mCurrentPage = computeCurrentPage(selection);
			if(mPageCount==0){
				vTopRight.setText("0/0");
			}else{
				vTopRight.setText(mCurrentPage + "/" + mPageCount);
			}
			((MediaGridView) vList).setNumColumns(getNumberColum());
			notifyDataSetChanged();
			setListSelection(selection);
		}

		@Override
		protected int getItemLayout() {
			return R.layout.mmp_asycgriditem;
		}

		@Override
		protected void initView(View v,boolean flag) {
			if(!flag){
				ViewHolder holder = new ViewHolder();
	
				holder.img = (ImageView) v.findViewById(R.id.multimedia_gv_img);
				holder.tv = (TextView) v.findViewById(R.id.multimedia_gv_tv);
				holder.layout = (RelativeLayout) v.findViewById(R.id.mmp_grid_highlight);
				v.setLayoutParams(new GridView.LayoutParams(mItemWidth,mItemHeight));
				v.setTag(holder);
			}else{
				v.setLayoutParams(new GridView.LayoutParams(mItemWidth,mItemHeight));
			}
		}

		@Override
		protected void bindView(View v, FileAdapter data, int position) {
			ViewHolder holder = (ViewHolder) v.getTag();
			if (data == null) {
				v.setBackgroundDrawable(null);
				holder.img.setImageDrawable(null);
				holder.tv.setText("");
				holder.layout.setBackgroundDrawable(null);
				return;
			}

			String path = data.getAbsolutePath();
			MtkLog.d(TAG, "BindView : " + path);

			/*Cancel task when load success, so move to loaded function
			 * and the task had add queue, will don't add.*/
			//cancelLastWork(holder.img);

			MultiFilesManager manager = ((MultiFilesManager) mFilesManager);
			if (data.isDirectory() || data.isIsoFile()) {
				int source = manager.getSourceType(path);
				switch (source) {
				case MultiFilesManager.SOURCE_LOCAL:
					holder.img.setImageDrawable(mFolder);
					break;
				case MultiFilesManager.SOURCE_SMB:
					holder.img.setImageDrawable(mSmbFolder);
					break;
				case MultiFilesManager.SOURCE_DLNA:
					holder.img.setImageDrawable(mDlnaFolder);
					break;
				default:
					break;
				}

				holder.tv.setText(data.getName());
				holder.tv.setVisibility(View.VISIBLE);
				holder.layout.setBackgroundDrawable(null);
			} else {
				holder.tv.setVisibility(View.GONE);
				int type = mFilesManager.getContentType();
				if (type == MultiMediaConstant.TEXT) {
					holder.img.setImageDrawable(mTextDefault);
				} else if (type == MultiMediaConstant.AUDIO) {
					holder.img.setImageDrawable(mAudioDefault);
				} else if (type == MultiMediaConstant.PHOTO) {
					holder.img.setImageDrawable(mPhotoDefault);
				} else if (type == MultiMediaConstant.VIDEO) {
					holder.img.setImageDrawable(mVideoDefault);
				} else if (type == MultiMediaConstant.THRD_PHOTO){
					holder.img.setImageDrawable(mPhotoDefault);
				}

				if (mCopyedFiles.size() > 0 && mCopyedFiles.contains(path)) {
					holder.layout
							.setBackgroundResource(R.drawable.mmp_gridview_copyed);
				} else if (mSelectedFiles.size() > 0 && mSelectedFiles.contains(path)) {
					holder.layout
							.setBackgroundResource(R.drawable.mmp_gridview_selected);
				} else {
					holder.layout.setBackgroundDrawable(null);
				}
				if(type == MultiMediaConstant.AUDIO)
				{
					String suffix=data.getSuffix();
					if(FileSuffixConst.DLNA_FILE_NAME_EXT_PCM.equalsIgnoreCase(suffix))
					{
						return;
					}
				}
				bindThumbnail(data, holder.img, path);
			}
		}

		private void bindThumbnail(FileAdapter data, ImageView view, String path) {
			Bitmap image = mCache.get(path);
			if (image != null) {
				MtkLog.i(TAG, "HitCache!!" + path);
				view.setImageBitmap(image);
			} else {
				MtkLog.i(TAG, "LoadBitmap!!" + path);
				if (mWorks.get(view) == null) {
					LoadBitmap work = new LoadBitmap(data, view);
					mWorks.put((View)view, work);
					mLoader.addWork(work);
				}
			}
		}

		private void cancelLastWork(View view) {
			LoadBitmap work = mWorks.get(view);
			

			 MtkLog.e(TAG, "Work Map Size : " + mWorks.size());
			if (work != null) {
				 MtkLog.w(TAG, "Cancel Work!!");
				//work.getData().stopThumbnail();
				mLoader.cancel(work);
				mWorks.remove(view);
				
			}

			 MtkLog.e(TAG, "Runnable Map Size : " + mRunnables.size());
			Runnable r = mRunnables.get(view);
			if (r != null) {
				 MtkLog.w(TAG, "Cancel Runnable!!");
				mBindHandler.removeCallbacks(r);
				mRunnables.remove(view);
			}
		}

		public void stop() {
			//stopThumbnail();
			synchronized (mWorks) {
				mLoader.clearQueue();
				mWorks.clear();
			}
		}

		public void stopFileDecode(){
//			LoadBitmap work = null;
//			Enumeration<View> views = mWorks.keys();
//			ArrayList<View> viewList = Collections.list(views);
//			View view = null;
//
//			for (int i = 0; i < mWorks.size(); i++) {
//				view = viewList.get(i);
//				work = mWorks.get(view);
//				if (work != null && work.getData() != null) {
//					FileAdapter file = work.getData();
//					if (!file.isDirectory()) {
//						file.stopThumbnail();
//						break;
//					}
//				}
//			}
		}
		private void logCaheSize() {
			ConcurrentHashMap<String, SoftReference<Bitmap>> map = mCache
					.getCache();
			Iterator<Entry<String, SoftReference<Bitmap>>> iterator = map
					.entrySet().iterator();
			int count = 0;
			int recycles = 0;
			while (iterator.hasNext()) {
				SoftReference<Bitmap> ref = iterator.next().getValue();
				if (null != ref) {
					Bitmap value = ref.get();
					if (null != value) {
						count++;
						if (!value.isRecycled()) {
							recycles++;
							value.recycle();
							value = null;
						}
					}
				}
			}

			Log.i(TAG, " count:" + count + " recycles:" + recycles);

		}
		// private void stopThumbnail(){
		//
		// MtkLog.d(TAG, "stopThumbnail : ---------------" );
		// LoadBitmap work = null;
		// Enumeration<View> views= mWorks.keys();
		// ArrayList< View> viewList = Collections.list(views);
		// View view = null;
		// for (int i=0; i < mWorks.size(); i++){
		// view = viewList.get(i);
		// work = mWorks.get(view);
		// work.getData().stopThumbnail();
		// }
		// }
		
		protected void cancel() {
			mLoader.clearQueue();
			mWorks.clear();
		}

		private class ViewHolder {
			ImageView img;
			TextView tv;
			RelativeLayout layout;
		}
		
	
		private class LoadBitmap implements LoadWork<Bitmap> {
			private FileAdapter mData;
			private ImageView vImage;
			private Bitmap mResult;
			private boolean mNeedCache = true;

			public LoadBitmap(FileAdapter data, ImageView iamge) {
				mData = data;
				vImage = iamge;
			}
			
			public FileAdapter getData(){
				return mData;
			}
			
			public Bitmap load() {
				Bitmap bitmap = null;
				try {
					bitmap = mData.getThumbnail(mThumbnailSize
							* MultiMediaConstant.ZOOM, mThumbnailSize
							* MultiMediaConstant.ZOOM,true);
					 if (bitmap == null){
					      if (getListContentType() == MultiMediaConstant.PHOTO){
							bitmap = ((BitmapDrawable) mFailed).getBitmap();
						} else if (getListContentType() == MultiMediaConstant.THRD_PHOTO) {
							bitmap = ((BitmapDrawable) mFailed).getBitmap();
						}
					  }
					/*if (getListContentType() == MultiMediaConstant.PHOTO
							&& bitmap == null) {
						bitmap = mData.getThumbnail(mThumbnailSize
								* MultiMediaConstant.ZOOM, mThumbnailSize
								* MultiMediaConstant.ZOOM);

						if (getListContentType() == MultiMediaConstant.PHOTO
								&& bitmap == null) {
							bitmap = ((BitmapDrawable) mFailed).getBitmap();
						}

					}*/
				} catch (OutOfMemoryError e) {
					Log.e(TAG, "Get Image Thumbnail!!", e);
					bitmap = ((BitmapDrawable) mFailed).getBitmap();
					mNeedCache = false;
				}

				mResult = bitmap;
				MtkLog.d(TAG, "Decode Bitmap : " + mResult);
				return bitmap;
			}

			public void loaded(Bitmap result) {
				/*by lei add for optimization*/
				String cacheInfo = mData.getCacheInfo();
				if(cacheInfo != null){
					mInforCache.put(mData.getAbsolutePath(), mData.getCacheInfo());
				}
				if (result == null) {
					int cntType = getListContentType();
					switch (cntType) {
					case MultiMediaConstant.AUDIO:
						mCache.put(mData.getAbsolutePath(),
								((BitmapDrawable) mAudioDefault).getBitmap());
						break;
					case MultiMediaConstant.VIDEO:
						mCache.put(mData.getAbsolutePath(),
								((BitmapDrawable) mVideoDefault).getBitmap());
						break;
					/*case MultiMediaConstant.PHOTO:
						mCache.put(mData.getAbsolutePath(),
								((BitmapDrawable) mFailed).getBitmap());
						break;*/
					default:
						break;
					}

				} else if (result != null && mNeedCache) {
					mCache.put(mData.getAbsolutePath(), result);
					mWorks.remove(vImage);
				}

				Runnable r = new BindImage();
				mRunnables.put(vImage, r);
				mBindHandler.post(r);
				// loaded a bitmap sleep 100 ms
				// try {
				// Thread.sleep(10);
				// } catch (InterruptedException e) {
				// e.printStackTrace();
				// }

			}

			private class BindImage implements Runnable {
				public void run() {
					if (mResult != null) {
						// TODO null image
						if (null != vImage.getDrawable())
							vImage.setImageBitmap(mResult);
					}
				}
			}
		}
	}
}
