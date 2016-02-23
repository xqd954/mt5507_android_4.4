package com.mediatek.ui.nav.epginfo;

import java.util.Date;
import java.util.List;

import android.content.Context;

import com.mediatek.tvcommon.TVChannel;
import com.mediatek.ui.nav.util.DataReader;
import com.mediatek.ui.util.MtkLog;

public class EPGChannelInfo {
	private static final String TAG = "EPGChannelInfo";

	private static EPGChannelInfo chInfo;
	private String mChannelName;
	private int mChannelNum;
	private TVChannel mTVChannel;

	public TVChannel getTVChannel() {
		return mTVChannel;
	}

	public void setTVChannel(TVChannel mTVChannel) {
		this.mTVChannel = mTVChannel;
	}

	private List<EPGProgramInfo> mTVProgramInfoList;


	public EPGChannelInfo(Context context) {
	}

	public static EPGChannelInfo getInstance(Context context) {
		if (chInfo == null) {
			chInfo = new EPGChannelInfo(context);
		}
		return chInfo;
	}

	public EPGChannelInfo(TVChannel channel) {
		mChannelName = channel.getChannelName();
		mChannelNum = channel.getChannelNum();
		mTVChannel = channel;
	}

	public EPGChannelInfo(String name, short mChannelNum, boolean mType) {
		this.mChannelName = name;
		this.mChannelNum = mChannelNum;
	}

	public int getPlayingTVProgramPositon() {
		if (mTVProgramInfoList == null) {
			return 0;
		}
//		final Date now = DataReader.getCurrentDate();
//		final Long time = now.getTime();
		final Long time = DataReader.getCurrentDateAsMills();
		int i = 0;
		for (; i < mTVProgramInfoList.size(); i++) {
			if (time >= mTVProgramInfoList.get(i).getmStartTime().getTime()
					&& time <= mTVProgramInfoList.get(i).getmEndTime()
							.getTime()) {
				break;
			} else {
				continue;
			}
		}
		if (i >= mTVProgramInfoList.size()) {
			return 0;
		}
		return i;
	}

	public int getNextPosition(EPGProgramInfo mTVProgramInfo) {
		final EPGProgramInfo local = mTVProgramInfo;
		if (local == null || local.isDrawLeftIcon()) {
			return 0;
		}
		if (mTVProgramInfoList == null || mTVProgramInfoList.size() == 0) {
			return 0;
		}

		Date mStartTime = local.getmStartTime();
		final Long time = mStartTime.getTime();
		int i = mTVProgramInfoList.size() - 1;
		MtkLog.d(TAG, "Current program start time: " + time);
		EPGProgramInfo child = mTVProgramInfoList.get(i);
		while (child != null && child.getmStartTime().getTime() > time) {
			if (i <= 0) {
				return 0;
			}
			child = mTVProgramInfoList.get(--i);
			MtkLog.d(TAG, "Next program start time: "
					+ child.getmStartTime().getTime());
		}
		return i;

	}

	public String getName() {
		return mChannelName;
	}

	public void setName(String name) {
		this.mChannelName = name;
	}

	public int getmChanelNum() {
		return mChannelNum;
	}

	public void setmChanelNum(short mChanelNum) {
		this.mChannelNum = mChanelNum;
	}

	public List<EPGProgramInfo> getmTVProgramInfoList() {
		return mTVProgramInfoList;
	}

	public void setmTVProgramInfoList(List<EPGProgramInfo> mTVProgramInfoList) {
		this.mTVProgramInfoList = mTVProgramInfoList;
	}

	public List<EPGProgramInfo> getmGroup() {
		return mTVProgramInfoList;
	}
}
