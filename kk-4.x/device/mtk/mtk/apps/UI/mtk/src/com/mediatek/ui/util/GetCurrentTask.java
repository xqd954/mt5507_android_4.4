package com.mediatek.ui.util;

import java.util.List;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningTaskInfo;
import android.content.ComponentName;
import android.content.Context;
import android.util.Log;

public class GetCurrentTask {
	
	private static final String TAG = "GetCurrentTask";
	
	private Context mContext;
	private static GetCurrentTask curTask;
	
	private GetCurrentTask(Context context){
		mContext = context;
	}
	
	public static GetCurrentTask getInstance(Context context){
		if(curTask == null){
			curTask = new GetCurrentTask(context);
		}
		return curTask;
	}
	
	private  ComponentName getCurRunningCN(){
		ActivityManager am = (ActivityManager) mContext.getSystemService(mContext.ACTIVITY_SERVICE);
		ComponentName cn = am.getRunningTasks(1).get(0).topActivity;
		return cn;
	}

	public List<RunningTaskInfo> getRunningTasks(){
		ActivityManager am = (ActivityManager) mContext.getSystemService(mContext.ACTIVITY_SERVICE);
	    List<RunningTaskInfo>   mTaskList =am.getRunningTasks(12);
	    return mTaskList ;
	}

	public String getCurRunningClass(){
		String className = getCurRunningCN().getClassName();
		Log.d(TAG,"Current Running Activity Name: " + className);
		return className;
	}
	
	public String getCurRunningPKG(){
		String packageName = getCurRunningCN().getPackageName();
		Log.d(TAG," Current Running Package Name: " + packageName);
		return packageName;
	}
	
	public boolean isCurTaskTKUI() {
		return getCurRunningPKG().equals("com.mediatek.ui");
	}

	public boolean isCurActivtyMeidaMainActivity() {
		return getCurRunningClass().contains("com.mediatek.ui.mmp.");
	}
	
	public boolean isCurActivityTkuiMainActivity() {
		return getCurRunningClass().equals("com.mediatek.ui.TurnkeyUiMainActivity");
	}
	public boolean isCurMediaActivity(){
		return getCurRunningClass().equals("com.mediatek.ui.mmp.MeidaMainActivity");
	}
	public boolean isCurMPGActivity(){
		return getCurRunningClass().equals("com.mediatek.ui.nav.EPGActivity");
	}
	
	public boolean isCICardVisible(){
		return (getCurRunningClass().equals("com.mediatek.ui.TurnkeyUiMainActivity")|| getCurRunningClass().equals("com.mediatek.ui.menu.MenuMain"));
	}
}
