package com.mediatek.ui.util;

import java.util.ArrayList;
import java.util.List;

import com.mediatek.netcm.dlna.DLNAManager;
import com.mediatek.ui.mmp.gamekit.filebrowse.MtkFileBrowseActivity;
import com.mediatek.ui.mmp.gamekit.filebrowse.MtkFileFlimBrowseActivity;
import com.mediatek.ui.mmp.multimedia.MediaPlayActivity;
import com.mediatek.ui.mmp.util.AsyncLoader;
import com.mediatek.ui.R;
import com.mediatek.ui.nav.commonview.TvView;

import android.app.Activity;
import android.app.Application;
import android.content.Context;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;

public class DestroyApp extends Application {
	private List<Activity> mainActivities = new ArrayList<Activity>();
	private static boolean isTopTask =false;
        private  static TvView  mTVDialogView;
    
	public static boolean isTopTask() {
		return isTopTask;
        }

	public static void setTopTask(boolean isTopTask) {
		DestroyApp.isTopTask = isTopTask;
	}

	public void add(Activity act) {
		mainActivities.add(act);
	}
    
	// close all Activity
	public void finishAll() {
		// fix cr DTV00416665
		DLNAManager.getInstance().stopDlna();
		//AsyncLoader.getInstance().stopLoader();
		for (Activity activity : mainActivities) {
			if (!activity.isFinishing()) {
				activity.finish();
			}
		}
		mainActivities.clear();
	}

	public void finishMediaPlayActivity() {
		boolean hasNext = true;

		while (hasNext) {
			if (mainActivities.size() >= 1) {
				int index = mainActivities.size() - 1;
				Activity top = mainActivities.get(index);
				if (top instanceof MediaPlayActivity) {
					Log.d("DDDD", "Finish : " + top);
					top.finish();
					mainActivities.remove(index);
				} else {
					hasNext = false;
				}
			} else {
				hasNext = false;
			}
		}
	}
	
	//add by 3d gamekit.
	public void finish3DBrowseActivity() {
		boolean hasNext = true;

		while (hasNext) {
			if (mainActivities.size() >= 1) {
				int index = mainActivities.size() - 1;
				Activity top = mainActivities.get(index);
				if (top instanceof MtkFileBrowseActivity) {
					Log.d("DDDD", "Finish : " + top);
					top.finish();
					mainActivities.remove(index);
					break;
				} else {
					hasNext = false;
				}
			} else {
				hasNext = false;
			}
		}
	}
	
	public  static TvView getTVDialogView(Context contex) {
		if(mTVDialogView ==null){
			mTVDialogView = new TvView(contex);
		}
		return mTVDialogView;
	}
	public void setTVDialogView(TvView mTVDialogView) {
		this.mTVDialogView = mTVDialogView;
	}

	public void remove(Activity activity) {
		if (mainActivities.size() > 0) {
			mainActivities.remove(activity);
		}
	}

}
