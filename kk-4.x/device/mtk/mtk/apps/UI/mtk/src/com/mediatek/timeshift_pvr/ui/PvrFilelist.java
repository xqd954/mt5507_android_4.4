/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.ui;

import android.app.Activity;
import android.graphics.Color;
import android.os.Handler;
import android.os.Message;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.LinearLayout.LayoutParams;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.RelativeLayout;
import android.widget.TableLayout;
import android.widget.TextView;

import com.mediatek.timeshift_pvr.controller.PVRArrayAdapter;
import com.mediatek.timeshift_pvr.controller.PVRFiles;
import com.mediatek.timeshift_pvr.controller.StateBase;
import com.mediatek.timeshift_pvr.controller.StateFileList;
import com.mediatek.timeshift_pvr.controller.StatePVR;
import com.mediatek.timeshift_pvr.controller.StatusType;
import com.mediatek.timeshift_pvr.manager.Core;
import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.ui.R;
import com.mediatek.ui.util.MtkLog;

import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

/**
 *
 */
public class PvrFilelist extends PopupWindow {

	private int mDuration = 15;
	private final int mDefaultDuration = 10;

	private Activity mContext;
	private StateBase mState;

	private final int mMsgWhat = 0;

	private TextView[] mPVRFileInfo = new TextView[7];
	private ListView mPVRFileListLV;
	private OnItemClickListener listener;

	private RelativeLayout mInfoWindow;
	private TableLayout mInfoWindowDetail;

	private boolean showInfoWindow = false;

	private List<PVRFiles> mFileList = new ArrayList<PVRFiles>();
	private List<PVRFiles> mCurrentPageList = new ArrayList<PVRFiles>();

	private int mCurrentPage = 0;
	private int subStartindex =1;
	private int mMaxPage = 0;
	private final int TOTAL_ITEM_EVERY_PAGE = 5;

	private Timer timer;

	private final static float wScale = 0.8f;
	private final static float hScale = 0.8f;

	private final String CLSName = this.getClass().getSimpleName() + ".";

	private class filiListHandler extends Handler {
		public void handleMessage(Message msg) {
			// TODO Auto-generated method stub

			switch (msg.what) {
			default:

				break;
			}
			super.handleMessage(msg);
		}
	};

	private filiListHandler mHandler;
	private Handler handler;

	public PvrFilelist(Activity context, StateBase state,
			List<PVRFiles> pvrFileList,Handler handler) {
		super(context.getLayoutInflater().inflate(
				R.layout.pvr_timeshfit_playlist, null), (int) (state
				.getManager().getTVWidth() * wScale), (int) (state.getManager()
				.getTVHeight() * hScale));

		this.mContext = context;
		this.mFileList = pvrFileList;
		this.handler = handler;

		initViews();
		initList();

		mState = state;
	}

	/**
	 * 
	 */
	private void initViews() {
		TextView tv1 = (TextView) this.getContentView().findViewById(
				R.id.pvr_channel_num);
		TextView tv2 = (TextView) this.getContentView().findViewById(
				R.id.pvr_channel_str);
		TextView tv3 = (TextView) this.getContentView().findViewById(
				R.id.pvr_programename);
		TextView tv4 = (TextView) this.getContentView().findViewById(
				R.id.pvr_channel_date);
		TextView tv5 = (TextView) this.getContentView().findViewById(
				R.id.pvr_week);
		TextView tv6 = (TextView) this.getContentView().findViewById(
				R.id.pvr_time);
		TextView tv7 = (TextView) this.getContentView().findViewById(
				R.id.pvr_duration);

		mPVRFileInfo = new TextView[] { tv1, tv2, tv3, tv4, tv5, tv6, tv7 };

		mInfoWindow = (RelativeLayout) getContentView().findViewById(
				R.id.pvr_file_info);
		mInfoWindowDetail = (TableLayout) getContentView().findViewById(
				R.id.playlist_filemeta);

	}

	public PvrFilelist(Activity context, int layoutID, int duration,
			String strInfo) {
		super(context.getLayoutInflater().inflate(layoutID, null),
				LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);

		this.mContext = context;
		this.mDuration = duration;

		setFocusable(true);

		initList();
	}

	public void setDuration(int duration) {
		this.mDuration = duration;
	}

