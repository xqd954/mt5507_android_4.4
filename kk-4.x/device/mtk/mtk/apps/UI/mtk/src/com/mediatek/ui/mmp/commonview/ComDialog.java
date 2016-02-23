package com.mediatek.ui.mmp.commonview;

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.view.Display;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.AdapterView.OnItemSelectedListener;

import com.mediatek.ui.R;
import com.mediatek.ui.mmp.commonview.ListAdapterMenu.MyOffsetListener;
import com.mediatek.ui.mmp.model.MultiFilesManager;
import com.mediatek.ui.mmp.multimedia.MediaPlayActivity;
import com.mediatek.ui.mmp.util.MenuFatherObject;
import com.mediatek.ui.mmp.util.MultiMediaConstant;
import com.mediatek.ui.mmp.util.Util;
import com.mediatek.ui.util.DestroyApp;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

public class ComDialog extends Dialog implements OnItemSelectedListener {

	private static final String TAG = "ComDialog";

	private List<MenuFatherObject> mDataList = new ArrayList<MenuFatherObject>();

	private Context mContext;

	protected ListView vListView;

	private int mStyle;

	public int selectPosition;

	protected View selectView;
	protected String mSelectShow;
	private ListAdapterMenu mAdapterMenu;

	private ListView.OnItemClickListener mListener;

	private int menuWidth = 200;

	private int menuHight = 250;

	private int marginY = 50;

	private int marginX = 0;

	protected int mMediaType;
	private  int PAGE_SIZE=ListAdapterMenu.PAGE_SIZE;

	public void setMediaType(int type) {
		mMediaType = type;
	}

	public int getMediaType() {
		return mMediaType;
	}

	public void setBackGround() {

		if (mMediaType == MultiMediaConstant.VIDEO) {
			this.findViewById(R.id.menu_layout).setBackgroundResource(
					R.drawable.mmp_video_menu_bg);
		} else {
			this.findViewById(R.id.menu_layout).setBackgroundResource(
					R.drawable.mmp_menu_bg);
		}

	}
	
	public int getCount() {
		if (null == mDataList) {
			return 0;
		}
		return mDataList.size();
	}

	public boolean isContain(String menuContent) {
		for (MenuFatherObject menuItem : mDataList) {
			if (menuItem.content.equals(menuContent)) {
				return true;
			}
		}
		return false;
	}

	public ComDialog(Context context, int theme) {
		super(context, theme);
	}

