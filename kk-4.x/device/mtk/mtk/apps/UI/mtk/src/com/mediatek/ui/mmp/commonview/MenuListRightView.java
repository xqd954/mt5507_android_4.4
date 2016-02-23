package com.mediatek.ui.mmp.commonview;

import java.util.List;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.view.KeyEvent;
import android.view.View;
import android.widget.ListView;
import android.widget.AdapterView.OnItemSelectedListener;

import com.mediatek.ui.R;
import com.mediatek.ui.mmp.commonview.MenuListView.MenuDismissCallBack;
import com.mediatek.ui.mmp.multimedia.MediaPlayActivity;
import com.mediatek.ui.mmp.multimedia.MtkFilesBaseListActivity;
import com.mediatek.ui.mmp.util.LogicManager;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;

public class MenuListRightView extends ComDialog implements
		OnItemSelectedListener {
	private static final String TAG = "MenuListRightView";

	private static final int HIDE_DELAYTIME = 10000;

	private static final int MSG_DISMISS_MENU = 1;

	private MenuDismissCallBack mCallback;

	private int mSelection = 0;
	private Context mContext;
	private LogicManager mLogicManager;
	
	public MenuListRightView(Context context, List<?> list,
			ListView.OnItemClickListener listener, MenuDismissCallBack callback) {

		super(context, list, R.layout.mmp_menulist_item, listener);
		mCallback = callback;
		mContext = context;
	}

	private Handler mHandler = new Handler() {

		public void handleMessage(android.os.Message msg) {

			switch (msg.what) {

			case MSG_DISMISS_MENU: {
				if (null != mCallback) {
					mCallback.onDismiss();
				}
				dismiss();
				break;
			}
			default:
				break;
			}
		};
	};

	public void setSelection(int position) {
		mSelection = position;
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		vListView.setSelection(mSelection);

		vListView.setOnKeyListener(new ListView.OnKeyListener() {

			public boolean onKey(View v, int keyCode, KeyEvent event) {
				MtkLog.d(TAG, "onKey" + keyCode);
				if (event.getAction() == KeyEvent.ACTION_UP) {
					mHandler.removeMessages(MSG_DISMISS_MENU);
					mHandler.sendEmptyMessageDelayed(MSG_DISMISS_MENU,
							HIDE_DELAYTIME);
				}
				switch (keyCode) {
				case KeyEvent.KEYCODE_DPAD_LEFT:
					return onKeyDown(keyCode, event);
				case KeyEvent.KEYCODE_DPAD_RIGHT:
					return true;
				default:
					break;
				}
				return false;
			}
		});
		mHandler.sendEmptyMessageDelayed(MSG_DISMISS_MENU, HIDE_DELAYTIME);
	}

	@Override
	protected void onStop() {
		super.onStop();
		mHandler.removeMessages(MSG_DISMISS_MENU);
		mCallback.sendMessage();
	}

	@Override
	public boolean dispatchKeyEvent(KeyEvent event) {
		if (mLogicManager == null){
			mLogicManager = LogicManager.getInstance(mContext);
		}
		if (mLogicManager.isAudioOnly()) {
			mLogicManager.setAudioOnly(false);
			return true;
		}
		int keyCode = event.getKeyCode();
		if (event.getAction() == KeyEvent.ACTION_DOWN) {
			switch (keyCode) {
			case KeyMap.KEYCODE_VOLUME_DOWN:
			case KeyMap.KEYCODE_VOLUME_UP:
			case KeyMap.KEYCODE_MTKIR_MUTE: {
				if (null != mContext && mContext instanceof MediaPlayActivity) {
					((MediaPlayActivity) mContext).onKeyDown(keyCode, event);
				} else if (null != mContext
						&& mContext instanceof MtkFilesBaseListActivity) {
					((MtkFilesBaseListActivity) mContext).onKeyDown(keyCode,
							event);
				}
				return true;
			}
			default:
				break;
			}
		}
		return super.dispatchKeyEvent(event);
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		MtkLog.d(TAG, "onKeyDown" + keyCode);
		switch (keyCode) {
		case KeyEvent.KEYCODE_DPAD_CENTER:
			break;
		case KeyEvent.KEYCODE_DPAD_RIGHT:
			break;
		case KeyEvent.KEYCODE_DPAD_LEFT:
			MtkLog.d(TAG, "KEYCODE_DPAD_LEFT" + keyCode);
			dismiss();
			break;
		case KeyEvent.KEYCODE_MENU: {
			if (null != mCallback) {
				mCallback.noDismissPannel();
			}
			dismiss();
			break;
		}
		default:
			break;
		}
		return super.onKeyDown(keyCode, event);
	}

}
