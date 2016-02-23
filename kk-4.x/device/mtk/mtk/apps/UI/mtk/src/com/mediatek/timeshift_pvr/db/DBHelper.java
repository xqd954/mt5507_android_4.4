package com.mediatek.timeshift_pvr.db;


import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

public class DBHelper extends SQLiteOpenHelper {

	public static final String  DATABASE_NAME 		= "alarm.db";
	public static final int 	DATABASE_VERSION 	= 1;
	public static final String  CONTACTS_TABLE 		= "schedulepvr";
	
	private static final String DATABASE_CREATE = "CREATE TABLE " + CONTACTS_TABLE +" ("
					+ AlarmColumn._ID+" integer primary key autoincrement,"
					+ AlarmColumn.TASKID+" text,"
					+ AlarmColumn.INPUTSOURCE+" text,"
					+ AlarmColumn.CHANNELNUM+" text,"
					+ AlarmColumn.STARTTIME+" text,"
					+ AlarmColumn.ENDTIME+" text,"
					+ AlarmColumn.SCHEDULETYPE+" integer,"	//0,record; 1,reminder.
					+ AlarmColumn.REPEATTYPE+" integer,"	//0,once; 1,daily; 2,week;
					+ AlarmColumn.DAYOFWEEK+" integer,"
					+ AlarmColumn.ISENABLE+" integer);";//0: enable; 1:disable.
	
	public DBHelper(Context context) {
		super(context, DATABASE_NAME, null, DATABASE_VERSION);
		
	}

	@Override
	public void onCreate(SQLiteDatabase db) {
		db.execSQL(DATABASE_CREATE);
	}

	@Override
	public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
		db.execSQL("DROP TABLE IF EXISTS " + CONTACTS_TABLE);
		onCreate(db);
	}

}
