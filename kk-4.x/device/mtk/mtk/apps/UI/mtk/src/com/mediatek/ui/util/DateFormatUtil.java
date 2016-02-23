package com.mediatek.ui.util;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import android.util.Log;

public class DateFormatUtil {
	private static final String TAG = "DateFormatUtil";
	/*
	 * this string to parse
	 */
	public static Date getDate(String string) {
		Date tempDate = null;
		SimpleDateFormat simpleDateFormat = (SimpleDateFormat) SimpleDateFormat
				.getDateTimeInstance();
		simpleDateFormat.applyPattern("yyyy-MM-dd HH:mm:ss");
		try {
			tempDate = null;
			String dateStr;
			 SimpleDateFormat mFormat = (SimpleDateFormat)SimpleDateFormat.getDateInstance(
                     SimpleDateFormat.DEFAULT, Locale.CHINA);
             mFormat.applyPattern("yyyy-MM-dd");
			dateStr = mFormat.format(
					new Date(System.currentTimeMillis()));

			Log.d(TAG, "getDate()--------dateStr---powoffTimeStr--------->"
					+ dateStr + "  " + string);
			tempDate = simpleDateFormat.parse(dateStr + "  " + string);
			SimpleDateFormat.getDateTimeInstance(SimpleDateFormat.DEFAULT,
					SimpleDateFormat.LONG, Locale.CHINA);
			Log.d(TAG, "getDate()-----tempDate--1111111111-----"
					+ simpleDateFormat.format(tempDate));
			Date currentSystem = new Date(System.currentTimeMillis());
			Long poweroff = tempDate.getTime();
			Long currentTime = currentSystem.getTime();
			if (poweroff < currentTime) {
				SimpleDateFormat.getDateTimeInstance(SimpleDateFormat.DEFAULT,
						SimpleDateFormat.LONG, Locale.CHINA);
				Log.d(TAG, "poweroff---->"
						+ simpleDateFormat.format(new Date(poweroff)));
				Log.d(TAG, "currentTime---->"
						+ simpleDateFormat.format(new Date(currentTime)));
				tempDate = new Date(System.currentTimeMillis() + 24 * 60 * 60
						* 1000 * 1L);
				dateStr = mFormat.format(tempDate);
				tempDate = simpleDateFormat.parse(dateStr + "  " + string);
			}
		} catch (ParseException e) {
			Log.d(TAG, "time is invalida"+string);
			// If you set the shutdown time string is not illegal, say the time
			// to set off the current time for tomorrow
			int hour = 0;
            int min = 1;
            int sec = 0;
            try {
                 hour = Integer.parseInt(string.substring(0,2));
                 min =  Integer.parseInt(string.substring(3,5));
                 sec =  Integer.parseInt(string.substring(6,8));
            } catch (Exception e2) {
            }
            
            tempDate = new Date(System.currentTimeMillis() + hour * 60 * 60
                    * 1000L+ min * 60
                    * 1000L+ sec * 1000L);
			e.printStackTrace();
		}
		SimpleDateFormat.getDateTimeInstance(SimpleDateFormat.DEFAULT,
				SimpleDateFormat.LONG, Locale.CHINA);
		Log.d(TAG, "getDate()-----tempDate---222222222----"
				+ simpleDateFormat.format(tempDate));
		return tempDate;
	}

	public static boolean checkPowOffTimerInvalid(String string) {
		Date tempDate = null;
		SimpleDateFormat simpleDateFormat = (SimpleDateFormat) SimpleDateFormat
				.getDateTimeInstance();
		simpleDateFormat.applyPattern("yyyy-MM-dd HH:mm:ss");
		try {
			tempDate = null;
			String dateStr;
            SimpleDateFormat mFormat = (SimpleDateFormat)SimpleDateFormat.getDateInstance(
                    SimpleDateFormat.DEFAULT, Locale.CHINA);
            mFormat.applyPattern("yyyy-MM-dd");
           dateStr = mFormat.format(
                   new Date(System.currentTimeMillis()));

           Log.d(TAG, "getDate()--------dateStr---powoffTimeStr--------->"
                   + dateStr + "  " + string);
			tempDate = simpleDateFormat.parse(dateStr + "  " + string);
			Date currentSystem = new Date(System.currentTimeMillis());
			Log.d(TAG, "poweroff---->" + simpleDateFormat.format(tempDate));
			Log.d(TAG, "currentTime---->"
					+ simpleDateFormat.format(currentSystem));
			Long poweroff = tempDate.getTime();
			Long currentTime = currentSystem.getTime();
			if (poweroff <= currentTime) {
				return true;
			}
		} catch (ParseException e) {
			return false;
		}
		return false;
	}

	public static boolean checkPowOnTimerInvalid(String string) {
		Date tempDate = null;
		SimpleDateFormat simpleDateFormat = (SimpleDateFormat) SimpleDateFormat
				.getDateTimeInstance();
		simpleDateFormat.applyPattern("yyyy-MM-dd HH:mm:ss");
		try {
			tempDate = null;
			String dateStr;
            SimpleDateFormat mFormat = (SimpleDateFormat)SimpleDateFormat.getDateInstance(
                    SimpleDateFormat.DEFAULT, Locale.CHINA);
            mFormat.applyPattern("yyyy-MM-dd");
           dateStr = mFormat.format(
                   new Date(System.currentTimeMillis()));

           Log.d(TAG, "getDate()--------dateStr---powoffTimeStr--------->"
                   + dateStr + "  " + string);
			tempDate = simpleDateFormat.parse(dateStr + "  " + string);
			Date currentSystem = new Date(System.currentTimeMillis());
			Long poweron = tempDate.getTime();
			Long currentTime = currentSystem.getTime();
			Log.d(TAG, "poweron---->" + simpleDateFormat.format(tempDate));
			Log.d(TAG, "currentTime---->"
					+ simpleDateFormat.format(currentSystem));
			if (poweron >= currentTime - 60000 && poweron <= currentTime) {
				MtkLog.d(TAG, "----power--on--start--system------");
				return true;
			}
		} catch (ParseException e) {			
			return false;
		}
		return false;
	}
}
