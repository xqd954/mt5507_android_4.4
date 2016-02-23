/**
 * this class is used for calculate time and format date in EPG
 */
package com.mediatek.ui.nav.epginfo;

import java.lang.ref.WeakReference;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import android.content.Context;

import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.SaveValue;
import com.mediatek.ui.nav.util.EPGConfig;

public class EPGTimeConvert {
	private SaveValue sv;

	private static EPGTimeConvert tmConvert;

	private EPGTimeConvert(Context context) {
		sv = SaveValue.getInstance(context);
	}

	public static EPGTimeConvert getInstance(Context context) {
		if (tmConvert == null) {
			tmConvert = new EPGTimeConvert(context);
		}
		return tmConvert;
	}

	/**
	 * convert hour to milliseconds
	 * 
	 * @param hour
	 * @return milliseconds
	 */
	public long getHourtoMsec(int hour) {
		long msec = (long) hour * 60 * 60 * 1000;
		return msec;
	}

	/**
	 * calculate EPG program width ,total width is 1
	 * 
	 * @param endTime
	 *            program end time
	 * @param startTime
	 *            program start time
	 * @return width
	 */
	public float countShowWidth(long endTime, long startTime) {
		return (float) (endTime - startTime)
				/ (1000 * 60 * EPGConfig.mTimeSpan * 60L);
	}

	/**
	 * calculate EPG program width
	 * 
	 * @param duration
	 *            program playing time
	 * @return width
	 */
	public float countShowWidth(long duration) {
		return (float) duration / (1000 * 60 * EPGConfig.mTimeSpan * 60L);
	}

	/**
	 * convert date to special string
	 * 
	 * @param date
	 * @return
	 */
	public String getDetailDate(Date date) {
		SimpleDateFormat formatter = new SimpleDateFormat(
				"E,dd-MM-yyyy HH:mm:ss", getLocaleLan());
		return formatter.format(date);
	}

	/**
	 * calculate the time to set
	 * 
	 * @param curTime
	 *            current time
	 * @param day
	 *            current day is 0
	 * @param startHour
	 * @return
	 */
	public long setDate(long curTime, int day, long startHour) {
		WeakReference reference = new WeakReference(new Date(curTime));
		Date mDate  = (Date) reference.get();
		mDate.setHours(0);
		mDate.setMinutes(0);
		mDate.setSeconds(0);
		return mDate.getTime() + (day * 24 + startHour) * 60 * 60 * 1000L;
//		return curTime
//				/ (1000 * 60 * 60 * 24)
//				* (1000 * 60 * 60 * 24)
//				+ (day * 24 + startHour + new Date(curTime).getTimezoneOffset() / 60)
//				* 60 * 60 * 1000L;
	}

	/**
	 * get special date string, format (21-03-2011)
	 * 
	 * @param date
	 * @return
	 */
	public String getSimpleDate(Date date) {
		SimpleDateFormat formatter = new SimpleDateFormat("dd/MM/yyyy",
				getLocaleLan());
		return formatter.format(date);
	}

	public static Date getNormalDate(String time) {
		SimpleDateFormat mSimpleDateFormat = new SimpleDateFormat(
				"dd/MM/yyyy HH:mm:ss");
		try {
			return mSimpleDateFormat.parse(time);
		} catch (ParseException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;
	}

	/**
	 * getspecial date string, format (03:15 - 04:30 Mon, 03-Jan)
	 * 
	 * @param mTVProgramInfo
	 * @return
	 */
	public String formatProgramTimeInfo(EPGProgramInfo mTVProgramInfo) {
		if (mTVProgramInfo == null || mTVProgramInfo.getmTitle() == null) {
			return null;
		}
		StringBuilder sb = new StringBuilder();
		SimpleDateFormat sformat = new SimpleDateFormat("HH:mm");
		sb.append(sformat.format(mTVProgramInfo.getmStartTime()) + " - "
				+ sformat.format(mTVProgramInfo.getmEndTime()));

		sformat = new SimpleDateFormat("E, dd-MMM", getLocaleLan());
		sb.append("   " + sformat.format(mTVProgramInfo.getmStartTime()));
		return sb.toString();
	}

	/**
	 * get hour and minute of date
	 * 
	 * @param date
	 * @return
	 */
	public String getHourMinite(Date date) {
		SimpleDateFormat formatter = new SimpleDateFormat("HH:mm",
				getLocaleLan());
		return formatter.format(date);
	}

	public String getHourMinite(long time) {
		SimpleDateFormat formatter = new SimpleDateFormat("HH:mm",
				getLocaleLan());
		return formatter.format(new Date(time));
	}

	private Locale getLocaleLan() {
		if (sv.readValue(MenuConfigManager.OSD_LANGUAGE) == 1) {
			return Locale.CHINA;
		} else {
			return Locale.US;
		}
	}

}
