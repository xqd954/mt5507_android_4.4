/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.ui;

import android.content.Context;
import android.view.KeyEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import com.mediatek.dm.MountPoint;
import com.mediatek.timeshift_pvr.controller.ScheduleItem;
import com.mediatek.timeshift_pvr.controller.StateScheduleList;
import com.mediatek.timeshift_pvr.controller.StateScheduleListAdapter;
import com.mediatek.timeshift_pvr.controller.StateScheduleListCallback;
import com.mediatek.timeshift_pvr.manager.Core;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.ui.R;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;

import java.util.ArrayList;
import java.util.List;

/**
 *
 */
public class ScheduleListDialog extends CommonDialog implements
		OnItemClickListener {

	private float wScale = 0.7f;
	private float hScale = 0.8f;

	private ListView mScheduleList;
	private TextView mNoRecord;
	private ImageView selectView;
	private TextView selectText;
	private TextView mDiskInfoTitle;

	private List<ScheduleItem> itemList = new ArrayList<ScheduleItem>();

	private StateScheduleListCallback<ScheduleItem> mCallback;

	private StateScheduleList mState;
	private Context mContext;

	/**
	 * @param context
	 * @param layoutID
	 */
	public ScheduleListDialog(Context context, StateScheduleList state) {
		super(context, R.layout.pvr_tshfit_schudulelist);
		mContext = context;
		this.getWindow().setLayout(
				(int) (state.getManager().getTVWidth() * wScale),
				(int) (state.getManager().getTVHeight() * hScale));

		this.mState = state;
		// initView2();
	}

	@Override
	public void initView() {
		super.initView();

		mScheduleList = (ListView) findViewById(R.id.schedulelist_list);
		mScheduleList.setDivider(null);
		mNoRecord = (TextView) findViewById(R.id.schedulelist_nofiles);
		mDiskInfoTitle = (TextView) findViewById(R.id.schedulelist_title_diskinfo);
		selectView = (ImageView) findViewById(R.id.schedulelist_icon_select);
		selectText = (TextView) findViewById(R.id.schedulelist_icon_selecttext);

		initListener();
	}

	private void initView2() {
		if (getItemList().size() > 0) {
			mScheduleList
					.setAdapter(new StateScheduleListAdapter<ScheduleItem>(
							mContext, getItemList()));
			mNoRecord.setVisibility(View.INVISIBLE);
			selectView.setVisibility(View.VISIBLE);
			selectText.setVisibility(View.VISIBLE);
		} else {
			mNoRecord.setVisibility(View.VISIBLE);
			mScheduleList.setVisibility(View.INVISIBLE);
	         selectView.setVisibility(View.GONE);
	            selectText.setVisibility(View.GONE);
		}
		setDiskInfoTitle();
		updateDissmissTimer();
		
		
		modifyUIWhenNoChannels();
	}
	private void modifyUIWhenNoChannels() {
		// TODO Auto-generated method stub
		if(mState.getTvLogicManager().getChannels().size()<=0){
			
			ImageView iconView=(ImageView) findViewById(R.id.schedule_add_item_icon);
			TextView strView=(TextView) findViewById(R.id.schedule_add_item_str);
			
			iconView.setVisibility(View.INVISIBLE);
			strView.setVisibility(View.INVISIBLE);
			
			mNoRecord.setText(mContext.getString(R.string.schedulelist_nofiles_disable_add));
		}
	}

	public void setDiskInfoTitle() {
		MountPoint mp = mState.getManager().getPvrMP();
		String str = Util.getGBSizeOfDisk(mp);
		mDiskInfoTitle.setText(str);
	}

	/*
	 * 
	 */
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		// setItemList(StateScheduleList.queryItem(mState.getActivity(),0));
		updateDissmissTimer();
		switch (keyCode) {
		case KeyEvent.KEYCODE_MEDIA_EJECT:
			if(mState.getTvLogicManager().getChannels().size()<1){
				return true;
			}
			ScheduleItem item=new ScheduleItem();
			item.setChannelName("1");
			item.setNewItem(true);
			
			mState.switchToScheduleItemInfo(item);
			
			break;
	     case KeyMap.KEYCODE_BACK:
	         this.dismiss();
	         MtkLog.e("KEYCODE_BACK", "KEYCODE_BACK");
			break;
		default:
			break;
		}

		return super.onKeyDown(keyCode, event);
	}

	/**
	 * 
	 */
	private void initListener() {
		mScheduleList.setOnItemClickListener(this);
	}

	/*
	 * 
	 */
	@Override
	public void onItemClick(AdapterView<?> parent, View view, int position,
			long id) {
		updateDissmissTimer();
		mCallback.onItemClick(getItemList().get(position));
	}

	public void setCallback(StateScheduleListCallback callback) {
		mCallback = callback;
	}

	public StateScheduleListCallback getmCallback() {
		return mCallback;
	}

	/**
	 * @return the itemList
	 */
	public List<ScheduleItem> getItemList() {
		return itemList;
	}

	/**
	 * @param itemList
	 *            the itemList to set
	 */
	public void setItemList(List<ScheduleItem> itemList) {
		this.itemList = itemList;
		initView2();
	}

	public void updateDissmissTimer(){
		((StateScheduleList)mState).mHandler.removeMessages(StateScheduleList.Auto_Dismiss_List_Dialog_Timer);
		((StateScheduleList)mState).mHandler.sendEmptyMessageDelayed(StateScheduleList.Auto_Dismiss_List_Dialog_Timer, Core.AUTO_DISSMISS_TIMER);
	}
}
