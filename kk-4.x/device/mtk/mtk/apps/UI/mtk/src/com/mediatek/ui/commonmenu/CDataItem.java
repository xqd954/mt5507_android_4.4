package com.mediatek.ui.commonmenu;

import java.util.Arrays;

public class CDataItem {

	public static final int CHANGEITEM=1;
	public static final int PROGRESSITEM=2;
	private String mConfigID;
	private String mConfigName;
	private String[] mConfigValues;
	private int mDefaultValue;
	private int mMaxValue;
	private int mMinValue;
	private int mItemType;
	public CDataItem(String mConfigID, String mConfigName,
			String[] mConfigValues, int mDefaultValue, int mMaxValue,
			int mMinValue, int mItemType) {
		super();
		this.mConfigID = mConfigID;
		this.mConfigName = mConfigName;
		this.mConfigValues = mConfigValues;
		this.mDefaultValue = mDefaultValue;
		this.mMaxValue = mMaxValue;
		this.mMinValue = mMinValue;
		this.mItemType = mItemType;
	}
	public String getmConfigID() {
		return mConfigID;
	}
	public void setmConfigID(String mConfigID) {
		this.mConfigID = mConfigID;
	}
	public String getmConfigName() {
		return mConfigName;
	}
	public void setmConfigName(String mConfigName) {
		this.mConfigName = mConfigName;
	}
	public String[] getmConfigValues() {
		return mConfigValues;
	}
	public void setmConfigValues(String[] mConfigValues) {
		this.mConfigValues = mConfigValues;
	}
	public int getmDefaultValue() {
		return mDefaultValue;
	}
	public void setmDefaultValue(int mDefaultValue) {
		this.mDefaultValue = mDefaultValue;
	}   
	public int getmMaxValue() {
		return mMaxValue;
	}
	public void setmMaxValue(int mMaxValue) {
		this.mMaxValue = mMaxValue;
	}
	public int getmMinValue() {
		return mMinValue;
	}
	public void setmMinValue(int mMinValue) {
		this.mMinValue = mMinValue;
	}
	public int getmItemType() {
		return mItemType;
	}
	public void setmItemType(int mItemType) {
		this.mItemType = mItemType;
	}
	@Override
	public String toString() {
		return "ThrDataItem [mConfigID=" + mConfigID + ", mConfigName="
				+ mConfigName + ", mConfigValues="
				+ Arrays.toString(mConfigValues) + ", mDefaultValue="
				+ mDefaultValue + ", mMaxValue=" + mMaxValue + ", mMinValue="
				+ mMinValue + ", mItemType=" + mItemType + "]";
	}
							
}
