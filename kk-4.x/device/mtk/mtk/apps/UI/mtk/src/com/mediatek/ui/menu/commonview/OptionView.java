package com.mediatek.ui.menu.commonview;

import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.RelativeLayout.LayoutParams;

import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.SaveValue;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.nav.util.NavIntegrationZoom;
import com.mediatek.ui.pippop.PippopUiLogic;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenModeConfig;
import android.os.SystemProperties;

/**
 * This view includes a text view (the name),the second text view(the current
 * value) and a ImageView(selected or not), it is one item of list view items.
 * you can press LEFT or RIGHT key to change value of the second text view
 * 
 * @author hs_yaoxiao
 */

public class OptionView extends ListViewItemView implements RespondedKeyEvent {

	private static final String TAG = "OptionView";
	// the item name, value of list view item
	private TextView mNameView;
	private TextView mValueView;
	private ImageView mRightImageIv;
	private OnValueChangedListener mValueChangedListener;
	private ScreenModeConfig scrMode;
	private MenuConfigManager mConfig;
	private static List<Timer> timers = new ArrayList<Timer>();
	private SaveValue saveV;
	private PippopUiLogic mLogic;

	private class MyTimerTask extends TimerTask {

		@Override
		public void run() {
			// TODO Auto-generated method stub
			if (mValueChangedListener != null) {
				mValueChangedListener.onValueChanged(OptionView.this,
						mDataItem.mInitValue);
			}
		}
	}

