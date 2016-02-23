package com.mediatek.ui.nav.adapter;

import java.util.List;
import android.content.Context;
import android.util.Log;
import android.widget.BaseAdapter;

public abstract class MTKBaseAdapter<T> extends BaseAdapter {
	Context mContext;
	List<T> group = null;

	public MTKBaseAdapter(Context context) {
		mContext = context;
	}

	public Context getContext() {
		return mContext;
	}

	public int getCount() {
		return (group == null) ? 0 : group.size();
	}

	public Object getItem(int position) {
		Log.d("outofindex", "position is :" + position + "==group.size() is :"
				+ group.size());
		if (position < 0) {
			position = 0;
		} else if (position >= group.size()) {
			position = group.size() - 1;
		}
		return group.get(position);
	}

	public long getItemId(int position) {
		return position;
	}

	@Override
	public boolean isEmpty() {
		return (group == null) ? true : group.isEmpty();
	}

	public void setGroup(List<T> g) {
		group = g;
	}

	public void addGroup(List<T> g) {
		if (group != null) {
			group.addAll(g);
		}
	}

	public List<T> getGroup() {
		return group;
	}

}
