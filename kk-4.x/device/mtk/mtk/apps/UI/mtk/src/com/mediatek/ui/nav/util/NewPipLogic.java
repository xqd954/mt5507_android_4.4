package com.mediatek.ui.nav.util;

import java.util.List;

import android.R.integer;
import android.app.ActivityManager;
import android.app.ActivityManagerNative;
import android.app.ActivityManager.RunningTaskInfo;
import android.app.ActivityManagerNative;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.widget.Toast;

import com.mediatek.tvcm.TVChannelSelector;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVInputManager;
import com.mediatek.tvcm.TVOutput;
import com.mediatek.tvcommon.ITVCommon;
import com.mediatek.tvcommon.TVCommonNative;
import com.mediatek.tvcommon.TVInputCommon;
import com.mediatek.tvcommon.TVOutputCommon;
import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.menu.commonview.MessageType;
import com.mediatek.ui.util.DestroyApp;
import com.mediatek.ui.util.GetCurrentTask;
import com.mediatek.ui.util.MtkLog;

/**
 * This class be used to abstract PIP logic from UI.
 * 
 * @author MTK40530
 * 
 */
public class NewPipLogic {
	String TAG = "PIP";
	private static NewPipLogic instance;
	private TVContent mTvContent;
	private TVInputManager mTvInputManager;
	private Context mContext;
	public static boolean mBreComponentName;
	public static final int PIP_POS_ZERO = 0;
	public static final int PIP_POS_ONE = 1;
	public static final int PIP_POS_TWO = 2;
	public static final int PIP_POS_THREE = 3;
	public static final int PIP_POS_FOUR = 4;

	public static final int PIP_SIZE_SMALL = 0;
	public static final int PIP_SIZE_MIDDLE = 1;
	public static final int PIP_SIZE_LARGE = 2;

	public static int currentSize = PIP_SIZE_SMALL;
	public static int currentPos = PIP_POS_ONE;
	private static float x, y, w, h;
	private ITVCommon mtv = null;