	public OptionView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		init();
	}

	public OptionView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		init();
	}

	public OptionView(Context context) {
		super(context);
		this.context = context;
		init();
	}

	public int getValue() {
		return this.mDataItem.getmInitValue();
	}

	public void setValue(int mPositon) {
		this.mDataItem.setmInitValue(mPositon);
		mValueView.setText(this.mDataItem.getmOptionValue()[mPositon]);
		if (mValueChangedListener != null) {
			mValueChangedListener.onValueChanged(this, mPositon);
		}
	}

	private void setViewName(String viewName) {
		mNameView.setText(viewName);
	}

	public TextView getNameView() {
		return mNameView;
	}

	public TextView getValueView() {
		return mValueView;
	}

	public void setValueChangedListener(
			OnValueChangedListener mValueChangedListener) {
		this.mValueChangedListener = mValueChangedListener;
	}

	private void init() {
		LinearLayout lv = (LinearLayout) inflate(mContext,
				R.layout.menu_option_view, null);
		LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
		addView(lv, params);
		mNameView = (TextView) findViewById(R.id.common_tv_itemname);
		mValueView = (TextView) findViewById(R.id.common_tv_itemshow);
		mRightImageIv = (ImageView) findViewById(R.id.common_iv_itemimage);
		scrMode = ScreenModeConfig.getInstance(context);
		mConfig = MenuConfigManager.getInstance(context);
		saveV = SaveValue.getInstance(context);
		mLogic = PippopUiLogic.getInstance(context);
	}

	public void setAdapter(SetConfigListViewAdapter.DataItem mDataItem) {
		this.mDataItem = mDataItem;
		mId = mDataItem.mItemID;
		setViewName(this.mDataItem.getmName());
		if (MenuConfigManager.SLEEP_TIMER.equals(mDataItem.getmItemID())) {
			Long timeLeft = NavIntegration.getInstance(context)
					.iPowerOffMinutesLeft("timetosleep");
			if (timeLeft > 0) {
				String[] optionTiemSleep = context.getResources()
						.getStringArray(R.array.menu_setup_sleep_timer_array);
				Pattern pattern = Pattern.compile("^\\d*\\d");
				Matcher matcher = pattern
						.matcher(optionTiemSleep[optionTiemSleep.length - 1]);
				mValueView.setText(matcher.replaceFirst(String
						.valueOf(timeLeft)));
				MtkLog.d(TAG, "leftTimer------------------------>"
						+ matcher.replaceFirst(String.valueOf(timeLeft)));
			} else {
				mValueView
						.setText(this.mDataItem.mOptionValue[this.mDataItem.mInitValue]);
			}
		} else {
			mValueView
					.setText(this.mDataItem.mOptionValue[this.mDataItem.mInitValue]);
		}

	}

	protected void switchValueNext() {
		if (mConfig.SCREEN_MODE.equals(mDataItem.mItemID)) {
			mDataItem.mInitValue = scrMode.getNextScreen();
			mValueView.setText(mDataItem.mOptionValue[mDataItem.mInitValue]);
			
			//when changing screen mode, need change zoom mode to default.
			if(!mLogic.isPipOrPopState()){
				NavIntegrationZoom.getInstance(mContext).setZoomMode(NavIntegrationZoom.ZOOM_1);
			}	
		} else if (mConfig.VIDEO_3D_MODE.equals(mDataItem.mItemID)) {
			// off to other value,toast a dialog
			if (mDataItem.mInitValue == 0
					&& MenuConfigManager.getInstance(mContext).get3DInitValue(
							false) != 0) {
				MenuConfigManager.getInstance(mContext).toastWearGlass();
			}
			mDataItem.mInitValue = MenuConfigManager.getInstance(mContext)
					.get3DInitValue(false);
			mValueView.setText(mDataItem.mOptionValue[mDataItem.mInitValue]);
			if (null !=TimeShiftManager.getInstance()) {
                TimeShiftManager.getInstance().stopAllRunning();
            }
		} else {
			if (mDataItem.mInitValue != mDataItem.mOptionValue.length - 1) {
				mValueView
						.setText(mDataItem.mOptionValue[++mDataItem.mInitValue]);
			} else {
				mDataItem.mInitValue = 0;
				mValueView
						.setText(mDataItem.mOptionValue[mDataItem.mInitValue]);
			}
			if(mConfig.FAST_BOOT.equals(mDataItem.mItemID)){
				saveV.saveValue(mConfig.FAST_BOOT,mDataItem.mInitValue);
				SystemProperties.set("sys.pm.warmboot",mDataItem.mInitValue+"");
			}
		}
		// if (mValueChangedListener != null
		// && !mId.equals(MenuConfigManager.NETWORK_INTERFACE)
		// && !mId.equals(MenuConfigManager.NETWORK_CONNECTION)) {
		mValueChangedListener.onValueChanged(this, mDataItem.mInitValue);

		if (mConfig.VIDEO_3D_NAV.equals(mDataItem.mItemID)) {
			// 3d nav,set value of 3d mode
			// 3d nav is auto,set 3d mode auto
			// else set 3d mode off
			if (mDataItem.mInitValue == 2) {
				mConfig.setValue(MenuConfigManager.VIDEO_3D_MODE, 1);
			} else {
				mConfig.setValue(MenuConfigManager.VIDEO_3D_MODE, 0);
			}
			if (mDataItem.mInitValue != mDataItem.mOptionValue.length - 1) {
				mValueView
						.setText(mDataItem.mOptionValue[++mDataItem.mInitValue]);
			} else {
				mDataItem.mInitValue = 0;
				mValueView
						.setText(mDataItem.mOptionValue[mDataItem.mInitValue]);
			}
		}

		if(MenuConfigManager.DLNA.equals(mDataItem.mItemID)){
			if(mDataItem.mInitValue==0){
				saveV.saveValue(MenuConfigManager.DLNA,0);
			}else if(mDataItem.mInitValue==1){
				saveV.saveValue(MenuConfigManager.DLNA,1);
			}
		}
		if(MenuConfigManager.MY_NET_PLACE.equals(mDataItem.mItemID)){
			if(mDataItem.mInitValue==0){
				saveV.saveValue(MenuConfigManager.MY_NET_PLACE,0);
			}else if(mDataItem.mInitValue==1){
				saveV.saveValue(MenuConfigManager.MY_NET_PLACE,1);
			}
		}
		MtkLog.v(TAG, "" + mDataItem.mInitValue);
	}

	protected void switchValuePrevious() {
		if (mConfig.SCREEN_MODE.equals(mDataItem.mItemID)) {
			mDataItem.mInitValue = scrMode.getPreScreen();
			mValueView.setText(mDataItem.mOptionValue[mDataItem.mInitValue]);
			
			//when changing screen mode, need change zoom mode to default.
			if(!mLogic.isPipOrPopState()){
				NavIntegrationZoom.getInstance(mContext).setZoomMode(NavIntegrationZoom.ZOOM_1);
			}	
		} else if (mConfig.VIDEO_3D_MODE.equals(mDataItem.mItemID)) {
			// off to other value,toast a dialog
			if (mDataItem.mInitValue == 0
					&& MenuConfigManager.getInstance(mContext).get3DInitValue(
							true) != 0) {
				MenuConfigManager.getInstance(mContext).toastWearGlass();
			}
			mDataItem.mInitValue = MenuConfigManager.getInstance(mContext)
					.get3DInitValue(true);
			mValueView.setText(mDataItem.mOptionValue[mDataItem.mInitValue]);
			if (null !=TimeShiftManager.getInstance()) {
                TimeShiftManager.getInstance().stopAllRunning();
            }
		} else {
			if (mDataItem.mInitValue != 0) {
				mValueView
						.setText(mDataItem.mOptionValue[--mDataItem.mInitValue]);
			} else {
				mDataItem.mInitValue = mDataItem.mOptionValue.length - 1;
				mValueView
						.setText(mDataItem.mOptionValue[mDataItem.mInitValue]);
			}
			if(mConfig.FAST_BOOT.equals(mDataItem.mItemID)){
				saveV.saveValue(mConfig.FAST_BOOT,mDataItem.mInitValue);
				SystemProperties.set("sys.pm.warmboot",mDataItem.mInitValue+"");
			}
		}
		// if (mValueChangedListener != null
		// && !mId.equals(MenuConfigManager.NETWORK_INTERFACE)) {
		mValueChangedListener.onValueChanged(this, mDataItem.mInitValue);

		if (mConfig.VIDEO_3D_NAV.equals(mDataItem.mItemID)) {
			// 3d nav,set value of 3d mode
			// 3d nav is auto,set 3d mode auto
			// else set 3d mode off
			if (mDataItem.mInitValue == 2) {
				mConfig.setValue(MenuConfigManager.VIDEO_3D_MODE, 1);
			} else {
				mConfig.setValue(MenuConfigManager.VIDEO_3D_MODE, 0);
			}
			if (mDataItem.mInitValue != 0) {
				mValueView
						.setText(mDataItem.mOptionValue[--mDataItem.mInitValue]);
			} else {
				mDataItem.mInitValue = mDataItem.mOptionValue.length - 1;
				mValueView
						.setText(mDataItem.mOptionValue[mDataItem.mInitValue]);
			}
		}

		if(MenuConfigManager.DLNA.equals(mDataItem.mItemID)){
			if(mDataItem.mInitValue==0){
				saveV.saveValue(MenuConfigManager.DLNA,0);
			}else if(mDataItem.mInitValue==1){
				saveV.saveValue(MenuConfigManager.DLNA,1);
			}
		}
		if(MenuConfigManager.MY_NET_PLACE.equals(mDataItem.mItemID)){
			if(mDataItem.mInitValue==0){
				saveV.saveValue(MenuConfigManager.MY_NET_PLACE,0);
			}else if(mDataItem.mInitValue==1){
				saveV.saveValue(MenuConfigManager.MY_NET_PLACE,1);
			}
		}
		// }
	}

	public void onKeyEnter() {
	}

	public void onKeyLeft() {
		if (mDataItem.isEnable) {
			// if ( mId.equals(MenuConfigManager.NETWORK_CONNECTION)) {
			// TimerTask tt = new MyTimerTask();
			// Timer timer = new Timer();
			// timer.schedule(tt, 500);
			// timers.add(timer);
			// MtkLog.v(TAG, "size:" + timers.size() + "********************");
			// for (int i = 0; i < timers.size() - 1; i++) {
			// timers.get(i).cancel();
			// }
			// }
			switchValuePrevious();
		}
	}

	public void onKeyRight() {
		if (mDataItem.isEnable) {
			// if (mId.equals(MenuConfigManager.NETWORK_INTERFACE)) {
			// TimerTask tt = new MyTimerTask();
			// Timer timer = new Timer();
			// timer.schedule(tt, 500);
			// timers.add(timer);
			// MtkLog.v(TAG, "size:" + timers.size() + "********************");
			// for (int i = 0; i < timers.size() - 1; i++) {
			// timers.get(i).cancel();
			// }
			// }
			switchValueNext();
		}
	}

	public void showValue(int value) {
		if (value < 0 || value > mDataItem.mOptionValue.length - 1) {
			throw new IllegalArgumentException("value is Illegal value");
		}
		mDataItem.mInitValue = value;
		mValueView.setText(mDataItem.mOptionValue[value]);
	}

	public void setRightImageSource(boolean isHighlight) {
		if (isHighlight) {
			mRightImageIv.setImageResource(R.drawable.menu_icon_select_hi);
		} else {
			mRightImageIv.setImageResource(R.drawable.menu_icon_select_nor);
		}
	}
}
