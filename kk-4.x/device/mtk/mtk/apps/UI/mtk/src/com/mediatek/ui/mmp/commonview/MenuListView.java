package com.mediatek.ui.mmp.commonview;

import java.util.ArrayList;
import java.util.List;

import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnDismissListener;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.view.KeyEvent;
import android.view.View;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.AdapterView.OnItemSelectedListener;

import com.mediatek.mmpcm.mmcimpl.Const;
import com.mediatek.mmpcm.photoimpl.ConstPhoto;
import com.mediatek.ui.R;
import com.mediatek.ui.mmp.model.FilesManager;
import com.mediatek.ui.mmp.model.MultiFilesManager;
import com.mediatek.ui.mmp.multimedia.MediaPlayActivity;
import com.mediatek.ui.mmp.multimedia.MtkFilesBaseListActivity;
import com.mediatek.ui.mmp.multimedia.MusicPlayActivity;
import com.mediatek.ui.mmp.multimedia.PhotoPlayActivity;
import com.mediatek.ui.mmp.multimedia.Photo3DPlayActivity;
import com.mediatek.ui.mmp.multimedia.Photo4K2KPlayActivity;


import com.mediatek.ui.mmp.util.GetDataImp;
import com.mediatek.ui.mmp.util.LogicManager;
import com.mediatek.ui.mmp.util.MenuFatherObject;
import com.mediatek.ui.mmp.util.MultiMediaConstant;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.mmp.util.TextUtils;

import com.mediatek.ui.util.MtkLog;

