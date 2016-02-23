package com.mediatek.ui.mmp.multimedia;

import java.util.ArrayList;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnKeyListener;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;

import com.mediatek.mmpcm.CommonStorage;
import com.mediatek.mmpcm.mmcimpl.Const;
//import com.mediatek.mmpcm.text.ITextEventListener;
//import com.mediatek.mmpcm.textimpl.TextConst;
import com.mediatek.mmpcm.mmcimpl.PlayList;
import com.mediatek.ui.mmp.commonview.TextReader.LoadListener;

import com.mediatek.ui.mmp.commonview.TextReader;
import com.mediatek.ui.R;
import com.mediatek.ui.mmp.commonview.ControlView.ControlPlayState;
import com.mediatek.ui.mmp.model.MultiFilesManager;
import com.mediatek.ui.mmp.util.LogicManager;
import com.mediatek.ui.mmp.util.MultiMediaConstant;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;
import android.os.SystemProperties;
import com.mediatek.ui.mmp.util.TextUtils;


public class TextPlayActivity extends MediaPlayActivity {

	private static final String TAG = "TextPlayActivity";

	private static final int MESSAGE_PLAY = 0;

	private static final int MESSAGE_PRE = 1;

	private static final int MESSAGE_POPHIDE = 2;

	private static final int MESSAGE_ONMEASURE = 3;
	
	private static final int MESSAGE_PLAY_NEXT = 4;
	
	private static final int MESSAGE_SKIP_TO_PAGE = 5;
	
	private static final int MESSAGE_DISMISS = 6;

	private static final int MESSAGE_PLAY_STATE_SHOW = 7;

	private static final int MESSAGE_POPSHOWDEL = 10000;

	private static final int DELAY_TIME = 6000;

	private static final int DELAY_REQUEST_TOTALPAGE = 500;
	
	private static final int DELAY_REQUEST_SKIPTOPAGE = 6000;
	
	private static final int MAX_SIZE= 10;

	private LinearLayout vLayout;

	private TextReader vTextView;
	private PlayList mPlayList;

	private int mTextSource = 0;
	
	/*true play, false pause*/
	private boolean mPlayStauts = true;
	private boolean isActivityLiving = true;
	//added by keke 1.5
	private boolean mSKIPPlayStauts = false;

	private TextUtils mTextUtils;



	private ControlPlayState mControlImp = new ControlPlayState() {

		public void play() {
			mPlayStauts = true;
			mHandler.sendEmptyMessageDelayed(MESSAGE_PLAY, DELAY_TIME);
		}

		public void pause() {
			mPlayStauts = false;
			mHandler.removeMessages(MESSAGE_PLAY);
		}
	};

