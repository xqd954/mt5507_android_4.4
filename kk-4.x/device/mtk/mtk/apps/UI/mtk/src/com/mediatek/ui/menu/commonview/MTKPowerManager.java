package com.mediatek.ui.menu.commonview;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import android.content.Context;
import android.util.Log;

import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVTimer;
import com.mediatek.tvcm.TVTimerManager;
import com.mediatek.tvcm.TVTimerManager.PowerOffTimer;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.SaveValue;
import com.mediatek.ui.util.DateFormatUtil;
import com.mediatek.ui.util.MtkLog;

public class MTKPowerManager {
	private static Object syncRoot = new Object();
	private Context mContext;
	private TVContent tvContent;
	TVTimerManager tm;
	private static String TAG = "MTKPowerManager";
	private static MTKPowerManager mPowerOff;
	private static TVTimer mPowerOffTimer;
	private Date mPowOffDate;
	private Date mPowOnDate;

	private MTKPowerManager(Context context) {
		this.mContext = context;
		tvContent = TVContent.getInstance(mContext);
		tm = tvContent.getTimerManager();
		// mPowerOffTimer = new
		// PowerOffTimer(MenuConfigManager.POWER_OFF_TIMER,tm);
	}

	public static synchronized MTKPowerManager getInstance(Context mContext) {
		if (mPowerOff == null) {
			mPowerOff = new MTKPowerManager(mContext);
		}
		return mPowerOff;
	}

	// public void init(){
	// mSaveValue = SaveValue.getInstance(mContext);
	// String powoffTimeStr =mSaveValue.readStrValue(MenuConfigManager.TIMER2);
	// mPowOffDate =DateFormatUtil.getDate(powoffTimeStr);
	// timePowerOff(MenuConfigManager.POWER_OFF_TIMER,mPowOffDate);
	// }
	// private synchronized Date getDate(String powoffTimeStr) {
	// Date tempDate = null;
	// SimpleDateFormat simpleDateFormat =(SimpleDateFormat)
	// SimpleDateFormat.getDateTimeInstance();
	// simpleDateFormat.applyPattern("yyyy-MM-dd HH:mm:ss");
	// try {
	// tempDate = null;
	// String dateStr;
	// dateStr =SimpleDateFormat.getDateInstance(SimpleDateFormat.DEFAULT,
	// Locale.CHINA).format(new Date(System.currentTimeMillis()));
	//			
	// Log.d("fff",
	// "getDate()--------dateStr---powoffTimeStr--------->"+dateStr+
	// "  "+powoffTimeStr);
	// tempDate =simpleDateFormat.parse(dateStr+ "  "+powoffTimeStr);
	// simpleDateFormat.getDateTimeInstance(SimpleDateFormat.DEFAULT,
	// SimpleDateFormat.LONG, Locale.CHINA);
	// Log.d("fff",
	// "getDate()-----tempDate--1111111111-----"+simpleDateFormat.format(tempDate));
	// Date currentSystem =new Date(System.currentTimeMillis());
	// Long poweroff =tempDate.getTime();
	// Long currentTime =currentSystem.getTime();
	// if(poweroff <currentTime){
	// simpleDateFormat.getDateTimeInstance(SimpleDateFormat.DEFAULT,
	// SimpleDateFormat.LONG, Locale.CHINA);
	// Log.d("fff", "poweroff---->"+simpleDateFormat.format(new
	// Date(poweroff)));
	// Log.d("fff", "currentTime---->"+simpleDateFormat.format(new
	// Date(currentTime)));
	// tempDate = new Date(System.currentTimeMillis()+24*60*60*1000*1L);
	// dateStr =SimpleDateFormat.getDateInstance(SimpleDateFormat.DEFAULT,
	// Locale.CHINA).format(tempDate);
	// tempDate =simpleDateFormat.parse(dateStr +"  "+powoffTimeStr);
	// }
	// } catch (ParseException e) {
	// Log.d("fff", "time is invalida");

