/**   
 * @Description: TODO()
 * 
 */
package com.mediatek.timeshift_pvr.manager;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbManager;
import android.os.Environment;
import android.os.IBinder;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.os.storage.IMountService;
import android.os.storage.StorageResultCode;
import android.text.format.Time;
import android.util.Log;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.animation.TranslateAnimation;

import com.mediatek.dm.MountPoint;
import com.mediatek.mmpcm.device.DevManager;
import com.mediatek.timeshift_pvr.controller.PVRFiles;
import com.mediatek.timeshift_pvr.controller.ScheduleItem;
import com.mediatek.timeshift_pvr.controller.StateScheduleList;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.util.MtkLog;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.math.BigDecimal;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.List;
import java.util.Locale;
import java.util.TimeZone;

/**
 * 
 */
public class Util {

	private final static boolean DEBUG = true && MtkLog.logOnFlag;
	private final static String TAG = "TimeShift_PVR";

	private final static boolean PRINT_TRACE = true;

	public static void Hlog(String string) {
		if (DEBUG)
			Log.d(TAG, string);
	}

	public static void showDLog(String string) {
		if (DEBUG)
			Log.d(TAG, string);
	}

	public static void showELog(String string) {
		if (DEBUG)
			Log.e(TAG, string);
	}

	public static void showDLog(String tag, String string) {
		if (DEBUG)
			Log.d(tag, string);
	}

	public static void showDLog(String tag, int string) {
		if (DEBUG)
			Log.d(tag, "" + string);
	}

	public static void showELog(String tag, String string) {
		if (DEBUG)
			Log.e(tag, string);
	}

	public static void showELog(String tag, int string) {
		if (DEBUG)
			Log.e(tag, "" + string);
	}

	public static void showTestButtons(Context context) {
		if (DEBUG) {

		}
	}

	public static String secondToString(int second) {
		SimpleDateFormat format = new SimpleDateFormat("HH:mm:ss",
				java.util.Locale.getDefault());
		TimeZone gmtZone = TimeZone.getTimeZone("GMT");
		format.setTimeZone(gmtZone);

		Date date = new Date();

		date.setTime(second * 1000); // offset 8hs
		return format.format(date);
	}

	public static String dateToString(Date date) {
		SimpleDateFormat format = new SimpleDateFormat("HH:mm:ss",
				java.util.Locale.getDefault());
		TimeZone gmtZone = TimeZone.getTimeZone("GMT");
		format.setTimeZone(gmtZone);

		return format.format(date);
	}

	/**
	 * covert date to year/month/day,
	 * 
	 * @param date
	 * @return
	 */
	public static String dateToStringYMD(Date date) {
		SimpleDateFormat format = new SimpleDateFormat("yyyy/MM/dd",
				java.util.Locale.getDefault());
		return format.format(date);
	}

	/**
	 * covert date to YearMonthDay,
	 * 
	 * @param
	 * @return yyyyMMdd_HHmm
	 */
	public static String dateToStringYMD2(Date date) {
		SimpleDateFormat format = new SimpleDateFormat("yyyyMMdd_HHmm");
		return format.format(date);
	}

