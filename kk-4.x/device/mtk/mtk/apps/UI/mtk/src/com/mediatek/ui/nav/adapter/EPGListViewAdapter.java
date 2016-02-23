package com.mediatek.ui.nav.adapter;

import java.util.List;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.os.Handler;
import com.mediatek.ui.util.MtkLog;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.mediatek.ui.R;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.nav.commonview.EPGLinearLayout;
import com.mediatek.ui.nav.commonview.EPGListView;
import com.mediatek.ui.nav.epginfo.EPGChannelInfo;
import com.mediatek.ui.nav.epginfo.EPGProgramInfo;
import com.mediatek.ui.nav.util.DataReader;
import com.mediatek.ui.nav.util.EPGConfig;

public class EPGListViewAdapter extends MTKBaseAdapter<EPGChannelInfo> {
	private static final String TAG = "EPGListViewAdpter";
	private int mStartTime;
	private int mDayNum;
	private Handler mHandler;
	NavIntegration navImp;
	DataReader mReader;
	private EPGListView mEPGListView;

	public EPGListView getEPGListView() {
		return mEPGListView;
	}

	public void setEPGListView(EPGListView epgListView) {
		mEPGListView = epgListView;
	}

	public Handler getHandler() {
		return mHandler;
	}

	public void setHandler(Handler mHandler) {
		this.mHandler = mHandler;
	}

	public int getDayNum() {
		return mDayNum;
	}

	public void setDayNum(int mDayNum) {
		this.mDayNum = mDayNum;
	}

	private int mWidth;

	public int getWidth() {
		return mWidth;
	}

	public void setWidth(int mWidth) {
		this.mWidth = mWidth;
	}

	public int getStartTime() {
		return mStartTime;
	}

	public void setStartTime(int mStartTime) {
		this.mStartTime = mStartTime;
	}

	public EPGListViewAdapter(Context mContext, int mStartTime) {
		super(mContext);
		this.mStartTime = mStartTime;
		navImp = NavIntegration.getInstance(mContext);
		mReader = DataReader.getInstance(mContext);
	}

	@Override
	public void setGroup(List<EPGChannelInfo> group) {
		// TODO Auto-generated method stub
		super.setGroup(group);
		if (group != null && group.size() > 0) {
			mReader.readChannelProgramInfoByTime(group, mDayNum, mStartTime,
					EPGConfig.mTimeSpan);
		}

	}

	public void setGroup(List<EPGChannelInfo> group, int dayNum) {
		super.setGroup(group);
		// TODO Auto-generated method stub
		if (group != null && group.size() > 0) {
			mDayNum = dayNum;
			mReader.readChannelProgramInfoByTime(group, dayNum, mStartTime,
					EPGConfig.mTimeSpan);
		}
	}

