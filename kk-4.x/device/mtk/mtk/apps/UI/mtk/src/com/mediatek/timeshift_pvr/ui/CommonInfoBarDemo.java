/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.ui;

import android.app.Activity;

import com.mediatek.ui.R;

/**
 *
 */
public class CommonInfoBarDemo extends BaseInfoBar {
	
	public CommonInfoBarDemo(Activity context) {
		super(context,R.layout.pvr_timeshfit_nf);
	}

	public CommonInfoBarDemo(Activity context, int layoutID, Long duration) {
		super(context,layoutID);

		this.mDefaultDuration = duration;
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
		//Timer work
		
	}
	
	
	
}
