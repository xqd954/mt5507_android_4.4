package com.mediatek.ui.mmp.commonview;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.io.InputStream;
import java.io.FileOutputStream;

import com.mediatek.ui.mmp.model.TextPage;
import com.mediatek.ui.mmp.util.TextUtils;

import android.content.Context;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.os.Handler;
import android.os.Message;
import android.text.TextPaint;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.TextView;
import android.os.Process;
import com.mediatek.ui.R;


import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.AsyncTask.Status;
import android.os.HandlerThread;
import com.mediatek.netcm.samba.SambaManager;


public class TextReader extends TextView {
	private static final String TAG = "TextReader";


	private static final String localPath = "/tmp/text_read_temp.txt";
	private static File tmpFile; 
	private int bufSize = 64 * 1024;

	private static final int REFEASHTEXT = 0;
	private static final int MSG_FLUSH = 1;
	private final static int MSG_ADD_WORK = 1;
	private final static int MSG_PLAY_WORK = 2;

	private List<TextPage> mPageList;
	//current show page index.
	private int mCurPageIndex;

	private int mPlayMode = TextUtils.PLAYER_MODE_LOCAL;
	//view width
	private int mWidth = 1920;
	//view height
	private int mHeight = 1080;
	//view paint.
	private Paint mPaint;
	//save file position, use for font size change reload file.
	private int lastLine = 1;
	//play file path
	private String mPath;
	//size of each line least.
	private int leastLineSize = 0;
	//mRefreshFile : flag about exchange load file use to cancel last loading work.
	private boolean mRefreshFile = false;
	//mRefreshed : flag about load file if show on screen.
	private boolean mRefreshed = false;
	//load file thread
	private static HandlerThread mThread;
	//load file handler
	private Handler mLoadFileHandler;

	private String mConstFont ;

	private boolean isLoading = false;


	private LoadListener mLoadListener;
	/**
	 * Asynchronous load files task
	 */
	//private LoadFilesTask mTask;

	
	//main handler refresh ui.
	private Handler mHandler = new Handler() {

		public void handleMessage(Message msg) {
			switch (msg.what) {
			case REFEASHTEXT:
				Log.d(TAG, "REFEASHTEXT mCurPageIndex = "+ mCurPageIndex);
				TextPage tmpPage = getPageByNum(mCurPageIndex);
				if (tmpPage != null) {
					lastLine = tmpPage.getStartPage();
					setText(tmpPage.getContent());	
				}
				break;
			case MSG_FLUSH:
				setText("");
				break;
			}

		}
	};


	public interface LoadListener{
		public void onLoadDone();
		/**
		 * file not support
		 */
		public void fileNotSupport();
		
		/**
		 * file load complete
		 */
		public void onComplete();
		
		/**
		 * File start to play;
		 */
		public void onPrepare();
		/**
		 * exit
		 */
		public void onExit();
			

	}


	/**
	 * set play mode
	 */
	public void setPlayMode(int playMode) {
		mPlayMode = playMode;
	}


	public void setLoadListener(LoadListener listener){

		mLoadListener = listener;
	}

	public void release(){
		mRefreshFile = true;
		if(tmpFile != null && tmpFile.exists()){
			tmpFile.delete();
		}
	}

	/*
	 * readerStatus: 0 Normal 1 Abnormal 2 Busy loading the text
	 */
	private int readerStatus = TextUtils.STATUS_NORMAL;

	public TextReader(Context context) {
		this(context, null, 0);
	}

	public TextReader(Context context, AttributeSet attrs) {
		this(context, attrs, 0);
	}

	public TextReader(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		mConstFont = context.getResources().getString(R.string.mmp_txt_const_font);
		mThread = new HandlerThread(TAG, Process.THREAD_PRIORITY_DEFAULT);
		mThread.start();
		mLoadFileHandler = new Handler(mThread.getLooper()) {
			public void handleMessage(Message msg) {

				switch (msg.what) {
				case MSG_ADD_WORK: 
					Log.d(TAG, mThread.getName() ); 
					Long start = System.currentTimeMillis();
					prepare();
					loadPage(mPath);
					
					Log.d(TAG, mThread.getName() + ":" +" leave cost time:" + 
							(System.currentTimeMillis() -start) );

					break;
				case MSG_PLAY_WORK: 
					Log.d(TAG, "mLoadFileHandler for play msg." ); 
					loadPage(mPath);
					break;
				default:
					break;
				
				}

			}
		};
		
	}


