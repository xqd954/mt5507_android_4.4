package com.mediatek.ui.nav.util;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import android.content.Context;

import com.mediatek.tv.common.ConfigType;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVTimerManager;
import com.mediatek.tvcm.TVTimerManager.TVTimerListener;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVEvent;
import com.mediatek.tvcommon.TVEventManager;
import com.mediatek.tvcommon.TVEventManager.EventListener;
import com.mediatek.tvcommon.TVSchedEventMonitor;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.nav.epginfo.EPGChannelInfo;
import com.mediatek.ui.nav.epginfo.EPGProgramInfo;
import com.mediatek.ui.nav.epginfo.EPGTimeConvert;
import com.mediatek.ui.util.MtkLog;

public class DataReader {

	private static final String TAG = "DataReader";

	private static DataReader dtReader;
	private static NavIntegration navImp;
	TVContent tvContent;
	TVSchedEventMonitor evMonitor;
	TVEventManager evManager;
	static TVTimerManager tmManager;

	private List<TVChannel> tvChannelList;
	private EPGTimeConvert tmCvt;
	private static MenuConfigManager mConfigManager;

	private DataReader(Context context) {

		navImp = NavIntegration.getInstance(context);
		tmCvt = EPGTimeConvert.getInstance(context);
		tvContent = TVContent.getInstance(context);
		evManager = tvContent.getEventManager();
		tmManager = tvContent.getTimerManager();
		evMonitor = evManager.getSchedMonitor();
		mConfigManager = MenuConfigManager.getInstance(context);
	}

	public void setChannelEventListener(EventListener evListener) {
		if (evMonitor != null)
			evMonitor.setListener(evListener);
	}

	public void registerTimerListener(TVTimerListener listener) {
		tmManager.addTimeListener(listener);
	}

	public void removeTimerListener(TVTimerListener listener) {
		tmManager.removeTimeListener(listener);
	}

	public static DataReader getInstance(Context context) {
		if (dtReader == null) {
			dtReader = new DataReader(context);
		}
		return dtReader;
	}

	public List<EPGProgramInfo> readChannelProgramInfoByTime(TVChannel ch,
			long startTime, long duration) {

		List<EPGProgramInfo> mTVProgramInfoList = new ArrayList<EPGProgramInfo>();
		evMonitor.setTime(startTime, duration);
		MtkLog.d(TAG, "--- set time : [" + startTime + "," + duration + "]");
		evMonitor.syncRead();
		final TVChannel tvChannel = ch;
		TVEvent[] events = evMonitor.getEvents(tvChannel);
		if (events != null) {
			for (int i = 0; i < events.length; i++) {
				long proStartTime = events[i].getStartTime();
				long proEndTime = proStartTime + events[i].getDuration();
				Date mStart = new Date(proStartTime);
				Date mEnd = new Date(proEndTime);
				MtkLog.d(TAG, "+++++++ event name ++++++++++++"
						+ events[i].getTitle());
				MtkLog.d(TAG, "+++++++ event detail ++++++++++++"
						+ events[i].getDetail());
				EPGProgramInfo mTVprogramInfo = new EPGProgramInfo(mStart,
						mEnd, events[i].getTitle(), events[i].getDetail(), 0,
						1, true);
				float width = getProWidth(mTVprogramInfo, events[i], startTime);
				mTVprogramInfo.setmScale(width);
				if (i == 0) {
					float mLeftMargin = tmCvt.countShowWidth(proStartTime,
							startTime);
					if (mLeftMargin > 0) {
						mTVprogramInfo.setLeftMargin(mLeftMargin);
					} else {
						mTVprogramInfo.setLeftMargin(0.0f);
					}

				} else {
					float mLeftMargin = getProLeftMargin(mTVprogramInfo,
							events[i - 1], events[i]);
					mTVprogramInfo.setLeftMargin(mLeftMargin);
					MtkLog.d(TAG, "+++++++ mLeftMargin ++++++++++++"
							+ mLeftMargin);
				}
				mTVProgramInfoList.add(mTVprogramInfo);
			}
		} else {
			EPGProgramInfo mTVprogramInfo = new EPGProgramInfo(new Date(
					startTime), new Date(startTime + duration), null, null, 0,
					1, true);
			mTVprogramInfo.setmScale(1.0f);
			mTVProgramInfoList.add(mTVprogramInfo);
		}
		return mTVProgramInfoList;
	}

	public float getProWidth(EPGProgramInfo epgProInfo, TVEvent event,
			long startTime) {
		float width = 0.0f;
		long proStartTime = event.getStartTime();
		long proEndTime = proStartTime + event.getDuration();
		long duration = tmCvt.getHourtoMsec(EPGConfig.mTimeSpan);
		if (proStartTime < startTime && proEndTime > (startTime + duration)) {
			epgProInfo.setDrawLeftIcon(true);
			epgProInfo.setDrawRightIcon(true);
			width = 1.0f;
		} else if (proStartTime < startTime) {
			width = tmCvt.countShowWidth(proEndTime, startTime);
			epgProInfo.setDrawLeftIcon(true);
		} else if (proEndTime > (startTime + duration)) {
			width = tmCvt.countShowWidth(startTime + duration, proStartTime);
			epgProInfo.setDrawRightIcon(true);
		} else {
			width = tmCvt.countShowWidth(event.getDuration());
		}
		MtkLog.d(TAG, " program width: " + width);
		return width;
	}

