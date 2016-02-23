/**   
* @Description: TODO()
*/
package com.mediatek.timeshift_pvr.controller;

import java.util.List;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.mediatek.dm.MountPoint;
import com.mediatek.timeshift_pvr.manager.Core;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.ui.R;

/**
 *
 */
public class StateInitDiskItemAdapter<T> extends BaseAdapter{

	private List<T> mDiskList;
	private LayoutInflater mInflater;
	
	public StateInitDiskItemAdapter(Context mContext, List<T> mBtnList) {
		super();
		this.mDiskList = mBtnList;
		
		 mInflater = (LayoutInflater)mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
	}
	
	public void setGroup(List<T> mBtnList){
	    this.mDiskList = mBtnList;
	}
	/*
	 * 
	 */
	@Override
	public int getCount() {
		// TODO Auto-generated method stub
		return mDiskList.size();
	}

	/*
	 * 
	 */
	@Override
	public T getItem(int position) {
		// TODO Auto-generated method stub
		return mDiskList.get(position);
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
			view = mInflater.inflate(R.layout.pvr_timeshfit_deviceitem_layout,null);
		}else
		{
			view=convertView;
		}

		setItemValue(view,(MountPoint)getItem(position));
		return view;
	}

	/**
	 * @param view
	 * @param item
	 */
	private void setItemValue(View view, MountPoint item) {
		// TODO Auto-generated method stub
		
        TextView label=(TextView)view.findViewById(R.id.disk_label);
        
        
        TextView size=(TextView)view.findViewById(R.id.disk_size);
        
        
        TextView isTshift=(TextView)view.findViewById(R.id.disk_is_tshift);
        
        
        TextView na=(TextView)view.findViewById(R.id.disk_na);
        label.setText(item.mVolumeLabel);
	    if (Core.NO_DEVICES.equals(item.mVolumeLabel)) {
	        size.setVisibility(View.INVISIBLE);
	        isTshift.setVisibility(View.INVISIBLE);
	        na.setVisibility(View.INVISIBLE);
        }else {
            size.setVisibility(View.VISIBLE);
            isTshift.setVisibility(View.VISIBLE);
            na.setVisibility(View.VISIBLE);
            size.setText(Util.getGBSizeOfDisk(item));
            isTshift.setText(Util.getIsTshift(item));
            na.setText("NA");  
        }
		
	}


}