	public void show() {
		if (isShowing()) {
			timer.cancel();
		}

		showAtLocation(mContext.findViewById(Core.ROOT_VIEW), Gravity.RIGHT
				| Gravity.BOTTOM, 50, 50);

		mPVRFileListLV = (ListView) getContentView().findViewById(
				R.id.playlist_list);
		mPVRFileListLV.setFocusableInTouchMode(false);
		//mPVRFileListLV.setAdapter(new PVRArrayAdapter<PVRFiles>(mContext,
		//		android.R.layout.simple_list_item_1, mCurrentPageList));
		
		mPVRFileListLV.setAdapter(new PVRArrayAdapter<PVRFiles>(mContext,
				android.R.layout.simple_list_item_1, mCurrentPageList,subStartindex,0));
		
		//mPVRFileListLV.setAdapter(new PvrFileListAdapter(mCurrentPageList,mContext));

		Util.showDLog("PvrFile.show(),listSize:"+mCurrentPageList.size());
		mPVRFileListLV.setDivider(null);
		mPVRFileListLV.setSelected(true);

		mPVRFileListLV.requestFocus();
		mPVRFileListLV.setSelection(0);
		setSelection(0);
		mPVRFileListLV.setOnItemClickListener(getListener());

		boolean hasFocus = false;
		if (mPVRFileListLV.getCount() > 0) {
			try {
				String focusView = mPVRFileListLV.getFocusedChild().getClass()
						.getSimpleName();
				hasFocus = true;
				Util.showDLog("mPVRFileList.getFocusedChild().getClass().getSimpleName():::"
						+ focusView);

			} catch (Exception e) {
				hasFocus = false;
			}

		}

		if (hasFocus) {
			Util.showDLog("mPVRFileList,item,hasFocus");
		} else {
			Util.showDLog("mPVRFileList,item,noFocus");
		}

		startTimeTask();
	}

	public void setLVonItemClickListener(OnItemClickListener listener) {
		mPVRFileListLV.setOnItemClickListener(listener);
	}

	public void dimissInfobar() {
		stopTImerTask();

		super.dismiss();
	}

	private void startTimeTask() {
		timer = new Timer();
		mDuration=15;

		TimerTask refreshTask = new TimerTask() {
			public void run() {
				// refresh the word
				mDuration--;
			}
		};

		TimerTask task = new TimerTask() {
			public void run() {

				timer.cancel();
				if (null!=handler)
				    handler.sendEmptyMessage(StateFileList.AUTO_DISMISS_FILE_LIST);
//				PvrFilelist.this.dismiss();
			}
		};

		timer.schedule(refreshTask, 1 * 1000, 1 * 1000);
		timer.schedule(task, mDuration * 1000);
	}

	private void stopTImerTask() {
		try {
			if (null!=timer)
			    timer.cancel();

			timer = null;
		} catch (Exception e) {
			Util.showELog("stopTImerTask(), " + e.toString());
		}
	}

	public void reSetTimeTask() {
		if (isShowing()) {
			setDuration(mDefaultDuration);
			timer.cancel();

			startTimeTask();
		}
		Util.showDLog("---------------reSetTimeTask---------------");
	}

	private void initList() {
		mCurrentPageList = mFileList.subList(0,
				Math.min(mFileList.size(), TOTAL_ITEM_EVERY_PAGE));

		refreshMaxPage();
		/*if ((mFileList.size() % TOTAL_ITEM_EVERY_PAGE) != 0) {
			mMaxPage++;
		}*/
	}

	private void refreshMaxPage() {
		mMaxPage = mFileList.size() / TOTAL_ITEM_EVERY_PAGE;
		if ((mFileList.size() % TOTAL_ITEM_EVERY_PAGE) != 0) {
			mMaxPage++;
		}
	}

	/**
	 * @param keycode
	 */
	public void onKeyDown(int keycode) {
		int selection = mPVRFileListLV.getSelectedItemPosition();
		
		if (selection == AdapterView.INVALID_POSITION) {
			if(mPVRFileListLV.getCount()>0){
				mPVRFileListLV.setSelection(0);
			}else
			{
				return;
			}
		}
		
		switch (keycode) {
		case KeyEvent.KEYCODE_DPAD_CENTER:
			mPVRFileListLV.getOnItemClickListener().onItemClick(mPVRFileListLV,
					mPVRFileListLV.getSelectedView(), selection,
					mPVRFileListLV.getSelectedItemId());
			break;
		case KeyEvent.KEYCODE_DPAD_UP:

			selection--;

			if (selection < 0) {
				pageUP();
				selection = 0;
			}else{
				setselectItem(selection);
			}
			break;

		case KeyEvent.KEYCODE_DPAD_DOWN:
			selection++;

			if (selection >= mPVRFileListLV.getCount()) {
				pageDown();
				selection = 0;
			}else{
				setselectItem(selection);
			}			
			break;

		case KeyEvent.KEYCODE_MTKIR_GREEN:
			pageDown();
			selection = 0;
			break;
		case KeyEvent.KEYCODE_MTKIR_YELLOW:
			if(TimeShiftManager.getInstance().pvrIsRecording()&&((PVRFiles)mPVRFileListLV.getSelectedItem()).isRecording){
				if(StatePVR.getInstance()!=null){
					StatePVR.getInstance().clearPVRParams();
					mState.getManager().restoreToDefault(StatusType.PVR);
				}
			}else
			{
				Util.showDLog("delete file,not recording-file");
			}
			if(((StateFileList)mState).equalPlayingFile(((StateFileList)mState).getSelectedFile().getAbsolutePath())){
				((StateFileList)mState).deletePvrFile();
				mState.getManager().restoreToDefault(StatusType.FILELIST);
			}
			
			
			((StateFileList)mState).deletePvrFile();
			
		    
//			showDeleteConfirmDialog();
			break;
		case KeyEvent.KEYCODE_MTKIR_RED:
			pageUP();
			selection = 0;
			break;
		case KeyEvent.KEYCODE_MTKIR_INFO:
		case KeyEvent.KEYCODE_MTKIR_BLUE:
			info();
			break;
		case KeyEvent.KEYCODE_BACK:
			// if has other window,like source list,info bar...,when press back
			// button,
			// all the window will dismiss,so ,have to avoid this issue.

			// dimissInfobar();
			debugInfo();
			break;
		default:
			break;
		}
	}
	
