/*
 * Copyright (C) 2006 The Android Open Source Project
 * 
 * ©2010-2013 TCL CORPORATION All Rights Reserved.
 */

package android.tclwidget;

import java.util.ArrayList;
import java.util.List;

import android.app.Dialog;
import android.content.Context;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.view.Window;
import android.view.WindowManager;
import android.util.DisplayMetrics;
import android.util.TypedValue;

public class TCLSenroiMenu extends Dialog {

	private Context mContext;
	List<DialogItem> mItemList = new ArrayList<DialogItem>();
	
	public static int WALLPAPER = com.android.internal.R.drawable.tcl_icon_wallpaper;
	public static int ADD = com.android.internal.R.drawable.tcl_icon_add;
	public static int ABOUT = com.android.internal.R.drawable.tcl_icon_about;
	public static int DELETE = com.android.internal.R.drawable.tcl_icon_delete;
	public static int MOVE = com.android.internal.R.drawable .tcl_icon_move;
	public static int QUIT = com.android.internal.R.drawable.tcl_icon_quit;
	public static int EDIT = com.android.internal.R.drawable.tcl_icon_edit;
	public static int SOUND = com.android.internal.R.drawable.tcl_icon_sound;
	public static int SET = com.android.internal.R.drawable.tcl_icon_set;
	public static int APPSEARCH = com.android.internal.R.drawable.tcl_icon_appsearch;

	private Window window = null;
	//added by xufei begin for 1080p and 720p compatible.
	DisplayMetrics mMetrics = new DisplayMetrics();
	//added by xufei end
	
	public TCLSenroiMenu(Context context) {
		super(context, com.android.internal.R.style.TCLSenroiMenu);
		//added by xufei begin for 1080p and 720p compatible.
		mMetrics.setToDefaults();
		//added by xufei end
		mContext = context;
		hline = new ImageView(context);
		hline.setLayoutParams(new LayoutParams(LayoutParams.FILL_PARENT, 1));
		vline = new ImageView(context);
		vline.setLayoutParams(new LayoutParams(1, LayoutParams.FILL_PARENT));
	}
	
	public TCLSenroiMenu(Context context,/*int x,*/int posY){
		this(context);
		//added by xufei begin for 1080p and 720p compatible.
		mMetrics.setToDefaults();
		//added by xufei end
		window = getWindow(); 
        WindowManager.LayoutParams wl = window.getAttributes();
       // wl.x = x; 
        wl.y = posY;   
        window.setAttributes(wl);
	}
	