	public View getView(int position, View convertView, ViewGroup parent) {
		ViewHolder mViewHolder;
		MtkLog.d(TAG, "-----getView----->[Position] " + position);
		if (convertView == null) {
			mViewHolder = new ViewHolder();
			convertView = LayoutInflater.from(mContext).inflate(
					R.layout.epg_listview_item_layout, null);
			mViewHolder.name = (TextView) convertView
					.findViewById(R.id.epg_channel_name);
			mViewHolder.mDynamicLinearLayout = (EPGLinearLayout) convertView
					.findViewById(R.id.epg_program_forecast_linearlayout);
			mViewHolder.mDynamicLinearLayout.setWidth(mWidth);
			convertView.setTag(mViewHolder);
		} else {
			mViewHolder = (ViewHolder) convertView.getTag();
		}
		if (getCount() > 0) {
			// mViewHolder.mDynamicLinearLayout.requestLayout();
			EPGChannelInfo mChannel = (EPGChannelInfo) getItem(position);
			if (mChannel != null) {
				Drawable analogIcon = mContext.getResources().getDrawable(
						R.drawable.epg_channel_icon);
				analogIcon.setBounds(0, 0, analogIcon.getMinimumWidth(),
						analogIcon.getMinimumWidth());
				Drawable nothingIcon = mContext.getResources().getDrawable(
						R.drawable.translucent_background);
				nothingIcon.setBounds(0, 0, analogIcon.getMinimumWidth(),
						analogIcon.getMinimumWidth());

				mViewHolder.name.setCompoundDrawables(nothingIcon, null, null,
						null);
				mViewHolder.name.setCompoundDrawablePadding(10);
				mViewHolder.name.setText("   " + mChannel.getmChanelNum()
						+ "   " + mChannel.getName());
			}
			// The first time to enter EPG, highlight the current program
			if (null != mChannel && mChannel.getTVChannel().equals(mReader.getCurrentPlayChannel())
					&& EPGConfig.init == true) {
				EPGConfig.init = false;

				if (mChannel.getmTVProgramInfoList() != null
						&& mChannel.getmTVProgramInfoList().size() > 0) {
					int index = mChannel.getPlayingTVProgramPositon();
					mViewHolder.mDynamicLinearLayout
							.setmCurrentSelectPosition(index);
					List<EPGProgramInfo> mChildViewData = mChannel
							.getmTVProgramInfoList();
					MtkLog.d(TAG,
							"---- getView----->[Playing TVProgram Position] "
									+ index + mChildViewData.size());
					MtkLog.d(TAG, "---- getView----->[Child View Size] "
							+ mChildViewData.size());
					mViewHolder.mDynamicLinearLayout.setAdpter(mChildViewData,
							true);
				}
			} else {
				// for digital TV
				if (null != mChannel && null != mChannel.getmTVProgramInfoList() 
						&& mChannel.getmTVProgramInfoList().size() > 0) {
					List<EPGProgramInfo> mChildViewData = mChannel
							.getmTVProgramInfoList();
					MtkLog.d(TAG, "---- getView----->[Child View Size] "
							+ mChildViewData.size());

					mViewHolder.mDynamicLinearLayout.setAdpter(mChildViewData,
							false);
					if (position == EPGConfig.SELECTED_CHANNEL_POSITION) {
						if (EPGConfig.FROM_WHERE == EPGConfig.FROM_KEYCODE_DPAD_LEFT) {
							mViewHolder.mDynamicLinearLayout
									.setSelectedPosition(mChildViewData.size() - 1);
						} else if (EPGConfig.FROM_WHERE == EPGConfig.FROM_KEYCODE_DPAD_RIGHT) {
							mViewHolder.mDynamicLinearLayout.setSelectedPosition(0);
						} else if (EPGConfig.FROM_WHERE == EPGConfig.FROM_KEYCODE_MTKIR_GREEN
								|| EPGConfig.FROM_WHERE == EPGConfig.FROM_KEYCODE_MTKIR_RED) {
							mViewHolder.mDynamicLinearLayout.setSelectedPosition(0);
						} else if (EPGConfig.FROM_WHERE == EPGConfig.FROM_ANOTHER_STREAM) {
							int index = mChannel.getNextPosition(mEPGListView
									.getLastSelectedTVProgram());
							mViewHolder.mDynamicLinearLayout.setSelectedPosition(index);
						}else if(EPGConfig.FROM_WHERE == EPGConfig.AVOID_PROGRAM_FOCUS_CHANGE){
							int index = mChannel.getPlayingTVProgramPositon();
							mViewHolder.mDynamicLinearLayout.setSelectedPosition(index);
						}
					}
				} else {
					mViewHolder.mDynamicLinearLayout
							.setBackgroundResource(R.drawable.epg_analog_channel_bg);

				}
			}
			if (position == group.size() - 1) {
				mHandler.sendEmptyMessage(EPGConfig.EPG_DATA_RETRIEVAL_FININSH);
			}
		}
		return convertView;
	}

	class ViewHolder {
		TextView name;
		EPGLinearLayout mDynamicLinearLayout;
	}

}
