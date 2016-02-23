package com.mediatek.timeshift_pvr.ui;

import android.app.Dialog;
import android.content.Context;
import android.view.View;

import com.mediatek.ui.R;

/**
 * 
 */
public class CommonDialog extends Dialog implements View.OnClickListener{

	protected Context mContext;
	
	protected CommonDialog(Context context,int layoutID) {
		super(context, R.style.MTK_Dialog_bg);
		this.mContext = context;

		setContentView(layoutID);
		initView();
	}


	public void initView() {
		
	}


	/*
	 * 
	 */
	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		
	}
}
