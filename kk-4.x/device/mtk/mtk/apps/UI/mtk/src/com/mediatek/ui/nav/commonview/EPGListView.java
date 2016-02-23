package com.mediatek.ui.nav.commonview;

import java.util.List;

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.widget.LinearLayout;
import android.widget.ListAdapter;
import android.widget.ListView;

import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVEventManager.EventListener;
import com.mediatek.ui.nav.EPGActivity;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.nav.adapter.EPGListViewAdapter;
import com.mediatek.ui.nav.epginfo.EPGChannelInfo;
import com.mediatek.ui.nav.epginfo.EPGProgramInfo;
import com.mediatek.ui.nav.util.DataReader;
import com.mediatek.ui.nav.util.EPGConfig;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.PageImp;

public class EPGListView extends ListView {
	private static final String TAG = "EPGListView";
	private PageImp mPageImp;
	private int mCurrentSelectedPosition = 0;
	private int mLastRightSelectedPosition = 0;
	private int pageSize;
	private int pageNum;
	private int mLastEnableItemPosition;
	private int mFirstEnableItemPosition;
	private UpDateListView mUpdate;

	private EPGLinearLayout mSelectedItemView, mNextSelectedItemView;
	private LinearLayout mListViewChildView;
	private EPGListViewAdapter mListViewAdpter;
	private Handler mHandler;
	private EPGProgramInfo mLastSelectedTVProgram;

	public EPGProgramInfo getLastSelectedTVProgram() {
		return mLastSelectedTVProgram;
	}

	private EPGChannelInfo mCurrentChannel;

	private DataReader mReader;
	private EPGActivity mEPGAcivity;

	private static NavIntegration mNavIntegration;
	private EventListener evListener = new EventListener() {

		public void onChange(TVChannel ch) {

			mSelectedItemView = getSelectedDynamicLinearLayout(getSelectedItemPosition());
			if (mListViewAdpter != null && mSelectedItemView != null) {
				int _position = mSelectedItemView.getmCurrentSelectPosition();
				if (mCurrentChannel != null && _position != -1) {
					List<EPGProgramInfo> childTVProgram = mCurrentChannel
							.getmTVProgramInfoList();
					if (childTVProgram != null && childTVProgram.size() > 0){
						if(_position >= childTVProgram.size()){
							_position = childTVProgram.size() - 1;
							mSelectedItemView.setmCurrentSelectPosition(_position);
						}
						mLastSelectedTVProgram = childTVProgram.get(_position);
					}
				}
				mListViewAdpter.setGroup(mListViewAdpter.getGroup());
				mListViewAdpter.notifyDataSetChanged();
				EPGConfig.SELECTED_CHANNEL_POSITION = getSelectedItemPosition();
				EPGConfig.FROM_WHERE = EPGConfig.FROM_ANOTHER_STREAM;
				mHandler.sendEmptyMessageDelayed(
						EPGConfig.EPG_PROGRAMINFO_SHOW, 1000);
			}
		}
	};;

	public Handler getHandler() {
		return mHandler;
	}

	public void setHandler(Handler mHandler) {
		this.mHandler = mHandler;
	}

