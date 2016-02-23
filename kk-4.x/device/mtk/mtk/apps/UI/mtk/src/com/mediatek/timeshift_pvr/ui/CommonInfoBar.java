/**   
 * @Description: TODO() 
 */
package com.mediatek.timeshift_pvr.ui;

import android.app.Activity;
import android.widget.TextView;

import com.mediatek.ui.R;

/**
 *
 */
public class CommonInfoBar extends BaseInfoBar {

	private TextView mInfo;
	private String mToastString="";
	
	public CommonInfoBar(Activity context) {
		super(context,R.layout.pvr_timeshfit_nf);
	}
	
	public CommonInfoBar(Activity context,String info) {
		super(context,R.layout.pvr_timeshfit_nf);
		mToastString=info;
	}

	public CommonInfoBar(Activity context, int layoutID, Long duration, String strInfo) {
		super(context,layoutID);

		this.mDefaultDuration = duration;
		this.mToastString = strInfo;
	}

	public void setInfo(String info) {
		mInfo.setText(info);
	}

	@Override
	public void initView() {
		super.initView();
		mInfo = (TextView) getContentView().findViewById(R.id.info);
		mInfo.setText(mToastString);
	}

	@Override
	public void doSomething() {
		
		super.doSomething();
	}
	
	
	
}