public class MenuListView extends ComDialog implements OnItemSelectedListener,
		OnDismissListener {

	private static final String TAG = "MenuListView";

	private static final int HIDE_DELAYTIME = 10000;

	private static final int MSG_DISMISS_MENU = 1;

	private MenuListRightView mdialog;

	private Context mContext;

	private TextUtils mTextUtils;

	private List<MenuFatherObject> mDataList = new ArrayList<MenuFatherObject>();

	ListView.OnItemClickListener mClickListener;

	private MenuDismissCallBack mCallback;

	private int mIndex = 0;

	public interface MenuDismissCallBack {
		void onDismiss();

		void sendMessage();

		void noDismissPannel();
	}

	private MenuDismissCallBack mMenuRightCallback = new MenuDismissCallBack() {

		public void onDismiss() {

			if (null != mCallback) {
				mCallback.onDismiss();
			}
			dismiss();
		}

		public void sendMessage() {
			mHandler.sendEmptyMessageDelayed(MSG_DISMISS_MENU, HIDE_DELAYTIME);
		}

		public void noDismissPannel() {
			dismiss();
		}
	};
	
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

	

	@SuppressWarnings("unchecked")
	public MenuListView(Context context, List<?> list,
			ListView.OnItemClickListener listener, MenuDismissCallBack callback) {
		super(context, list, R.layout.mmp_menulist_item, listener);
		mContext = context;
		mClickListener = listener;
		mDataList = (List<MenuFatherObject>) list;
		mTextUtils = TextUtils.getInstance(context);

		mCallback = callback;
	}

	public void showDelete(boolean show) {
		mDataList.get(mIndex + 5).enable = show;
		initList(mDataList);
	}

	public void showCopy(boolean show) {
		mDataList.get(mIndex + 3).enable = show;
		initList(mDataList);
	}

	public void showPaste(boolean show) {
		mDataList.get(mIndex + 4).enable = show;
		initList(mDataList);
	}

	public void updateItem(int index, String newText) {
		mDataList.get(mIndex + index).content = newText;
		initList(mDataList);
	}
	
	public void updateItem(int index, MenuFatherObject obj) {
		mDataList.set(mIndex + index,obj);
		initList(mDataList);
	}
	
	public MenuFatherObject getItem(int index) {  
		return mDataList.get(mIndex+index);
	}

	public void setList(int position1, String content1, boolean hasnext1,
			int position2, String content2, boolean hasnext2, int position3,
			String content3, boolean hasnext3) {
		mDataList.get(position1).content = content1;
		mDataList.get(position2).content = content2;
		mDataList.get(position2).hasnext = hasnext2;

		mDataList.get(position3).content = content3;
		mDataList.get(position3).hasnext = hasnext3;
		initList(mDataList);
	}

	@Override
	public void dismiss() {
		if (null != mdialog && mdialog.isShowing()) {
			mdialog.dismiss();
		}
		super.dismiss();
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		vListView.setOnKeyListener(new ListView.OnKeyListener() {

			public boolean onKey(View v, int keyCode, KeyEvent event) {

				MtkLog.d(TAG, "onKey");
				if (event.getAction() == KeyEvent.ACTION_DOWN) {
				switch (keyCode) {
				case KeyEvent.KEYCODE_DPAD_RIGHT:
				case KeyEvent.KEYCODE_DPAD_CENTER:
				case KeyEvent.KEYCODE_ENTER:

					return onKeyDown(keyCode, event);

				default:
						mHandler.removeMessages(MSG_DISMISS_MENU);
						mHandler.sendEmptyMessageDelayed(MSG_DISMISS_MENU,
								HIDE_DELAYTIME);
					break;
				}
				}
				return false;
			}
		});
		vListView.setFocusableInTouchMode(true);
		mHandler.sendEmptyMessageDelayed(MSG_DISMISS_MENU, HIDE_DELAYTIME);
	}
	private LogicManager mLogicManager;
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
		MtkLog.d(TAG, "onKeyDown");
		switch (keyCode) {
		case KeyEvent.KEYCODE_DPAD_RIGHT:
		case KeyEvent.KEYCODE_DPAD_CENTER:
		case KeyEvent.KEYCODE_ENTER:

		{
			if (mDataList.get(selectPosition).hasnext) {
				MtkLog.d(TAG, "onKeyDown" + keyCode);
				mHandler.removeMessages(MSG_DISMISS_MENU);

				String content = mDataList.get(selectPosition).content;
				int type = getMediaType();
				List<MenuFatherObject> menuList = GetDataImp.getInstance()
						.getChildList(mContext, content);
				mdialog = new MenuListRightView(mContext, menuList,
						mClickListener, mMenuRightCallback);
				mdialog.setOnDismissListener(this);

				mdialog.setMediaType(type);

				mdialog.setSelection(getSelection(content, menuList));
				mdialog.show();
				mdialog.setWindowPosition();
				selectView.findViewById(R.id.mmp_menulist_img).setVisibility(
						View.VISIBLE);
				((TextView) selectView.findViewById(R.id.mmp_menulist_tv))
						.setTextColor(Color.GREEN);
				return true;
			} else {
				return false;
			}
		}
		case KeyMap.KEYCODE_MENU: {
			dismiss();
			break;
		}
//		case KeyMap.KEYCODE_MTKIR_MUTE:
//			return	onMute();			 
			 

		default:
			break;
		}
		return super.onKeyDown(keyCode, event);
	}

	
	 boolean onMute() {
		if(mediaPlayActivity!=null)
		{
			mediaPlayActivity.onMute();
			return true;
		}
		return false;
	}

	public int getSelection(String content, List<MenuFatherObject> menuList) {

		if (content.equals(mContext.getResources().getString(
				R.string.mmp_menu_picturemode))) {
			return LogicManager.getInstance(mContext).getCurPictureMode();
		} else if (content.equals(mContext.getResources().getString(
				R.string.mmp_menu_screenmode))) {

			int[] modes = LogicManager.getInstance(mContext)
					.getAvailableScreenMode();
			int curMode = LogicManager.getInstance(mContext).getCurScreenMode();
			int position = -1;
			for (int i = 0, length = modes.length; i < length; i++) {
				if (modes[i] >= 0) {
					position++;
					if (modes[i] == curMode) {
						return position;
					}
				}
			}
			return 0;
		}

		// Fix DTV00352617 bug

		else if (content.equals(mContext.getResources().getString(
				R.string.mmp_menu_repeat))) {
			int mediaType = MediaPlayActivity.getMediaType();
			int filterType;
			switch (mediaType) {
			case MultiMediaConstant.AUDIO: {
				filterType = Const.FILTER_AUDIO;
				break;
			}
			case MultiMediaConstant.VIDEO: {
				filterType = Const.FILTER_VIDEO;
				break;
			}
			case MultiMediaConstant.PHOTO: {
				filterType = Const.FILTER_IMAGE;
				break;
			}
			case MultiMediaConstant.TEXT: {
				filterType = Const.FILTER_TEXT;
				break;
			}
			default:
				filterType = 0;
				break;
			}
			int repeat = LogicManager.getInstance(mContext).getRepeatModel(
					filterType);

			if (repeat == Const.REPEAT_NONE) {
				return 0;
			} else if (repeat == Const.REPEAT_ONE) {
				return 1;
			} else if (repeat == Const.REPEAT_ALL) {
				return 2;
			}
		}
		//add by xudong chen 20111204 fix DTV00379662
		else if (content.equals(mContext.getString(R.string.mmp_menu_display))) {
			int perLine = MediaPlayActivity.getPerLine();
			if (perLine == MusicPlayActivity.OFFLINE) {
				return 0;

			} else if (perLine == MusicPlayActivity.SINGLINE) {
				return 1;

			} else if (perLine == MusicPlayActivity.MULTILINE) {
				return 2;

			}
		}
		//end
		

		else if (content.equals(mContext.getString(R.string.mmp_menu_duration))) {
			int time = 0;
			if(mContext instanceof Photo3DPlayActivity){
					
				time = Photo3DPlayActivity.getDelayedTime();

			}else if(mContext instanceof PhotoPlayActivity){

				time = PhotoPlayActivity.getDelayedTime();

			}else if(mContext instanceof Photo4K2KPlayActivity){
				time = Photo4K2KPlayActivity.getDelayedTime();

			}

			if (time == MediaPlayActivity.DELAYED_SHORT) {
				return 0;

			} else if (time == MediaPlayActivity.DELAYED_MIDDLE) {
				return 1;

			} else if (time == MediaPlayActivity.DELAYED_LONG) {
				return 2;

			}
		}

		else if (content.equals(mContext.getString(R.string.mmp_menu_effect))) {
			int effect = LogicManager.getInstance(mContext).getImageEffect();
			switch (effect) {
			case ConstPhoto.DEFAULT: {
				return 0;
			}
			case ConstPhoto.dissolve: {
				return 1;
			}
			case ConstPhoto.wipe_right: {
				return 2;
			}
			case ConstPhoto.wipe_left: {
				return 3;
			}
			case ConstPhoto.wipe_top: {
				return 4;
			}
			case ConstPhoto.wipe_bottom: {
				return 5;
			}
			case ConstPhoto.box_in: {
				return 6;
			}
			case ConstPhoto.box_out: {
				return 7;
			}
			case ConstPhoto.RADNOM: {
				return 8;
			}

			default:
				break;
			}
		}

		else if (content.equals(mContext.getString(R.string.mmp_menu_shuffle))) {
			int fileType;
			switch (mMediaType) {
			case MultiMediaConstant.VIDEO:
				fileType = Const.FILTER_VIDEO;
				break;
			case MultiMediaConstant.AUDIO:
				fileType = Const.FILTER_AUDIO;
				break;
			case MultiMediaConstant.PHOTO:
				fileType = Const.FILTER_IMAGE;
				break;
			case MultiMediaConstant.TEXT:
				fileType = Const.FILTER_TEXT;
				break;
			default:
				fileType = 0;
				break;
			}
			boolean isShuffle = LogicManager.getInstance(mContext)
					.getShuffleMode(fileType);
			return isShuffle ? 1 : 0;
		}

		else if (content.equals(mContext.getString(R.string.mmp_menu_sort))) {
			int sortType = MultiFilesManager.getInstance(mContext)
					.getSortType();
			int contentType = MultiFilesManager.getInstance(mContext)
					.getContentType();

			switch (contentType) {
			case MultiMediaConstant.PHOTO:
			case MultiMediaConstant.TEXT: {
				if (sortType == FilesManager.SORT_BY_NAME) {
					return 1;
				} else {
					return 0;
				}
			}
			case MultiMediaConstant.AUDIO: {
				if (sortType == FilesManager.SORT_BY_ALBUM) {
					return 3;
				} else if (sortType == FilesManager.SORT_BY_ARTIST) {
					return 2;
				} else if (sortType == FilesManager.SORT_BY_GENRE) {
					return 1;
				} else {
					return 0;
				}
			}
			case MultiMediaConstant.VIDEO: {
				if (sortType == FilesManager.SORT_BY_NAME) {
					return 2;
				} else if (sortType == FilesManager.SORT_BY_DATE) {
					return 1;
				} else {
					return 0;
				}
			}
			}
		}

		else if (content
				.equals(mContext.getString(R.string.mmp_menu_mediatype))) {
			MtkLog.i(TAG, "----------mMediaType " + mMediaType);
			int mediaType = MultiFilesManager.getInstance(mContext)
					.getContentType();
			switch (mediaType) {
			case FilesManager.CONTENT_PHOTO:
				return 0;
			case FilesManager.CONTENT_AUDIO:
				return 1;
			case FilesManager.CONTENT_VIDEO:
				return 2;
			case FilesManager.CONTENT_TEXT:
				return 3;
			default:
				break;
			}
		}

		else if (content.equals(mContext.getString(R.string.mmp_menu_thumb))) {
			int size = ((MtkFilesBaseListActivity) mContext).mThumbnailSize;
			switch (size) {
			case MultiMediaConstant.SMALL:
				return 0;
			case MultiMediaConstant.MEDIUM:
				return 1;
			case MultiMediaConstant.LARGE:
				return 2;
			default:
				break;
			}
		} else if (content.equals(mContext.getString(R.string.mmp_menu_size))) {
			return mTextUtils.getFontSizeIndex();//LogicManager.getInstance(mContext).getFontSize();
		} else if (content.equals(mContext.getString(R.string.mmp_menu_style))) {
			return mTextUtils.getFontStyleIndex();//LogicManager.getInstance(mContext).getFontStyle();
		} else if (content.equals(mContext.getString(R.string.mmp_menu_color))) {
			return mTextUtils.getFontColorIndex();//LogicManager.getInstance(mContext).getFontColor();
		} else if (content.equals(mContext.getString(R.string.mmp_menu_zoom))) {
			int size =  LogicManager.getInstance(mContext).getPicCurZoom();
			switch(size){
			case ConstPhoto.ZOOM_1X:
				return 0;
			case ConstPhoto.ZOOM_2X:
				return 1;
			case ConstPhoto.ZOOM_4X:
				return 2;
			default:
				return 0;
			}
		}
		// Added by Dan for fix bug DTV00389362
		else if (content.equals(mContext.getString(R.string.mmp_menu_timeoffset))) {
			int mode =  LogicManager.getInstance(mContext).getLrcOffsetMode();
			return mode;
		}

		return 0;
	}

	public void onDismiss(DialogInterface dialog) {
		MtkLog.d(TAG, "mdialog is dismiss");
		init();

	}

	public void removeDataItem(int index) {
		if (index < mDataList.size()) {
			mDataList.remove(index);
		}
	}

	public void removeItem(int index) {
		mDataList.remove(index);
	//	mIndex--;
		initList(mDataList);
	}
	
	// Added by Dan for fix bug DTV00379191
	public void setFirstIndex(int index){
		mIndex = index;
	}
	
	MediaPlayActivity mediaPlayActivity;
	public void mControlView(MediaPlayActivity mediaPlayActivity) {
		
		this.mediaPlayActivity=mediaPlayActivity;
	}
	
	// Added by Dan for fix bug DTV00389330
	public boolean isInLrcOffsetMenu(){
		return GetDataImp.getInstance().isInLrcOffsetMenu();
	}
}