	public EPGListView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		if (context instanceof EPGActivity) {
			mEPGAcivity = (EPGActivity) context;
		}
		mReader = DataReader.getInstance(context);
		mNavIntegration = NavIntegration.getInstance(context);
		mReader.setChannelEventListener(evListener);
	}

	public EPGListView(Context context, AttributeSet attrs) {
		super(context, attrs);
		if (context instanceof EPGActivity) {
			mEPGAcivity = (EPGActivity) context;
		}
		mReader = DataReader.getInstance(context);
		mNavIntegration = NavIntegration.getInstance(context);
		mReader.setChannelEventListener(evListener);
	}

	public EPGListView(Context context) {
		super(context);
		if (context instanceof EPGActivity) {
			mEPGAcivity = (EPGActivity) context;
		}
		mReader = DataReader.getInstance(context);
		mNavIntegration = NavIntegration.getInstance(context);
		mReader.setChannelEventListener(evListener);
	}

	public void setAdapter(ListAdapter adapter) {
		mFirstEnableItemPosition = 0;
		mLastEnableItemPosition = adapter.getCount() - 1;
		mListViewAdpter = (EPGListViewAdapter) adapter;
		mListViewAdpter.setEPGListView(this);
		super.setAdapter(adapter);

	}

	public int getPageNum() {
		return pageNum;
	}

	/*
	 * init, loading data
	 */
	public void initData(List<?> list, int perPage, UpDateListView update,
			int pageIndex) {
		mPageImp = new PageImp(list, perPage);
		this.pageSize = perPage;
		pageNum = mPageImp.getPageNum();
		mUpdate = update;
		if (pageIndex > 1) {
			mPageImp.gotoPage(pageIndex);
		}
	}

	/*
	 * init ,loading data
	 */
	public void initData(List<?> list, int perPage) {
		mPageImp = new PageImp(list, perPage);
	}

	/*
	 * Get the current display data set
	 */
	public List<?> getCurrentList() {
		return mPageImp.getCurrentList();
	}

	/*
	 * get the current page num
	 */
	public int getCurrentPageNum() {
		return mPageImp.getCurrentPage();
	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {

		boolean handled = false;
		if (this == null || this.hasFocus() == false) {
			return false;
		}

		switch (keyCode) {
		case KeyMap.KEYCODE_MTKIR_CHUP:
			dispatchKeyEvent(new KeyEvent(KeyEvent.ACTION_DOWN,
					KeyEvent.KEYCODE_DPAD_DOWN));
			break;
		case KeyMap.KEYCODE_MTKIR_CHDN:
			dispatchKeyEvent(new KeyEvent(KeyEvent.ACTION_DOWN,
					KeyEvent.KEYCODE_DPAD_UP));
			break;
		case KeyEvent.KEYCODE_DPAD_LEFT:
			mHandler.removeMessages(EPGConfig.EPG_PROGRAMINFO_SHOW);

			mCurrentSelectedPosition = getSelectedItemPosition();
			if (mCurrentSelectedPosition < 0) {
				mCurrentSelectedPosition = mLastRightSelectedPosition;
			}
			mLastRightSelectedPosition = mCurrentSelectedPosition;
			mSelectedItemView = getSelectedDynamicLinearLayout(mCurrentSelectedPosition);
			if (mSelectedItemView != null) {
				int boundHours = mReader.getFirstDayStartTime();
				int index = mSelectedItemView.getmCurrentSelectPosition();
				MtkLog.d(TAG, "KeyEvent.KEYCODE_DPAD_LEFT---------index--->"
						+ index);
				if (mListViewAdpter.getDayNum() == 0
						&& mListViewAdpter.getStartTime() == boundHours
						&& index == 0) {
					return true;
				}
				handled = mSelectedItemView.onKeyLeft();

				if (handled == false) {
					handled = changeTimeZoom(keyCode);
				}
			}
			mHandler.sendEmptyMessageDelayed(EPGConfig.EPG_PROGRAMINFO_SHOW,
					1000);
			break;
		case KeyEvent.KEYCODE_DPAD_RIGHT:
			mHandler.removeMessages(EPGConfig.EPG_PROGRAMINFO_SHOW);

			mCurrentSelectedPosition = getSelectedItemPosition();
			if (mCurrentSelectedPosition < 0) {
				mCurrentSelectedPosition = mLastRightSelectedPosition;
			}
			mLastRightSelectedPosition = mCurrentSelectedPosition;
			mSelectedItemView = getSelectedDynamicLinearLayout(mCurrentSelectedPosition);

			if (mSelectedItemView != null) {
				int index = mSelectedItemView.getmCurrentSelectPosition();
				int boundHours = mReader.getFirstDayStartTime();
				if (mListViewAdpter.getDayNum() == EPGConfig.mMaxDayNum
						&& mListViewAdpter.getStartTime() == (boundHours
								- EPGConfig.mTimeSpan - 1)
						&& index == mSelectedItemView.getChildCount()) {
					return true;
				}
				handled = mSelectedItemView.onKeyRight();
				if (handled == false) {
					handled = changeTimeZoom(keyCode);
				}	
			}
			mHandler.sendEmptyMessageDelayed(EPGConfig.EPG_PROGRAMINFO_SHOW,
					1000);
			break;
		case KeyEvent.KEYCODE_DPAD_DOWN:
			EPGConfig.FROM_WHERE = EPGConfig.FROM_KEYCODE_MTKIR_OTHER;
			mHandler.removeMessages(EPGConfig.EPG_PROGRAMINFO_SHOW);

			mCurrentSelectedPosition = getSelectedItemPosition();
			if (mCurrentSelectedPosition < 0) {
				mCurrentSelectedPosition = mLastRightSelectedPosition;
			}
			mLastRightSelectedPosition = mCurrentSelectedPosition;
			mCurrentChannel = (EPGChannelInfo) getItemAtPosition(mCurrentSelectedPosition);

			mSelectedItemView = getSelectedDynamicLinearLayout(mCurrentSelectedPosition);
			if (mSelectedItemView != null) {
				int _position = mSelectedItemView.getmCurrentSelectPosition();
				mSelectedItemView.clearSelected();
				// next EPGLinearLayout should choose child view
				if (mCurrentChannel != null && _position != -1) {
					List<EPGProgramInfo> childTVProgram = mCurrentChannel
							.getmTVProgramInfoList();
					if (childTVProgram != null && childTVProgram.size() > 0)
						mLastSelectedTVProgram = childTVProgram.get(_position);
				} else {
					mLastSelectedTVProgram = null;
				}
			}

			// if postion is last item of the page,when keycode is
			// KeyEvent.KEYCODE_DPAD_DOWN
			if (mCurrentSelectedPosition == mLastEnableItemPosition) {
				/*
				 * if the current page is not last page
				 */
				if (mPageImp.getCurrentPage() != pageNum) {
					mPageImp.nextPage();
					mUpdate.updata();
					setSelection(mFirstEnableItemPosition);
					break;
				}

				/*
				 * if the count page num is one,the first position should be
				 * selected
				 */
				if (pageNum == 1) {
					setSelection(mFirstEnableItemPosition);
					break;
				}
				/*
				 * if the current page is last page,the first position of first
				 * page should be selected
				 */
				if (mPageImp.getCurrentPage() == mPageImp.getPageNum()) {
					mPageImp.headPage();
					mUpdate.updata();
					setSelection(mFirstEnableItemPosition);
					break;
				}
			}
			// navImp.iSetChannel(NavIntegration.CHANNEL_UP);
			break;

		case KeyEvent.KEYCODE_DPAD_UP:
			EPGConfig.FROM_WHERE = EPGConfig.FROM_KEYCODE_MTKIR_OTHER;
			mHandler.removeMessages(EPGConfig.EPG_PROGRAMINFO_SHOW);

			mCurrentSelectedPosition = getSelectedItemPosition();
			if (mCurrentSelectedPosition < 0) {
				mCurrentSelectedPosition = mLastRightSelectedPosition;
			}
			mLastRightSelectedPosition = mCurrentSelectedPosition;
			mCurrentChannel = (EPGChannelInfo) getItemAtPosition(mCurrentSelectedPosition);

			mSelectedItemView = getSelectedDynamicLinearLayout(mCurrentSelectedPosition);
			if (mSelectedItemView != null) {
				int _position = mSelectedItemView.getmCurrentSelectPosition();
				mSelectedItemView.clearSelected();
				if (mCurrentChannel != null && _position != -1) {
					List<EPGProgramInfo> childTVProgram = mCurrentChannel
							.getmTVProgramInfoList();
					if (childTVProgram != null && childTVProgram.size() > 0)
						mLastSelectedTVProgram = childTVProgram.get(_position);
				} else {
					mLastSelectedTVProgram = null;
				}
			}

			if (mCurrentSelectedPosition == mFirstEnableItemPosition) {
				if (mPageImp.getCurrentPage() != 1) {
					mPageImp.prePage();
					mUpdate.updata();
					setSelection(mLastEnableItemPosition);
					break;
				}
				if (pageNum == 1) {
					setSelection(mLastEnableItemPosition);
					break;
				}
				if (pageNum > 1 && mPageImp.getCurrentPage() == 1) {
					mPageImp.lastPage();
					mUpdate.updata();
					setSelection(mLastEnableItemPosition);
					break;
				}
			}
			// navImp.iSetChannel(NavIntegration.CHANNEL_DOWN);
			break;
		case KeyEvent.KEYCODE_DPAD_CENTER:
		case KeyEvent.KEYCODE_ENTER:
			return false;
		default:
			break;
		}
		return super.onKeyDown(keyCode, event);
	}

	private boolean changeTimeZoom(int keyCode) {
		int mStartTime = mListViewAdpter.getStartTime();
		int mDayNum = mListViewAdpter.getDayNum();

		if (keyCode == KeyEvent.KEYCODE_DPAD_LEFT) {
			EPGConfig.FROM_WHERE = EPGConfig.FROM_KEYCODE_DPAD_LEFT;

			if (mStartTime >= EPGConfig.mTimeSpan) {
				mStartTime = mStartTime - EPGConfig.mTimeSpan;
				mListViewAdpter.setStartTime(mStartTime);
			} else {
				if (mDayNum > 0) {
					mDayNum = mDayNum - 1;
					mListViewAdpter.setDayNum(mDayNum);
					mStartTime = 24 - EPGConfig.mTimeSpan + mStartTime;
					mListViewAdpter.setStartTime(mStartTime);
				} else {
					return false;
				}
			}
		} else if (keyCode == KeyEvent.KEYCODE_DPAD_RIGHT) {
			EPGConfig.FROM_WHERE = EPGConfig.FROM_KEYCODE_DPAD_RIGHT;

			if (mStartTime < 24 - EPGConfig.mTimeSpan) {
				mStartTime = mStartTime + EPGConfig.mTimeSpan;
				mListViewAdpter.setStartTime(mStartTime);
			} else {
				if (mDayNum < EPGConfig.mMaxDayNum) {
					mDayNum = mDayNum + 1;
					mListViewAdpter.setDayNum(mDayNum);
					mStartTime = (mStartTime + EPGConfig.mTimeSpan) % 24;
					mListViewAdpter.setStartTime(mStartTime);
				} else {
					return false;
				}
			}
		}
		mHandler.removeMessages(EPGConfig.EPG_DATA_RETRIEVING);
		mHandler.sendEmptyMessage(EPGConfig.EPG_DATA_RETRIEVING);
		Message message = mHandler.obtainMessage();
		message.what = EPGConfig.EPG_SYNCHRONIZATION_MESSAGE;
		message.arg1 = mDayNum;
		message.arg2 = mStartTime;
		mHandler.sendMessage(message);
		mListViewAdpter.setGroup(mListViewAdpter.getGroup(), mListViewAdpter
				.getDayNum());
//		 mListViewAdpter.notifyDataSetChanged();
		setAdapter(mListViewAdpter);
		setSelection(EPGConfig.SELECTED_CHANNEL_POSITION);

		return true;
	}

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		if (this == null || this.hasFocus() == false) {
			return false;
		}
		switch (keyCode) {
		case KeyMap.KEYCODE_MTKIR_CHUP:
			dispatchKeyEvent(new KeyEvent(KeyEvent.ACTION_UP,
					KeyEvent.KEYCODE_DPAD_DOWN));
			break;
		case KeyMap.KEYCODE_MTKIR_CHDN:
			dispatchKeyEvent(new KeyEvent(KeyEvent.ACTION_UP,
					KeyEvent.KEYCODE_DPAD_UP));
			break;
		case KeyEvent.KEYCODE_DPAD_UP:
		case KeyEvent.KEYCODE_DPAD_DOWN:
			EPGConfig.avoidFoucsChange = true;
			EPGConfig.SELECTED_CHANNEL_POSITION = getSelectedItemPosition();
			if (EPGConfig.SELECTED_CHANNEL_POSITION < 0) {
				EPGConfig.SELECTED_CHANNEL_POSITION = 0;
			}
			mCurrentChannel = (EPGChannelInfo) getItemAtPosition(EPGConfig.SELECTED_CHANNEL_POSITION);
			if (mCurrentChannel != null) {
				mNextSelectedItemView = getSelectedDynamicLinearLayout(EPGConfig.SELECTED_CHANNEL_POSITION);
				int postion = mCurrentChannel
						.getNextPosition(mLastSelectedTVProgram);
				if (mNextSelectedItemView != null) {
				    mNextSelectedItemView.setSelectedPosition(postion);
				}
				if (!mCurrentChannel.getTVChannel().equals(
						mNavIntegration.iGetCurrentChannel())) {
					mNavIntegration.iSelectChannel(mCurrentChannel
							.getTVChannel());
				}
			}

			mHandler.sendEmptyMessageDelayed(EPGConfig.EPG_PROGRAMINFO_SHOW,
					1000);
			break;
		default:
			break;
		}
		return super.onKeyUp(keyCode, event);
	}

	/*
	 * update data interface
	 */
	public interface UpDateListView {
		void updata();
	}

	public EPGLinearLayout getSelectedDynamicLinearLayout(int mPosition) {
		EPGLinearLayout mDynamicLinearLayout = null;
		mListViewChildView = (LinearLayout) getChildAt(mPosition);
		if (mListViewChildView != null) {
			mDynamicLinearLayout = (EPGLinearLayout) mListViewChildView
					.getChildAt(1);
		} else {
			mListViewChildView = (LinearLayout) getSelectedView();
			if (mListViewChildView != null)
				mDynamicLinearLayout = (EPGLinearLayout) mListViewChildView
						.getChildAt(1);
		}
		return mDynamicLinearLayout;
	}

	public void rawChangedOfChannel(){
		mSelectedItemView = getSelectedDynamicLinearLayout(getSelectedItemPosition());
		if (mListViewAdpter != null && mSelectedItemView != null) {
			int _position = mSelectedItemView.getmCurrentSelectPosition();
			if (mCurrentChannel != null && _position != -1) {
				List<EPGProgramInfo> childTVProgram = mCurrentChannel
						.getmTVProgramInfoList();
				if (childTVProgram != null && childTVProgram.size() > 0){
					if(_position >= childTVProgram.size()){
						_position = childTVProgram.size() - 1;
						mSelectedItemView.setmCurrentSelectPosition(_position);
					}
					mLastSelectedTVProgram = childTVProgram.get(_position);
				}
			}
			mSelectedItemView.clearSelected();
			mListViewAdpter.setGroup(mListViewAdpter.getGroup());
			setAdapter(mListViewAdpter);
			setSelection(EPGConfig.SELECTED_CHANNEL_POSITION);
//			mListViewAdpter.notifyDataSetChanged();
			
			EPGConfig.SELECTED_CHANNEL_POSITION = getSelectedItemPosition();
			if(EPGConfig.avoidFoucsChange){
				EPGConfig.FROM_WHERE = EPGConfig.FROM_ANOTHER_STREAM;
			}else{
				EPGConfig.FROM_WHERE = EPGConfig.AVOID_PROGRAM_FOCUS_CHANGE;
			}
			mHandler.sendEmptyMessageDelayed(
					EPGConfig.EPG_PROGRAMINFO_SHOW, 1000);
		}
	}
}
