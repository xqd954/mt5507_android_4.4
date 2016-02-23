package com.mediatek.timeshift_pvr.db;

import android.provider.BaseColumns;

public class AlarmColumn implements BaseColumns {
	public AlarmColumn() {
	}
	// Cloumns
	public static final String TASKID = "taskId";
	public static final String INPUTSOURCE = "inputSrc";
	public static final String CHANNELNUM = "channelNum";
	public static final String STARTTIME = "startTime";
	public static final String ENDTIME = "endTime";
	public static final String SCHEDULETYPE = "scheduleType";
	public static final String REPEATTYPE = "repeatType";
	public static final String DAYOFWEEK = "dayofweek";
	public static final String ISENABLE = "isEnable";

	public static final String[] PROJECTION = { _ID,// 0
			TASKID,// 1
			INPUTSOURCE,// 2
			CHANNELNUM, // 3
			STARTTIME, // 4
			ENDTIME, // 5
			SCHEDULETYPE, // 6
			REPEATTYPE, // 7
			DAYOFWEEK, // 8
			ISENABLE // 9
	};

}
