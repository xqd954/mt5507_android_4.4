package com.mediatek.ui.mmp.gamekit.filebrowse;

import java.io.File;
import java.io.FileDescriptor;

import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Bitmap.Config;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;

import com.mediatek.gamekit.GKTexture;
import com.mediatek.gamekit.GKView;
import com.mediatek.gamekit.GKView.MessageListener;
import com.mediatek.mmpcm.mmcimpl.Const;
import com.mediatek.mmpcm.mmcimpl.PlayList;
import com.mediatek.ui.R;
import com.mediatek.ui.mmp.gamekit.content.MessageConst;
import com.mediatek.ui.mmp.gamekit.content.MutilMediaConst;
import com.mediatek.ui.mmp.gamekit.media.MtkGLES2TextureInfo;
import com.mediatek.ui.mmp.gamekit.media.MtkThumbnailManager;
import com.mediatek.ui.mmp.gamekit.util.AsyncLoader;
import com.mediatek.ui.mmp.gamekit.util.BrowseList;
import com.mediatek.ui.mmp.gamekit.util.FileUtil;
import com.mediatek.ui.mmp.gamekit.util.TextureMananger;
import com.mediatek.ui.mmp.gamekit.util.AsyncLoader.LoadWork;
import com.mediatek.ui.mmp.model.FileAdapter;
import com.mediatek.ui.mmp.model.FilesManager;
import com.mediatek.ui.mmp.model.MultiFilesManager;
import com.mediatek.ui.mmp.multimedia.MtkFilesBaseListActivity;
import com.mediatek.ui.mmp.multimedia.MtkFilesGridActivity;
import com.mediatek.ui.mmp.multimedia.MtkFilesListActivity;
import com.mediatek.ui.mmp.multimedia.MusicPlayActivity;
import com.mediatek.ui.mmp.multimedia.Photo3DPlayActivity;
import com.mediatek.ui.mmp.multimedia.PhotoPlayActivity;
import com.mediatek.ui.mmp.multimedia.Photo4K2KPlayActivity;

import com.mediatek.ui.mmp.multimedia.TextPlayActivity;
import com.mediatek.ui.mmp.multimedia.VideoPlayActivity;
import com.mediatek.ui.mmp.util.Util;
import com.mediatek.ui.util.DestroyApp;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;

public class MtkFileFlimBrowseActivity extends MtkFileBrowseActivity implements GKView.LoadListener{

	public static final String MEDIA_PATH = "media_path";
	public static final String RECURSER_MODE = "RecurserMode";
	public static final String TO_3DBROWSE = "To3DBrowse";
	public static final String FROM_PLAYTYPE="FromMusicPlay";
	public static final String CURRENT_BROWSE_STATUS ="CurrentBrowseStatus";
	//support small window video play on board.
	public static  boolean mIsSupportVideoPlay = false;
	
	private String TAG = "MtkMainActivity.java";

	private Bundle mExtras;
	
	private AnimalStatus mAnimalStatus = AnimalStatus.ANIMAL_UNKNOW;

	public enum BrowseStatus {
		FROM_GRID_STATUS, 
		FROM_LIST_STATUS,
		FROM_PLAY_STATUS,
		NORMAL_STATUS,
	}
	
	public enum AnimalStatus{
		ANIMAL_UNKNOW,
		ANIMAL_START,
		ANIMAL_END,
	}
	
	/** Called when the activity is first created. */
	public void onCreate(Bundle savedInstanceState) {
		MtkLog.d(TAG, "onCreate\n");
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);