	public float getProLeftMargin(EPGProgramInfo mTVprogramInfo,
			TVEvent preTvEvent, TVEvent currentTvEvent) {
		float leftMargin = 0.0f;
		long preEndTime = preTvEvent.getStartTime() + preTvEvent.getDuration();
		long currentStartTime = currentTvEvent.getStartTime();

		leftMargin = tmCvt.countShowWidth(currentStartTime, preEndTime);
		return leftMargin;
	}

	/**
	 * 
	 * @param chList
	 *            channel list
	 * @param dayNum
	 *            [0-7]
	 * @param startTime
	 *            the duration start time
	 * @param mTimeSpan
	 *            the duration time
	 */
	public void readChannelProgramInfoByTime(List<EPGChannelInfo> chList,
			int dayNum, int startTime, int mTimeSpan) {
		int size = chList.size();
		TVChannel[] channels = new TVChannel[size];
		for (int i = 0; i < size; i++) {
			channels[i] = chList.get(i).getTVChannel();
		}
		evMonitor.setChannels(channels);
		int chIdx = 0;
		for (EPGChannelInfo child : chList) {
			// if (info.getmType() == false) {
			long sTime = tmCvt.setDate(getCurrentDateAsMills(), dayNum,
					startTime);

			long duration = tmCvt.getHourtoMsec(mTimeSpan);
			MtkLog.d(TAG, "--- set start time : [" + getCurrentDateAsMills()
					+ "]");
			List<EPGProgramInfo> mTVProgramInfo = new ArrayList<EPGProgramInfo>();
			mTVProgramInfo = readChannelProgramInfoByTime(channels[chIdx++],
					sTime, duration);
			child.setmTVProgramInfoList(mTVProgramInfo);
			// }
		}
	}

	public TVChannel getCurrentPlayChannel() {
		TVChannel mChannel;
		mChannel = navImp.iGetCurrentChannel();
		return mChannel;
	}

	public int getChannelPosition(TVChannel channel) {
		int position = 0;
		// channel in the collection location
		if(null != tvChannelList){
			position = tvChannelList.indexOf(channel);
		}
		
		return position;
	}

	public int getCurrentPlayChannelPosition() {
		int position;
		position = getChannelPosition(getCurrentPlayChannel());
		return position;
	}

	public EPGChannelInfo getChannelByChannelNum(short channelNum) {
		List<EPGChannelInfo> mChannelList = getChannelList();
		for (EPGChannelInfo child : mChannelList) {
			if (child.getmChanelNum() == channelNum)
				return child;
		}
		return null;
	}

	/*
	 * get current time
	 */
	public static Date getCurrentDate() {
		long curTime;
//		if(mConfigManager.getDefault(MenuConfigManager.TUNER_MODE) ==ConfigType.BS_SRC_AIR){
//			curTime = tmManager.getBroadcastUTC()/*+ new Date(tmManager.getBroadcastUTC()).getTimezoneOffset() / 60* 60 * 60 * 1000L*/;
//		}else{
			curTime = tmManager.getBroadcastUTC() ;
			MtkLog.d(TAG, " getCurrentDate() come in");
//		}
//		Date mDate = new Date(curTime);
		WeakReference reference = new WeakReference(new Date(curTime));
		Date mDate  = (Date) reference.get();
		return mDate;
	}
	/*
	 * get current time as millSecond
	 */
	public static long getCurrentDateAsMills(){
		MtkLog.d(TAG, " getCurrentDateAsMills() come in");
		long curTimeMillSeconds = tmManager.getBroadcastUTC();
		return curTimeMillSeconds;
	}

	/*
	 * Take the first day start time
	 */
	public int getFirstDayStartTime() {
		return getCurrentDate().getHours();
	}

	/*
	 * set current channel
	 */
	public static boolean getCurrentPlayChannel(EPGChannelInfo channel) {
		return true;
	}

	public List<EPGChannelInfo> getChannelList() {
		List<EPGChannelInfo> mChannelList = new ArrayList<EPGChannelInfo>();
		EPGChannelInfo chInfo = null;
		tvChannelList = navImp.iGetChannelList();
		if(null != tvChannelList){
		for (int i = 0; i < tvChannelList.size(); i++) {
			chInfo = new EPGChannelInfo(tvChannelList.get(i));
			mChannelList.add(chInfo);
		}
	 }
		return mChannelList;
	}

	public TVEvent[] getTvEvents(TVChannel ch) {
		if (ch != null) {
			return evMonitor.getEvents(ch);
		} else {
			return null;
		}
	}

	public TVEvent[] getTvEvents() {
		return evManager.getPFEvent();
	}

}
