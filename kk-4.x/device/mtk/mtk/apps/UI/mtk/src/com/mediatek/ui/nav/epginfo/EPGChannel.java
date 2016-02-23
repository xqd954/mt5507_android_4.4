package com.mediatek.ui.nav.epginfo;

import java.util.Date;
import android.content.Context;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVTimerManager;
import com.mediatek.tvcommon.TVEvent;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.nav.util.DataReader;
import com.mediatek.ui.util.MtkLog;

public class EPGChannel {

	private static final String TAG = "EPGChannel";
	static EPGChannel epgChannel;
	TVContent tvContent;
	TVTimerManager tmManager;
	DataReader dtReader;
	EPGTimeConvert tmCvt;
	Context mContext;
	MenuConfigManager mConfigManager;

	private EPGChannel(Context context) {
		mContext = context;
		dtReader = DataReader.getInstance(context);
		tmCvt = EPGTimeConvert.getInstance(context);
		tvContent = TVContent.getInstance(context);
		tmManager = tvContent.getTimerManager();
		mConfigManager = MenuConfigManager.getInstance(context);
	}

	public static EPGChannel getInstance(Context context) {
		if (epgChannel == null) {
			epgChannel = new EPGChannel(context);

		}
		return epgChannel;
	}

	private TVEvent getCurEvent() {
		TVEvent[] events = dtReader.getTvEvents();
		if (events != null) {
			return events[0];
		}
		return null;
	}

	private TVEvent getNextEvent() {
		TVEvent[] events = dtReader.getTvEvents();
		if (events != null && events.length > 1) {
			return events[1];
		}
		return null;
	}

	/**
	 * get current event title, if analog channel
	 * 
	 * @return event title for digital channel and "no program title" for analog
	 *         channel
	 */
	public String getCurEventTitle() {
		String epg_pro_title = mContext.getResources().getString(
				R.string.nav_channel_infoTitle);
		TVEvent curEvent = getCurEvent();
		if (curEvent != null) {
			return curEvent.getTitle();
		} else {
			return epg_pro_title;
		}
	}

	/**
	 * get current event playing time(hh:mm - hh:mm)
	 * 
	 * @return current event duration time
	 */
	public String getCurEventTime() {
		TVEvent curEvent = getCurEvent();
		if (curEvent != null) {
			long startT ;
//			if(mConfigManager.getDefault(MenuConfigManager.TUNER_MODE) ==ConfigType.BS_SRC_AIR){
//				startT = curEvent.getStartTime()/*+ new Date(tmManager.getBroadcastUTC()).getTimezoneOffset() / 60* 60 * 60 * 1000L*/;
//			}else{
				startT = curEvent.getStartTime();
//			}
			long endT = curEvent.getDuration() + startT;

			String duration = tmCvt.getHourMinite(startT) + " - "
					+ tmCvt.getHourMinite(endT);
			return duration;
		} else {
			return null;
		}
	}

	/**
	 * get next event title of current channel
	 * 
	 * @return next event title
	 */
	public String getNextEventTitle() {
		TVEvent nextEvent = getNextEvent();
		if (nextEvent != null) {
			return nextEvent.getTitle();
		}
		return null;
	}

	/**
	 * get next event duration time of current channel
	 * 
	 * @return next event time(format hh:mm-hh:mm)
	 */
	public String getNextEventTime() {
		TVEvent nextEvent = getNextEvent();
		if (nextEvent != null) {
			long startT ;
//			if(mConfigManager.getDefault(MenuConfigManager.TUNER_MODE) ==ConfigType.BS_SRC_AIR){
//				startT = nextEvent.getStartTime()/*+ new Date(tmManager.getBroadcastUTC()).getTimezoneOffset() * 60 * 1000L*/;
//			}else{
				startT = nextEvent.getStartTime();
//			}
			long endT = nextEvent.getDuration() + startT;

			String duration = tmCvt.getHourMinite(startT) + " - "
					+ tmCvt.getHourMinite(endT);
			return duration;
		}
		return null;
	}

	/**
	 * get event detail information of current channel
	 * 
	 * @return details
	 */
	public String getEventDetail() {
		TVEvent curEvent = getCurEvent();
		if (curEvent != null) {
			return curEvent.getDetail();			
		} else {
			return null;
		}

	}

	/**
	 * get event sub title description of current channel
	 * 
	 * @return sub title description
	 */
	public String getsubTitleDtl() {
		TVEvent curEvent = getCurEvent();
		if (curEvent != null) {
			return "Che";
		}
		return null;
	}

	/**
	 * if current event has sub title
	 * 
	 * @return
	 */
	public Boolean hasSubTitle() {
		TVEvent curEvent = getCurEvent();
		if (curEvent != null) {
			return true;
		} else {
			return false;
		}
	}

	public String getProgramType() {
		String type = mContext.getResources().getString(
				R.string.epg_info_program_type);
		TVEvent curEvent = getCurEvent();
		int index = 0;
		if (curEvent != null) {
			index = curEvent.findFirstMainCategary();
			String[] mainType = mContext.getResources().getStringArray(
					R.array.nav_epg_filter_type);
			MtkLog.d(TAG, "index: " + index);
			if (index >= 0 && index < mainType.length) {
				MtkLog.d(TAG, "main type: " + mainType[index]);
				return mainType[index];
	                }
                }
		return type;

	}

}
