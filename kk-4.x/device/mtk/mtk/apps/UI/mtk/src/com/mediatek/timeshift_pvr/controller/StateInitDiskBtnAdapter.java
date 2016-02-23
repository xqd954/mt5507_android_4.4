/**   
* @Description: TODO()
*/
package com.mediatek.timeshift_pvr.controller;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;

import com.mediatek.ui.R;

/**
 *
 */
public class StateInitDiskBtnAdapter extends BaseAdapter{

	private String[] mBtnList;
	private LayoutInflater mInflater;
	
	public StateInitDiskBtnAdapter(Context mContext, String[] mBtnList) {
		super();
		this.mBtnList = mBtnList;
		
		 mInflater = (LayoutInflater)mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
	}
	
	/*
	 * 
	 */
	@Override
	public int getCount() {
		// TODO Auto-generated method stub
		return mBtnList.length;
	}

	/*
	 * 
	 */
	@Override
	public String getItem(int position) {
		// TODO Auto-generated method stub
		return mBtnList[position];
	}

	/*
	 * 
	 */
	@Override
	public long getItemId(int position) {
		// TODO Auto-generated method stub
		return 0;
	}

	/*
	 * 
	 */
	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		// TODO Auto-generated method stub
		 View view;
		 
		if(convertView==null){
			view = mInflater.inflate(R.layout.pvr_timeshfit_btn,null);
		}else
		{
			view=convertView;
		}
		
		Button btn=(Button)view.findViewById(R.id.btn);
		String value = getItem(position);
		if(value==null){
		    value = "NoSpace";
		}
		btn.setText(value);
		
		return view;
	}


}
