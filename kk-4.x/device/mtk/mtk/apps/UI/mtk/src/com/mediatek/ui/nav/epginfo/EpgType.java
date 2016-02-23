package com.mediatek.ui.nav.epginfo;

import java.util.List;

import com.mediatek.ui.R;
import com.mediatek.ui.nav.EPGActivity;
import com.mediatek.ui.nav.adapter.EPGTypeListAdapter;
import com.mediatek.ui.nav.adapter.EPGTypeListAdapter.EPGListViewDataItem;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.Display;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.AdapterView.OnItemSelectedListener;

public class EpgType extends Dialog {

	private static final String TAG = "EpgType";

	ListView epgList;
	ListView epgSubList;
	EPGTypeListAdapter listAdapter;
	EPGTypeListAdapter subAdapter;
	List<EPGListViewDataItem> mData;
	private Context mContext;

	public EpgType(Context context, boolean cancelable,
			OnCancelListener cancelListener) {
		super(context, cancelable, cancelListener);
		this.mContext = context;
	}

	public EpgType(Context context, int theme) {
		super(context, theme);
		this.mContext = context;
	}

	public EpgType(Context context) {
		super(context, R.style.Theme_EpgSubTypeDialog);
		this.mContext = context;
	}

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.epg_type_main);
		setWindowPosition();
		epgList = (ListView) findViewById(R.id.nav_epg_type_view);
		epgSubList = (ListView) findViewById(R.id.nav_epg_sub_type_view);
		epgList.setDividerHeight(0);
		epgSubList.setDividerHeight(0);

		listAdapter = new EPGTypeListAdapter(mContext, epgList, epgSubList);
		subAdapter = new EPGTypeListAdapter(mContext, epgList, epgSubList);
		mData = listAdapter.loadEPGFilterTypeData();
		listAdapter.setEPGGroup(mData);
		if (mData != null) {
			epgList.setAdapter(listAdapter);
			MtkLog.d(TAG, "*********** mData is not null**************");
			subAdapter.setEPGGroup(mData.get(0).getSubChildDataItem());
		} else {
			MtkLog.d(TAG, "*********** mData is null **************");
		}
		epgSubList.setAdapter(subAdapter);

		epgList.setOnKeyListener(new View.OnKeyListener() {

			public boolean onKey(View v, int keyCode, KeyEvent event) {
				// TODO Auto-generated method stub
				if (event.getAction() == KeyEvent.ACTION_DOWN) {
					listAdapter.onMainKey(v, keyCode);
				}
				return false;
			}

		});
		epgList.setOnItemSelectedListener(new OnItemSelectedListener() {

			public void onItemSelected(AdapterView<?> parent, View view,
					int position, long id) {
				// TODO Auto-generated method stub
				List<EPGListViewDataItem> subChildDataItem = listAdapter
						.getEPGData().get(position).getSubChildDataItem();
				subAdapter.setEPGGroup(subChildDataItem);
				epgSubList.setAdapter(subAdapter);

			}

			public void onNothingSelected(AdapterView<?> parent) {
				// TODO Auto-generated method stub

			}

		});

		epgSubList.setOnKeyListener(new View.OnKeyListener() {

			public boolean onKey(View v, int keyCode, KeyEvent event) {
				// TODO Auto-generated method stub
				if (event.getAction() == KeyEvent.ACTION_DOWN) {
					subAdapter.onSubKey(v, keyCode);
				}
				return false;
			}

		});

	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_MTKIR_BLUE:
		case KeyEvent.KEYCODE_B:
		case KeyEvent.KEYCODE_BACK:
			if (this.isShowing()) {
				dismiss();
				EPGActivity epgActivity = (EPGActivity)mContext;
				epgActivity.changeBottomViewText(false, 0);
			}
			break;
		case KeyEvent.KEYCODE_MTKIR_SOURCE:
			return true;
		default:
			break;
		}
		
		if (keyCode == KeyMap.KEYCODE_VOLUME_UP
				|| keyCode == KeyMap.KEYCODE_VOLUME_DOWN) {
			return true;
		}
		return super.onKeyDown(keyCode, event);
	}
	
	

	private static int menuWidth = 800;
	private static int menuHeight = 610;
	public void setWindowPosition() {
		Window window = getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();
		menuWidth = 650*ScreenConstant.SCREEN_WIDTH /1280;
		menuHeight = 610*ScreenConstant.SCREEN_HEIGHT/720;
		lp.width = menuWidth;
		lp.height = menuHeight;
		window.setAttributes(lp);
	}

	
}