	// tempDate =new Date(System.currentTimeMillis()+24*60*60*1000L);
	// e.printStackTrace();
	// }
	// simpleDateFormat.getDateTimeInstance(SimpleDateFormat.DEFAULT,
	// SimpleDateFormat.LONG, Locale.CHINA);
	// Log.d("fff",
	// "getDate()-----tempDate---222222222----"+simpleDateFormat.format(tempDate));
	// return tempDate;
	// }
	public void timePowerOff(String hour, String minute, String second,
			String str) throws ParseException {
		SimpleDateFormat simpleDateFormat = new SimpleDateFormat(
				"yyyy-MM-dd HH:mm:ss");
		Date mDate = simpleDateFormat.parse("" + hour + ":" + minute + ":"
				+ second);
		PowerOffTimer powerOff = tm.getPowerOffTimer(str);
		powerOff.setTimer(mDate);
		powerOff.start();
	}

	public synchronized void timePowerOff(String name, Date date) {
		SimpleDateFormat simpleDateFormat = (SimpleDateFormat) SimpleDateFormat
				.getDateTimeInstance(SimpleDateFormat.DEFAULT,
						SimpleDateFormat.LONG, Locale.CHINA);
		MtkLog.d("fff", "timePowerOff---**************->"
				+ simpleDateFormat.format(date));
		mPowerOffTimer = tm.getPowerOffTimer(name);
		mPowerOffTimer.setTimer(date);
		mPowerOffTimer.start();
	}

	public void timePowerOff(long time, String str) {
		mPowerOffTimer = tm.getPowerOffTimer(str);
		mPowerOffTimer.setTimer(time);
		mPowerOffTimer.start();
	}

	public void timePowerOff(String str) {
		mPowerOffTimer = tm.getPowerOffTimer(str);
		mPowerOffTimer.setTimer(0);
		mPowerOffTimer.start();
	}

	public synchronized void updatePowerOff(String powoffTimeStr) {
		Date updatePowOffDate = DateFormatUtil.getDate(powoffTimeStr);
		if (mPowOffDate != null) {
			SimpleDateFormat simpleDateFormat = (SimpleDateFormat) SimpleDateFormat
					.getDateTimeInstance(SimpleDateFormat.DEFAULT,
							SimpleDateFormat.LONG, Locale.CHINA);
			MtkLog.d("fff", "updatePowerOff------mPowOffDate----->"
					+ simpleDateFormat.format(mPowOffDate));
			MtkLog.d("fff", "updatePowerOff------updateDate----->"
					+ simpleDateFormat.format(updatePowOffDate));
			if (mPowOffDate.equals(updatePowOffDate)) {
				return;
			}
		}
		mPowOffDate = updatePowOffDate;
		timePowerOff(MenuConfigManager.POWER_OFF_TIMER, mPowOffDate);
	}

	public synchronized void cancelPowOffTimer(String name) {
		mPowerOffTimer = tm.getPowerOffTimer(name);
		mPowerOffTimer.cancel();
	}

	public synchronized void updatePowerOn(String powOnTimeStr) {
		Date updatePowOnDate = DateFormatUtil.getDate(powOnTimeStr);
		if (mPowOnDate != null) {
			if (mPowOnDate.equals(updatePowOnDate)) {
				return;
			}
		}
		SimpleDateFormat simpleDateFormat = (SimpleDateFormat) SimpleDateFormat
				.getDateTimeInstance(SimpleDateFormat.DEFAULT,
						SimpleDateFormat.LONG, Locale.CHINA);
		MtkLog.d("fff", "updatePoweron------mPowOnDate----->"
				+ simpleDateFormat.format(updatePowOnDate));
		mPowOnDate = updatePowOnDate;
		setPowerOn(mPowOnDate);
	}

	public void setPowerOn(Date date) {
		tm.setPowerOnTimer(date);
	}

	public synchronized void cancelPowOnTimer() {

	}
}
