package com.mediatek.ui.commonmenu;

import java.util.List;
import android.content.Context;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;



public class CMenuAdapter extends BaseAdapter {

	private Context mContext;
	private List<CDataItem> mMenuList;
		
	public CMenuAdapter(Context context, List<CDataItem> mMenuList
			) {
		mContext = context;
		this.mMenuList = mMenuList;				
	}

	@Override
	public int getCount() {
		return mMenuList.size();
	}

	@Override
	public Object getItem(int position) {
		if(mMenuList!=null&&mMenuList.size()>position){
			return mMenuList.get(position);
		}
		return null;
	}

	@Override
	public long getItemId(int position) {
		return position;
	}
	
	private int getType(int position){
		return mMenuList.get(position).getmItemType();
	}
	
	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		LocalView locView=new LocalView();
		int type=getType(position);
		
	
			switch (type) {
			case CDataItem.CHANGEITEM:
				if (locView.changeView == null) {
					locView.changeView = new CChangeValueItem(mContext);
					
				}
				Log.e("adapter",mMenuList.get(position)+"" );
				locView.changeView.setData(mMenuList.get(position));
				System.out.println("-----position------"+position);
				return 	locView.changeView;
			case CDataItem.PROGRESSITEM:
				if (locView.proView == null) {
					locView.proView = new CProgressItem(mContext);
				}
				System.out.println("-----position------"+position);
				locView.proView.setData(mMenuList.get(position));				
				return locView.proView;
			default:
				break;
			}
		System.out.println("-----position----x--"+position);
		return convertView;
	}

	
	private class LocalView{
		CChangeValueItem changeView;
		CProgressItem proView;
	}



}