	private void setselectItem(int selection){
		//int lastSelection=mPVRFileListLV.getSelectedItemPosition();
		setSelection(selection);
		try {
			Util.showDLog(CLSName + "mPVRFileList.getCount():"
					+ mPVRFileListLV.getCount());
			
			for (int i = 0; i < mPVRFileListLV.getCount(); i++) {
				 mPVRFileListLV.getChildAt(i).setBackgroundColor(Color.TRANSPARENT);
			}

			mPVRFileListLV.getChildAt(selection).setBackgroundResource(
					R.drawable.tk_cm_item_selected);
		} catch (Exception e) {
			e.printStackTrace();
			Util.showDLog(CLSName
					+ "mPVRFileList.getChildAt(selection).setBackgroundColor(Color.YELLOW)");
			e.toString();
		}

	}

	/**
	 * 
	 */
	private void debugInfo() {
		Util.showDLog("-----------------------------------------------------");
		Util.showDLog("mPVRFileList.size():" + mPVRFileListLV.getCount());
		Util.showDLog("mFileList.size():" + mFileList.size());
		Util.showDLog("mCurrentPageList.size():" + mCurrentPageList.size());
		Util.showDLog("mCurrentPage: " + mCurrentPage);
		Util.showDLog("mMaxPage: " + mMaxPage);
	}

	/**
	 * 
	 */
	private void info() {
		Util.showDLog(CLSName + "info()");

		if (showInfoWindow)
			mInfoWindow.setVisibility(View.INVISIBLE);
		else
			mInfoWindow.setVisibility(View.VISIBLE);

		showInfoWindow = !showInfoWindow;
	}

	/**
	 * 
	 */
	private void showDeleteConfirmDialog() {
		if (mCurrentPageList.size() < 1) {
			return;
		}
		PVRFileDeleteConfirmDialog dialog = new PVRFileDeleteConfirmDialog(
				mContext, mState);
		dialog.show();
	}

	public PVRFiles getSelectedFile() {
		int selection = mPVRFileListLV.getSelectedItemPosition();
		PVRFiles fileName = mCurrentPageList.get(selection);
		return fileName;
	}
	public void deleteFile() {
		int selection = mPVRFileListLV.getSelectedItemPosition();

		PVRFiles fileName = mCurrentPageList.get(selection);
		//boolean value = MtkTvRecordPlayback.getInstance().deletePlaybackListItem(selection);//(fileName.getAbsolutePath());
		boolean value = mState.getController().deletePVRFile(fileName);
		MtkLog.e("deletevalue", "deletevalue:"+value);
		if (value) {
			mCurrentPageList.remove(selection);
			int index = mFileList.indexOf(fileName);
			if (index != -1) {
				mCurrentPageList.remove(fileName);
			}

		} else {
			mCurrentPageList.remove(selection);
			
		}

		if (mCurrentPageList.size() == 0) {		
			pageUP();
		} else {
			refreshCurrentPage(selection==0?0:selection-1);
		}

		refreshMaxPage();
	}

	/**
	 * 
	 */
	private void pageDown() {
		Util.showDLog(CLSName + "pageDown()");
		++mCurrentPage;
		if(mCurrentPage>=mMaxPage){
			/*mCurrentPage=mMaxPage;
			mCurrentPageList = mFileList.subList(
					(mCurrentPage-1) * TOTAL_ITEM_EVERY_PAGE,
					Math.min((mCurrentPage) * TOTAL_ITEM_EVERY_PAGE,
							mFileList.size()));
			if(mCurrentPageList==null || mCurrentPageList.size()<1){
				mCurrentPage = 0;
			}else{
				
			}*/
			
			mCurrentPage = 0;
		}
		
		if(mCurrentPage<0){
            mCurrentPage=0;
        }
		subStartindex = mCurrentPage*TOTAL_ITEM_EVERY_PAGE+1;
		mCurrentPageList = mFileList.subList(
				mCurrentPage * TOTAL_ITEM_EVERY_PAGE,
				Math.min((mCurrentPage + 1) * TOTAL_ITEM_EVERY_PAGE,
						mFileList.size()));

		refreshAdapter(0);
		
	}

