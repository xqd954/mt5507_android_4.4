package com.mediatek.ui.menu.commonview;

import java.util.List;

import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem.DataType;
import com.mediatek.ui.menu.util.PageImp;
import com.mediatek.ui.nav.commonview.EPGLinearLayout;

import android.content.Context;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.widget.LinearLayout;
import android.widget.ListAdapter;
import android.widget.ListView;

/*
 * the effect of ListView pagination
 */
public class CustListView extends ListView {

	private Context context;
	private PageImp mPageImp;
	// to save this ListView data collection
	private List<?> mListGroup;
	// ListView selected location
	private int mCurrentSelectedPosition = 0;
	// ListView on a correct position
	private int mBeforeSelectedPosition = 0;
	// the number of records per page
	private int pageSize;
	// page number
	private int pageNum;
	// remainder of the records
	private int mRemainderRecord;
	private int mLastEnableItemPosition;
	private int mFirstEnableItemPosition;

	private UpDateListView mUpdate;

	public CustListView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
	}

	public CustListView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
	}

	public CustListView(Context context) {
		super(context);
		this.context = context;
	}

	public void setAdapter(ListAdapter adapter) {
		super.setAdapter(adapter);
		mLastEnableItemPosition = getLastEnableItemPosition();
		mFirstEnableItemPosition = getFirstEnableItemPosition();
	}

	public int getPageNum() {
		return pageNum;
	}

	/*
	 * initialization
	 */
	public void initData(List<?> list, int perPage, UpDateListView update,
			int pageIndex) {
		mListGroup = list;
		mPageImp = new PageImp(list, perPage);
		this.pageSize = perPage;
		pageNum = mPageImp.getPageNum();
		mRemainderRecord = mPageImp.getCount() % pageSize;
		mUpdate = update;
		if (pageIndex > 0) {
			mPageImp.gotoPage(pageIndex);
		}
	}

	/*
	 * initialization
	 */
	public void initData(List<?> list, int perPage) {
		mPageImp = new PageImp(list, perPage);
	}

	// get the last available position
	private int getLastEnableItemPosition() {
		SetConfigListViewAdapter adapter = (SetConfigListViewAdapter) getAdapter();
		return adapter.getLastEnableItemPosition();
	}

	// get the first available position
	private int getFirstEnableItemPosition() {
		SetConfigListViewAdapter adapter = (SetConfigListViewAdapter) getAdapter();
		return adapter.getFirstEnableItemPosition();
	}

	/*
	 * get the current display data set
	 */
	public List<?> getCurrentList() {
		return mPageImp.getCurrentList();
	}

	/*
	 * get the page number of the current page
	 */
	public int getCurrentPageNum() {
		return mPageImp.getCurrentPage();
	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_DPAD_DOWN:
			mCurrentSelectedPosition = getSelectedItemPosition();
			if (mCurrentSelectedPosition < 0) {
				mCurrentSelectedPosition = mBeforeSelectedPosition;
			}

			mBeforeSelectedPosition = mCurrentSelectedPosition;
			DataItem mCurrentDataItem = (DataItem) getItemAtPosition(mCurrentSelectedPosition);
			if (mCurrentDataItem.getDataType() == DataType.DATETIMEVIEW) {
				DateTimeInputView childView = (DateTimeInputView) getSelectedView();
				childView.setCurrentSelectedPosition(-1);
			}
			if (mCurrentSelectedPosition == mLastEnableItemPosition) {
				/*
				 * when the last item is not the last page,flip page
				 */
				if (mPageImp.getCurrentPage() != pageNum) {
					mPageImp.nextPage();
					mUpdate.updata();
					setSelection(mFirstEnableItemPosition);
					break;
				}
				if (pageNum == 1) {
					setSelection(mFirstEnableItemPosition);
					break;
				}
				if (mPageImp.getCurrentPage() == mPageImp.getPageNum()) {
					mPageImp.headPage();
					mUpdate.updata();
					setSelection(mFirstEnableItemPosition);
					break;
				}
			}
			break;

		case KeyEvent.KEYCODE_DPAD_UP:
			/*
			 * When in the current page last line and only a page, then back to
			 * the first item
			 */
			mCurrentSelectedPosition = getSelectedItemPosition();
			if (mCurrentSelectedPosition < 0) {
				mCurrentSelectedPosition = mBeforeSelectedPosition;
			}
			mBeforeSelectedPosition = mCurrentSelectedPosition;
			DataItem mCurrentDataItem2 = (DataItem) getItemAtPosition(mCurrentSelectedPosition);
			if (mCurrentDataItem2.getDataType() == DataType.DATETIMEVIEW) {
				DateTimeInputView childView = (DateTimeInputView) getSelectedView();
				childView.setCurrentSelectedPosition(-1);
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

		default:
			break;
		}
		return super.onKeyDown(keyCode, event);
	}

	public boolean onKeyUp(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_DPAD_DOWN:
		case KeyEvent.KEYCODE_DPAD_UP:
			int mSeletedPosition = getSelectedItemPosition();
			if (mSeletedPosition < 0) {
				mSeletedPosition = 0;
			}
			DataItem mCurrentDataItem = (DataItem) getItemAtPosition(mSeletedPosition);
			if (mCurrentDataItem.getDataType() == DataType.DATETIMEVIEW) {
				DateTimeInputView childView = (DateTimeInputView) getSelectedView();
				childView.setCurrentSelectedPosition(0);
			}
			break;
		}
		return super.onKeyUp(keyCode, event);
	}

	public interface UpDateListView {
		void updata();
	}

	public void resetPosition() {
		mFirstEnableItemPosition = getFirstEnableItemPosition();
		mLastEnableItemPosition = getLastEnableItemPosition();
	}

}