	public int getPx(int dp) {
			return (int)TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, dp, mMetrics);
	}
	
	void addItem(DialogItem item) {
		mItemList.add(item);
	}
	
	/**
	 * Add dialog item to dialog.
     * @author TCL TVOS Team
	 * @param index The sequence of the item
	 * @param name The name of the item
	 * @param resId The icon resource id of the item
	 * @param listener The listener of the item
	 * @param enable whether the item can user
	 * */
	public void addItem(int index, String name, int resId, View.OnClickListener listener, boolean enable) {
		addItem(new DialogItem(index, name, resId, listener, enable));
	}
	
	/**
	 * Add the item to dialog.
     * @author TCL TVOS Team
	 * @see #addItem(int, String, int, android.view.View.OnClickListener, boolean)
	 * */
	public void addItem(int index, String name, int resId, View.OnClickListener listener) {
		addItem(new DialogItem(index, name, resId, listener, true));
	}
	
	static public class DialogItem {
		int index;
		String name;
		int resId;
		View.OnClickListener listener;
		boolean enable;
		DialogItem(int index, String name, int resId, View.OnClickListener listener, boolean enable) {
			this.index = index;
			this.name = name;
			this.resId = resId;
			this.listener = listener;
			this.enable = enable;
		}
	}
	
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_MENU) {
			dismiss();
			return true;
		}
		return super.onKeyDown(keyCode, event);
	}
	
	private ImageView hline, vline;
	
	class MenuButton extends Button {

		public LinearLayout.LayoutParams params;

		public MenuButton(Context context) {
			super(context);
		}
		
		public MenuButton(Context context, int resIcon, String name, int resBackground) {
			super(context);
			setCompoundDrawablesWithIntrinsicBounds(0, resIcon, 0, 0);
			setBackgroundResource(resBackground);
			setTextAppearance(context, com.android.internal.R.style.tcl_toast_text);
			setText(name);
			//setLayoutParams(new LayoutParams(171, 135));
			setGravity(Gravity.CENTER);
			//setPadding(0, 25, 0, 0);
			//setTextSize(24);
			if(mItemList.size() >= 5)
				params = new LinearLayout.LayoutParams(getPx(200), getPx(145));
			else if(mItemList.size() == 4)
				params = new LinearLayout.LayoutParams(getPx(250), getPx(145));
			else if(mItemList.size() == 3)
				params = new LinearLayout.LayoutParams(getPx(333), getPx(145));
			else if(mItemList.size() == 2)
				params = new LinearLayout.LayoutParams(getPx(500), getPx(145));
			params.weight = 1.0f;
			setLayoutParams(params);
		}
	}
	
	class Hline extends ImageView {
		public Hline(Context context) {
			super(context);
			setBackgroundResource(com.android.internal.R.drawable.tcl_separateline_h);
			setLayoutParams(new LayoutParams(LayoutParams.FILL_PARENT, 1));
		}
	}
	
	class VlineT extends ImageView {
		public VlineT(Context context) {
			super(context);
			if(mItemList.size() > 5)
				setBackgroundResource(com.android.internal.R.drawable.tcl_separateline_v_t);
			else
				setBackgroundResource(com.android.internal.R.drawable.tcl_separateline2);
			setLayoutParams(new LayoutParams(1, LayoutParams.FILL_PARENT));
		}
	}

	class VlineB extends ImageView {
		public VlineB(Context context) {
			super(context);
			setBackgroundResource(com.android.internal.R.drawable.tcl_separateline_v_b);
			setLayoutParams(new LayoutParams(1, LayoutParams.FILL_PARENT));
		}
	}
	
    /**
     * Show the view for the list of items.
     * @author TCL TVOS Team
     */
	public void show() {
		LinearLayout mainlayout = new LinearLayout(mContext);
		LayoutParams params = new LinearLayout.LayoutParams(getPx(1000), LayoutParams.WRAP_CONTENT);
		mainlayout.setLayoutParams(params);
		int size = mItemList.size();
		
		if (size > 10) {
			return;
		}
		else if (size > 5) {
			mainlayout.setOrientation(LinearLayout.VERTICAL);
			LinearLayout layoutTop = new LinearLayout(mContext);
			layoutTop.setOrientation(LinearLayout.HORIZONTAL);
			LinearLayout layoutBottom = new LinearLayout(mContext);
			layoutBottom.setOrientation(LinearLayout.HORIZONTAL);
			LinearLayout layoutMid = new LinearLayout(mContext);
			layoutMid.setOrientation(LinearLayout.HORIZONTAL);
			layoutMid.addView(new Hline(mContext));
			mainlayout.addView(layoutTop);
			mainlayout.addView(layoutMid);
			mainlayout.addView(layoutBottom);
			
			int resBg;
			for (int itemId = 0; itemId < size; itemId++) {
				final DialogItem dialogItem = mItemList.get(itemId);
				if (itemId == 0) {
					resBg = com.android.internal.R.drawable.tcl_menu_left_top;
				}
				else if (itemId > 0 && itemId < 4) {
					resBg = com.android.internal.R.drawable.tcl_menu_centre_top;
				}
				else if (itemId == 4) {
					resBg = com.android.internal.R.drawable.tcl_menu_right_top;
				}
				else if (itemId == 5) {
					if (size == 6) {
						resBg = com.android.internal.R.drawable.tcl_menu_bottom;
					}
					else {
						resBg = com.android.internal.R.drawable.tcl_menu_left_bottom;
					}
				}
				else if (itemId == size - 1) {
					resBg = com.android.internal.R.drawable.tcl_menu_right_bottom;
				}
				else {
					resBg = com.android.internal.R.drawable.tcl_menu_centre_bottom;
				}
				
				final MenuButton btn = new MenuButton(mContext, dialogItem.resId, dialogItem.name, resBg);
				btn.setEnabled(dialogItem.enable);
				btn.setFocusable(dialogItem.enable);
				btn.setOnClickListener(new View.OnClickListener() {
					public void onClick(View v) {
						dialogItem.listener.onClick(btn);
						TCLSenroiMenu.this.dismiss();
					}
				});
				
				if (itemId < 5) {
					if (itemId != 0)
						layoutTop.addView(new VlineT(mContext));
					btn.setPadding(0, getPx(24), 0, 0);
					layoutTop.addView(btn);
				}
				else {
					if (itemId != 5)
						layoutBottom.addView(new VlineB(mContext));
					btn.setPadding(0, getPx(10), 0, 0);
					layoutBottom.addView(btn);
				}
			}
		}
		else if (size >= 2) {
			int resBg;
			for (int itemId = 0; itemId < size; itemId++) {
				final DialogItem dialogItem = mItemList.get(itemId);
				if (itemId == 0) {
					resBg = com.android.internal.R.drawable.tcl_menu_left;
				}
				else if (itemId == size - 1) {
					resBg = com.android.internal.R.drawable.tcl_menu_right;
					mainlayout.addView(new VlineT(mContext));
				}
				else {
					resBg = com.android.internal.R.drawable.tcl_menu_mid;
					mainlayout.addView(new VlineT(mContext));
				}
				final MenuButton btn = new MenuButton(mContext, dialogItem.resId, dialogItem.name, resBg);
				btn.setEnabled(dialogItem.enable);
				btn.setFocusable(dialogItem.enable);
				btn.setOnClickListener(new View.OnClickListener() {
					public void onClick(View v) {
						dialogItem.listener.onClick(btn);
						TCLSenroiMenu.this.dismiss();
					}
				});
				btn.setPadding(0, getPx(24), 0, 0);
				mainlayout.addView(btn);
			}
		}
		else {
			return;
		}
		setContentView(mainlayout);
		super.show();
	}

	@Override
	public void dismiss() {
		// TODO Auto-generated method stub
		if (tclFloatMenuListener != null){
			tclFloatMenuListener.onDismissInfo();
		}
		super.dismiss();
	}

	TCLMenuDismissListener tclFloatMenuListener = null;
	public void setOnFloatMenuListener(TCLMenuDismissListener tclFloatMenuListener){
		this.tclFloatMenuListener = tclFloatMenuListener;
	}
	
	public interface TCLMenuDismissListener {
		public void onDismissInfo();
	}

}
