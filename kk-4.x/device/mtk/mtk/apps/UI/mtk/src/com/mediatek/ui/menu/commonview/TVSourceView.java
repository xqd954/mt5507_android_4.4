package com.mediatek.ui.menu.commonview;

import java.util.List;

import android.content.Context;
import android.os.SystemProperties;
import android.util.AttributeSet;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.mediatek.tvcommon.TVCommonManager;
import com.mediatek.tvcommon.TVOutputCommon;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem;
import com.mediatek.ui.menu.util.EditChannel;

/**
 * A view to show input source block in parental control, you can press enter to
 * block one input source or unblock one input source. If current input source
 * is not TV, whether you block or unblock TV input source, TV Button will not
 * show in top of the menu
 * 
 * @author mtk40462
 * 
 */
public class TVSourceView extends ListViewItemView implements RespondedKeyEvent {

	private static final String TAG = "TVSourceView";
	private TextView num, sourceName;
	private ImageView image;
	// source information array
	private String[] mData;
	private EditChannel editChannel;

	// mark if selected channel is blocked
	private boolean isBlock = false;

	// tv input source position
//	private String tvPosition1 = "1";
//	private String tvPosition2 = "2";

	private List<DataItem> mSubChildGroup;

	public List<DataItem> getSubChildGroup() {
		return mSubChildGroup;
	}

	public void setmGroup(List<DataItem> mSubChildGroup) {
		this.mSubChildGroup = mSubChildGroup;
	}

	public TVSourceView(Context context) {
		super(context);
		this.context = context;
		init();
	}

	public TVSourceView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		init();
	}

	public TVSourceView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		init();
	}

	/**
	 * init, get items id of input source list and editChannel instance
	 */
	public void init() {
		LinearLayout lv = (LinearLayout) inflate(mContext,
				R.layout.menu_parental_tvsource_view, null);
		LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
		addView(lv, params);

		num = (TextView) findViewById(R.id.common_tvsourceview_text_num);
		sourceName = (TextView) findViewById(R.id.common_ctvsourceview_text_name);
		image = (ImageView) findViewById(R.id.common_tvsourceview_image);
		editChannel = EditChannel.getInstance(context);
	}

	/**
	 * change background of block or unblock input source item
	 * 
	 * @param selected
	 *            true if the selected input source is blocked
	 */
	public void changeBackGround(boolean selected) {
		if (selected) {
			image.setImageResource(R.drawable.poweron_channel_selected);
		} else {
			image.setImageResource(R.drawable.poweron_channel_normal);
		}
	}

	public void setAdapter(SetConfigListViewAdapter.DataItem mDataItem) {
		this.mDataItem = mDataItem;
		mData = mDataItem.getmOptionValue();
		setInputSourceData();
		mId = mDataItem.getmItemID();
		this.mSubChildGroup = mDataItem.getmSubChildGroup();
		// mData[1]: input source name
		isBlock = TVCommonManager.getInstance(mContext).isInputSourcePhysicalBlocked(mData[1]);
//		isBlock = editChannel.isInputBlock(mData[1]);
		changeBackGround(isBlock);
	}

	/**
	 * set input source number and name
	 */
	public void setInputSourceData() {
		num.setText(mData[0]);
		int mhlNum = TVOutputCommon.getInstance(mContext).getMHLPortNum(
				TVCommonManager.getInstance(mContext).getCurrentOutput());
		int supportMHL = SystemProperties.getInt("mtk.isSupportMHL", 0);
		if (mData[1].equals("hdmi" + mhlNum) && (supportMHL == 1)) {
			sourceName.setText(mData[1] + "/MHL");
		}else{
			sourceName.setText(mData[1]);
		}	
	}

	/**
	 * when ENTER key is pressed in block input source, this method will be
	 * called
	 * 
	 * @param tabButton
	 *            the top buttons in the top of menu
	 */
	public void onKeyEnter(TabMenuButtonBar tabButton) {
		TVCommonManager mTVCommonManager = TVCommonManager.getInstance(mContext);
		// check if the selected input source is blocked
		if (mTVCommonManager.isInputSourcePhysicalBlocked(mData[1]) == true) {
			editChannel.blockInput(mData[1], false);
		}else {
			editChannel.blockInput(mData[1], true);
		}
		
		if (editChannel.isCurrentSourceTv()) {
			
			if (mTVCommonManager.isInputSourcePhysicalBlocked(editChannel.getCurrentInput())) {
				tabButton.mButtonTV.setVisibility(View.GONE);
				tabButton.mButtonAudio
						.setNextFocusRightId(R.id.widget_buttonSetUp);
				tabButton.mButtonSetup
						.setNextFocusLeftId(R.id.widget_buttonAudio);
			}else {
				tabButton.mButtonTV.setVisibility(View.VISIBLE);
				tabButton.mButtonAudio
						.setNextFocusRightId(R.id.widget_buttonTV);
				tabButton.mButtonSetup.setNextFocusLeftId(R.id.widget_buttonTV);
			}		
		}
		isBlock = mTVCommonManager.isInputSourcePhysicalBlocked(mData[1]);
		changeBackGround(isBlock);

	}

	public void onKeyEnter() {
		// TODO Auto-generated method stub

	}

	public void onKeyLeft() {
		// TODO Auto-generated method stub

	}

	public void onKeyRight() {
		// TODO Auto-generated method stub

	}

	public void setValue(int value) {
		// TODO Auto-generated method stub

	}

	public void showValue(int value) {
		// TODO Auto-generated method stub

	}

	public int getValue() {
		// TODO Auto-generated method stub
		return 0;
	}

}
