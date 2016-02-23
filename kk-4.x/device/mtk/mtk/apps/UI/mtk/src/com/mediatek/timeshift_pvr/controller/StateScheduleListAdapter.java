/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.controller;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.ui.R;

import java.util.List;

/**
 *
 */
public class StateScheduleListAdapter<T> extends BaseAdapter {

	private List<T> mDiskList;
	private LayoutInflater mInflater;
	private Context mContext;

	public StateScheduleListAdapter(Context mContext, List<T> itemList) {
		super();
		this.mDiskList = itemList;
		this.mContext = mContext;
		mInflater = (LayoutInflater) mContext
				.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
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

		if (convertView == null) {
			view = mInflater.inflate(R.layout.pvr_tshfit_schudule_item_layout,
					null);
		} else {
			view = convertView;
		}

		setItemValue(view, (ScheduleItem) getItem(position));
		return view;
	}

	/**
	 * id:1001365643949250
	 * @param view
	 * @param item
	 */
	private void setItemValue(View view, ScheduleItem item) {
		TextView label = (TextView) view
				.findViewById(R.id.schedule_channel_name);
		label.setText("CH"+item.getChannelName());

		TextView size = (TextView) view.findViewById(R.id.schedule_date);
		size.setText(Util.dateToStringYMD3(item.getStartTime()));

		TextView duration = (TextView) view
				.findViewById(R.id.schedule_duration);
		
		
		duration.setText(Util.longToHrMin(item.getDuration()));

		TextView repeatType = (TextView) view
				.findViewById(R.id.schedule_internel);
		String[] repeat = mContext.getResources().getStringArray(
				R.array.pvr_tshift_repeat_type);
		
		repeatType.setText(repeat[item.getRepeatType()]);

		TextView scheduleType = (TextView) view
				.findViewById(R.id.schedule_notification);
		String[] schedule = mContext.getResources().getStringArray(
				R.array.pvr_tshift_schedule_type);
		String type=schedule[item.getRemindType()];
		scheduleType.setText(type);

	}

}
