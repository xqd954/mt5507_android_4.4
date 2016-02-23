package com.mediatek.ui.nav.adapter;

import java.util.List;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.os.SystemProperties;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnTouchListener;
import android.widget.BaseAdapter;
import android.widget.TextView;
import android.widget.AbsListView.LayoutParams;

import com.mediatek.ui.pippop.PippopUiLogic;
import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

import com.mediatek.tvcommon.TVOutputCommon;

public class SourceListAdapter extends BaseAdapter {
	/*
	 * context
	 */
	private Context mContext;
	private LayoutInflater mInflater;
	private List<String> mSourceList,mConflictInputsList;
	// private TVInputManager tvInputSource;
	private Drawable mSourceSelectedIcon;
	private Drawable mSourceUnSelectedIcon;
	private Drawable mConflictIcon;
	private  PippopUiLogic mPippopUiLogic;
	int mhlNum = -1;
	public SourceListAdapter(Context context, List<String> mSourceList, List<String> mConflictList

	, Drawable mSourceSelectedIcon, Drawable mSourceUnSelectedIcon,Drawable mConflictIcon) {
		mContext = context;
		mPippopUiLogic = PippopUiLogic.getInstance(mContext);
		mInflater = LayoutInflater.from(mContext);
		this.mSourceList = mSourceList;
		this.mConflictInputsList = mConflictList;
		// this.tvInputSource = tvInputSource;
		this.mSourceSelectedIcon = mSourceSelectedIcon;
		this.mSourceUnSelectedIcon = mSourceUnSelectedIcon;
		this.mConflictIcon = mConflictIcon;
		mhlNum = TVOutputCommon.getInstance(mContext).getMHLPortNum(mPippopUiLogic.getOutputFocus());
		MtkLog.d("tag", "mhlNum:"+mhlNum);
	}

	public int getCount() {
		return mSourceList.size();
	}

	public Object getItem(int position) {
		return position;
	}

	public long getItemId(int position) {
		return position;
	}

	public View getView(int position, View convertView, ViewGroup parent) {
		MtkLog.d("tag", "source adapter get view");
		ViewHolder hodler;
		if (convertView == null) {
			convertView = mInflater.inflate(R.layout.nav_source_item, null);
			hodler = new ViewHolder();
			hodler.mTextView = (TextView) convertView
					.findViewById(R.id.nav_source_list_item_tv);
			hodler.mTextView.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, (int)(ScreenConstant.SCREEN_HEIGHT*0.56*0.83/10)));
			convertView.setTag(hodler);
		} else {
			hodler = (ViewHolder) convertView.getTag();
		}
		
		/*Python doesn't support MHL,so need add a property for modifying the UI */
		int supportMHL=SystemProperties.getInt("mtk.isSupportMHL", 0);
		MtkLog.d("tag", "mtk.isSupportMHL:"+supportMHL);
		
		if (mSourceList.get(position).equals("hdmi"+mhlNum)&&(supportMHL==1)) {
		    hodler.mTextView.setText(mSourceList.get(position)+"/MHL");
        }else {
            hodler.mTextView.setText(mSourceList.get(position));
        }
		// set padding
		hodler.mTextView.setPadding(20, 0, 0, 0);
		hodler.mTextView.setCompoundDrawablePadding(20);

		if (("sub").equals(mPippopUiLogic.getOutputFocus())) {
			if (position == mSourceList.indexOf(mPippopUiLogic
					.getSubOutputSourceName())) {
				hodler.mTextView.setCompoundDrawables(mSourceSelectedIcon,
						null, null, null);
			} else if (mConflictInputsList.contains(mSourceList.get(position))) {
				hodler.mTextView.setCompoundDrawables(mConflictIcon, null,
					null, null);
		} else {
				hodler.mTextView.setCompoundDrawables(mSourceUnSelectedIcon,
						null, null, null);
			}
		} else {
			if (position == mSourceList.indexOf(mPippopUiLogic
					.getMainOutputSourceName())) {
				hodler.mTextView.setCompoundDrawables(mSourceSelectedIcon,
						null, null, null);
			} else if (mConflictInputsList.contains(mSourceList.get(position))) {
				hodler.mTextView.setCompoundDrawables(mConflictIcon, null,
						null, null);
			} else {
				hodler.mTextView.setCompoundDrawables(mSourceUnSelectedIcon,
						null, null, null);
			}
		}

		return convertView;
	}

	private class ViewHolder {
		TextView mTextView;
	}
}