	/**
	 * @param date
	 * @return yyyy-MM-dd/HH:mm
	 */
	public static String dateToStringYMD3(Date date) {
		SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd/HH:mm",
				Locale.getDefault());
		format.setTimeZone(TimeZone.getDefault());
		return format.format(date);
	}

	public static String timeMillisToChar() {
		String str = Long.toString(System.currentTimeMillis());
		return str.substring(str.length() - 4);
	}

	public static Date strToDate(String str) {
		SimpleDateFormat format = new SimpleDateFormat("yyyy/MM/dd",
				Locale.getDefault());
		format.setTimeZone(TimeZone.getDefault());

		Date date = null;

		try {
			date = format.parse(str.toString());
		} catch (ParseException e) {
			return null;
		}
		return date;
	}

	public static long dateToMills(Date date) {
		Time time = new Time();

		time.set(date.getSeconds(), date.getMinutes(), date.getHours(),
				date.getDay(), date.getMonth(), date.getYear());
		Long when = time.toMillis(true);
		return when;
	}

	public static Date strToTime(String str) {
		SimpleDateFormat format = new SimpleDateFormat("HH:mm:ss",
				Locale.getDefault());
		format.setTimeZone(TimeZone.getDefault());
		Date date = null;

		try {
			date = format.parse(str.toString());
		} catch (ParseException e) {
			return null;
		}
		return date;
	}

	public static Date getDateTime(String str) {
		Calendar calendar = Calendar.getInstance();
		String year = calendar.get(Calendar.YEAR) + "";
		String month = (calendar.get(Calendar.MONTH) + 1) + "";
		String day = calendar.get(Calendar.DAY_OF_MONTH) + "";
		String date1 = calendar.get(Calendar.DATE) + "";
		str = year + "-" + month + "-" + day + "/" + str;
		MtkLog.e("getDateTime", "sItem:getDateTime" + str);
		SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd/HH:mm:ss",
				Locale.getDefault());
		format.setTimeZone(TimeZone.getDefault());
		Date date = null;

		try {
			date = format.parse(str.toString());
		} catch (ParseException e) {
			return null;
		}
		return date;
	}

	public static Date getDateTime(long time) {
		Date date = new Date(time);
		GregorianCalendar gCalendar = new GregorianCalendar();
		gCalendar.setTime(date);
		SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd/HH:mm:ss",
				Locale.getDefault());
		format.setTimeZone(TimeZone.getDefault());
		String sbString = format.format(gCalendar.getTime());
		Date date2 = null;

		try {
			date2 = format.parse(sbString.toString());
		} catch (ParseException e) {
			return null;
		}
		return date2;
	}

	public static int strToSecond(String str) {
		SimpleDateFormat format = new SimpleDateFormat("HH:mm:ss",
				Locale.getDefault());
		format.setTimeZone(TimeZone.getDefault());
		Date date = null;

		try {
			date = format.parse(str.toString());
		} catch (ParseException e) {
			return 0;
		}
		int msTime = date.getHours() * 3600 + date.getMinutes() * 60
				+ date.getSeconds();

		return msTime;
	}

	/**
	 * @param str
	 *            like "978237236736",return 2000/12/31
	 */
	public static String longStrToDateStr(String str) {
		Date date = new Date(Long.parseLong(str));
		SimpleDateFormat format = new SimpleDateFormat("yyyy/MM/dd",
				Locale.getDefault());
		format.setTimeZone(TimeZone.getDefault());
		return format.format(date);
	}

	/**
	 * @param str
	 *            like "978237236736",return "12:33:56"
	 */
	public static String longStrToTimeStr(String str) {
		Date date = new Date(Long.parseLong(str));
		SimpleDateFormat format = new SimpleDateFormat("HH:mm:ss",
				Locale.getDefault());
		format.setTimeZone(TimeZone.getDefault());
		return format.format(date);
	}

	public static String longStrToTimeStr(Long msTime) {
		Date date = new Date(msTime);
		SimpleDateFormat format = new SimpleDateFormat("HH:mm:ss",
				Locale.getDefault());
		format.setTimeZone(TimeZone.getTimeZone("GMT"));
		return format.format(date);
	}

	public static String longToHrMin(Long time) {
		Date date = new Date(time);
		Calendar ca = Calendar.getInstance();
		ca.setTime(date);
		ca.setTimeZone(TimeZone.getTimeZone("GMT"));

		String sf = String.format("%dhr %dmin", ca.get(Calendar.HOUR_OF_DAY),
				ca.get(Calendar.MINUTE));

		return sf;
	}

	public static Date addDateAndTime(Date date1, Date date2) {
		Date newDate = new Date();

		newDate.setYear(date1.getYear());
		newDate.setMonth(date1.getMonth());
		newDate.setDate(date1.getDate());

		newDate.setHours(date2.getHours());
		newDate.setMinutes(date2.getMinutes());
		newDate.setSeconds(date2.getSeconds());

		return newDate;
	}

	/**
	 * Must have 300MB free size.
	 * 
	 * @param manager
	 * @return
	 */
	public static float speedTest(TimeShiftManager manager) {

		if (!manager.hasRemovableDisk()) {
			return -1f;
		}
		int index = (int) (Math.random() * 1000);

		if (manager.getPvrMP() == null) {
			manager.setPvrMP(manager.getDeviceList().get(0));

		}
		String path = String.format(manager.getPvrMP().mMountPoint + "/"
				+ "speedTest%d.dat", index);
		Util.showDLog("path:" + path);
		File testFile = new File(path);

		float maxSpeed = 0.0f;
		Long MinTime = Long.MAX_VALUE;

		if (testFile.exists()) {
			testFile.delete();
		}
		try {
			testFile.createNewFile();
		} catch (IOException e) {
			e.printStackTrace();
			return 0f;
		}

		int bufferSize = 1024 * 120; // 7.7
		final Long defaultCount = 300L;
		Long counts = defaultCount;

		byte[] writeByte = new byte[(int) (bufferSize)];

		FileOutputStream fis;
		Long startTime;
		Long endTime;
		try {
			fis = new FileOutputStream(testFile);
			startTime = System.currentTimeMillis();
			Long startTime1 = 0L;
			Long startTime2 = 0L;

			while (counts > 0) {
				startTime1 = System.currentTimeMillis();
				fis.write(writeByte);
				startTime2 = System.currentTimeMillis();

				if (MinTime > startTime2 - startTime1
						&& (startTime2 - startTime1) > 0) {
					MinTime = startTime2 - startTime1;
				}

				counts--;
			}
			fis.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			return 0f;
		} catch (IOException e) {
			e.printStackTrace();
			return 0f;
		} finally {
			testFile.delete();
		}

		endTime = System.currentTimeMillis();

		maxSpeed = (float) (bufferSize * 1000f / MinTime / 1024 / 1024);
		// System.out.println(String.format("MaxSpeed:%3.5f MB/s ", maxSpeed));
		maxSpeed = (new BigDecimal(maxSpeed).setScale(1,
				BigDecimal.ROUND_HALF_UP)).floatValue();
		System.out.println(String.format("MaxSpeed:%3.1f MB/s ", maxSpeed));

		float average = bufferSize * defaultCount * 1000L
				/ (endTime - startTime) / 1024 / 1024;
		System.out.println(String.format("average:%3.1f MB/s ", average));

		return average;
	}

	/**
	 * @param mp
	 * @return freeSize/Total Size +"GB"
	 */
	public static String getGBSizeOfDisk(MountPoint mp) {
		float tSize;
		float fSize;

		if (mp != null) {
			tSize = (float) mp.mTotalSize / 1024 / 1024;
			fSize = (float) mp.mFreeSize / 1024 / 1024;

		} else {
			tSize = 0f;
			fSize = 0f;
		}

		return String.format("%.1f/%.1f GB", fSize, tSize);
	}

	/**
	 * @param item
	 * @return
	 */
	public static CharSequence getIsTshift(MountPoint item) {
		// TODO Auto-generated method stub
		String mountPoint = item.mMountPoint;
		// File file = new File(mountPoint + File.separator + "PVR");
		boolean enable = TimeShiftManager.enableTshiftModule();
		
		String mp=TimeShiftManager.getInstance().getSaveValue().readStrValue(Core.PVR_DISK);
		Util.showDLog("getIsTshift(),mountPoint:"+mountPoint);
		Util.showDLog("getIsTshift(),mpReadValue:"+mp);
		
		File file = new File(item.mMountPoint + Core.PVR_DISK_TAG);
		if (mp.equalsIgnoreCase(mountPoint)&&file.exists() && (!file.isDirectory())) {
			if (enable) {
				return "TIMESHIFT/PVR";
			} else {
				return "PVR";
			}
		} else {
			if (enable) {
				return " Not TSHIFT/PVR ";
			} else {
				return " Not PVR ";
			}
		}
	}

	public static void itemToStringArray(ScheduleItem item) {
		System.out.println(item.getDate().toLocaleString());

		Calendar ca = Calendar.getInstance();
		ca.clear();
		ca.set(2012, 9, 05);

		System.out.println(ca.get(Calendar.YEAR));
		System.out.println(ca.get(Calendar.MONTH));
		System.out.println(ca.get(Calendar.DATE));

		dataFormat(ca.getTime());
	}

	public static void dataFormat(Date date) {
		SimpleDateFormat format = new SimpleDateFormat(
				"HH:mm:ss,EEEEEEEE,yyyy/MM/dd");
		System.out.println(format.format(date));
	}

	public static void longToDate(Long time) {
		Date date = new Date(time);
		dataFormat(date);
	}

	public static String[] covertFreeSizeToArray(boolean auto, Long freeSize) {
		String[] list;

		int count = 0;
		count = (int) (freeSize / (1024 * 512L));

		float size = 0L;

		Util.showDLog("FreeSize:" + freeSize);

		if (count == 0) {
			Util.showELog("There is not enough space.");
			return null;
		}
		if (count >= 7 && auto) {
			count = 6;
		}
		list = new String[count];

		for (int i = 0; i < count; i++) {
			size += 500L;

			if (size < 1000) {
				list[i] = String.format("%dMB", (int) size);
			} else {
				list[i] = String.format("%.1fGB", size / 1000);
			}
		}
		return list;
	}

	public static boolean makeDIR(String path) {
		File folder = new File(path.toString());
		if (!folder.exists() || !folder.isDirectory()) {
			if (!folder.mkdir()) {
				return false;
			}
		}
		return true;
	}

	public static void tempSetPVR(String diskPath) {

		ArrayList<MountPoint> deviceList = DevManager.getInstance()
				.getMountList();

		Util.showDLog("deviceList.size()::" + deviceList.size());

		if (deviceList != null && deviceList.size() > 0) {
			for (MountPoint point : deviceList) {
				removePVR(point.mMountPoint + Core.PVR_DISK_TAG);

				Util.showDLog("mMountPoint: " + point.mMountPoint);
				Util.showDLog("mVolumeLabel: " + point.mVolumeLabel);
				Util.showDLog("mDeviceName: " + point.mDeviceName);
				Util.showDLog("mFreeSize: " + point.mFreeSize);
				Util.showDLog("mTotalSize: " + point.mTotalSize);
			}

			File file = new File(diskPath + Core.PVR_DISK_TAG);
			if (!file.exists()) {
				try {
					file.createNewFile();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}

	}

	public static void removePVR(String diskPath) {
		File file = new File(diskPath);
		if (file.exists()) {
			try {
				file.delete();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	public static void showRecordToast(Activity activity) {
		// Toast.makeText(activity,
		// "Prepare pvr files,will start record after 10s",
		// Toast.LENGTH_LONG).show();
	}

	public static List<PVRFiles> getPvrFiles(String folder) {
		List<PVRFiles> list = new ArrayList<PVRFiles>();

		File file = new File(folder);
		Log.e("pvr0603", "folder:" + folder + "_file:" + file.getName());

		File[] files = file.listFiles();
		Log.e("pvr0603", "files:" + files);

		if (null == files) {
			return list;
		}

		for (int i = 0; i < files.length; i++) {
			if (files[i].isDirectory()) {
				list.addAll(getPvrFiles(files[i].getAbsolutePath()));
			} else {
				if (files[i].getName().endsWith(Core.SUFFIX_PVR)) {
					PVRFiles pvr = new PVRFiles(files[i].getAbsolutePath());
					pvr.setChannelNum(pvr.getParentFile().getName());
					if (TimeShiftManager.getInstance() != null) {
						if (pvr.getAbsolutePath().equalsIgnoreCase(
								TimeShiftManager.getInstance().getController()
										.getTsFile())) {
							pvr.isRecording = true;
						}
					}
					list.add(pvr);
				}
			}
		}

		return list;
	}

	public static void commonAimationIn(Context context, View view, int animID) {
		Animation anim = AnimationUtils.loadAnimation(context, animID);
		anim.setFillAfter(true);
		anim.setFillBefore(false);
		anim.setRepeatCount(-1);
		// anim.setStartOffset(startOffset);

		view.startAnimation(anim);
		Util.showDLog("commonAimationIn:" + view.getId());
	}

	/**
	 * @param context
	 * @param rect
	 *            (float fromXDelta, float toXDelta, float fromYDelta, float
	 *            toYDelta)
	 * @param duration
	 * @return
	 */
	public static TranslateAnimation commonAimationIn(Context context,
			float fromX, Long duration) {
		TranslateAnimation anim = new TranslateAnimation(fromX, -500f, 0f, 0f);

		anim.setFillAfter(true);
		anim.setFillBefore(true);
		anim.setDuration(duration);
		anim.setRepeatCount(-1);
		return anim;
	}

	public static TranslateAnimation commonAimationIn(Context context,
			float offsetX) {
		TranslateAnimation anim = new TranslateAnimation(0, offsetX, 0f, 0f);

		Util.showDLog("commonAimationIn,offsetX: " + offsetX);

		anim.setFillAfter(true);
		anim.setFillBefore(true);
		anim.setDuration(0L);
		anim.setRepeatCount(-1);
		return anim;
	}

	public static ScheduleItem getScheduleItem(Context context, int taskID) {
		// query item by TaskId from DB
		// double-check item's schedule
		// start record or remaind.if isPlaying mmp or flash....??
		List<ScheduleItem> items = new ArrayList<ScheduleItem>();
		items = StateScheduleList.queryItem(context, taskID);
		if (items != null && items.size() > 0) {
			for (int i = 0; i < items.size(); i++) {
				Util.showDLog(taskID + ":/" + items.get(i).getDuration());
				return items.get(i);
			}
		}
		return null;
	}

	public static void startPvrRecord(Context context, int taskID) {
		Util.showDLog("startPvrRecord(),taskID:" + taskID);
		Intent intent = new Intent(context, TurnkeyUiMainActivity.class);
		intent.setAction("com.mediatek.ui.schedulepvr");
		intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK
				| Intent.FLAG_ACTIVITY_BROUGHT_TO_FRONT
				| Intent.FLAG_ACTIVITY_SINGLE_TOP);
		intent.putExtra(Core.ALARM_ACTION_TAG, taskID);

		context.startActivity(intent);
	}

	public static void startAlarmDialog(Context context, int taskID) {
		Util.showDLog("startAlarmDialog(),taskID:" + taskID);

		Intent intent = new Intent(Core.ALARM_DIALOG_ACTION);
		intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		intent.putExtra(Core.ALARM_ACTION_TAG, taskID);
		context.startActivity(intent);
	}

	public static void fomatDisk(MountPoint mountPoint) {
		boolean bol = false;
		IMountService iMountService = null;
		TimeShiftManager.getInstance().isFormating = true;
		try {
			IBinder iBinder = ServiceManager.getService("mount");
			showELog("fomatDisk", "iBinder:" + iBinder);
			if (iBinder != null) {
				iMountService = IMountService.Stub.asInterface(iBinder);
				String extern_sd = Environment.getExternalStorageDirectory()
						.getAbsolutePath();
				showELog("fomatDisk", "usb_path:" + extern_sd);
				showELog("fomatDisk", "start unmountVolume.");
				iMountService.unmountVolume(extern_sd, true, true);
				showELog("fomatDisk", "end unmountVolume.");
				Thread.sleep(4000);
				showELog("fomatDisk", "start formatVolume");
				int result_format = iMountService.formatVolume(extern_sd);
				showELog("fomatDisk", "end formatVolume");
				showELog("fomatDisk", "format result_:" + result_format);
				if (result_format == StorageResultCode.OperationSucceeded) {
					Thread.sleep(4000);
					int result_mount = iMountService.mountVolume(extern_sd);
					showELog("result_format", "result_mount:" + result_mount);
				}
			}
			UsbManager usbManager;

		} catch (Exception e) {
			// TODO: handle exception
			TimeShiftManager.isFormating = false;
			MtkLog.e("fomatDisk", "Exception:" + e.getMessage());
		}
	}

	public static void removePVRTag(String diskPath) {
		Util.showDLog("removePVRTag(),diskPath:?," + diskPath);
		File file = new File(diskPath + Core.PVR_DISK_TAG);
		file.delete();
	}

	public static void removeTshiftTag(String diskPath) {
		Util.showDLog("removeTshiftTag(),diskPath:?," + diskPath
				+ "Not,ready!!,Util.java");
		// File file = new File(diskPath + Core.TSHIFT_DISK_TAG);
		// file.delete();
	}

	public static void printStackTrace() {
		Throwable tr = new Throwable();
		Log.getStackTraceString(tr);
		tr.printStackTrace();
	}
}
