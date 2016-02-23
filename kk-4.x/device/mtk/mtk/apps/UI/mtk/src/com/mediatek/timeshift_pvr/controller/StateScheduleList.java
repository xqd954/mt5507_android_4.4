/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.controller;

import android.app.Activity;
import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnDismissListener;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Handler;
import android.os.Message;

import com.mediatek.timeshift_pvr.db.AlarmColumn;
import com.mediatek.timeshift_pvr.db.SchedulePVRProvider;
import com.mediatek.timeshift_pvr.manager.Core;
import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.timeshift_pvr.ui.ScheduleListDialog;
import com.mediatek.timeshift_pvr.ui.ScheduleListItemInfoDialog;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.List;

/**
 * 
 */
public class StateScheduleList extends StateBase implements
		StateScheduleListCallback<ScheduleItem>, OnDismissListener {

	private ScheduleListDialog mScheduleListWindow;
	private ScheduleListItemInfoDialog mItemDialogWindow;

	private static StateScheduleList mStateSelf;
	public MyHandler mHandler;

	private final static int SHOW_LIST = 1;
	private final static int SHOW_LIST_ITEM = 2;
	public final static int Auto_Dismiss_Info_Dialog_Timer = 3;
	public final static int Auto_Dismiss_List_Dialog_Timer = 4;

	private int temp_count = 1;

	private ScheduleItem mItem;
	private Context mContext;

	public static class MyHandler extends Handler {
		WeakReference<Activity> mActivity;

		MyHandler(Activity activity) {
			mActivity = new WeakReference<Activity>(activity);
		}

		public void handleMessage(Message msg) {
			if (mStateSelf == null) {
				return;
			}
			switch (msg.what) {
			case SHOW_LIST:
				mStateSelf.showListDialog();
				break;

			case SHOW_LIST_ITEM:
				mStateSelf.showItemInfoDialog(mStateSelf.mItem);
				break;
			case Auto_Dismiss_Info_Dialog_Timer:
				mStateSelf.dimissItemInfoDialog();
				break;
			case Auto_Dismiss_List_Dialog_Timer:
				mStateSelf.dimissListDialog();
				break;

			default:
				break;
			}

			super.handleMessage(msg);
		}
	}

	public static StateScheduleList getInstance(Context mContext,
			TimeShiftManager manager) {
		if (mStateSelf == null) {
			mStateSelf = new StateScheduleList(mContext, manager);
		}
		return mStateSelf;
	}

	public static StateScheduleList getInstance() {
		if (mStateSelf != null) {
			return mStateSelf;
		} else {
			return null;
		}
	}
	public StateScheduleList(Context mContext, TimeShiftManager manager) {
		super(mContext, manager);
		setType(StatusType.SCHEDULELIST);
		this.mContext = mContext;
		mStateSelf = this;
		mHandler = new MyHandler(manager.getActivity());
		initViews2();
	}

	public StateScheduleList(Context mContext, TimeShiftManager manager,
			ScheduleItem item) {
		super(mContext, manager);
		setType(StatusType.SCHEDULELIST);
		this.mContext = mContext;
		mStateSelf = this;
		this.mItem = item;
		mHandler = new MyHandler(manager.getActivity());

		initViews2();
	}

	@Override
	public void initViews() {
		super.initViews();
	}

	public void initViews2() {
		if (mItem == null) {
			showListDialog();
		} else {
			showItemInfoDialog(mItem);
		}

	}

	/**
	 * 
	 */
	private void showListDialog() {
		mScheduleListWindow = new ScheduleListDialog(mContext, this);
		mScheduleListWindow.setCallback(this);
		mScheduleListWindow.setOnDismissListener(this);
		mScheduleListWindow.show();

		List<ScheduleItem> itemList = new ArrayList<ScheduleItem>();
		itemList = queryItem();

		mScheduleListWindow.setItemList(itemList);

	}

	/**
	 * @return
	 */
	public List<ScheduleItem> queryItem() {
		// TODO Auto-generated method stub
		ContentResolver resolver = this.getActivity().getContentResolver();
		Cursor cursor = resolver.query(SchedulePVRProvider.CONTENT_URI, null,
				null, null, null);
		return queryItemList(cursor);
	}

	/**
	 * @return
	 */
	public static void deleteAllItems(Context context) {
		// TODO Auto-generated method stub
		Util.showDLog("deleteAllItems(),After system restore.");
		ContentResolver resolver = context.getContentResolver();
		Cursor cursor = resolver.query(SchedulePVRProvider.CONTENT_URI, null,
				null, null, null);
		List<ScheduleItem> itemLists = queryItemList(cursor);

		AlarmManager am = (AlarmManager) context
				.getSystemService(Context.ALARM_SERVICE);

		for (int i = 0; i < itemLists.size(); i++) {
			ScheduleItem item = itemLists.get(i);

			Uri uri = Uri.parse(SchedulePVRProvider.CONTENT_URI.toString()
					+ "/" + String.valueOf(item.getTaskID()));
			Util.showDLog("URI:"+uri.toString());
			int row = context.getContentResolver().delete(uri, null, null);

			if (row != -1) {
				Intent intent = new Intent(Core.ALARM_ACTION);
				intent.putExtra(Core.ALARM_ACTION_TAG,
						String.valueOf(item.getTaskID()));
				Util.showDLog("makeIntent(),"
						+ String.valueOf(item.getTaskID()));

				PendingIntent pi = PendingIntent.getBroadcast(
						context.getApplicationContext(), 0, intent, 0);
				am.cancel(pi);
			}
		}
	}
	public static List<ScheduleItem> queryItem(Context context,int taskid) {
		ContentResolver resolver = context.getContentResolver();
		String where = AlarmColumn.TASKID + "=" + taskid;
		
		return queryItemList(resolver.query(SchedulePVRProvider.CONTENT_URI, null,
				where, null, null));
	}

	public ScheduleItem getScheduleItem() {
		List<ScheduleItem> itemList = new ArrayList<ScheduleItem>();
		itemList = queryItem();
		ScheduleItem sItem = Collections.min(itemList);
		Util.showDLog(
				"sItem",
				"sItem" + sItem.getStartTime() + "___item:"
						+ sItem.getChannelName());
		return sItem;// .getStartTime().getTime();
	}

	public int getChannelToStart() {

		return 0;
	}

	/**
	 * @param resolver
	 * @return
	 */
	private static List<ScheduleItem> queryItemList(Cursor cursor) {

		List<ScheduleItem> list = new ArrayList<ScheduleItem>();
		cursor.moveToFirst();
		if (cursor.getCount() < 1) {
			cursor.close();
			return list;
		}
		do {
			ScheduleItem item = new ScheduleItem();

			try {
				String taskID = cursor.getString(cursor
						.getColumnIndex(AlarmColumn.TASKID));
				String source = cursor.getString(cursor
						.getColumnIndex(AlarmColumn.INPUTSOURCE));
				String channelNum = cursor.getString(cursor
						.getColumnIndex(AlarmColumn.CHANNELNUM));
				String startTime = cursor.getString(cursor
						.getColumnIndex(AlarmColumn.STARTTIME));
				String endTime = cursor.getString(cursor
						.getColumnIndex(AlarmColumn.ENDTIME));

				int scheduleType = cursor.getInt(cursor
						.getColumnIndex(AlarmColumn.SCHEDULETYPE));
				int repeatType = cursor.getInt(cursor
						.getColumnIndex(AlarmColumn.REPEATTYPE));
				int dayOfWeek = cursor.getInt(cursor
						.getColumnIndex(AlarmColumn.DAYOFWEEK));
				int enable = cursor.getInt(cursor
						.getColumnIndex(AlarmColumn.ISENABLE));

				item.setTaskID(Integer.parseInt(taskID));
				item.setSrcType(source);
				item.setChannelName(channelNum);
				item.setStartTime(new Date(Long.parseLong(startTime)));
				item.setEndTime(new Date(Long.parseLong(endTime)));

				item.setDuration(Long.parseLong(endTime)
						- Long.parseLong(startTime));

				item.setRemindType(scheduleType);
				item.setRepeatType(repeatType);
				item.setWeeklyRepeat(dayOfWeek);
				item.setEnble(enable == 1 ? true : false);
			} catch (Exception e) {
				e.printStackTrace();
				continue;
			}
			list.add(item);
		} while (cursor.moveToNext());
		cursor.close();
		return list;
	}

	/*
	 * 
	 */
	@Override
	public void onResume() {
		// TODO Auto-generated method stub

	}

	/* 
	 */
	@Override
	public void onPause() {
		// TODO Auto-generated method stub
	}

	/* 
	 */
	@Override
	public void onStop() {
		// TODO Auto-generated method stub

	}

	/* 
	 */
	@Override
	public void onRelease() {
		try {
			// TODO Auto-generated method stub
			if (mItemDialogWindow != null && mItemDialogWindow.isShowing()) {
				mItemDialogWindow.dismiss();
			}
			if (mScheduleListWindow != null && mScheduleListWindow.isShowing()) {
				mScheduleListWindow.dismiss();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void tryToRelease() {
		boolean listWindow = false;
		boolean infoWindow = false;

		if (mItemDialogWindow == null || !mItemDialogWindow.isShowing()) {
			infoWindow = true;
		}
		if (mScheduleListWindow == null || !mScheduleListWindow.isShowing()) {
			listWindow = true;
		}

		if (listWindow && infoWindow) {
			if (mItemDialogWindow != null) {
				mItemDialogWindow.setOnDismissListener(null);
			}
			if (mScheduleListWindow != null) {
				mScheduleListWindow.setOnDismissListener(null);
			}
			mStateSelf.getManager().restoreToDefault(mStateSelf);
		}
	}

	/*
	 * 
	 */
	@Override
	public boolean onItemClick(ScheduleItem t) {

		dimissListDialog();
		switchToScheduleItemInfo(t);

		return false;
	}

	/*
	 * 
	 */
	@Override
	public boolean switchToScheduleItemInfo(ScheduleItem t) {
		dimissListDialog();
		showItemInfoDialog(t);
		return false;
	}

	/**
	 * @param t
	 */
	private void showItemInfoDialog(ScheduleItem t) {
		mItemDialogWindow = new ScheduleListItemInfoDialog(getActivity(), this,
				t);
		mItemDialogWindow.setCallback(this);
		mItemDialogWindow.setOnDismissListener(this);
		mItemDialogWindow.show();

	}

	/*
	 * 
	 */
	@Override
	public boolean switchToScheduleList(ScheduleItem t) {
		dimissItemInfoDialog();
		showListDialog();
		return false;
	}

	private void dimissListDialog() {
		if (mScheduleListWindow != null && mScheduleListWindow.isShowing()) {
			mScheduleListWindow.dismiss();
		}
	}

	private void dimissItemInfoDialog() {
		if (mItemDialogWindow != null && mItemDialogWindow.isShowing()) {
			mItemDialogWindow.dismiss();
		}
	}

	/**
	 * 
	 */
	public void insertItem(ScheduleItem item) {
		ContentValues values = new ContentValues();

		String time=String.valueOf(System.currentTimeMillis());
		String taskid=time.substring(time.length()-9);
		
		if(item.getTaskID()==0){
			item.setTaskID(Integer.parseInt(taskid));
			String taskInsert = Integer.parseInt(taskid) + "";
			Util.showDLog("insertItem",
					"sItem:delete:insert:" + Integer.parseInt(taskid));
			values.put(AlarmColumn.TASKID, taskInsert);
		}
		
		temp_count++;

		values.put(AlarmColumn.CHANNELNUM, item.getChannelName());

		Long startTime = Util.addDateAndTime(item.getDate(),
				item.getStartTime()).getTime();
		String startStr = startTime.toString();
		values.put(AlarmColumn.STARTTIME, startStr);

		Long endTime = Util.addDateAndTime(item.getDate(), item.getEndTime())
				.getTime();
		String endStr = endTime.toString();
		values.put(AlarmColumn.ENDTIME, endStr);

		values.put(AlarmColumn.INPUTSOURCE, item.getSrcType());
		values.put(AlarmColumn.SCHEDULETYPE, item.getRemindType());
		values.put(AlarmColumn.REPEATTYPE, item.getRepeatType());
		values.put(AlarmColumn.DAYOFWEEK, item.getWeeklyRepeat());

		Uri result = getActivity().getContentResolver().insert(
				SchedulePVRProvider.CONTENT_URI, values);

		if (result != null) {
			addSchedleTask(item);
		}
		switchToScheduleList(null);
	}

	/**
	 * 
	 */
	public void deleteItem(ScheduleItem item) {

		Uri uri = Uri.parse(SchedulePVRProvider.CONTENT_URI.toString() + "/"
				+ String.valueOf(item.getTaskID()));
		int row = getActivity().getContentResolver().delete(uri, null, null);
		if (row != -1) {
			deleteScheduleTask(item);
		}

	}

	/**
	 * @param item
	 *            ,item's task id can't be 0.
	 */
	public void replaceItem(ScheduleItem item) {
		if (item.getTaskID() == 0) {
			throw new NullPointerException("item's task id can't be 0");
		}

		ContentValues values = new ContentValues();

		values.put(AlarmColumn.TASKID, String.valueOf(item.getTaskID()));
		String str = item.getChannelName();
		values.put(AlarmColumn.CHANNELNUM, str);
		Util.showDLog("AlarmColumn.CHANNELNUM:" + str);
		Long startTime = Util.addDateAndTime(item.getDate(),
				item.getStartTime()).getTime();
		String startStr = startTime.toString();
		values.put(AlarmColumn.STARTTIME, startStr);

		Long endTime = Util.addDateAndTime(item.getDate(), item.getEndTime())
				.getTime();
		String endStr = endTime.toString();
		values.put(AlarmColumn.ENDTIME, endStr);

		values.put(AlarmColumn.INPUTSOURCE, item.getSrcType());
		values.put(AlarmColumn.SCHEDULETYPE, item.getRemindType());
		values.put(AlarmColumn.REPEATTYPE, item.getRepeatType());
		values.put(AlarmColumn.DAYOFWEEK, item.getWeeklyRepeat());

		Uri uri = Uri.parse(SchedulePVRProvider.CONTENT_URI.toString() + "/"
				+ String.valueOf(item.getTaskID()));
		int row = getActivity().getContentResolver().update(uri, values, null,
				null);
		if (row != -1) {
			replactSchedleTask(item);
		}

	}

	private void addSchedleTask(ScheduleItem item) {
		AlarmManager am = (AlarmManager) getActivity().getSystemService(
				Context.ALARM_SERVICE);

		PendingIntent pi = makeIntent(item);

		Long startTime = Util.addDateAndTime(item.getDate(),
				item.getStartTime()).getTime();

		if (item.getRepeatType() == ScheduleItem.REPEAT_TYPE_ONCE) {
			am.set(AlarmManager.RTC, startTime, pi);
		} else {
			am.setRepeating(AlarmManager.RTC, startTime,
					AlarmManager.INTERVAL_DAY, pi);
		}
	}

	private void replactSchedleTask(ScheduleItem item) {
		deleteScheduleTask(item);
		addSchedleTask(item);
	}

	private void deleteScheduleTask(ScheduleItem item) {
		AlarmManager am = (AlarmManager) getActivity().getSystemService(
				Context.ALARM_SERVICE);

		PendingIntent pi = makeIntent(item);
		am.cancel(pi);
	}

	/**
	 * @param item
	 * @return
	 */
	private PendingIntent makeIntent(ScheduleItem item) {
		Intent intent = new Intent(Core.ALARM_ACTION);
		intent.putExtra(Core.ALARM_ACTION_TAG, String.valueOf(item.getTaskID()));
		Util.showELog("makeIntent(),"+String.valueOf(item.getTaskID()));
		
		PendingIntent pi = PendingIntent.getBroadcast(getActivity()
				.getApplicationContext(), 0, intent, 0);
		return pi;
	}

	/*
	 * 
	 */
	@Override
	public boolean onKeyDown(int keycode) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void onDismiss(DialogInterface dialog) {
		// TODO Auto-generated method stub
		tryToRelease();
	}

	public boolean isRunning() {
		if ((mScheduleListWindow != null && mScheduleListWindow.isShowing())
				|| (mItemDialogWindow != null && mItemDialogWindow.isShowing())) {
			return true;
		}
		return false;
	}

}
