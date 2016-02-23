/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.db;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import com.mediatek.timeshift_pvr.controller.ScheduleItem;
import com.mediatek.timeshift_pvr.controller.StateScheduleList;
import com.mediatek.timeshift_pvr.manager.Core;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.ui.menu.MenuMain;

import java.util.Date;
import java.util.List;

/**
 *
 */
public class SchedulePvrAlarmBR extends BroadcastReceiver {

	private Context mContext;
	private boolean schedulePvrIsEnable = true;

	/*
	 * 
	 */
	@Override
	public void onReceive(Context context, Intent intent) {
		mContext = context;

		if (!schedulePvrIsEnable) {
			return;
		}

		String action = intent.getAction();

		// query item by TaskId from DB
		// double-check item's schedule
		// start record or remaind.if isPlaying mmp or flash....??

		if (action.equalsIgnoreCase(Core.ALARM_ACTION)) {
			String str = intent.getStringExtra(Core.ALARM_ACTION_TAG);
			Util.showDLog("intent.getStringExtra(),taskID:" + str);

			int taskid = 0;
			if (str != null) {
				taskid = Integer.parseInt(str);
			} else {
				return;
			}

			List<ScheduleItem> items = StateScheduleList.queryItem(context,
					taskid);
			if (items != null && items.size() > 0) {
				for (int i = 0; i < items.size(); i++) {
					ScheduleItem item = items.get(i);
					if (item.getRepeatType() == ScheduleItem.REPEAT_TYPE_WEEK) {
						if (!checnWeekList(item)) {
							return;
						}
					}
					
					if (item.getRemindType() == item.SCHEDULE_TYPE_REMINDER) {
						Util.startAlarmDialog(mContext, item.getTaskID());
					} else if (item.getRemindType() == item.SCHEDULE_TYPE_RECORD) {
						if(MenuMain.mScanningStatus){	//If is scanning...
							Util.startAlarmDialog(mContext, item.getTaskID());
							break;
						}
						
						Util.startPvrRecord(mContext, item.getTaskID());
					}

					break; // now,only get the first Item.
				}
			} else {
				Util.showDLog("queryItem(),size==0,taskID=" + str);
			}
		}
	}

	/**
	 * @param item
	 * @return
	 */
	private boolean checnWeekList(ScheduleItem item) {
		Date date = new Date();
		int dayOfWeek = date.getDay();
		Util.showDLog("checnWeekList(),dayOfWeek:" + dayOfWeek);

		if (Math.pow(2, dayOfWeek) == (item.getWeeklyRepeat() & ((int) Math
				.pow(2, dayOfWeek)))) {
			return true;
		} else {
			return false;
		}

	}
}
