package com.mediatek.ui.commonview;

import java.util.List;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.ListView;

import com.mediatek.ui.util.PageImp;

/*
 * the ListView of Paging effect 
 */
public class CustListView extends ListView {

	private static final String TAG = "CustListView";
	/*
	 * Update the data to the interface, will require the development of
	 * personnel implementation
	 */
	private UpDateListView mUpdate;
	/*
	 * the implements class of Paging algorithm
	 */
	private PageImp mPageImp;
	/*
	 * The current of the selected item
	 */
	private int mSelectItem;

	/*
	 * Construction method
	 */
	public CustListView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	/*
	 * Construction method
	 */
	public CustListView(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	/*
	 * Construction method
	 */
	public CustListView(Context context) {
		super(context);
	}

	/*
	 * initial
	 */
	public void initData(List<?> list, int perPage, UpDateListView update) {
		mPageImp = new PageImp(list, perPage);
		mUpdate = update;
		// this.setOnItemSelectedListener(mItemSelectedListener);

	}

	/*
	 * initial
	 */
	public void initData(List<?> list, int perPage) {
		mPageImp = new PageImp(list, perPage);
		// this.setOnItemSelectedListener(mItemSelectedListener);
	}

	/*
	 * Access to current display data set
	 */
	public List<?> getCurrentList() {
		return mPageImp.getCurrentList();
	}

	/*
	 * Get the next page display data set
	 */
	public List<?> getNextList() {
		mPageImp.nextPage();
		return getCurrentList();
	}

	/*
	 * Get back page shows data set
	 */
	public List<?> getPreList() {
		mPageImp.prePage();
		return getCurrentList();
	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {

		mSelectItem = this.getSelectedItemPosition();
		Log.d(TAG, "onKeyDown mSelectItem:" + mSelectItem);
		switch (keyCode) {
		case KeyEvent.KEYCODE_DPAD_DOWN:
			/*
			 * When in the current page last line, and not the last page, and it
			 * changes the page
			 */
			if (mPageImp.getCount() <= 0) {
				return false;
			}

			if ((mSelectItem + 1) % mPageImp.getPerPage() == 0
					|| (mSelectItem + 1) % mPageImp.getCount() == 0) {
				mPageImp.nextPage();
				if (mUpdate != null) {
					mUpdate.updata();
				}
				// setOnItemSelectedListener(mItemSelectedListener);
				setSelection(0);
			}
			/*
			 * When in the current page last line, and is the last page, then
			 * back to the first item
			 */
			if ((mSelectItem + 1) % mPageImp.getPerPage() == 0
					&& mPageImp.getPageNum() == 1) {
				setSelection(0);
			}
			break;
		case KeyEvent.KEYCODE_DPAD_UP:
			/*
			 * When in the current page first line, and not the home page, is
			 * back to back
			 */

			Log.d(TAG, "onkey  up mSelectItem  =   " + mSelectItem
					+ " mPageImp.getPerPage()" + mPageImp.getPerPage());
			if (mPageImp.getCount() <= 0) {
				return false;
			}

			if (mSelectItem % mPageImp.getPerPage() <= 0) {

				if (mUpdate != null) {
					mUpdate.updata();
				}
				// setOnItemSelectedListener(mItemSelectedListener);
				if (mPageImp.getPerPage() < mPageImp.getCount()) {
					setSelection(mPageImp.getPerPage() - 1);
				} else {
					setSelection(mPageImp.getCount() - 1);
				}

			}
			/*
			 * When in the current page first line, and the current page is the
			 * home page, is back to the last line
			 */
			if (mSelectItem % mPageImp.getPerPage() == 0
					&& mPageImp.getPageNum() == 1) {
				setSelection(mPageImp.getPerPage() - 1);
			}
			break;
//		case KeyEvent.KEYCODE_DPAD_CENTER:
//		case KeyEvent.KEYCODE_ENTER:
//			return false;
		default:
			break;
		}
		return super.onKeyDown(keyCode, event);
	}

	// OnItemSelectedListener mItemSelectedListener = new
	// OnItemSelectedListener() {
	//
	// public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2,
	// long arg3) {
	// mSelectItem = arg2;
	// Log.d(TAG, "mItemSelectedListener mSelectItem:" + mSelectItem);
	// }
	//
	// public void onNothingSelected(AdapterView<?> arg0) {
	// }
	// };

	/*
	 * Data update to the interface
	 */
	public interface UpDateListView {
		void updata();
	}

	public PageImp getPageImp() {
		return mPageImp;
	}
}
