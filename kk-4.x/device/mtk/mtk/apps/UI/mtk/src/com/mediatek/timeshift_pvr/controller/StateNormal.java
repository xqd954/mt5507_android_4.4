/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.controller;

import android.app.Activity;
import android.content.Context;
import android.view.KeyEvent;
import android.view.View;

import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.pippop.PippopUiLogic;
import com.mediatek.ui.util.KeyMap;

/**
 * 
 */
public class StateNormal extends StateBase implements IStateInterface {
	private Activity mActivity;

	public StateNormal(Activity mContext, TimeShiftManager manager) {
		super(mContext, manager);
		mActivity = mContext;
		setType(StatusType.NORMAL);
	}

	/*
	 * (non-Javadoc)
	 */
	@Override
	public boolean onKeyDown(int keycode) {
	    Util.Hlog("onKeyDown: StateNormal.java");

		switch (keycode) {
		case KeyMap.KEYCODE_MTKIR_PLAYPAUSE:
		case KeyEvent.KEYCODE_MTKIR_FREEZE: // tshift
		case KeyEvent.KEYCODE_MTKIR_RECORD: // pvr

			if(getManager().getTvLogicManager().isPipOrPopState()){
				return false;
			}
			
			TimeShiftManager manager = getManager();

			if (manager.dtvNotReady()) {
				Util.showDLog("Not support Tshift/pvr");
				if (!manager.timeShiftIsEnable()
						&& ((keycode == KeyEvent.KEYCODE_MTKIR_FREEZE) || (keycode == KeyMap.KEYCODE_MTKIR_PLAYPAUSE))) {
					return false;
				}
				TurnkeyUiMainActivity.getInstance().hideInputPwdView();
				manager.showFeatureNotAvaiable();
				return false;
			}

			if (keycode == KeyEvent.KEYCODE_MTKIR_FREEZE) {
				if (!manager.timeShiftIsEnable()) {
					return false;
				}
			}

			if (keycode == KeyMap.KEYCODE_MTKIR_PLAYPAUSE) {
				if (!manager.timeShiftIsEnable()) {
					return false;
				}
				if (manager.pvrIsRunning()) {
					return false;
				}
			}
			if (manager.diskIsReady()) {
				if (manager.getPvrMP() == null&&manager.checkPvrMP()) {
					
					manager.setState(new StateDiskSetting(mContext, manager));
					onRelease();
				} else {
					if(manager.checkPvrMP())
					{
						//manager.setDefaultPvrMP();
					}else
					{
						manager.setState(new StateDiskSetting(mContext, manager));
						onRelease();
						return true;
					}
					
					if (manager.hasFreeSize(keycode)) {
						if (keycode == KeyEvent.KEYCODE_MTKIR_FREEZE) {
							if (manager.hasTshiftFile()) {
//								manager.setState(new StateTimeShift(mContext,
//										manager));
							    manager.setState(new StateTimeShift(mContext,
                                      manager));
							} else {
								manager.setState(new StateInitDisk(mContext,
										manager));
							}
						} else {
							manager.setState(new StatePVR(mContext, manager));
						}
						onRelease();
					} else {
						manager.showDiskNotEnoughSpace();
					}
				}
			} else {
				manager.showDiskNotReady();
			}
			return true;

		case KeyEvent.KEYCODE_MTKIR_REPEAT:
			if(PippopUiLogic.getInstance(mContext).isPipOrPopState()){
				return false;
			}
			
			if (getManager().diskIsReady()) {
			    if (null!=getManager().getActivity()) {
			        ((TurnkeyUiMainActivity)getManager().getActivity()).hideAll();
                }
			    
				if (getManager().setState(
						StateFileList.getInstance(getManager()))) {
					((StateFileList) getManager().getState()).reShowPVRlist();
				}
				onRelease();
			} else {
				getManager().showDiskNotReady();
			}
			return true;
		default:
			break;
		}

		return false;
	}

	/*
	 * 
	 */
	@Override
	public void onResume() {
		// TODO Auto-generated method stub
		setRunning(true);
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
		// TODO Auto-generated method stub

	}
}
