package com.mediatek.ui.nav.util;

import java.util.ArrayList;
import java.util.List;

import com.mediatek.tvcommon.TVChannel;

public class ListData<T> {

	public static final int COUNT_EACH_PAGE = 7;
	private static final String TAG = " ListData ";
	private List<T> mShowList = new ArrayList<T>();
	private List<T> mAllList = new ArrayList<T>();

	private int theShowListStartIndex = 0;
	private int theShowListEndIndex = COUNT_EACH_PAGE - 1;

	public ListData(List<T> mAllDataList, T currentT) {
		//	
		if (mAllDataList != null) {
			this.mAllList = mAllDataList;
		}

		if (mAllList.size() <= COUNT_EACH_PAGE) {

			theShowListStartIndex = 0;
			theShowListEndIndex = mAllList.size() - 1;

		} else {
			if (currentT == null || (null != mAllDataList && !mAllDataList.contains(currentT))) {
				theShowListStartIndex = 0;
				theShowListEndIndex = COUNT_EACH_PAGE - 1;
			} else {
				theShowListStartIndex = mAllList.indexOf(currentT);
				theShowListEndIndex = (theShowListStartIndex + COUNT_EACH_PAGE - 1);
			}

		}

	}

	public List<T> getAllData() {
		return mAllList;
	}

	// return pro page channels
	public List<T> getPrePageData() {
		if (mAllList.size() <= COUNT_EACH_PAGE) {
			theShowListStartIndex = 0;
			theShowListEndIndex = mAllList.size() - 1;

		} else {
			if (theShowListStartIndex == 0) {
				theShowListEndIndex = mAllList.size() - 1;
				theShowListStartIndex = mAllList.size() - COUNT_EACH_PAGE;

			} else {
				theShowListEndIndex = theShowListStartIndex - 1;

				theShowListStartIndex = (theShowListEndIndex + mAllList.size()
						+ 1 - COUNT_EACH_PAGE)
						% mAllList.size();
			}

		}
		return getShowData();
	}

	public List<T> getNextPageData() {

		if (mAllList.size() <= COUNT_EACH_PAGE) {

			theShowListStartIndex = 0;
			theShowListEndIndex = mAllList.size() - 1;

		} else {

			theShowListStartIndex = theShowListEndIndex + 1;
			theShowListEndIndex = (theShowListStartIndex + COUNT_EACH_PAGE - 1)
					% mAllList.size();

		}
		return getShowData();
	}

	public List<T> getShowData(T currentT) {

		if (!mShowList.contains(currentT) && mAllList.size() != 0
				&& mAllList.contains(currentT)) {
			theShowListStartIndex = mAllList.indexOf(currentT);
			theShowListEndIndex = (theShowListStartIndex + COUNT_EACH_PAGE - 1)
					% mAllList.size();
		}

		return getShowData();

	}

	/**
	 * 
	 * @param currentT
	 * @param position
	 * @return
	 */
	public List<T> getShowData(T currentT, int position) {
		if (mAllList.indexOf(currentT) == -1) {
			theShowListStartIndex = 0;
			theShowListEndIndex = theShowListStartIndex + 6;
		} else {
			if (position == -1) {
				theShowListStartIndex = mAllList.indexOf(currentT);
				theShowListEndIndex = (theShowListStartIndex + COUNT_EACH_PAGE - 1)
						% mAllList.size();
			} else {
				if (mAllList.size() < position) {
					theShowListStartIndex = 0;
					theShowListEndIndex = mAllList.size() - 1;
				} else {
					theShowListStartIndex = ((mAllList.indexOf(currentT) + mAllList
							.size()) - position)
							% mAllList.size();
					theShowListEndIndex = (theShowListStartIndex
							+ COUNT_EACH_PAGE - 1)
							% mAllList.size();
				}
			}
		}

		return getShowData();
	}

	public int getIdxInShowList(T t) {
		int index = mShowList.indexOf(t);
		if (index == -1 && t != null) {
			index = mShowList.indexOf(findChannelByFrequence(mShowList,
					((TVChannel) t).getFreq()/1000000));
		}
		return index;
	}

	private TVChannel findChannelByFrequence(List<T> mAllDataList,
			float frequence) {
		TVChannel result = null;
    Float mFreqFloat = (float)0 ;
		for (T data : mAllDataList) {
			TVChannel channel = (TVChannel) data;
			mFreqFloat = ((float)(channel.getFreq()))/1000000 ;
			if (mFreqFloat.equals(frequence)) {
				result = channel;
			}
		}

		return result;
	}

	private List<T> getShowData() {
		mShowList.clear();
		if (mAllList.size() == 0) {
			return mShowList;
		}

		if (mAllList.size() <= COUNT_EACH_PAGE) {
			mShowList.addAll(mAllList);
			return mAllList;
		}
		if (theShowListStartIndex < theShowListEndIndex) {
			for (int i = theShowListStartIndex; i < theShowListEndIndex + 1; i++) {
				mShowList.add(mAllList.get(i));
			}

		} else {
			for (int i = theShowListStartIndex; i < mAllList.size(); i++) {
				mShowList.add(mAllList.get(i));
			}
			for (int i = 0; i <= theShowListEndIndex; i++) {
				mShowList.add(mAllList.get(i));
			}
		}
		return mShowList;
	}

	/**
	 * 
	 * @return all list size
	 */
	public int getCount() {
		return mAllList.size();
	}

	public void updateAllData(List<T> newAllData) {
		this.mAllList = newAllData;
	}

	public String toString() {
		return mAllList + "  " + mShowList;
	}
}