	/**
	 * 
	 */
	private void pageUP() {
		Util.showDLog(CLSName + "pageUP()");
		if(mCurrentPage==0){//Fix CR:DTV00583562
			if(0==mMaxPage){
				setSelection(mCurrentPageList.size()-1);
				return;
			}
			mCurrentPage = mMaxPage - 1;
			
			if(mCurrentPage<0){
				mCurrentPage=0;
			}
			subStartindex = mCurrentPage*TOTAL_ITEM_EVERY_PAGE+1;
			mCurrentPageList = mFileList.subList(
					mCurrentPage * TOTAL_ITEM_EVERY_PAGE,
					Math.min((mCurrentPage + 1) * TOTAL_ITEM_EVERY_PAGE,
							mFileList.size()));
			
			refreshAdapter(mCurrentPageList.size()-1);
			
			//setSelection(mCurrentPageList.size()-1);
		}else
		{
			mCurrentPage = Math.max(0, (--mCurrentPage));
			subStartindex = mCurrentPage*TOTAL_ITEM_EVERY_PAGE+1;
			mCurrentPageList = mFileList.subList(
					mCurrentPage * TOTAL_ITEM_EVERY_PAGE,
					Math.min((mCurrentPage + 1) * TOTAL_ITEM_EVERY_PAGE,
							mFileList.size()));	
			refreshAdapter(mCurrentPageList.size()-1);
		}
	}

	private void refreshCurrentPage(int selectItem) {
		mCurrentPageList = mFileList.subList(
				mCurrentPage * TOTAL_ITEM_EVERY_PAGE,
				Math.min((mCurrentPage + 1) * TOTAL_ITEM_EVERY_PAGE,
						mFileList.size()));
		refreshAdapter(selectItem);
	}

	/**
	 * 
	 */
	private void refreshAdapter(int selectItem) {
		PVRArrayAdapter<PVRFiles> pVRArrayAdapter=new PVRArrayAdapter<PVRFiles>(mContext,
				android.R.layout.simple_list_item_1, mCurrentPageList,subStartindex,selectItem);
		mPVRFileListLV.setAdapter(pVRArrayAdapter);
		//pVRArrayAdapter.notifyAll();
		
		
//		mPVRFileListLV.setAdapter(new PvrFileListAdapter(mCurrentPageList,mContext));
		//fix DTV00587114
		if(mFileList.size()==0){
			MtkLog.d("chengcl", "mFileList.size==0");
			return;
		}
		mPVRFileListLV.setSelection(selectItem);
		setSelection(selectItem);
		
		//setselectItem(selectItem);
	}

	void setSelection(int index) {

		mInfoWindowDetail.setLeft(10);
		
		if (index >= mPVRFileListLV.getCount()) {
			
			mPVRFileInfo[0].setText("");
			mPVRFileInfo[1].setText("");
			mPVRFileInfo[2].setText("");
			mPVRFileInfo[3].setText("");
			mPVRFileInfo[4].setText("");
			mPVRFileInfo[5].setText("");
			mPVRFileInfo[6].setText("");
			return;
		}
		mPVRFileListLV.setFocusableInTouchMode(true);
		mPVRFileListLV.setSelection(index);
		Util.showDLog("setSelection(),"+index);

		mPVRFileInfo[0].setText(mCurrentPageList.get(index).getChannelNum());
		mPVRFileInfo[1].setText(mCurrentPageList.get(index).getChannelName());
		mPVRFileInfo[2].setText(mCurrentPageList.get(index).getProgramName());
		mPVRFileInfo[3].setText(mCurrentPageList.get(index).getDate());
		mPVRFileInfo[4].setText(mCurrentPageList.get(index).getWeek());
		mPVRFileInfo[5].setText(mCurrentPageList.get(index).getTime());
		mPVRFileInfo[6].setText(mCurrentPageList.get(index).getDurationStr());
		Util.showDLog("refresh PVR files Channel Info");
	}

	/**
	 * @return the listener
	 */
	public OnItemClickListener getListener() {
		return listener;
	}

	/**
	 * @param listener
	 *            the listener to set
	 */
	public void setListener(OnItemClickListener listener) {
		this.listener = listener;
	}

}
