/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.ui;

import android.app.Activity;
import android.view.Gravity;

import com.mediatek.timeshift_pvr.controller.StateBase;
import com.mediatek.timeshift_pvr.manager.Core;
import com.mediatek.ui.R;

/**
 *
 */
public class PVRControlbar extends BaseInfoBar {

	private static float wScale = 0.5f;
	private static float hScale = 0.23f; //hight change to 0.2f to 0.23f fix DTV00585450
	
	StateBase state;
	
	public PVRControlbar(Activity context) {
		super(context, R.layout.pvr_timeshfit_nf);
	}

	public PVRControlbar(Activity context, int layoutID, Long duration, StateBase state) {
		super(context, layoutID, duration,(int)(state.getManager().getTVWidth() * wScale), (int)(state.getManager().getTVHeight() * hScale));
		
		this.state=state;
	}

	@Override
	public void doSomething() {
		super.doSomething();
	}

	@Override
	public void show() {
		// TODO Auto-generated method stub
		super.show();

	}

	@Override
	public void setLocation() {
		// TODO Auto-generated method stub
		// super.setLocation();
		
		showAtLocation(mContext.findViewById(Core.ROOT_VIEW),
				Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL, 0, 30);
	}
}