		//setContentView(R.layout.mmp_file_thrd_browse);
		super.onCreate(savedInstanceState);
		initView();
		setContentView(mEngineView);
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
	}

	// --------------------------- view load and bitmap load
	// start----------------------------------------
	protected void initView() {
		mExtras = getIntent().getExtras();
		if (mExtras != null){
			mFromPath = mExtras.getString(MEDIA_PATH);
			mFromStatus = (BrowseStatus) mExtras.getSerializable(CURRENT_BROWSE_STATUS);
			mFromType = mExtras.getInt(FROM_PLAYTYPE);
			mFromPostion = mExtras.getInt(MtkFilesBaseListActivity.INTENT_NAME_SELECTION);
			mRecuseMode = mExtras.getInt(RECURSER_MODE);
		}	
		//mEngineView = (GKView) findViewById(R.id.gl_filebrowse_view);
		mEngineView = new GKView(this, "/system/resource/FileBrowse.blend");
		mBrowseList = BrowseList.getList();
		mTexutreManager = TextureMananger.getTextureManager(this);
		mTexutreManager.reset();
		mEngineView.setMessageListener(mGkListener);
		super.initView();
		mEngineView.setLoadListener(this);
	}

	protected void reloadContent() {
		PlayList pl = PlayList.getPlayList();
		
		int type = mManager.getContentType();
		int idx = 0;
		switch (type) {
		case FilesManager.CONTENT_PHOTO:
			type = Const.FILTER_IMAGE;
			break;
		case FilesManager.CONTENT_AUDIO:
			type = Const.FILTER_AUDIO;
			break;
		case FilesManager.CONTENT_VIDEO:
			type = Const.FILTER_VIDEO;
			break;
		case FilesManager.CONTENT_TEXT:
			type = Const.FILTER_TEXT;
			break;
		case FilesManager.CONTENT_THRDPHOTO:
			type = Const.FILTER_IMAGE;
			break;

		default:
			break;
		}

		int blSize = mBrowseList.getNum();	
		if (blSize == 0) {
			return;
		}
			
		int plSize = pl.getFileNum(type);
		idx = pl.getCurrentIndex(type);
		idx += (blSize - plSize);

		reloadView(idx);
	}
	
	protected void bindView(FileAdapter f, int textureId) {
		Bitmap bm = null;
		if (f.isDirectory() || f.isDevice()) {
			bm = mFolder.getBitmap();
			setTexture(bm, textureId, f, false);
		} else {
			LoadBitmapListener loadBitmapL = new LoadBitmapListener(textureId,
					f);
			bm = mThumbnailManager.getThumbnail(f, loadBitmapL);
					
			if (bm == null) {
				if (f.isVideoFile()) {
					bm = mVideoIcon.getBitmap();
				} else if (f.isTextFile()) {
					bm = mTextIcon.getBitmap();
				} else if (f.isAudioFile()) {
					bm = mAudioIcon.getBitmap();
				} else if (f.isPhotoFile()) {
					bm = mPhotoIcon.getBitmap();
				}
				
				setTexture(bm, textureId, f, false);
			} else {
				setTexture(bm, textureId, f);
			}
		}		
	}

	protected void initBoardView(FileAdapter f, int index) {
		Bitmap bm = null;
		int textureId = mTexutreManager.getId(index);
		LoadBitmapListener loadBitmapL = new LoadBitmapListener(textureId, f);
		bm = mThumbnailManager.getThumbnail(f, loadBitmapL);
		
		if (bm != null) {
			setTexture(bm, textureId, f);
			return;
		}

		if (f.isDirectory() || f.isDevice()) {
			MtkLog.d(TAG, "this is folder.....");
			bm = mFolder.getBitmap();

		} else if (f.isTextFile()) {
			MtkLog.d(TAG, "this is Text file....");
			bm = mTextIcon.getBitmap();

		} else if (f.isVideoFile()) {
			MtkLog.d(TAG, "this is video....");
			bm = mVideoIcon.getBitmap();

		} else if (f.isPhotoFile() || f.isThrdPhotoFile()) {
			MtkLog.d(TAG, "this is photo....");
			bm = mPhotoIcon.getBitmap();

		} else if (f.isAudioFile()) {
			MtkLog.d(TAG, "this is audio....");
			bm = mAudioIcon.getBitmap();
		}
		setTexture(bm, textureId, f, false);
	}

	private void testDrawBitmap(Bitmap bitmap) {
		final Bitmap bm = bitmap;

				String textureName = getString(mTexutreManager.getId(0));
				
				GKTexture textureInfo = mEngineView.getTexture(mEngineView, textureName);
				int w = textureInfo.getWidth();
				int h = textureInfo.getHeight();

				if (bm != null) {
					mEngineView.uploadImage(mEngineView, textureInfo, bm);
				}
	}

	private void drawBitmapToTexture(MtkGLES2TextureInfo tInfo, Bitmap bitmap, boolean isRecycled) {
		int w = tInfo.getWidth();
		int h = tInfo.getHeight();
		int dbw = bitmap.getWidth();
		int dbh = bitmap.getHeight();

		int nw = (w > dbw) ? w : dbw;
		int nh = (h > dbh) ? h : dbh;

		Bitmap bm = Bitmap.createBitmap(nw, nh, Bitmap.Config.ARGB_8888);
		Canvas canvas = new Canvas(bm);

		Paint p = new Paint();
		p.setTextSize(14);
		p.setColor(Color.GREEN);

		FileAdapter f = tInfo.getFileAdapter();
		String name = f.getName();
		int nLen = name.length();
		int bw = (nw - bitmap.getWidth()) / 2;
		int bh = (nh - bitmap.getHeight()) / 2;
		bw = (bw > 0) ? bw : 0;
		bh = (bh > 0) ? bh : 0;
		nLen = (int) ((nw - p.getTextSize() * nLen / 2) / 2);
		nLen = (nLen > 0) ? nLen : 0;

		canvas.drawBitmap(bitmap, bw, bh, p);

		canvas.drawText(name, nLen, nh - 10, p);

		tInfo.uploadImage(mEngineView,  bm);
		if (isRecycled) {
			if (bitmap != null && !bitmap.isRecycled()) {
				bitmap.recycle();
				bitmap = null;
			}			
		}
	}

	protected void setTexture(final Bitmap bm, final int textureId,
			final FileAdapter fAdapter){
		setTexture(bm, textureId, fAdapter, true);
	}

	protected void setTexture(final Bitmap bm, final int textureId,
			final FileAdapter fAdapter, boolean isRecycled) {

		MtkLog.d(TAG, "setTexture queueEvent bitmap  = " + bm);
		MtkLog.d(TAG, "setTexture queueEvent textureId  = " + getString(textureId));

		if (bm == null)
			return;

		String textureName = getString(textureId);
		MtkGLES2TextureInfo textureInfo = mTexutreManager
				.getTextureInfo(textureName);

		if (bm != null && textureInfo != null) {
			if (!bm.equals(textureInfo.getBitmap())) {
				textureInfo.setPath(fAdapter.getPath());
				textureInfo.setFileAdapter(fAdapter);
				drawBitmapToTexture(textureInfo, bm, isRecycled);

			}
		} else {
			textureName = getString(textureId);
			GKTexture tInfo = mEngineView.getTexture(mEngineView, textureName);;
			textureInfo = new MtkGLES2TextureInfo(tInfo);
			textureInfo.setPath(fAdapter.getPath());
			textureInfo.setFileAdapter(fAdapter);

			if (bm != null) {

				drawBitmapToTexture(textureInfo, bm, isRecycled);
			}
			mTexutreManager.putTextureInfo(textureName, textureInfo);
		}

	}

	// --------------------------- key control
	// start----------------------------------------

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		return mEngineView.onTouchEvent(event);
	}

	public void onBackPressed() {
		MtkLog.i(TAG, "onKeyDown onBackPressed... ");
		super.onBackPressed();
	}

	protected void exitAPP() {
		super.exitAPP();
		mHandler.sendEmptyMessage(MessageConst.MSG_EXIT_ANIMAL);
	}

	/**
	 * Repeat key duration
	 */
	public static final int KEY_DURATION = 400;
	/**
	 * Last key down milliseconds
	 */
	protected long mLastKeyDownTime;

	/**
	 * Is the current key down valid
	 * 
	 * @return true:valid,false:invalid
	 */
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

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {

		MtkLog.i(TAG, "onKeyDown value " + keyCode);
		
		//Forbid process key when gamekit Loading.
		if (mManager == null){
			return true;
		}
		
		switch (keyCode) {
		case KeyEvent.KEYCODE_BACK:

			MtkLog.i(TAG, "onKeyDown back ");
			// mHandler.sendEmptyMessage(MessageConst.MSG_EXIT_ANIMAL);
			return super.onKeyDown(keyCode, event);
		case KeyEvent.KEYCODE_DPAD_LEFT:
			if (isValid()) {
				luaMoveLeft();
			}
			return true;
		case KeyEvent.KEYCODE_DPAD_RIGHT:
			if (isValid()) {
				luaMoveRight();
			}
			return true;
		case KeyEvent.KEYCODE_DPAD_CENTER:
		case KeyEvent.KEYCODE_ENTER:
			// play File.
			MtkLog.i(TAG, "onKeyDown enter..");
			if (isValid()){
				openContent();
			}
			return true;
		case KeyMap.KEYCODE_MTKIR_BLUE:
			/*if (isValid()){
				mHandler.sendEmptyMessage(MessageConst.MSG_EXIT_ANIMAL);
			}*/
			return true;
		case KeyMap.KEYCODE_DPAD_UP:
		case KeyMap.KEYCODE_DPAD_DOWN:
			/*if (isValid()){
				playVideoOnBoard();
			}*/
			return true;
		case KeyMap.KEYCODE_MTKIR_ANGLE: {
			mThumbnailManager.destroyThumbnail();
			mBrowseList.cleanList();	
			Util.exitMmpActivity(this);
			return true;
		}
		
		case KeyMap.KEYCODE_MTKIR_GUIDE:{
			mThumbnailManager.destroyThumbnail();
			mBrowseList.cleanList();	
			Util.startEPGActivity(this);
			return true;
		}
		
		default:
			break;

		}
		return true;
		//return super.onKeyDown(keyCode, event);
	}
	
	@Override
	protected void onRestart() {
		// TODO Auto-generated method stub
		super.onRestart();
		MtkLog.d(TAG, "onRestart\n");
		mHandler.sendEmptyMessage(MessageConst.MSG_RELOAD_CONTENT);
	}


	@Override
	protected void onResume() {
		super.onResume();
		MtkLog.d(TAG, "onResume\n");
	}

	@Override
	protected void onPause() {
		super.onPause();
		MtkLog.d(TAG, "onPause\n");
	}

	@Override
	protected void onStop() {
		super.onStop();
		MtkLog.d(TAG, "onStop\n");

	}

	@Override
	public void onDestroy() {
		((DestroyApp) getApplication()).remove(this);
		mTexutreManager.clean();
		super.onDestroy();
	}

	private void change2DFileMode() {
		String textureName = mTexutreManager.getFoucsTextureName();
		MtkGLES2TextureInfo tInfo = mTexutreManager.getTextureInfo(textureName);
		int foucsIndx = 0;
		if (tInfo != null) {
			// get focus file path.
			String path = tInfo.getPath();
			foucsIndx = mBrowseList.getSpecailIndx(path);
		}
		Intent intent;
		if (mBackStatus == BrowseStatus.FROM_LIST_STATUS){
		intent = new Intent(this, MtkFilesListActivity.class);
		intent.putExtra(MtkFilesBaseListActivity.INTENT_NAME_PATH, mManager
				.getCurrentPath());
		intent.putExtra(MtkFilesBaseListActivity.INTENT_NAME_SELECTION,
				foucsIndx + 1);
		intent.putExtra("Mode", 0);
		intent.putStringArrayListExtra(
				MtkFilesBaseListActivity.INTENT_NAME_COPYED_FILES, null);
		} else {
			intent = new Intent(this, MtkFilesGridActivity.class);
			intent.putExtra(MtkFilesBaseListActivity.INTENT_NAME_PATH, mManager
					.getCurrentPath());
			intent.putExtra(MtkFilesBaseListActivity.INTENT_NAME_SELECTION, foucsIndx);
		}
		// add for ThumbnailSize bug
		// intent.putExtra("mThumbnailSize", mThumbnailSize);

		startActivity(intent);
	}

	// --------------------------- browse file content
	// start----------------------------------------

	private void playVideoOnBoard() {
		if(!mIsSupportVideoPlay) return;
		
		if (mManager.getContentType() != MultiFilesManager.CONTENT_VIDEO){
			return;
		}
		
		String textureName = mTexutreManager.getFoucsTextureName();
		MtkGLES2TextureInfo tInfo = mTexutreManager.getTextureInfo(textureName);
		if (tInfo == null)
			return;
		// get focus file path.
		String path = tInfo.getPath();
		FileAdapter fa = tInfo.getFileAdapter();
		Bitmap bm = Bitmap.createBitmap(1, 1, Config.ARGB_8888);
		setTexture(bm, mTexutreManager.getId(mTexutreManager.getFoucsIndex()),
				fa);
		String str = "mPlayer = OgreKit.MTKMediaPlayer_getInstance();"
				+ "mPlayer.setLooping(mPlayer, 0);"
				+ "mPlayer.play(mPlayer,\""
				+ path
				+ "\");"
				+ "mPlayer.setVideoRect(mPlayer, 1920/3, 1080/3 -20, 1920/3 + 650, 1080/3+350);";

		MtkLog.d(TAG, "playFile start str" + str);
		mEngineView.runScript(str, false);
	}

	private void openContent() {
		//Processing lua animal.
		if (mAnimalStatus == AnimalStatus.ANIMAL_START) {
			return;
		}
		
		String textureName = mTexutreManager.getFoucsTextureName();

		MtkGLES2TextureInfo tInfo = mTexutreManager.getTextureInfo(textureName);

		MtkLog.d(TAG, "Play File... textureName = " + textureName + "tInfo" + tInfo);
		if (tInfo == null || tInfo.getFileAdapter() == null) return;
		// get focus file path.
		String path = tInfo.getPath();
		FileAdapter fa = tInfo.getFileAdapter();
		MtkLog.d(TAG, "Play File... textureName = " + textureName + "path = " + path);

		Message msg = new Message();
		msg.what = MessageConst.MSG_PLAY_ANIMAL;
		msg.obj = fa;
		mHandler.sendMessageDelayed(msg, 1);
		//
	}

	/**
	 * Play a File.
	 * 
	 * @param path
	 */
	protected void playFile(FileAdapter fAdapter) {

		MtkLog.d(TAG, "playFile start path" + fAdapter.getPath());
		int type = FileUtil.getTypeByFile(fAdapter);
		mTexutreManager.clean();
		AsyncLoader.getInstance().clearQueue();
		mThumbnailManager.destroyThumbnail();
		
		if (type == MutilMediaConst.CONTENT_VIDEO) {
			Intent intent = new Intent(this, VideoPlayActivity.class);
			startActivity(intent);
		} else if (type == MutilMediaConst.CONTENT_AUDIO) {
			Intent intent = new Intent(this, MusicPlayActivity.class);
			intent.putExtra(TO_3DBROWSE, type);
			intent.putExtra(CURRENT_BROWSE_STATUS, mFromStatus);
			intent.putExtra(MEDIA_PATH, mManager.getCurrentPath());
			intent.putExtra(RECURSER_MODE, mRecuseMode);
			startActivity(intent);
			
			finish();

		} else if (type == MutilMediaConst.CONTENT_PHOTO) {
			Intent intent = null;
			if(Util.PHOTO_4K2K_ON){
				intent = new Intent(this, Photo4K2KPlayActivity.class);
				intent.putExtra(Photo4K2KPlayActivity.PLAY_MODE,
							Photo4K2KPlayActivity.NORMAL_MODE);
			}else{
				intent = new Intent(this, PhotoPlayActivity.class);
				intent.putExtra(PhotoPlayActivity.PLAY_MODE,
							Photo4K2KPlayActivity.NORMAL_MODE);
				
			}

			intent.putExtra(TO_3DBROWSE, type);
			intent.putExtra(CURRENT_BROWSE_STATUS, mFromStatus);
			intent.putExtra(MEDIA_PATH, mManager.getCurrentPath());
			intent.putExtra(RECURSER_MODE, mRecuseMode);
			
			startActivity(intent);
			finish();
		} else if (type == MutilMediaConst.CONTENT_TEXT) {
			Intent intent = new Intent(this, TextPlayActivity.class);
			startActivity(intent);
		} else if (type == MutilMediaConst.CONTENT_THRDPHOTO) {
			Intent intent = new Intent(this, Photo3DPlayActivity.class);
			
			intent.putExtra(TO_3DBROWSE, type);
			intent.putExtra(CURRENT_BROWSE_STATUS, mFromStatus);
			intent.putExtra(MEDIA_PATH, mManager.getCurrentPath());
			intent.putExtra(RECURSER_MODE, mRecuseMode);
			
			startActivity(intent);
			
			finish();
		}
	}

	// --------------------------- event
	// start----------------------------------------
	private Handler mHandler = new Handler() {

		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			Bitmap bm = null;
			int textureId = 0;
			FileAdapter fAdapter = null;
			String p = null;
			switch (msg.what) {
			case MessageConst.MSG_THUM_FRESH_LEFT:

				mTexutreManager.moveLeft();
				textureId = mTexutreManager.getNextId();
				fAdapter = mBrowseList.getNext();

				MtkLog.d(TAG, "MSG_THUM_FRESH_LEFT path =" + fAdapter + "Texture ="
						+ textureId);
				// last file up to middle position.
				if (fAdapter == null) {
					setDefaultTexture(textureId);
					break;
				}
				
				bindView(fAdapter, textureId);

				break;
			case MessageConst.MSG_THUM_FRESH_RIGHT:

				mTexutreManager.moveRight();
				textureId = mTexutreManager.getPreId();
				fAdapter = mBrowseList.getPre();

				MtkLog.d(TAG, "MSG_THUM_FRESH_RIGHT path = " + fAdapter);
				if (fAdapter == null) {
					setDefaultTexture(textureId);
					break;
				}

				bindView(fAdapter, textureId);

				break;

			case MessageConst.MSG_THUM_FRESH_ADD:				
				textureId = msg.arg1;
				fAdapter = (FileAdapter)msg.obj;
				p = fAdapter.getPath();
				MtkLog.d(TAG, "MSG_THUM_FRESH_ADD cur path : " + mManager.getCurrentPath());
				MtkLog.d(TAG, "MSG_THUM_FRESH_ADD back path : " + mManager.getBackPath(p));
				if ( mRecuseMode == MtkFilesBaseListActivity.MODE_RECURSIVE
						||mManager.getCurrentPath().equals(mManager.getBackPath(p))) {
					// If target was change ,don't upload image to texture.
					bm = mThumbnailManager.getThumbnail(fAdapter);
					setTexture(bm, textureId, fAdapter);
				}

				break;
			case MessageConst.MSG_PLAY_ANIMAL:
				fAdapter = (FileAdapter)msg.obj;
				p = fAdapter.getPath();
				if (fAdapter.isDirectory()) {
					if (mRecuseMode == MtkFilesBaseListActivity.MODE_RECURSIVE){
						recursive();
					} else {
						openDir(p);
						resetDefaultTexture();
					}
				} else {
					luaStopVideo();
					// create playlist.
					int pos = mBrowseList.getSpecailIndx(p);
					mManager.getPlayList(pos);
					playFile(fAdapter);				
				}
				break;
								
			case MessageConst.MSG_START_ANIMAL:
				luaInit();
				initFilesManager();
				break;

			case MessageConst.MSG_EXIT_ANIMAL:
				luaExitAnim();

				break;

			case MessageConst.MSG_EXIT_APP:
				change2DFileMode();
				mThumbnailManager.destroyThumbnail();
				mBrowseList.cleanList();		
				finish();
				break;

			case MessageConst.MSG_RELOAD_CONTENT:
				reloadContent();
				break;

				
			//As below for test start
			/*case MessageConst.MSG_AUTO_PLAY_NEXT:
				luaMoveRight();
				
				break;
			
			case MessageConst.MSG_AUTO_PLAY_PRE:
				luaMoveLeft();
				break;*/
			
			//As below for test end.
			default:
				break;
			}
		}
	};
	
	protected void removePasteBitmapMsgs(){
		mHandler.removeMessages(MessageConst.MSG_THUM_FRESH_ADD);
	}

	// --------------------------- lua animal control
	// start----------------------------------------
	//for video small window play.
	private void resetOldFoucsTexture() {
		if (!mIsSupportVideoPlay) return;
		
		if (mManager.getContentType() != MultiFilesManager.CONTENT_VIDEO){
			return;
		}
		
		String textureName = mTexutreManager.getFoucsTextureName();
		MtkGLES2TextureInfo tInfo = mTexutreManager.getTextureInfo(textureName);
		if (tInfo != null) {
			Bitmap bm = null;
			bm = tInfo.getBitmap();
			if (bm.getWidth() <= 1 && bm.getHeight() <= 1){
				return;
			}
			luaStopVideo();
			// get focus file path.
			String path = null; //= tInfo.getPath();
			FileAdapter fa = tInfo.getFileAdapter();
			path = fa.getPath();

			bm = mThumbnailManager.getThumbnail(path);
			if (bm == null){

				if (fa.isDirectory()){
					bm = mFolder.getBitmap();
				}else{
					bm = mVideoIcon.getBitmap();
				}				
			}
			setTexture(bm, mTexutreManager.getId(mTexutreManager
					.getFoucsIndex()), fa);
		}
	}

	protected void luaStopVideo() {
		if (!mIsSupportVideoPlay) return;
		
		if (mManager.getContentType() != MultiFilesManager.CONTENT_VIDEO){
			return;
		}
		
		String textureName = mTexutreManager.getFoucsTextureName();
		MtkGLES2TextureInfo tInfo = mTexutreManager.getTextureInfo(textureName);
		if (tInfo != null) {
			Bitmap bm = null;
			bm = tInfo.getBitmap();
			if (bm.getWidth() <= 1 && bm.getHeight() <= 1){
				return;
			}
		}
		String str = "mPlayer = OgreKit.MTKMediaPlayer_getInstance();"
				+ "mPlayer.stop(mPlayer);";
		mEngineView.runScript(str, false);
	}

	private void luaMoveLeft() {
		
		//if no file, forbid move.
		if (mBrowseList.getNum() <= 0){
			MtkLog.d(TAG, "luaMoveLeft....file size = 0");
			return;
		}

		MtkGLES2TextureInfo info = mTexutreManager.getFoucsTextureInfo();		
		if (info != null){
			int foucsIdx = mBrowseList.getSpecailIndx(info.getPath());
			MtkLog.d(TAG, "luaMoveLeft...." + foucsIdx);
			// move to tail.
			if (foucsIdx + 1 >= mBrowseList.getNum()) return ;
		}
		
		if (mAnimalStatus == AnimalStatus.ANIMAL_END
				|| mAnimalStatus == AnimalStatus.ANIMAL_UNKNOW) {
			resetOldFoucsTexture();

			String chunk = "moveLeftAnim();";
			mAnimalStatus = AnimalStatus.ANIMAL_START;
			mEngineView.runScript(chunk, false);
			MtkLog.i(TAG, chunk);
		}
		
		//for Test.
		//mHandler.sendEmptyMessageDelayed(MessageConst.MSG_AUTO_PLAY_PRE, 3000);
	}

	private void luaMoveRight() {
		
		//if no file, forbid move.
		if (mBrowseList.getNum() <= 0) {
			MtkLog.d(TAG, "luaMoveRight.... file size = 0");
			return;	
		}
		
		MtkGLES2TextureInfo info = mTexutreManager.getFoucsTextureInfo();		
		if (info != null){
			int foucsIdx = mBrowseList.getSpecailIndx(info.getPath());
			MtkLog.d(TAG, "luaMoveRight...." + foucsIdx);
			//move to head.
			if (foucsIdx - 1 < 0) return ;
		}
			
		if (mAnimalStatus == AnimalStatus.ANIMAL_END
				|| mAnimalStatus == AnimalStatus.ANIMAL_UNKNOW) {
		resetOldFoucsTexture();

		String chunk = "moveRightAnim();";
		mAnimalStatus = AnimalStatus.ANIMAL_START;
		mEngineView.runScript(chunk, false);
		MtkLog.i(TAG, chunk);
		}
		
		//for Test.
		//mHandler.sendEmptyMessageDelayed(MessageConst.MSG_AUTO_PLAY_NEXT, 3000);
	}

	private void luaExitAnim() {
		if (mAnimalStatus == AnimalStatus.ANIMAL_END
				|| mAnimalStatus == AnimalStatus.ANIMAL_UNKNOW) {
			luaStopVideo();
			String chunk = "exitAnim();";
			mAnimalStatus = AnimalStatus.ANIMAL_START;
			mEngineView.runScript(chunk, false);
		}
	}

	private void luaInit() {
		if (mAnimalStatus == AnimalStatus.ANIMAL_END
				|| mAnimalStatus == AnimalStatus.ANIMAL_UNKNOW){
			String chunk = "initData(); startAnim();";
			mAnimalStatus = AnimalStatus.ANIMAL_START;
			mEngineView.runScript(chunk, false);
		}
	}

	// --------------------------- inline class start
	// ----------------------------------------
	public class LoadBitmapListener implements Listener {
		private int mTargetTextureId = 0;
		private String mPath;
		private FileAdapter fAdapter;

		/**
		 * @deprecated
		 * @param id
		 * @param p
		 */
		public LoadBitmapListener(int id, String p) {
			mTargetTextureId = id;
			mPath = p;
		}
		
		public LoadBitmapListener(int id, FileAdapter f) {
			mTargetTextureId = id;
			fAdapter = f;
		}
		

		public void onEvent() {
			Message msg = new Message();
			msg.what = MessageConst.MSG_THUM_FRESH_ADD;
			msg.arg1 = mTargetTextureId;
			msg.obj = fAdapter;
			mHandler.sendMessage(msg);
			return;

		}
	};

	public interface Listener {
		public void onEvent();
	};

	private MessageListener mGkListener = new MessageListener() {
		private String TAG = "MtkMainActivity.mGkListener";


		/**
		 * Here you can catch every message that is fired inside gamekits
		 * gkMessageManager.
		 */
		public void onMessage(String from, String to, String subject,
				String body) {
			MtkLog.d(TAG, "Message from gk:" + subject + ":" + body);
			if (subject != null && subject.equals(MessageConst.ANIMAL_END)) {
				if (body != null) {
					if (body.equals(MessageConst.ANIMAL_LEFT)) {
						mHandler
								.sendEmptyMessage(MessageConst.MSG_THUM_FRESH_LEFT);					
					} else if (body.equals(MessageConst.ANIMAL_RIGHT)) {
						mHandler
								.sendEmptyMessage(MessageConst.MSG_THUM_FRESH_RIGHT);
					} else if (body.equals(MessageConst.ANIMAL_EXIT)) {
						mHandler.sendEmptyMessage(MessageConst.MSG_EXIT_APP);
					}
				}
				mAnimalStatus = AnimalStatus.ANIMAL_END;
			}
		}
	};
	
	public void finish(){
		removePasteBitmapMsgs();
		super.finish();
	}

	@Override
	public void onLoaded() {
		// TODO Auto-generated method stub
		mHandler.sendEmptyMessage(MessageConst.MSG_START_ANIMAL);
	}
}
