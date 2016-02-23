package com.mediatek.ui.nav.util;

import android.text.Layout;
import android.widget.ScrollView;
import android.widget.TextView;

import com.mediatek.ui.nav.epginfo.EPGChannel;
import com.mediatek.ui.util.MtkLog;

public class DetailTextReader {
	private TextView tv;
	private static DetailTextReader tr = null;
	private static String TAG = "DetailTextReader";
	private int PAGE_LINE = 5;
	private int curPage = 1;

	private TextReaderPageChangeListener pageChangeListener;

	private DetailTextReader() {
	}

	public void registerPageChangeListener(
			TextReaderPageChangeListener textReaderPageChangeListener) {
		this.pageChangeListener = textReaderPageChangeListener;
	}

	public static DetailTextReader getInstance() {
		if (tr == null) {
			MtkLog.i(TAG, "First Create the TextReader Object");
			tr = new DetailTextReader();
		}
		MtkLog.i(TAG, "Get the existed TextReader Object");
		return tr;
	}

	public void autoScroll(int arg0) {
		// TODO Auto-generated method stub

	}

	public void exitSearch() {
		// TODO Auto-generated method stub

	}

	public int getCurPagenum() {
		if (tv == null) {
			MtkLog.i(TAG, "No Textview specified, Need to set a Textview");
			return 0;
		}
		if (tv.getHeight() == 0) {
			MtkLog
					.i(TAG,
							"Need to refresh the textview for getting the correct pagenum");
			return 0;
		} else {
			return curPage;
		}
	}

	public int getCurPos() {
		// TODO Auto-generated method stub
		if (tv == null) {
			MtkLog.i(TAG, "No Textview specified, Need to set a Textview");
			return 0;
		}
		Layout l = tv.getLayout();
		int line = l.getLineForVertical(tv.getScrollY()
				+ tv.getTotalPaddingTop());
		int off = l.getOffsetForHorizontal(line, tv.getScrollX()
				+ tv.getTotalPaddingLeft());
		MtkLog.i(TAG, "Get the current Position:" + off);
		return off;
	}

	// public String getCurText() {
	// if (tv == null) {
	// MtkLog.i(TAG, "No Textview specified, Need to set a Textview");
	// return null;
	// }
	// Layout l = tv.getLayout();
	// int startLine = l.getLineForVertical(
	// tv.getScrollY() + tv.getTotalPaddingTop());
	// int start = tv.getLayout().getOffsetForHorizontal(startLine,
	// tv.getScrollX() + tv.getTotalPaddingLeft());
	// }

	public String getPreviewBuf(String arg0) {
		// TODO Auto-generated method stub
		return null;
	}

	public int getTotalPage() {
		if (tv == null) {
			MtkLog.i(TAG, "No Textview specified, Need to set a Textview");
			return 0;
		}
		if (tv.getLineCount() == 0) {
			MtkLog
					.i(TAG,
							"Need to refresh the textview for getting the correct pagenum");
			return 0;
		} else {
			if (tv.getLineCount() % PAGE_LINE != 0) {
				return tv.getLineCount() / PAGE_LINE + 1;
			} else {
				return tv.getLineCount() / PAGE_LINE;
			}
		}
	}

	public void loadPos(int pos) {
		// TODO Auto-generated method stub
		if (tv == null) {
			MtkLog.i(TAG, "No Textview specified, Need to set a Textview");
			return;
		}
		Layout l = tv.getLayout();
		int line = l.getLineForOffset(pos);
		float sy = l.getLineBottom(line);
		MtkLog.i(TAG, "Load the position:" + pos);
		tv.scrollTo(0, (int) sy);
	}

	public void pageDown() {
		if (tv == null) {
			MtkLog.i(TAG, "No Textview specified, Need to set a Textview");
			return;
		}
		if (curPage >= getTotalPage()) {
			MtkLog.i(TAG, "Reach the first page");
			tv.scrollTo(0, (curPage - 1) * PAGE_LINE * tv.getLineHeight());
		} else {
			MtkLog.i(TAG, "Page up");
			curPage++;
			tv.scrollTo(0, (curPage - 1) * PAGE_LINE * tv.getLineHeight());
		}
		pageChangeListener.onPageChanged(getCurPagenum());
	}

	public void pageUp() {
		if (tv == null) {
			MtkLog.i(TAG, "No Textview specified, Need to set a Textview");
			return;
		}
		if (curPage <= 1) {
			MtkLog.i(TAG, "Reach the first page");
			tv.scrollTo(0, (curPage - 1) * PAGE_LINE * tv.getLineHeight());
		} else {
			MtkLog.i(TAG, "Page up");
			curPage--;
			tv.scrollTo(0, (curPage - 1) * PAGE_LINE * tv.getLineHeight());
		}
		pageChangeListener.onPageChanged(getCurPagenum());
	}

	public int playFirst() {
		// TODO Auto-generated method stub
		return 0;
	}

	public int playNext() {
		// TODO Auto-generated method stub
		return 0;
	}

	public int playPrev() {
		// TODO Auto-generated method stub
		return 0;
	}

	public void scrollLnDown() {
	}

	public void scrollLnUp() {
	}

	public void searchNext() {
		// TODO Auto-generated method stub

	}

	public void searchPrev() {
		// TODO Auto-generated method stub

	}

	public void searchText(String arg0, String arg1) {
		// TODO Auto-generated method stub

	}

	public void setBackgroundColor(String arg0) {
		// TODO Auto-generated method stub

	}

	public void setFontColor(String arg0) {
		// TODO Auto-generated method stub

	}

	public void setFontSize(float arg0) {
		// TODO Auto-generated method stub

	}

	public void setFontStyle(String arg0, String arg1) {
		// TODO Auto-generated method stub

	}

	public void setPlayMode(int arg0) {
		// TODO Auto-generated method stub

	}

	public void setScreenHeight(int arg0) {
		// TODO Auto-generated method stub

	}

	public void setScrollView(ScrollView arg0) {
		// TODO Auto-generated method stub

	}

	public void setTextView(TextView textView) {
		this.tv = textView;
		pageChangeListener.onPageChanged(getCurPagenum());
		tv.scrollTo(0, (curPage - 1) * PAGE_LINE * tv.getLineHeight());
	}

	public void skipToPage(int arg0) {
		// TODO Auto-generated method stub

	}

	public boolean hasUpPage() {
		if (getCurPagenum() - 1 > 0) {
			return true;
		} else {
			return false;
		}
	}

	public boolean hasNextPage() {
		if (getTotalPage() - getCurPagenum() >= 1) {
			return true;
		} else {
			return false;
		}

	}

	public interface TextReaderPageChangeListener {
		public void onPageChanged(int page);
	}
}
