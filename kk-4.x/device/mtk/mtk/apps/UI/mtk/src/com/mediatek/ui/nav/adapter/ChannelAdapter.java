package com.mediatek.ui.nav.adapter;

import java.util.List;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnTouchListener;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.mediatek.tvcommon.TVChannel;
import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;

public class ChannelAdapter extends BaseAdapter {
	/*
	 * context
	 */
	private Context mContext;
	private LayoutInflater mInflater;
	private List<TVChannel> mcurrentChannelList;
	private String TAG = "ChannelAdapter";

	public ChannelAdapter(Context context, List<TVChannel> mcurrentChannelList) {
		mContext = context;
		mInflater = LayoutInflater.from(mContext);
		this.mcurrentChannelList = mcurrentChannelList;
	}

	public int getCount() {
		int s = 0;
		if (mcurrentChannelList != null) {
			s = mcurrentChannelList.size();
		}
		return s;
	}

	public TVChannel getItem(int position) {
		return mcurrentChannelList.get(position);
	}

	public long getItemId(int position) {
		return position;
	}

	public void updateData(List<TVChannel> mcurrentChannelList) {
		this.mcurrentChannelList = mcurrentChannelList;
		notifyDataSetChanged();
	}

	public View getView(int position, View convertView, ViewGroup parent) {
		ViewHolder hodler;
		TVChannel mCurrentChannel;
		if (convertView == null) {
			convertView = mInflater.inflate(R.layout.nav_channel_item, null);			
			hodler = new ViewHolder();
			hodler.mChannelNumberTextView = (TextView) convertView
					.findViewById(R.id.nav_channel_list_item_NumberTV);
			hodler.mChannelNameTextView = (TextView) convertView
					.findViewById(R.id.nav_channel_list_item_NameTV);
			convertView.setTag(hodler);
		} else {
			hodler = (ViewHolder) convertView.getTag();
		}
		mCurrentChannel = mcurrentChannelList.get(position);
		hodler.mChannelNumberTextView.setText(""
				+ mCurrentChannel.getChannelNum());
		hodler.mChannelNameTextView.setText(mCurrentChannel.getChannelName());			

		return convertView;
	}

	private class ViewHolder {
		TextView mChannelNumberTextView;
		TextView mChannelNameTextView;
	}
}