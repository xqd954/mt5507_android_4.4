package com.mediatek.ui.nav.adapter;

import java.util.ArrayList;
import java.util.List;

import com.mediatek.ui.R;
import com.mediatek.ui.menu.util.SaveValue;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

import android.content.Context;
import android.graphics.Color;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView.LayoutParams;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;

public class EPGTypeListAdapter extends BaseAdapter {

	private static final String TAG = "EPGListAdapter";
	List<EPGListViewDataItem> mData;
	Context mContext;
	SaveValue sv;

	ListView mList;
	ListView sList;

	ViewHolder mViewHolder;

	static int mPosition = 0;
	static int sPosition = 0;
	private static boolean mFocus = true;

	EPGTypeListAdapter(Context context) {
		mContext = context;
		sv = SaveValue.getInstance(context);

	}

	public EPGTypeListAdapter(Context context, ListView mainList,
			ListView subList) {
		mContext = context;
		mList = mainList;
		sList = subList;
		mPosition = 0;
		sPosition = 0;
		mFocus = true;
		sv = SaveValue.getInstance(context);
	}

	public int getCount() {
		// TODO Auto-generated method stub
		if (mData == null) {
			return 0;
		}
		return mData.size();
	}

	public Object getItem(int position) {
		// TODO Auto-generated method stub
		return mData == null ? null : mData.get(position);
	}

	public long getItemId(int position) {
		// TODO Auto-generated method stub
		return position;
	}

