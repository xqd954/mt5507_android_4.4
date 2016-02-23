package com.mediatek.ui.nav.epginfo;

import java.util.Date;

import com.mediatek.ui.util.MtkLog;

public class EPGProgramInfo {

	private static final String TAG = "EPGProgramInfo";

	private Date mStartTime;
	private String mStartTimeStr;
	private Date mEndTime;
	private String mEndTimeStr;
	private String mTitle;
	private String describe;
	private int mainType;
	private int subType;
	private boolean mCancel = false;
	private boolean hasSubTitle = false;
	private float mScale;
	private float mLeftMargin;

	private final static int NULL_VALUE = -1;

	private boolean isDrawRightIcon = false;
	private boolean isDrawLeftIcon = false;

	public EPGProgramInfo() {
		// TODO Auto-generated constructor stub
	}

	public EPGProgramInfo(Date mStartTime, Date mEndTime, String mTitle) {
		this(mStartTime, mEndTime, mTitle, "", NULL_VALUE, NULL_VALUE, false);
	}

	public EPGProgramInfo(Date mStartTime, Date mEndTime, String mTitle,
			String describe, int mainType, int subType, boolean mCancel) {
		this.mStartTime = mStartTime;
		this.mEndTime = mEndTime;
		this.mTitle = mTitle;
		this.describe = describe;
		this.mainType = mainType;
		this.subType = subType;
		this.mCancel = mCancel;
	}

	public EPGProgramInfo(String mStartTimeStr, String mEndTimeStr,
			String mTitle, String describe, int mainType, int subType,
			boolean mCancel, float mScale) {
		this.mStartTimeStr = mStartTimeStr;
		this.mEndTimeStr = mEndTimeStr;
		this.mTitle = mTitle;
		this.describe = describe;
		this.mainType = mainType;
		this.subType = subType;
		this.mCancel = mCancel;
		this.mScale = mScale;
		this.mStartTime = EPGTimeConvert.getNormalDate(mStartTimeStr);
		this.mEndTime = EPGTimeConvert.getNormalDate(mEndTimeStr);
	}

	public EPGProgramInfo(String mStartTimeStr, String mEndTimeStr,
			String mTitle) {
		this(mStartTimeStr, mEndTimeStr, mTitle, "", NULL_VALUE, NULL_VALUE,
				false, 0.0f);
	}

	public boolean isDrawLeftIcon() {
		return isDrawLeftIcon;
	}

	public void setDrawLeftIcon(boolean isDrawLeftIcon) {
		this.isDrawLeftIcon = isDrawLeftIcon;
	}

	public boolean isDrawRightwardIcon() {
		return isDrawRightIcon;
	}

	public void setDrawRightIcon(boolean isDrawRightIcon) {
		this.isDrawRightIcon = isDrawRightIcon;
	}

	public String getmStartTimeStr() {
		return mStartTimeStr;
	}

	public void setmStartTimeStr(String mStartTimeStr) {
		this.mStartTimeStr = mStartTimeStr;
	}

	public String getmEndTimeStr() {
		return mEndTimeStr;
	}

	public void setmEndTimeStr(String mEndTimeStr) {
		this.mEndTimeStr = mEndTimeStr;
	}

	public String getmTitle() {
		return mTitle;
	}

	public void setmTitle(String mTitle) {
		this.mTitle = mTitle;
	}

	public String getDescribe() {
		MtkLog.d(TAG, "EPG event detail: " + describe);
		return describe;
	}

	public void setDescribe(String describe) {
		this.describe = describe;
	}

	public int getMainType() {
		return mainType;
	}

	public void setMainType(int mainType) {
		this.mainType = mainType;
	}

	public int getSubType() {
		return subType;
	}

	public void setSubType(int subType) {
		this.subType = subType;
	}

	public boolean ismCancel() {
		return mCancel;
	}

	public void setmCancel(boolean mCancel) {
		this.mCancel = mCancel;
	}

	public boolean isHasSubTitle() {
		return hasSubTitle;
	}

	public void setHasSubTitle(boolean mHasSubTilte) {
		this.hasSubTitle = mHasSubTilte;
	}

	public float getmScale() {
		return mScale;
	}

	public void setmScale(float mScale) {
		this.mScale = mScale;
	}

	public Date getmStartTime() {
		return mStartTime;
	}

	public float getLeftMargin() {
		return mLeftMargin;
	}

	public void setLeftMargin(float mLeftMargin) {
		this.mLeftMargin = mLeftMargin;
	}

	public void setmStartTime(Date mStartTime) {
		this.mStartTime = mStartTime;
	}

	public Date getmEndTime() {
		return mEndTime;
	}

	public void setmEndTime(Date mEndTime) {
		this.mEndTime = mEndTime;
	}

	public String getTitle() {
		return mTitle;
	}

	public void setTitle(String title) {
		this.mTitle = title;
	}

	public int countTimeZoom(int hours, int timeZoomSpan) {
		// Subscript is to start from zero, so to minus 1
		int timeZoom = hours % timeZoomSpan == 0 ? hours / timeZoomSpan - 1
				: hours / timeZoomSpan;
		return timeZoom;
	}

	public float dateToMinutes(Date time, int mTimeZoomSpan, boolean flag) {
		int timeZoom = countTimeZoom(time.getHours(), mTimeZoomSpan);
		if (flag == true) {
			return (time.getHours() - timeZoom * mTimeZoomSpan - 1) * 60.0f
					+ time.getMinutes();
		}
		return ((timeZoom + 1) * mTimeZoomSpan + 1 - time.getHours()) * 60.0f
				- time.getMinutes();
	}
}