	@SuppressWarnings("unchecked")
	public ComDialog(Context context, List<?> list, int style,
			ListView.OnItemClickListener listener) {
		this(context, R.style.dialog);
		mDataList = (List<MenuFatherObject>) list;
		mContext = context;
		mStyle = style;
		mListener = listener;
	}

	
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.mmp_menulistview);
		setBackGround();
		vListView = (ListView) findViewById(R.id.mmp_menulistview_list);
		mAdapterMenu = new ListAdapterMenu(mContext, mDataList, mStyle);
		vListView.setAdapter(mAdapterMenu);
		setListener();
		setDialogParams();
		LinearLayout m = (LinearLayout)findViewById(R.id.menu_layout);
		m.getBackground().setAlpha(220);
	}

	
	public boolean dispatchKeyEvent(KeyEvent event) {
		int keyCode = event.getKeyCode();
		switch (keyCode) {
		case KeyMap.KEYCODE_VOLUME_UP:
		case KeyMap.KEYCODE_VOLUME_DOWN:
			return true;
		default:
			break;
		}

		return super.dispatchKeyEvent(event);
	}

	public void initItem(int position, String content) {
		if (position < mDataList.size() - 1) {
			mDataList.get(position).content = content;
		}
		mAdapterMenu.initList(mDataList);
		mAdapterMenu.notifyDataSetChanged();
	}

	public void addItem(int location, MenuFatherObject obj) {
		mDataList.add(location, obj);
		if (null != mAdapterMenu) {
			mAdapterMenu.notifyDataSetChanged();
		}
	}

	public void setItem(int position, String content) {
		if (position < mDataList.size() - 1) {
			mDataList.get(position).content = content;
		}
		if (null != mAdapterMenu) {
			mAdapterMenu.notifyDataSetChanged();
		}
	}

	// Added by Dan for fix bug DTV00384878
	public void setItemEnabled(int position, boolean enabled){
		if (position >= 0 && position < mDataList.size()) {
			mDataList.get(position).enable = enabled;
		}
	}
	
	public void initList(List<MenuFatherObject> list) {
		mDataList = list;
		if (null != mAdapterMenu) {
			mAdapterMenu.initList(mDataList);
			mAdapterMenu.notifyDataSetChanged();
		}
	}

	private void setListener() {
		mAdapterMenu.setMyOffsetListener(new MyOffsetListener() {
			public void offset(int offset) {
				selectPosition = vListView.getSelectedItemPosition() + offset;
			}
		});
		vListView.setOnItemSelectedListener(this);
		vListView.setOnItemClickListener(mListener);
	}

	private void setDialogParams() {
		WindowManager m = getWindow().getWindowManager();
		Display display = m.getDefaultDisplay();
		Window window = getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();

		menuWidth = (int) (ScreenConstant.SCREEN_WIDTH * 0.15);
		menuHight = (int) (menuWidth * 4/ 3 + (int)(ScreenConstant.SCREEN_HEIGHT*0.014));
		marginY = (int) (ScreenConstant.SCREEN_HEIGHT * 3 / 8) - (int)(ScreenConstant.SCREEN_HEIGHT*0.16)- menuHight / 2;
		marginX = (int) (ScreenConstant.SCREEN_WIDTH * 0.05);

		lp.width = menuWidth;
		lp.height = menuHight;

		MtkLog.i(TAG,
				"----------------------- display.getWidth:"
						+ ScreenConstant.SCREEN_WIDTH);

		lp.x = ScreenConstant.SCREEN_WIDTH / 2 - ((3 * menuWidth) / 2) - marginX;
		lp.y = marginY;
		window.setAttributes(lp);

		// setWindowPosition();
	}

	public void setWindowPosition() {
		WindowManager m = getWindow().getWindowManager();
		Display display = m.getDefaultDisplay();
		Window window = getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();
		lp.x = ScreenConstant.SCREEN_WIDTH / 2 - menuWidth / 2 - marginX;
		lp.y = marginY;
		window.setAttributes(lp);
	}

	public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2,
			long arg3) {
		selectPosition = arg2 + mAdapterMenu.getOffset();
		TextView tvTextView = (TextView) arg1
				.findViewById(R.id.mmp_menulist_tv);
		selectView = arg1;	
		mSelectShow = tvTextView.getText().toString();
	}

	public void onNothingSelected(AdapterView<?> arg0) {
	}
	
	
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		int position = vListView.getSelectedItemPosition();
		int offset=mAdapterMenu.getOffset();
		switch (keyCode) {
		case KeyEvent.KEYCODE_DPAD_CENTER:
		case KeyEvent.KEYCODE_ENTER:
		case KeyEvent.KEYCODE_DPAD_RIGHT: {
			if (selectView != null) {
				selectView.findViewById(R.id.mmp_menulist_img).setVisibility(
						View.VISIBLE);
				((TextView) selectView.findViewById(R.id.mmp_menulist_tv))
						.setTextColor(Color.GREEN);
			}
			break;
		}
		case KeyEvent.KEYCODE_DPAD_DOWN:
			if (position == PAGE_SIZE - 1) {
				int lastenabled = getCount() - 1;
				if(MultiFilesManager.getInstance(mContext).getCurrentSourceType()==MultiFilesManager.SOURCE_DLNA){
					lastenabled = getCount() - 3;
				}
				if (mSelectShow.equals(mDataList.get(lastenabled).content)) {
					mAdapterMenu.setOffset(0);
					mAdapterMenu.notifyDataSetChanged();
					vListView.setSelection(0);
				} else {
					if ((offset + PAGE_SIZE) >= getCount()) {
						mAdapterMenu.setOffset(0);
						mAdapterMenu.notifyDataSetChanged();
						vListView.setSelection(0);
					} else {
						mAdapterMenu.setOffset(offset + 1);
						mAdapterMenu.notifyDataSetChanged();
						vListView.setSelection(PAGE_SIZE-1);
					}
				}

			} else {
				int lastenabled = getCount() - 1;
				if(MultiFilesManager.getInstance(mContext).getCurrentSourceType()==MultiFilesManager.SOURCE_DLNA){
					lastenabled = getCount() - 3;
				}
				if (mSelectShow.equals(mDataList.get(lastenabled).content)) {
					mAdapterMenu.setOffset(0);
					mAdapterMenu.notifyDataSetChanged();
					vListView.setSelection(0);
					break;
				}
				
				if (position == (getCount() - 1 - offset)) {
					mAdapterMenu.setOffset(0);
					mAdapterMenu.notifyDataSetChanged();
					vListView.setSelection(0);
					break;
				}
				for (int i = 0; i < getCount() - 1; i++) {
					if (mSelectShow.equals(mDataList.get(i).content)) {
						if (i == getCount() - 1) {
							mAdapterMenu.setOffset(0);
							mAdapterMenu.notifyDataSetChanged();
							vListView.setSelection(0);
							break;
						} else {
							MtkLog.i("keke", i+"---"+offset+"------2------"+(mAdapterMenu.getCount()-1));
							while(!mDataList.get(i+1).enable){
								i++;
								if(i==getCount() - 2){
									break;				
								}
							}
							offset=i+2-mAdapterMenu.getCount();	
							mAdapterMenu.setOffset(offset);
							mAdapterMenu.notifyDataSetChanged();
							vListView.setSelection(mAdapterMenu.getCount()-1);
						}
					}
				}

			}
			break;
		case KeyEvent.KEYCODE_DPAD_UP:
			if (position == 0) {
				if (offset > 0) {
					while(!mDataList.get(offset-1).enable){
						offset--;
						if(offset==1){
							break;
						}				
					}
					offset=offset-1;
					mAdapterMenu.setOffset(offset);
					mAdapterMenu.notifyDataSetChanged();
					vListView.setSelection(0);
				} else {

					if (getCount() > PAGE_SIZE) {
						offset = getCount() - PAGE_SIZE;
						mAdapterMenu.setOffset(offset);
						mAdapterMenu.notifyDataSetChanged();
						vListView.setSelection(PAGE_SIZE-1);
					} else {
						vListView.setSelection(getCount() - 1);
					}

				}

			} else {

				for (int i = (offset + position - 1); i > 0; i--) {
					if (mDataList.get(i).enable && i > offset) {
						vListView.setSelection(i - offset);
						break;
					} else if (mDataList.get(i).enable && i <= offset) {
						offset--;
						mAdapterMenu.setOffset(offset);
						mAdapterMenu.notifyDataSetChanged();
						vListView.setSelection(0);
						break;
					}
				}

			}
			break;
		case KeyMap.KEYCODE_MTKIR_CHUP:
			onCHKeyDown(KeyMap.KEYCODE_MTKIR_CHUP);
			break;
		case KeyMap.KEYCODE_MTKIR_CHDN:
			onCHKeyDown(KeyMap.KEYCODE_MTKIR_CHDN);
			break;
		case KeyMap.KEYCODE_MTKIR_ANGLE:
			dismiss();
//			LogicManager.getInstance(mContext).restoreVideoResource();
//			LogicManager.getInstance(mContext).finishAudioService();
//			LogicManager.getInstance(mContext).finishVideo();
//			DestroyApp destroyApp = (DestroyApp) mContext
//					.getApplicationContext();
//			destroyApp.finishAll();
			Util.exitMmpActivity(mContext);
			break;
		case KeyMap.KEYCODE_MTKIR_NEXT:
		case KeyMap.KEYCODE_MTKIR_PREVIOUS:
		case KeyMap.KEYCODE_MTKIR_PLAYPAUSE:
			if (null != mContext && mContext instanceof MediaPlayActivity) {
				((MediaPlayActivity) mContext).onKeyDown(keyCode, event);
				return true;
			}
		default:
			break;
		}
		return super.onKeyDown(keyCode, event);
	}

	public void init() {
		selectView.findViewById(R.id.mmp_menulist_img).setVisibility(
				View.INVISIBLE);
		((TextView) selectView.findViewById(R.id.mmp_menulist_tv))
				.setTextColor(Color.WHITE);
	}

	// add by keke fix DTV00384939
	private void onCHKeyDown(int keycode) {
		int position = vListView.getSelectedItemPosition();
		int offset = mAdapterMenu.getOffset();

		if (keycode == KeyMap.KEYCODE_MTKIR_CHUP) {

			if (position == 0) {

				if (getCount() > PAGE_SIZE && offset != PAGE_SIZE) {

					if (offset == 0) {
						offset = PAGE_SIZE;
						mAdapterMenu.setOffset(offset);
						mAdapterMenu.notifyDataSetChanged();
						vListView.setSelection(0);
					} else {
						mAdapterMenu.setOffset(0);
						mAdapterMenu.notifyDataSetChanged();
						vListView.setSelection(0);
					}

				} else if (offset == PAGE_SIZE) {
					mAdapterMenu.setOffset(0);
					mAdapterMenu.notifyDataSetChanged();
					vListView.setSelection(0);
				}

			} else {
				for (int j = offset; j < getCount() - 1; j++) {
					if (mDataList.get(j).enable) {
						if ((j - offset) < position) {
							vListView.setSelection(j - offset);
							return;
						} else {
							break;
						}

					}
				}
				mAdapterMenu.setOffset(0);
				mAdapterMenu.notifyDataSetChanged();
				vListView.setSelection(0);
			}

		} else if (keycode == KeyMap.KEYCODE_MTKIR_CHDN) {

			if (position == (PAGE_SIZE-1)) {
				if (getCount() > PAGE_SIZE) {
					offset = PAGE_SIZE;
					mAdapterMenu.setOffset(offset);
					mAdapterMenu.notifyDataSetChanged();
					vListView.setSelection(getCount() - offset - 1);
				}
			} else {
				if (position == (getCount() - offset - 1)) {
					MtkLog.i("keke", "--1---" + (getCount() - offset - 1));
					offset = 0;
					mAdapterMenu.setOffset(offset);
					mAdapterMenu.notifyDataSetChanged();
					position = mAdapterMenu.getCount() - 1;
					while (!mDataList.get(position).enable) {
						position--;
						if (position <= 0) {
							break;
						}
					}
					MtkLog.i("keke", "---2--" + position);
					vListView.setSelection(position);
				} else {
					if (getCount() > PAGE_SIZE) {
						int mcount = offset + mAdapterMenu.getCount() - 1;
						while (!mDataList.get(mcount).enable) {
							mcount--;
						}
						if (mcount == position) {
							offset = PAGE_SIZE;
							mAdapterMenu.setOffset(offset);
							mAdapterMenu.notifyDataSetChanged();
							vListView.setSelection(getCount() - offset - 1);
						} else {
							vListView.setSelection(mcount - offset);
						}
					} else {
						vListView.setSelection(getCount() - 1);
					}
				}
			}
		}

	}
}
