/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.controller;

import android.app.Activity;
import android.content.Context;
import android.content.res.Resources;

import com.mediatek.timeshift_pvr.manager.Controller;
import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.timeshift_pvr.manager.Util;

/**
 * 
 */
public class StateBase  implements IStateInterface {

	private boolean isRunning = false;

	private TimeShiftManager manager;
	public Context mContext;

	private Controller controller;
	private TVLogicManager tvLogicManager;
	
	
	private StatusType type = StatusType.UNKNOWN;

	public StateBase(Context context, TimeShiftManager manager) {
		super();
		
		this.manager = manager;
		this.mContext = context;
		
		initViews();
	}

	public void initViews(){
		Util.showDLog("statebase,initViews(),"+this.getClass().getSimpleName());
	}
	
	public StatusType getType() {
		return type;
	}

	public void setType(StatusType type) {
		this.type = type;
	}

	public boolean isRunning() {
		return isRunning;
	}

	public void setRunning(boolean isRunning) {
		this.isRunning = isRunning;
	}

	public TimeShiftManager getManager() {
		return manager;
	}

	public void setManager(TimeShiftManager mManager) {
		this.manager = mManager;
	}

	public Activity getActivity() {
		return getManager().getActivity();
	}

	public Resources getResource() {
		return getActivity().getResources();
	}

	/**
	 * @return the controller
	 */
	public Controller getController() {
		return getManager().getController();
	}

	/**
	 * @param controller the controller to set
	 */
	public void setController(Controller controller) {
		this.controller = controller;
	}

	/**
	 * @return the tvLogicManager
	 */
	public TVLogicManager getTvLogicManager() {
		return getManager().getTvLogicManager();
	}

	/**
	 * @param tvLogicManager the tvLogicManager to set
	 */
	public void setTvLogicManager(TVLogicManager tvLogicManager) {
		this.tvLogicManager = tvLogicManager;
	}

	@Override
	public boolean onKeyDown(int keycode) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void onResume() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onPause() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onStop() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onRelease() {
		Util.showDLog("onRelease()-"+this.getType().name());
	}

	@Override
	public boolean equals(Object o) {
		// TODO Auto-generated method stub

		if (o instanceof StateBase) {
			if (((StateBase) o).getType() == getType()) {
				return true;
			}
		}
		return false;
	
	}
}
