package com.mediatek.ui.commonmenu;

import com.mediatek.ui.util.ScreenConstant;
import android.content.Context;
import android.util.AttributeSet;
import android.view.WindowManager;
import android.widget.LinearLayout;
import android.widget.ListView;

public class CListItem extends LinearLayout implements CItemAction{

	protected CDataItem mDataItem;
	public CListItem(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	public CListItem(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	public CListItem(Context context) {
		super(context);
		setLayoutParams(new ListView.LayoutParams(
				ListView.LayoutParams.MATCH_PARENT, getSize(context)));
		setFocusable(true);
		setClickable(true);
		setEnabled(true);
	}
	public  void setData(CDataItem dataItem){
		
	};
	
	public int getSize(Context context) {
		WindowManager wm = (WindowManager) context
				.getSystemService(Context.WINDOW_SERVICE);
		int height = 1;
		if(wm != null){
			 height = ScreenConstant.SCREEN_HEIGHT;
			}
		return height / 15;
	}

	@Override
	public void onPressLeft() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onPressRigth() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onPressEnter() {
		// TODO Auto-generated method stub
		
	}
	
	
	
	
	
	

}
