package com.mediatek.ui.cec;

import java.util.List;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.LinearLayout;
import android.widget.TextView;
import com.mediatek.ui.menu.commonview.RespondedKeyEvent;


import com.mediatek.ui.R;
/**
 * This view includes a text view (the name) and a image view(selected or not),
 * it is one item of list view items. you can press ENTER or RIGHT key to show
 * its sub view
 * 
 * @author hs_yaoxiao
 */

public class CecMainItemView extends CecListItemView implements
		RespondedKeyEvent {

	private TextView mNameView;
	private List<CecListviewAdapter.CecDataItem> mSubChildGroup;

	public CecMainItemView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		init();
	}

	public CecMainItemView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		init();
	}

	public CecMainItemView(Context context) {
		super(context);
		this.context = context;
		init();
	}

	public void setValue(int value) {
	}

	public int getValue() {
		return 0;
	}

	public void showValue(int value) {
	}

	public TextView getNameView() {
		return mNameView;
	}

	private void setViewName(String viewName) {
		mNameView.setText(viewName);
	}

	private void init() {
//		this.addView(inflate(context, R.layout.menu_hava_subpage_view, null));
		LinearLayout lv = (LinearLayout) inflate(mContext,
				R.layout.menu_hava_subpage_view, null);
		LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
		addView(lv, params);

		mNameView = (TextView) findViewById(R.id.common_itemname);
		
	}

	public void setAdapter(CecListviewAdapter.CecDataItem mDataItem) {
		this.mDataItem = mDataItem;
		setViewName(mDataItem.getName());
//		this.mSubChildGroup = mDataItem.getmSubChildGroup();
		if (mDataItem.mItemID.equals(CecManager.CEC_Able_ID)) {
		    initEnable(mDataItem.mInitValue);
        }
		
	}

	public void onKeyEnter() {
	    changeName();
	}

	public void onKeyLeft() {
	}

	public void onKeyRight() {
	    changeName();
	}
	
	public void initEnable(int value){
	    if (value==0) {
	        mNameView.setText(R.string.menu_setup_cec_enablesys);
        }else if (value==1){
            mNameView.setText(R.string.menu_setup_cec_disablesys);
        }
	    
	}
	private void changeName(){
	    if (mDataItem.mItemID.equals(CecManager.CEC_Able_ID)) {
	        String vName = mNameView.getText().toString();
	        if (vName.equals(context.getString(R.string.menu_setup_cec_enablesys))) {
	            mNameView.setText(R.string.menu_setup_cec_disablesys);
	            CecManager.getInstance().setSACon();
	        }else if(vName.equals(context.getString(R.string.menu_setup_cec_disablesys))){
	            mNameView.setText(R.string.menu_setup_cec_enablesys);
	            CecManager.getInstance().setSACoff();
	        }
        }
	}

}