	public View getView(int position, View convertView, ViewGroup parent) {
		// TODO Auto-generated method stub

		if (convertView == null) {
			mViewHolder = new ViewHolder();
			convertView = LayoutInflater.from(mContext).inflate(
					R.layout.epg_type_item_layout, null);
			mViewHolder.imageView = (ImageView) convertView
					.findViewById(R.id.epg_type_icon);
			mViewHolder.mTextView = (TextView) convertView
					.findViewById(R.id.epg_type_name);
			convertView.setTag(mViewHolder);
		} else {
			mViewHolder = (ViewHolder) convertView.getTag();
		}
		if (mData != null && mData.size() > 0) {
			if (null !=mData.get(position) 
					 &&  mData.get(position).data.length() != 0) {
				if (mViewHolder.mTextView != null) {
					MtkLog.d(TAG, "--- Text View is null ---");
				mViewHolder.mTextView.setText(mData.get(position).data);
				mViewHolder.mTextView.setTextColor(Color.LTGRAY);
				MtkLog.i(TAG, "++++++++++ getView() Position: " + position
						+ "    Data: "
						+ sv.readBooleanValue(mData.get(position).data));
				if (sv.readBooleanValue(mData.get(position).data)) {
					mViewHolder.imageView.setVisibility(View.VISIBLE);
				} else {
					mViewHolder.imageView.setVisibility(View.INVISIBLE);
				}
			 }
			}

		}
		convertView.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, (int)(ScreenConstant.SCREEN_HEIGHT*0.85*0.83/14)));
		return convertView;
	}

	class ViewHolder {
		ImageView imageView;
		TextView mTextView;
	}

	public List<EPGListViewDataItem> getEPGData() {
		return mData;
	}

	public void setEPGGroup(List<EPGListViewDataItem> data) {
		this.mData = data;
	}

	public List<EPGListViewDataItem> loadEPGFilterTypeData() {
		String[][] sType = new String[11][20];
		String[] mType = mContext.getResources().getStringArray(
				R.array.nav_epg_filter_type);
		sType[0] = mContext.getResources().getStringArray(
				R.array.nav_epg_subtype_movie);
		sType[1] = mContext.getResources().getStringArray(
				R.array.nav_epg_subtype_news);
		sType[2] = mContext.getResources().getStringArray(
				R.array.nav_epg_subtype_show);
		sType[3] = mContext.getResources().getStringArray(
				R.array.nav_epg_subtype_sports);
		sType[4] = mContext.getResources().getStringArray(
				R.array.nav_epg_subtype_children);
		sType[5] = mContext.getResources().getStringArray(
				R.array.nav_epg_subtype_music);
		sType[6] = mContext.getResources().getStringArray(
				R.array.nav_epg_subtype_arts);
		sType[7] = mContext.getResources().getStringArray(
				R.array.nav_epg_subtype_social);
		sType[8] = mContext.getResources().getStringArray(
				R.array.nav_epg_subtype_education);
		sType[9] = mContext.getResources().getStringArray(
				R.array.nav_epg_subtype_leisure);
		sType[10] = mContext.getResources().getStringArray(
				R.array.nav_epg_subtype_special);
		List<EPGListViewDataItem> mDataGroup = new ArrayList<EPGListViewDataItem>();
		for (int i = 0; i < mType.length; i++) {
			EPGListViewDataItem mTypeData = new EPGListViewDataItem(mType[i]);
			List<EPGListViewDataItem> mSubTypeData = new ArrayList<EPGListViewDataItem>();
			for (int j = 0; j < sType[i].length; j++) {
				EPGListViewDataItem sTypeData = new EPGListViewDataItem(
						sType[i][j]);
				mSubTypeData.add(sTypeData);
			}
			mTypeData.setSubChildDataItem(mSubTypeData);
			mDataGroup.add(mTypeData);

		}
		return mDataGroup;

	}

	public class EPGListViewDataItem {
		protected String data;
		boolean marked = false;
		List<EPGListViewDataItem> mSubChildDataItem;

		public List<EPGListViewDataItem> getSubChildDataItem() {
			return mSubChildDataItem;
		}

		public void setSubChildDataItem(
				List<EPGListViewDataItem> mSubChildDataItem) {
			this.mSubChildDataItem = mSubChildDataItem;
		}

		public EPGListViewDataItem() {

		}

		public EPGListViewDataItem(String data) {
			this.data = data;
		}

		public String getData() {
			return data;
		}

		public void setData(String data) {
			this.data = data;
		}

		public boolean isMarked() {
			return marked;
		}

		public void setMarked(boolean marked) {
			this.marked = marked;
		}
	}

	public void onMainKey(View v, int keyCode) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_DPAD_RIGHT:
			if (mFocus) {
				sPosition = 0;
				MtkLog.d(TAG, " ==== sub list select item position: "
						+ sPosition);
				((TextView) ((LinearLayout) mList.getSelectedView())
						.getChildAt(1)).setTextColor(Color.YELLOW);
				sList.setSelection(sPosition);
				sList.requestFocus();
				mFocus = false;
			}
			break;
		case KeyEvent.KEYCODE_DPAD_UP:
			mPosition--;
			MtkLog.d(TAG, " ==== main list select item position: " + mPosition);
			MtkLog.i(TAG, " ==== main list item nums: "
					+ mList.getAdapter().getCount());
			if (mFocus) {
				if (mPosition < 0) {
					mPosition = mList.getAdapter().getCount() - 1;
					MtkLog.i(TAG, " main list select item position: "
							+ mPosition);
					mList.setSelection(mPosition);
				}
			}

			break;
		case KeyEvent.KEYCODE_DPAD_DOWN:
			mPosition++;
			MtkLog.d(TAG, " ==== main list select item position: " + mPosition);
			if (mFocus) {
				if (mPosition >= mList.getAdapter().getCount()) {
					mPosition = 0;
					MtkLog.i(TAG, " main list select item position: "
							+ mPosition);
					mList.setSelection(mPosition);
				}
			}
			break;
		case KeyEvent.KEYCODE_DPAD_CENTER:
		case KeyEvent.KEYCODE_E:
			MtkLog.d(TAG, "Position: " + mPosition + "      is selected: "
					+ mData.get(mPosition).marked);

			if (sv.readBooleanValue(mData.get(mPosition).data)) {
				mData.get(mPosition).marked = false;
				((LinearLayout) mList.getSelectedView()).getChildAt(0)
						.setVisibility(View.INVISIBLE);
				sv.saveBooleanValue(mData.get(mPosition).data, false);
				for (int i = 0; i < mData.get(mPosition).getSubChildDataItem()
						.size(); i++) {
					sv.saveBooleanValue(mData.get(mPosition)
							.getSubChildDataItem().get(i).data, false);
				}
				((BaseAdapter) sList.getAdapter()).notifyDataSetChanged();

			} else {
				mData.get(mPosition).marked = true;

				((LinearLayout) mList.getSelectedView()).getChildAt(0)
						.setVisibility(View.VISIBLE);
				sv.saveBooleanValue(mData.get(mPosition).data, true);
			}
			break;
		}
	}

	public void onSubKey(View v, int keyCode) {
		switch (keyCode) {
		case KeyEvent.KEYCODE_DPAD_LEFT:
			if (!mFocus) {
				mList.setSelection(mPosition);
				mList.requestFocus();
				((TextView) ((LinearLayout) mList.getSelectedView())
						.getChildAt(1)).setTextColor(Color.LTGRAY);
				sPosition = 0;
				mFocus = true;
			}
		case KeyEvent.KEYCODE_DPAD_DOWN:
			sPosition++;
			MtkLog.d(TAG, " ==== sub list select item position: " + sPosition);
			if (sPosition >= sList.getAdapter().getCount()) {
				sPosition = 0;
				sList.setSelection(sPosition);
			}
			break;
		case KeyEvent.KEYCODE_DPAD_UP:
			sPosition--;
			MtkLog.d(TAG, " ==== sub list select item position: " + sPosition);
			if (sPosition < 0) {
				sPosition = sList.getAdapter().getCount() - 1;
				sList.setSelection(sPosition);
			}
			break;
		case KeyEvent.KEYCODE_DPAD_CENTER:
		case KeyEvent.KEYCODE_E:
			MtkLog.d(TAG, "Sub List Position: " + sPosition
					+ "      is selected: " + mData.get(sPosition).marked);

			if (sv.readBooleanValue(mData.get(sPosition).data)) {
				mData.get(sPosition).marked = false;
				MtkLog.d(TAG, "------- sub list not marked ---------");
				((LinearLayout) sList.getSelectedView()).getChildAt(0)
						.setVisibility(View.INVISIBLE);
				sv.saveBooleanValue(mData.get(sPosition).data, false);

			} else {
				mData.get(sPosition).marked = true;
				MtkLog.d(TAG, "-------sub list is marked ---------");
				((LinearLayout) sList.getSelectedView()).getChildAt(0)
						.setVisibility(View.VISIBLE);
				sv.saveBooleanValue(mData.get(sPosition).data, true);
				setMainItemVisible();
			}
			break;
		}
	}

	public void setMainItemVisible() {
		((LinearLayout) mList.getChildAt(mPosition)).getChildAt(0)
				.setVisibility(View.VISIBLE);
		sv.saveBooleanValue(((EPGTypeListAdapter) mList.getAdapter()).mData
				.get(mPosition).data, true);
	}
}