	public void init(int color,float size,int style){
		super.setTextSize(size);
		super.setTextColor(color);
		super.setTypeface(Typeface.DEFAULT, style);

	}

	public void setPath(String path){
		if(mPath != path){
			mPath = path;
			
			if(mPlayMode != TextUtils.PLAYER_MODE_LOCAL && tmpFile != null && tmpFile.exists()){
				tmpFile.delete();
			}
		}
		lastLine = 1;
		//mRefreshFile = true;
//		cancelLoadFile();
	}

	private void initLoadReader() {

		Log.d(TAG, "initReader ");
		mPageList = new ArrayList<TextPage>();
		readerStatus = TextUtils.STATUS_NORMAL;
		mCurPageIndex = 0;
		mRefreshed = false;
		mRefreshFile = false;
		isLoading = true;
		mPaint = this.getPaint();
		int fontSize = (int)(mWidth / mPaint.measureText(mConstFont));
		leastLineSize = fontSize;


		
	}
	
	@Override
	protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		// TODO Auto-generated method stub
		super.onMeasure(widthMeasureSpec, heightMeasureSpec);
		Log.d(TAG,
				"onMeasure wid = " + this.getWidth() + " h ="
						+ this.getHeight());
		if(this.getWidth() > 0 && this.getHeight() >0){
			mWidth = this.getWidth();
			mHeight = this.getHeight();

		}
		
		
	}

	private TextPage getPageByNum(int pageNum) {
		Log.d(TAG, "getPageByNum pageNum= " + pageNum);
		TextPage tmpPage = null;
		if (mPageList != null && pageNum >= 0 && pageNum < mPageList.size()) {
			tmpPage = mPageList.get(pageNum);

		}
		Log.d(TAG, "getPageByNum tmpPage= " + (tmpPage!=null ? tmpPage.getStartPage(): null) );
		return tmpPage;

	}
	
	

	
	
	
	private void loadPage(String path){
		FileInputStream fis = null;
		BufferedReader br = null;

		try {
			initLoadReader();
			File file = new File(path);
			fis = new FileInputStream(file);
			br = new BufferedReader(
					new InputStreamReader(fis, TextUtils
							.getCharset(file)));
			int pageLine = mHeight / getLineHeight();
			Log.d(TAG, "Page Play loadFile" + pageLine);
			String line;
			int pageLineIndex = 0;
			int lineIndex = 1;
			int curPageLine = 0;
			StringBuilder sb = new StringBuilder();
			if(mLoadListener!= null){
				if ( file.length() == 0) {
					mLoadListener.fileNotSupport();
					readerStatus = TextUtils.STATUS_ABNORMAL;
					return ;
				} else{
					mLoadListener.onPrepare();
				}
			}
			Log.d(TAG,"TextReader_play_text_test_pass");
			while ((line = br.readLine()) != null && !mRefreshFile ) {
				String subString = line;
				while( mPaint.measureText(subString) > mWidth && !mRefreshFile){
					int subLen = subString.length();
					int index = leastLineSize < subLen ? leastLineSize : subLen;
					for(;index <= subLen;index++ ){
						if(mRefreshFile){

							break;
						}
						String newString = subString.substring(0, index);
						if(mPaint.measureText(newString) > mWidth){
							sb.append(subString.substring(0, index-1)).append("\n");
							pageLineIndex++;
							if (pageLineIndex > 0 && pageLineIndex % pageLine == 0) {
								addPage(curPageLine ,sb.toString());
								curPageLine = lineIndex;
								sb = new StringBuilder();

							}
							break;
						}
						
	
					}
					
					subString = subString.substring(index-1, subString.length());
					Log.d(TAG,"line > width last subString");

					
				}
				
				
				
				sb.append(subString).append("\n");
				pageLineIndex++;
				
				lineIndex++;
				
				if (pageLineIndex > 0 && pageLineIndex % pageLine == 0) {
					addPage(curPageLine ,sb.toString());
					curPageLine = lineIndex;
					sb = new StringBuilder();

				}

			}
			if(!mRefreshFile){
				if (sb.length() > 0) {
					addPage(curPageLine ,sb.toString());
					

				}
				
				
				if(!mRefreshed && mPageList != null && mPageList.size() > 0){
					Log.d(TAG,"load file done pagesize = "+mPageList.size());
					mCurPageIndex = mPageList.size()-1;
					mHandler.sendEmptyMessage(REFEASHTEXT);
			
				}

				
				if(mLoadListener!= null){
				
					mLoadListener.onLoadDone();
				}
				
			}

			
			
			

		} catch (Exception ex) {

			ex.printStackTrace();
			readerStatus = TextUtils.STATUS_ABNORMAL;
			if(mLoadListener!= null){
				mLoadListener.fileNotSupport();
			}
		}finally{
			isLoading = false;

			try{
				br.close();

			}catch(Exception e){

				e.printStackTrace();
			}

		}
		
		
	}
	
	

	
	private void addPage(int line,String sub){
		TextPage page = new TextPage(line, sub);
		mPageList.add(page);
		Log.d(TAG,"add page startLine = "+ line +" pageNum = "+mPageList.size());
		
		
	
		if(!mRefreshed){
			if(line == lastLine){
				mCurPageIndex = mPageList.size()-1;
				mHandler.sendEmptyMessage(REFEASHTEXT);
				mRefreshed = true;
			}else if(line > lastLine){
				mCurPageIndex = mPageList.size()-2;
				mHandler.sendEmptyMessage(REFEASHTEXT);	
				mRefreshed = true;
			}
		}
		
		
	}

	public void play(boolean prepare) {

		mHandler.sendEmptyMessage(MSG_FLUSH);
		if (mPath == null || mPath.length() <= 0) {
			readerStatus = TextUtils.STATUS_ABNORMAL;
			Log.d(TAG,"fileNotSupport play filepath = "+ mPath );
			if(mLoadListener!= null){
				mLoadListener.onExit();
			}
			return;
		}
		mRefreshFile = true;
		if(mLoadFileHandler != null){
			
			mLoadFileHandler.sendEmptyMessage(prepare ? MSG_ADD_WORK : MSG_PLAY_WORK);
		}


	}


	private boolean  prepare(){
		Log.d(TAG,"prepare mPlayMode = "+ mPlayMode);
		if(mPlayMode != TextUtils.PLAYER_MODE_LOCAL){
		
			if (mPlayMode == TextUtils.PLAYER_MODE_SAMBA) {

				try {
					mPath = getLocalPath(SambaManager.getInstance()
							.getSambaDataSource(mPath).newInputStream());
				} catch (Exception e) {
					e.printStackTrace();
					readerStatus = TextUtils.STATUS_ABNORMAL;
					if(mLoadListener!= null){
						mLoadListener.fileNotSupport();

					}
					return false;
				}

			} else {
				readerStatus = TextUtils.STATUS_ABNORMAL;
				if(mLoadListener!= null){
					mLoadListener.onExit();
				}
				return false;
			}
		}

		return true;


	}

	private  String getLocalPath(InputStream input) throws Exception {
		//String path = localPath;
		//int bufSize = 1024;
		tmpFile = new File(localPath);

		tmpFile.deleteOnExit();

		byte[] buffer = new byte[bufSize];
		FileOutputStream output = new FileOutputStream(tmpFile);
		int ret ;
		while ((ret = input.read(buffer)) != -1) {
			output.write(buffer, 0, ret);
		}


		output.close();
		input.close();
		
		return localPath;
	}

	public void pageUp() {
		Log.d(TAG, "prePage mCurPageIndex = "+ mCurPageIndex);
		if (readerStatus != TextUtils.STATUS_NORMAL) {
			Log.d(TAG, "prePage No Textview specified, Need to set a Textview");
			return;
		}
		
		if(mCurPageIndex > 0){
			mCurPageIndex--;
			mHandler.sendEmptyMessage(REFEASHTEXT);
		}
		
	}

	public void pageDown() {
		Log.d(TAG, "nextPage mCurPageIndex = "+ mCurPageIndex + "pagesize = " +mPageList.size());
		if (readerStatus != TextUtils.STATUS_NORMAL) {
			Log.d(TAG, "nextPage No Textview specified, Need to set a Textview");
			return;
		}
		
		if(mPageList != null ){

			if(mCurPageIndex < mPageList.size()-1){
				mCurPageIndex++;
				mHandler.sendEmptyMessage(REFEASHTEXT);

			}else{
				if(mLoadListener!= null && !isLoading){
					mLoadListener.onComplete();
				}
			}
			
		}
	
	}



	public boolean isEnd(){
			
		if (readerStatus != TextUtils.STATUS_NORMAL) {
			Log.d(TAG, "nextPage No Textview specified, Need to set a Textview");
			return true;
		}
		if(mPageList != null ){
			if(mCurPageIndex < mPageList.size()-1){
				return false;
			}
		}

		return true;

		

	}

	/**
	 * skip to page you want
	 * 
	 * @param pageNum
	 */
	public void skipToPage(int pageNum) {
		Log.d(TAG, "skipToPage pageNum= " + pageNum);
		if (readerStatus != TextUtils.STATUS_NORMAL) {
			Log.d(TAG, "No Textview specified, Need to set a Textview");
			return;
		}


		if(mPageList != null ){
		
			if(pageNum <= mPageList.size()){
				mCurPageIndex = pageNum - 1;
				mHandler.sendEmptyMessage(REFEASHTEXT);

			}
		}



	}

	/**
	 * load need play position
	 * 
	 * @param pos
	 */
	public void loadPosByLine(int lineNum) {

	}

	/**
	 * Get text of the total number of pages
	 */
	public int getTotalPage() {
		Log.d(TAG, "getTotalPage ~");
		if (readerStatus != TextUtils.STATUS_NORMAL) {
			Log.d(TAG, "No Textview specified, Need to set a Textview");
			return 0;
		}
		int totalPage = 0;
		if (mPageList != null) {
			totalPage = mPageList.size();
		}
		Log.d(TAG, "getTotalPage totalPage = " + totalPage);
		return totalPage;
	}

	/**
	 * gain current page number
	 */
	public int getCurPagenum() {
		Log.d(TAG, "getCurPagenum ~");
		if (readerStatus != TextUtils.STATUS_NORMAL) {
			Log.d(TAG, "No Textview specified, Need to set a Textview");
			return 0;
		}

		return mCurPageIndex+1;

	}

	/**
	 * set font color
	 * 
	 * @param color
	 *            the string color name such as"red"
	 */
	public void setFontColor(int color) {

		if (readerStatus != TextUtils.STATUS_NORMAL) {
			Log.d(TAG,
					"No Textview specified, Need to set a Textview  readerStatus = "
							+ readerStatus);
			return;
		}
		super.setTextColor(color);
	}

	/**
	 * set font size
	 * 
	 * @param size
	 *            the less than 50 greater than 1 type floating point number
	 */
	public void setFontSize(float size) {

		if (readerStatus != TextUtils.STATUS_NORMAL) {
			Log.d(TAG,
					"No Textview specified, Need to set a Textview  readerStatus = "
							+ readerStatus);
			return;
		}
		if (size < 1f || size > 50f) {
			Log.d(TAG, "Not specify the correct size,  1f - 50f is avaliable");
		} else {

			super.setText("");
			super.setTextSize(size);
			play(false);

		}


	}

	/**
	 * set font style
	 * 
	 * @param typeface
	 *            Change the font type of the surface of the tv
	 * @param style
	 *            change the font style
	 */
	public void setFontStyle(int style) {

		if (readerStatus != TextUtils.STATUS_NORMAL) {
			Log.d(TAG,
					"No Textview specified, Need to set a Textview  readerStatus = "
							+ readerStatus);
			return;
		}
		super.setText("");
		super.setTypeface(Typeface.DEFAULT, style);

		if (style == Typeface.BOLD || style == Typeface.BOLD_ITALIC) {
			TextPaint tp = this.getPaint();
			tp.setFakeBoldText(true);
		}
		play(false);

	}


	

}
