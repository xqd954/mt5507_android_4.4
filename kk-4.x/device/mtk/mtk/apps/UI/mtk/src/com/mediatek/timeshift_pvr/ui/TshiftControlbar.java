/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.ui;

import android.app.Activity;
import android.view.Gravity;

import com.mediatek.timeshift_pvr.controller.StateTimeShift;
import com.mediatek.timeshift_pvr.manager.Core;
import com.mediatek.ui.R;

/**
 *
 */
public class TshiftControlbar extends BaseInfoBar {

	private StateTimeShift callback;

	public TshiftControlbar(Activity context) {
		super(context, R.layout.pvr_timeshfit_nf);
	}

	public TshiftControlbar(Activity context, int layoutID, Long duration) {
		super(context, layoutID);
		this.mDefaultDuration=duration;
	}

	public void setInfo(String info) {

	}

	@Override
	public void initView() {

		super.initView();
	}

	@Override
	public void doSomething() {
		super.doSomething();
		callback.refreshUI();
	}

	@Override
	public void show(BaseInfoBar bBar) {
		// TODO Auto-generated method stub
		super.show(bBar);

	}

	@Override
	public void setLocation() {
		// TODO Auto-generated method stub
		// super.setLocation();
		showAtLocation(mContext.findViewById(Core.ROOT_VIEW), Gravity.BOTTOM
				| Gravity.CENTER_HORIZONTAL, 0, 30);
	}

	public StateTimeShift getCallback() {
		return callback;
	}

	public void setCallback(final StateTimeShift callback) {
		this.callback = callback;
	}

}
