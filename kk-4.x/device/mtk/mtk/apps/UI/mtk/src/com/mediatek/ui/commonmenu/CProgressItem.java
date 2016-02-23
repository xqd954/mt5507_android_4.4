package com.mediatek.ui.commonmenu;


import com.mediatek.ui.R;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.widget.ProgressBar;
import android.widget.TextView;

public class CProgressItem extends CListItem {
	private TextView mConfigName;
	private TextView mDefaultValue;
	private ProgressBar mProgressBar;
	private CMenuIntegration mCMenuIntegration;
//	private String ID;
	
	

//	public String getID() {
//		return ID;
//	}
//
//	public void setID(String iD) {
//		ID = iD;
//	}

//	private OnValueChangedListener mValueChangedListener;
//
//	public void setValueChangedListener(
//			OnValueChangedListener mValueChangedListener) {
//		this.mValueChangedListener = mValueChangedListener;
//	}
//	
	public CProgressItem(Context context, AttributeSet attrs) {
		super(context, attrs);
		
		initView(context);
	}

	public CProgressItem(Context context) {
		super(context);
		
		initView(context);
	}
	public CProgressItem(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		initView(context);
	}
	
	public void setName(String name){
		mConfigName.setText(name);
	}
	
	@Override
	public void setData(CDataItem dataItem){
		this.mDataItem=dataItem;
//		ID=dataItem.getmConfigID();
		Log.e("dataitem", "dateItem:"+mDataItem+mConfigName+mDataItem.getmConfigName());
		mConfigName.setText(mDataItem.getmConfigName());
		mProgressBar.setMax(mDataItem.getmMaxValue()-mDataItem.getmMinValue());
		if(mDataItem.getmDefaultValue()<mDataItem.getmMaxValue()){
			mProgressBar.setProgress(mDataItem.getmDefaultValue()-mDataItem.getmMinValue());
		}
		mDefaultValue.setText(mDataItem.getmDefaultValue()+"");
	}
	private void initView(Context context){
		mCMenuIntegration=CMenuIntegration.getInstance(context);
		LayoutInflater.from(context).inflate(R.layout.thr_menu_progress_item, this);
		mConfigName=(TextView)this.findViewById(R.id.thr_menu_progress_name);
		mDefaultValue=(TextView)this.findViewById(R.id.thr_menu_progress_value);
		mProgressBar=(ProgressBar)this.findViewById(R.id.thr_menu_progress);
		
	}
    

	@Override
	public void onPressLeft() {
		int defValue=mDataItem.getmDefaultValue();
		if(defValue>=mDataItem.getmMinValue()&&defValue<=mDataItem.getmMaxValue()){
			if(defValue>mDataItem.getmMinValue()){
				defValue--;
			}
		}else{
			defValue=mDataItem.getmMinValue();
		}				
		mProgressBar.setProgress(defValue-mDataItem.getmMinValue());
		mDefaultValue.setText(defValue+"");
		mDataItem.setmDefaultValue(defValue);
		onValueChanged(mDataItem.getmConfigID(), defValue, -1);
	}

	@Override
	public void onPressRigth() {
		int defValue=mDataItem.getmDefaultValue();
		if(defValue>=mDataItem.getmMinValue()&&defValue<=mDataItem.getmMaxValue()){
			if(defValue<mDataItem.getmMaxValue()){
				defValue++;
			}
		}else{
			defValue=mDataItem.getmMinValue();
		}
		mProgressBar.setProgress(defValue-mDataItem.getmMinValue());
		mDefaultValue.setText(defValue+"");
		mDataItem.setmDefaultValue(defValue);
		onValueChanged(mDataItem.getmConfigID(), defValue, 1);
	}

	private void onValueChanged(String ID,int value,int step){
		if(ID.equals(CMenuIntegration.CONFIG_VOLUME)){
			mCMenuIntegration.setVolume(step);
		}
		
//		mDataItem.setmDefaultValue(value);
//		mConfigValue.setText(mDataItem.getmConfigValues()[value]);
//		mCMenuIntegration.setNextEffect(ID, step);
//		if(ID.equals(CMenuIntegration.CONFIG_PICTURE_EFFECT)){
			
			
//		}else if(ID.equals(CMenuIntegration.CONFIG_SOUND_EFFECT)){
//			
//		}
	}
	
	@Override
	public void onPressEnter() {
		
	}

}