	private static Toast mToast = null;
	private static final String START_ACTION = "com.mediatek.mmp.start";
	private Handler myHandler = new Handler(){
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case MessageType.MESSAGE_FOR_PIPSIZE_DELAY:
//				setTvViewPosition();
				DestroyApp.getTVDialogView(mContext.getApplicationContext()).show(mContext);
				break;
			case MessageType.MESSAGE_FOR_CHECK_SWAP:
				MtkLog.d(TAG,"come in myHandler MESSAGE_FOR_CHECK_SWAP");
				GetCurrentTask currentTask = GetCurrentTask.getInstance(mContext);
				if (currentTask.isCurActivityTkuiMainActivity() && !isTvNormal()) {
					MtkLog.d(TAG,"come in myHandler MESSAGE_FOR_CHECK_SWAP to resume TV");
					resumeMainOutput();
				}else{
					MtkLog.i(TAG,"~~~~~~~~~~~~"+currentTask.getCurRunningClass());
				}
				break;
			default:
				break;
			}
		}
	};

	private NewPipLogic(Context context) {
		mContext = context;
		mTvContent = TVContent.getInstance(context);
		mTvInputManager = mTvContent.getInputManager();
		mtv = TVCommonNative.getDefault(mContext);
	}

	public static synchronized NewPipLogic getInstance(Context context) {
		if (instance == null) {
			instance = new NewPipLogic(context);
		}
		return instance;
	}

	public boolean isCurrentModePIP() {
		return pipOn;
	}

	//private boolean isCurrentModeNormal() {
	//	return (mTvInputManager.getOuputMode() == TVInputManager.OUTPUT_MODE_NORMAL);
	//}

	public int getCurrentPipPosition() {
		return currentPos;
	}

	public int getCurrentPipSize() {
		return currentSize;
	}

	private boolean isSubInputsourceTV() {
		String mSource = mTvInputManager.getCurrInputSource("sub");

		if (mSource == null) {
			return false;
		}
     String mType = mTvInputManager.getTypeFromInputSource(mSource);
		return (null != mType  && mType.equals(
				TVInputManager.INPUT_TYPE_TV));
	}

	/*private boolean isMainInputsourceTV() {
		String mSource = mTvInputManager.getCurrInputSource("main");

		if (mSource == null) {
			return false;
		}
    if(mTvInputManager.getTypeFromInputSource(mSource) == null){
    	return false;
    }else{
		return mTvInputManager.getTypeFromInputSource(mSource).equals(
				TVInputManager.INPUT_TYPE_TV);
	  } 
	}*/

	public void channelUp() {
		if (isCurrentModePIP() && isSubInputsourceTV()) {
			TVChannelSelector mTvChannelSelector = mTvContent
					.getChannelSelector();
			mTvChannelSelector.channelUp();
		}
	}

	public void channelDown() {
		if (isCurrentModePIP() && isSubInputsourceTV()) {
			TVChannelSelector mTvChannelSelector = mTvContent
					.getChannelSelector();
			mTvChannelSelector.channelDown();
		}
	}

	/**
	 * for user to set sub screen to appropriate size.
	 */
	public void changePipSize() {
		if (isCurrentModePIP()) {
			currentSize++;
			if (currentSize > PIP_SIZE_LARGE) {
				currentSize = PIP_SIZE_SMALL;
			}
			setScreenPos();
			setTvViewPosition();
				DestroyApp.getTVDialogView(mContext.getApplicationContext()).hide();
				mTvContent.setScreenPosition("sub", x, y, w, h);
				myHandler.removeMessages(MessageType.MESSAGE_FOR_PIPSIZE_DELAY);
				myHandler.sendEmptyMessageDelayed(MessageType.MESSAGE_FOR_PIPSIZE_DELAY, MessageType.FOR_PIPSIZE_DELAY_TIME);
		}
	}

	/**
	 * for user to set sub output to appropriate position.
	 */
	public void changePipPosition() {
		if (isCurrentModePIP()) {
			currentPos++;
			if (currentPos > PIP_POS_FOUR) {
				currentPos = PIP_POS_ZERO;
			}
			setScreenPos();
			mTvContent.setScreenPosition("sub", x, y, w, h);
			setTvViewPosition();
		}
	}

	private void setScreenPos() {
		switch (currentPos) {
		case PIP_POS_ZERO:
			if (currentSize == PIP_SIZE_SMALL) {
				x = 0.1f;
				y = 0.7f;
				w = 0.3f;
				h = 0.9f;
			} else if (currentSize == PIP_SIZE_MIDDLE) {
				x = 0.1f;
				y = 0.6f;
				w = 0.4f;
				h = 0.9f;
			} else {
				x = 0.1f;
				y = 0.5f;
				w = 0.5f;
				h = 0.9f;
			}
			break;

		case PIP_POS_ONE:
			if (currentSize == PIP_SIZE_SMALL) {
				x = 0.7f;
				y = 0.7f;
				w = 0.9f;
				h = 0.9f;
			} else if (currentSize == PIP_SIZE_MIDDLE) {
				x = 0.6f;
				y = 0.6f;
				w = 0.9f;
				h = 0.9f;
			} else {
				x = 0.5f;
				y = 0.5f;
				w = 0.9f;
				h = 0.9f;
			}
			break;

		case PIP_POS_TWO:
			if (currentSize == PIP_SIZE_SMALL) {
				x = 0.4f;
				y = 0.4f;
				w = 0.6f;
				h = 0.6f;
			} else if (currentSize == PIP_SIZE_MIDDLE) {
				x = 0.35f;
				y = 0.35f;
				w = 0.65f;
				h = 0.65f;
			} else {
				x = 0.3f;
				y = 0.3f;
				w = 0.7f;
				h = 0.7f;
			}
			break;

		case PIP_POS_THREE:
			if (currentSize == PIP_SIZE_SMALL) {
				x = 0.7f;
				y = 0.1f;
				w = 0.9f;
				h = 0.3f;
			} else if (currentSize == PIP_SIZE_MIDDLE) {
				x = 0.6f;
				y = 0.1f;
				w = 0.9f;
				h = 0.4f;
			} else {
				x = 0.5f;
				y = 0.1f;
				w = 0.9f;
				h = 0.5f;
			}
			break;

		case PIP_POS_FOUR:
			if (currentSize == PIP_SIZE_SMALL) {
				x = 0.1f;
				y = 0.1f;
				w = 0.3f;
				h = 0.3f;
			} else if (currentSize == PIP_SIZE_MIDDLE) {
				x = 0.1f;
				y = 0.1f;
				w = 0.4f;
				h = 0.4f;
			} else {
				x = 0.1f;
				y = 0.1f;
				w = 0.5f;
				h = 0.5f;
			}
			break;

		default:
			break;
		}

		MtkLog.i("SCAL", "~~~~~~~(x,y,w,h)" + "(" + x + "," + y + "," + w + ","
				+ h + ")");
	}

	private void setTvViewPosition() {
		DestroyApp.getTVDialogView(mContext.getApplicationContext())
				.setTvViewPosition(x, y, w, h);
		DestroyApp.getTVDialogView(mContext.getApplicationContext()).show(
				mContext);
	}

	/**
	 * swap the 3rd AP and TV AP
	 */
	private static int mLastTastId = 0;
	public void swap3rdappTvapp() {
                ActivityManager mActivityManager = (ActivityManager) mContext
				.getSystemService(Context.ACTIVITY_SERVICE);
		List<RunningTaskInfo> runningTaskList = mActivityManager
				.getRunningTasks(8);
		List<RunningTaskInfo> newRunningTaskList = mActivityManager
				.getMtkRunningTasks();
			
		
		if (runningTaskList.size()>newRunningTaskList.size()) {
			int i = 0,j = 0;
			while(i<runningTaskList.size() && j<newRunningTaskList.size())
			{
						RunningTaskInfo child = runningTaskList.get(i);
						RunningTaskInfo newChild = newRunningTaskList.get(i);
						if (child.id!=newChild.id)
						{
								runningTaskList.remove(i);
						}
						else
						{
							i++;
							j++;
						}
			}
			while(i<runningTaskList.size())
			{
				runningTaskList.remove(i);
			}
			
		}
		
		if (runningTaskList != null && runningTaskList.size() > 1) {
			RunningTaskInfo currenTaskInfo = runningTaskList.get(0);
			ComponentName currentActivity = runningTaskList.get(0).topActivity;

			GetCurrentTask ct = GetCurrentTask.getInstance(mContext);
			
			if ("com.mediatek.ui.TurnkeyUiMainActivity".equals(currentActivity
					.getClassName())) {
					Intent mIntent = new Intent(START_ACTION);
					mContext.sendBroadcast(mIntent);
				if (mBreComponentName) {
					pauseMainOutput();

					try {
						ActivityManagerNative.getDefault().moveTaskToFront(
								mLastTastId, 0, null);
					} catch (RemoteException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}

					
					myHandler.removeMessages(MessageType.MESSAGE_FOR_CHECK_SWAP);
					MtkLog.d(TAG,"come in send MessageType.MESSAGE_FOR_CHECK_SWAP");
					myHandler.sendEmptyMessageDelayed(MessageType.MESSAGE_FOR_CHECK_SWAP, 700);
					
				}
			} else {
                    if(ct.isCurActivtyMeidaMainActivity()){
			toastNofunction();
		        return;
	            }

				if (!ct.isCurTaskTKUI()) {
					mBreComponentName = true;
					mLastTastId = currenTaskInfo.id;
					
					changeModeToNormal();

					try {
						for (int i = 0; i < runningTaskList.size(); i++) {
							if ("com.mediatek.ui.TurnkeyUiMainActivity"
									.equals(runningTaskList.get(i).baseActivity
											.getClassName())) {
								ActivityManagerNative.getDefault()
										.moveTaskToFront(
												runningTaskList.get(i).id, 0, null);
								break;
							}
						}
					} catch (RemoteException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
			}
		}
	}

	/**
	 * open sub output and move the input source of main output to sub.
	 */
	private void changeModeToPip() {
//		 mTvInputManager.enterOutputMode(TVInputCommon.OUTPUT_MODE_NORMAL);
//		 mTvInputManager.enterOutputMode(TVInputCommon.OUTPUT_MODE_PIP);
//		 TVOutput mOutput = mTvInputManager.getOutput("main");
//		 String mString = null;
//		 if (null != mOutput) {
//		 mString = mOutput.getInput().getName();
//		 }
//		 if (null != mString) {
//		 mTvInputManager.changeInputSource("sub", mString);
//		 setScreenPos();
//		 setTvViewPosition();
//		 mTvContent.setScreenPosition("sub", x, y, w, h);
//		 }
		try {
				if(null != mtv){
			    mtv.enterOutputMode(ITVCommon.OUTPUT_MODE_APP_PIP);
			}
			
//			TVCommonNative.getDefault(mContext)
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		setScreenPos();
		setTvViewPosition();
		mTvContent.setScreenPosition("sub", x, y, w, h);
	}

	public void changeModeToNormal() {
		try {
			if(null != mtv){
			   	mtv.enterOutputMode(ITVCommon.OUTPUT_MODE_NORMAL);
			}
			pipOn = false;
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		// mTvInputManager.enterOutputMode(TVInputCommon.OUTPUT_MODE_NORMAL);
	//	mTvContent.setColorKey(false, 0xfff74dff);
		// String mString =
		// mTvInputManager.getOutput("sub").getInput().getName();
		// mTvInputManager.enterOutputMode(TVInputManager.OUTPUT_MODE_NORMAL);
		// mTvInputManager.changeInputSource("main", mString);
		// }

		// if (!isCurrentModeNormal()) {
		// TVOutput output = mTvInputManager.getOutput("sub");
		// output.stop();
		// }
	}

	private boolean pipOn = false;
	
	public void switchOutputMode() {
		if (TurnkeyUiMainActivity.isResume) {
			MtkLog.i(TAG, "tk ui is running, can't change output mode.");
			return;
		}

		GetCurrentTask ctask = GetCurrentTask.getInstance(mContext);
		
		if(ctask.isCurActivtyMeidaMainActivity()){
			toastNofunction();
			return;
		}
		
		if (!ctask.isCurTaskTKUI()&& null != mtv) {
			try {
				int currentOutputMode = mtv.getCurrentOutputMode();
				if(ITVCommon.OUTPUT_MODE_NORMAL == currentOutputMode){
					pipOn = false;
				}
			} catch (RemoteException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
//			 if (isCurrentModePIP()) {
//			 DestroyApp.getTVDialogView(mContext.getApplicationContext())
//			 .hide();
//			 // changeModeToNormal();
//			 TVOutputCommon common = TVOutputCommon.getInstance(mContext);
//			 common.stop("sub");
//			 } else {
//			 changeModeToPip();
//			 DestroyApp.getTVDialogView(mContext.getApplicationContext())
//			 .show(mContext);
//			 }
			if (pipOn) {
				DestroyApp.getTVDialogView(mContext.getApplicationContext())
						.hide();
				// changeModeToNormal();
				TVOutputCommon common = TVOutputCommon.getInstance(mContext);
				mTvContent.setScreenPosition("sub", 0, 0, 0, 0);
				//common.stop("sub");
				common.stopEx("sub", true);
				pipOn = false;
			} else {
				changeModeToPip();
				DestroyApp.getTVDialogView(mContext.getApplicationContext())
						.show(mContext);
//				DestroyApp.getTVDialogView(mContext.getApplicationContext())
//						.startCheckPosition();
				pipOn = true;
			}
		}
	}

	private static boolean tvNormal = false;

	public static boolean isTvNormal() {
		return tvNormal;
	}

	public static void setTvNormal(boolean tvNormal) {
		NewPipLogic.tvNormal = tvNormal;
	}

	public void pauseMainOutput() {
		setTvNormal(false);
		MtkLog.i(TAG, "~~~~~~~~~~~~~~~~~enter pauseMainOutput");
		TVOutput output = mTvInputManager.getOutput("main");
		if(null != output){
		  output.stop();
		}
		mTvContent.leaveTV();
	}
	
	public void pauseSubOutput() {
		MtkLog.i(TAG, "~~~~~~~~~~~~~~~~~enter pauseSubOutput");
		TVOutput subOutput = mTvInputManager.getOutput("sub");
		if(null != subOutput){
		  subOutput.stop();
		}
	}

	public void resumeMainOutput() {
		setTvNormal(true);
		MtkLog.i(TAG, "~~~~~~~~~~~~~~~~~enter resumeMainOutput");

		mTvContent.enterTV();

//		mTvInputManager.enterOutputMode(TVInputCommon.OUTPUT_MODE_NORMAL);
		try {
			if(null != mtv){
				mtv.enterOutputMode(ITVCommon.OUTPUT_MODE_NORMAL);
			}
		
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		TVOutput output = mTvInputManager.getOutput("main");
   	if(null != output){
		 output.connect(output.getInput());
		}
		DestroyApp.getTVDialogView(mContext.getApplicationContext()).hide();
	}
	
	public void resumeMainOutputWithNoConnect() {
		setTvNormal(true);
		MtkLog.i(TAG, "~~~~~~~~~~~~~~~~~enter resumeMainOutputWithNoConnect");

		mTvContent.enterTV();

		mTvInputManager.enterOutputMode(TVInputCommon.OUTPUT_MODE_NORMAL);
		DestroyApp.getTVDialogView(mContext.getApplicationContext()).hide();
	}
	
	private void toastNofunction(){
		String mString = mContext.getString(R.string.nav_no_function);
		if(mToast == null){
			mToast = Toast.makeText(mContext,mString, 0);
			mToast.show();
		}else{
			mToast.cancel();
			mToast = Toast.makeText(mContext,mString, 0);
			mToast.show();
		}
	}
}
