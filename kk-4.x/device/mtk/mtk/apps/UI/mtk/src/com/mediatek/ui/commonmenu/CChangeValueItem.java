package com.mediatek.ui.commonmenu;


import com.mediatek.ui.R;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.widget.TextView;

public class CChangeValueItem extends CListItem  {
	private TextView mConfigName;
	private TextView mConfigValue;
	private String name;
	private int position;
	private CMenuIntegration mCMenuIntegration;
			
	public CChangeValueItem(Context context, AttributeSet attrs) {
		super(context, attrs);
		
		initView(context);
	}

	public CChangeValueItem(Context context) {
		super(context);
		
		initView(context);
	}
	public CChangeValueItem(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		
		initView(context);
	}
	@Override
	public void setData(CDataItem dataItem){
		this.mDataItem=dataItem;
//		ID=dataItem.getmConfigID();
		name=mDataItem.getmConfigName();
		position=mDataItem.getmDefaultValue();
		String value=mDataItem.getmConfigValues()[position];
		mConfigName.setText(name);
		if(position<mDataItem.getmConfigValues().length){
			mConfigValue.setText(value);
		}
	}
	private void initView(Context context){
		mCMenuIntegration=CMenuIntegration.getInstance(context);
		LayoutInflater.from(context).inflate(R.layout.thr_menu_change_item, this);
		mConfigName=(TextView)this.findViewById(R.id.thr_menu_change_name);
		mConfigValue=(TextView)this.findViewById(R.id.thr_menu_change_value);
	}
	

	@Override 
	public void onPressLeft() {
		defValue=mDataItem.getmDefaultValue();
		if(defValue>=0&&defValue<mDataItem.getmConfigValues().length){
			if(defValue>0){
				defValue--;
			}else{
				defValue=mDataItem.getmConfigValues().length-1;
			}
		}else{
			defValue=0;
		}
		//onValueChanged(mDataItem.getmConfigID(), defValue,-1);
		onValueChangedLeft(mDataItem.getmConfigID(), defValue,-1);
	}
	private int defValue=0;
	@Override
	public void onPressRigth() {

		defValue=mDataItem.getmDefaultValue();
		if(defValue>=0&&defValue<mDataItem.getmConfigValues().length){
			if(defValue<mDataItem.getmConfigValues().length-1){
				defValue++;
			}else{
				defValue=0;
			}
		}else{
			defValue=0;
		}
				
		onValueChangedRight(mDataItem.getmConfigID(),defValue,1);
	}

//	private void onValueChanged(String ID,int value,int step){
//		mDataItem.setmDefaultValue(defValue);
//		mConfigValue.setText(mDataItem.getmConfigValues()[defValue]);
//		mCMenuIntegration.setNextEffect(ID, step);
//	}
	
	private void onValueChangedRight(String ID,int value,int step){
		mDataItem.setmDefaultValue(defValue);
		mConfigValue.setText(mDataItem.getmConfigValues()[defValue]);		
		
		if(ID.equals(CMenuIntegration.CONFIG_PICTURE_EFFECT)){
			mCMenuIntegration.setNextPictureEffect();
		}else if(ID.equals(CMenuIntegration.CONFIG_SOUND_EFFECT)){
			mCMenuIntegration.setNextSoundEffect();
		}/*else if(ID.equals(CMenuIntegration.CONFIG_TRACK)){
			mCMenuIntegration.setTrackEffect((short)value);
		}*/		
	}
	
	private void onValueChangedLeft(String ID,int value,int step){
		mDataItem.setmDefaultValue(defValue);
		mConfigValue.setText(mDataItem.getmConfigValues()[defValue]);
		
		if(ID.equals(CMenuIntegration.CONFIG_PICTURE_EFFECT)){
			mCMenuIntegration.setPrePictureEffect();
		}else if(ID.equals(CMenuIntegration.CONFIG_SOUND_EFFECT)){
			mCMenuIntegration.setPreSoundEffect();
		}/*else if(ID.equals(CMenuIntegration.CONFIG_TRACK)){
			mCMenuIntegration.setTrackEffect((short)value);
		}*/		
	}
	
	@Override
	public void onPressEnter() {
		// TODO Auto-generated method stub		
	}

	

	public int getDefaultValue() {

		return defValue;
	}
}