	private Handler mHandler = new Handler() {

		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			switch (msg.what) {
			case MESSAGE_PLAY:	
				playPage(true);
				break;
			case MESSAGE_PRE:	
				playPage(false);
				break;
			case MESSAGE_PLAY_NEXT:	
				playSpFile(mPlayList.getNext(Const.FILTER_TEXT, Const.MANUALNEXT));
				break;
			case MESSAGE_DISMISS:
				dismissNotSupprot();
				break;
			case MESSAGE_ONMEASURE: 
				if (null != mControlView) {
					if(isNotSupport){
						mControlView.setPhotoTimeType("");
					}else{
						mControlView.setPhotoTimeType(getPageNum());
					}
				}
				break;
			case MESSAGE_SKIP_TO_PAGE:
				skipToPage();
				break;
			case MESSAGE_PLAY_STATE_SHOW:
				mControlView.setPlayIcon();
				break;
			default:
				break;
			}
		}

	};

	private void playPage(boolean next){
		if(isNotSupport && !mHandler.hasMessages(MESSAGE_PLAY_NEXT)){
			mHandler.sendEmptyMessageDelayed(MESSAGE_PLAY_NEXT, DELAY_TIME);
			return;
		}
		if(next){
			vTextView.pageDown();
		}else{
			vTextView.pageUp();
		}
		
		if(null != mControlView ){
			mControlView.setPhotoTimeType(getPageNum());
		}
		if(mPlayStauts){
			mHandler.sendEmptyMessageDelayed(MESSAGE_PLAY, DELAY_TIME);
		}
	}
	


	private void playSpFile(String path){
		if (isActivityLiving) {
			dismissNotSupprot();
		}
		if (mHandler.hasMessages(MESSAGE_PLAY)) {
			mHandler.removeMessages(MESSAGE_PLAY);
		}
		
		try {
			if(menuDialog != null ){
				menuDialog.dismiss();
			}
			if(menuDialogFontList != null ){
				menuDialogFontList.dismiss();

			}
			
		} catch (Exception ex) {
			
			ex.printStackTrace();
		}
		
		vTextView.setPath(path);
		vTextView.play(true);
		reSetController();
		setControlView();

	}

	/**
	 * Set control bar info
	 */
	public void setControlView() {
		if (mControlView != null) {
			mControlView.setRepeat(Const.FILTER_TEXT);
			mControlView.setPhotoTimeType(getPageNum());
			mControlView.setFileName(mLogicManager
					.getCurrentFileName(Const.FILTER_TEXT));
			mControlView.setFilePosition(mLogicManager.getTextPageSize());
		}
		if(null!=mInfo && mInfo.isShowing())
		{
			mInfo.setTextView();
		}
	}

	/**
	 * {@inheritDoc}
	 */
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		if(0!=SystemProperties.getInt(AUTO_TEST_PROPERTY,0) ){
			autoTest(Const.FILTER_TEXT,MultiFilesManager.CONTENT_TEXT);
		}
		setContentView(R.layout.mmp_textplay);
		findView();
		getIntentData();
		init();
		//add by keke for fix DTV00381264
		mControlView.setRepeatVisibility(Const.FILTER_TEXT);
		showPopUpWindow(vLayout);
		MtkLog.d(TAG,"onCreate~~");
		setRepeatMode();
	}

	protected void onResume() {
		super.onResume();
		
		vTextView.play(true);
		MtkLog.d(TAG,"onResume~~");

		
	}


	private LoadListener txtLoadListener = new LoadListener(){
		
		
		public void onLoadDone(){
			MtkLog.d(TAG,"txtLoadListener onLoadDone");
			if (mPlayStauts) {
				if (mHandler.hasMessages(MESSAGE_PLAY)) {
					mHandler.removeMessages(MESSAGE_PLAY);
				}
				mHandler.sendEmptyMessageDelayed(MESSAGE_PLAY, DELAY_TIME);
			}
			mHandler.sendEmptyMessage(MESSAGE_ONMEASURE);


		}

		public void fileNotSupport(){
			MtkLog.d(TAG,"txtLoadListener fileNotSupport");
			isNotSupport = true;
			mHandler.sendEmptyMessage(MESSAGE_ONMEASURE);
			featureNotWork(TextPlayActivity.this.getResources().getString(
					R.string.mmp_file_notsupport));
			mHandler.sendEmptyMessage(MESSAGE_PLAY_STATE_SHOW);
			mHandler.sendEmptyMessageDelayed(MESSAGE_PLAY_NEXT, DELAY_TIME);
			
			
		}


		public void onComplete() {
			MtkLog.d(TAG,"txtLoadListener onComplete");
			if (mPlayStauts) {
				mHandler.removeMessages(MESSAGE_PLAY);
				mHandler.sendEmptyMessage(MESSAGE_PLAY_NEXT);
			}

		}

		public void onExit() {
			MtkLog.d(TAG,"txtLoadListener onExit~");
			finish();
		}
 	
		public void onPrepare() {
			MtkLog.d(TAG,"txtLoadListener onPrepare");
			isNotSupport = false;
			mHandler.sendEmptyMessage(MESSAGE_ONMEASURE);
		}	


	};

	private void init() {
		mTextUtils = TextUtils.getInstance(this);
		mLogicManager = LogicManager.getInstance(this);
		mPlayList = PlayList.getPlayList();
		vTextView.setPlayMode(mTextSource);
		vTextView.setPath(mPlayList.getCurrentPath(Const.FILTER_TEXT));
		
		vTextView.init(mTextUtils.getFontColor(),mTextUtils.getFontSize(),mTextUtils.getFontStyle());

		setControlView();
	}

	
	private void getIntentData() {
		
		mTextSource = MultiFilesManager.getInstance(this)
				.getCurrentSourceType();

		switch (mTextSource) {
		case MultiFilesManager.SOURCE_LOCAL:
			mTextSource = TextUtils.PLAYER_MODE_LOCAL;
			break;
		case MultiFilesManager.SOURCE_SMB:
			mTextSource = TextUtils.PLAYER_MODE_SAMBA;
			break;
		case MultiFilesManager.SOURCE_DLNA:
			mTextSource = TextUtils.PLAYER_MODE_DLNA;
			break;
		default:
			break;
		}
	}

	private void findView() {
		vLayout = (LinearLayout) findViewById(R.id.mmp_text);
		vTextView = (TextReader) findViewById(R.id.mmp_text_show);
		vTextView.setLoadListener(txtLoadListener);
		getPopView(R.layout.mmp_popuptext, MultiMediaConstant.TEXT, mControlImp);
		
		
	}
	
	
	
	/**
	 * {@inheritDoc}
	 */
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		MtkLog.d(TAG, "onKeyDown:" + keyCode);
		
		switch (keyCode) {
		//added by keke 1.5 for cr DTV00388058
		case KeyMap.KEYCODE_DPAD_CENTER:
			if (mHandler.hasMessages(MESSAGE_SKIP_TO_PAGE)) {
				mHandler.removeMessages(MESSAGE_SKIP_TO_PAGE);
				skipToPage();
				return true;
			}
			if(isNotSupport){
				return true;
			}
			break;
		//end 
		case KeyMap.KEYCODE_DPAD_UP:
			if (!isValid()){
				return true;
			}
			
			reSetController();
			mHandler.removeMessages(MESSAGE_PLAY);
			mHandler.sendEmptyMessage(MESSAGE_PRE);
			
			return true;
		case KeyMap.KEYCODE_DPAD_DOWN:
			if (!isValid()){
				return true;
			}
			reSetController();
			if(!vTextView.isEnd()){
				mHandler.removeMessages(MESSAGE_PLAY);
				mHandler.sendEmptyMessage(MESSAGE_PLAY);
			}
			return true;
		case KeyMap.KEYCODE_DPAD_LEFT:
			if (!isValid()){
				return true;
			}
			reSetController();
			mHandler.removeMessages(MESSAGE_PLAY);
			mHandler.sendEmptyMessage(MESSAGE_PRE);
			
			return true;
		case KeyMap.KEYCODE_DPAD_RIGHT:
			if (!isValid()){
				return true;
			}
			reSetController();
			if(!vTextView.isEnd()){
				mHandler.removeMessages(MESSAGE_PLAY);
				mHandler.sendEmptyMessage(MESSAGE_PLAY);
			}
			return true;
		case KeyMap.KEYCODE_MTKIR_PREVIOUS:
		case KeyMap.KEYCODE_MTKIR_CHDN:
			if (!isValid()){
				return true;
			}
			mHandler.removeMessages(MESSAGE_PLAY_NEXT);
			playSpFile(mPlayList.getNext(Const.FILTER_TEXT, Const.MANUALPRE));
			
			return true;
		case KeyMap.KEYCODE_MTKIR_NEXT:
		case KeyMap.KEYCODE_MTKIR_CHUP:
			if (!isValid()){
				return true;
			}
			mHandler.removeMessages(MESSAGE_PLAY_NEXT);
			playSpFile(mPlayList.getNext(Const.FILTER_TEXT, Const.MANUALNEXT));
			return true;
		case KeyMap.KEYCODE_VOLUME_DOWN:
		case KeyMap.KEYCODE_VOLUME_UP:
		case KeyMap.KEYCODE_MTKIR_MUTE: {
			if (null!=mLogicManager.getAudioPlaybackService()) {
				currentVolume=mLogicManager.getVolume();
				maxVolume=mLogicManager.getMaxVolume();
				break;
			} else {
				return true;
			}
		}
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
			reSetController();
			parseSkipToPage(keyCode);
			break;
			
		case KeyMap.KEYCODE_BACK: {
			finish();
			break;
		}	
		default:
			break;
		}

		return super.onKeyDown(keyCode, event);
	}
	

	
	private void removeMessage() {
		mHandler.removeMessages(MESSAGE_PLAY);
		mHandler.removeMessages(MESSAGE_ONMEASURE);
		mHandler.removeMessages(MESSAGE_PLAY_NEXT);
		mHandler.removeMessages(MESSAGE_DISMISS);
		dismissNotSupprot();
	}

	private int mSkipPage = 0;
	private ArrayList<Integer> mPageNum = new ArrayList<Integer>();
	protected void parseSkipToPage(int keyCode){
		int pageNum;
		if (keyCode < KeyMap.KEYCODE_0 || keyCode > KeyMap.KEYCODE_9){
			return;
		}
		pageNum = keyCode - KeyMap.KEYCODE_0;

		mPageNum.add(new Integer(pageNum));
		if (setSkipToPage(getSkipPage())) {
			mHandler.removeMessages(MESSAGE_SKIP_TO_PAGE);
			mHandler.sendEmptyMessageDelayed(MESSAGE_SKIP_TO_PAGE,
					DELAY_REQUEST_SKIPTOPAGE);
			
			// added by keke 1.05 for cr DTV00388058 and  DTV00387862
			if (mHandler.hasMessages(MESSAGE_PLAY)) {
				mHandler.removeMessages(MESSAGE_PLAY);
				mSKIPPlayStauts = true;
			}
			//end
		}
	}
	/**
	 * Get to skip to page.
	 * @return
	 */
	protected int getSkipPage(){
		int pageNum = 0;
		for (Integer p : mPageNum){
			pageNum *= 10;
			pageNum += p.intValue();
		}
		if (pageNum > Integer.MAX_VALUE){
			pageNum = Integer.MAX_VALUE;
		}
		if (pageNum <= vTextView.getTotalPage()){
			mSkipPage = pageNum;
		}
		MtkLog.d(TAG,"getSkipPage mSkipPage ="+mSkipPage);
		return mSkipPage;
	}
	
	protected void skipToPage(){		
		MtkLog.d(TAG,"getSkipPage mSkipPage ="+mSkipPage);
		int pageNum = mSkipPage;
		if (pageNum <= 0){
			return;
		}
		mPageNum.clear();
		reSetController();
		vTextView.skipToPage(pageNum);
		setPageSize();	
		// added by keke 1.5 for fix cr DTV00388058 and  DTV00387862
		if (mSKIPPlayStauts) {
			mHandler.sendEmptyMessageDelayed(MESSAGE_PLAY, DELAY_TIME);
			mSKIPPlayStauts = false;
		}
		//end
	}
	/**
	 * May be skip to page.
	 * @param num
	 * @return true to success, flase to failed
	 */
	protected boolean setSkipToPage(int num) {
		if (mControlView != null) {
			String result;
			int currentPos = num;
			int count = vTextView.getTotalPage();
			if (currentPos > 0 && count > 0 && num <= count) {
				result = currentPos + "/" + count;
				mControlView.setPhotoTimeType(result);
				return true;
			} 
		}
		
		return false;
	}
	
	
	

	private void setPageSize() {
		if (mControlView != null) {
			mControlView.setPhotoTimeType(getPageNum());
		}
	}


	
	private String getPageNum() {
			
		int currentPos = vTextView.getCurPagenum();
		int count = vTextView.getTotalPage();

		String result = "";
		if (currentPos > 0 && count > 0) {
			result = currentPos + "/" + count;
		}

		return result;
	}
	
	@Override
	protected void onDestroy() {
		
		isActivityLiving = false; 
		vTextView.release();
		removeMessage();
		super.onDestroy();
	}
	protected void setFontStyle(int style) {
		mTextUtils.saveFontStyle(style);
		vTextView.setFontStyle(style);
		
		
	}

	protected void setFontColor(int color) {
		mTextUtils.saveFontColor(color);
		vTextView.setFontColor(color);
		
	}


	protected void setFontSize(float size){
		mTextUtils.saveFontSize(size);
		vTextView.setFontSize(size);
		
	}
}
