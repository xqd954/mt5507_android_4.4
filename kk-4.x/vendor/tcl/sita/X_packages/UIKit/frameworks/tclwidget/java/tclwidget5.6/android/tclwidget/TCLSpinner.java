/*
 * Copyright (C) 2006 The Android Open Source Project
 * 
 * ©2010-2013 TCL CORPORATION All Rights Reserved.
 */

package android.tclwidget;


import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.database.DataSetObserver;
import android.graphics.drawable.BitmapDrawable;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewParent;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.LinearLayout;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.Spinner;
import android.widget.SpinnerAdapter;
import android.widget.AbsListView;

public class TCLSpinner extends Spinner {

	//{{tianjing@tcl, public to private
	private static PopupWindow mPopup;
	//}}

	public TCLSpinner(Context context) {
		this(context,null);
	}

	public TCLSpinner(Context context, AttributeSet attrs) {
		 this(context, attrs, com.android.internal.R.attr.tclspinnerStyle);
		 this.setPadding(17,5,5,6);
	}

	public TCLSpinner(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	@Override
	protected void onAttachedToWindow() {
		super.onAttachedToWindow();
	}

	@Override
	protected void onDetachedFromWindow() {
		super.onDetachedFromWindow();

		if (mPopup != null && mPopup.isShowing()) {
			mPopup.dismiss();
			mPopup = null;
		}
	}

	// when clicked popupwindow is made
	@Override
	public boolean performClick() {
		Context context = getContext();
		
		LayoutInflater layoutInflater = (LayoutInflater) context
				.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		View view = layoutInflater.inflate(com.android.internal.R.layout.tcl_spinner_popwindow, null);
		//view.setBackgroundDrawable(context.getResources().getDrawable(com.android.internal.R.drawable.tcl_listview_818_bg));
		final TCLListView lv_group = (TCLListView) view.findViewById(com.android.internal.R.id.tcl_spinner_listview);
		
		lv_group.setAdapter(new DropDownAdapter(getAdapter()));
		lv_group.setChoiceMode(AbsListView.CHOICE_MODE_SINGLE);
		int pos = this.getSelectedItemPosition();
		if(pos > INVALID_POSITION ){
			lv_group.setSelection(pos);
			lv_group.setItemChecked(pos,true);
		}
	//	lv_group.setOnItemSelectedListener(this.getOnItemSelectedListener());
	//	lv_group.setOnItemClickListener(this.getOnItemClickListener());
	//	lv_group.setOnItemLongClickListener(this.getOnItemLongClickListener());
		
		lv_group.setOnKeyListener(new OnKeyListener(){
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				// TODO Auto-generated method stub
				if( ((keyCode == KeyEvent.KEYCODE_DPAD_CENTER || keyCode == KeyEvent.KEYCODE_ENTER) ) &&  event.getAction() == KeyEvent.ACTION_UP)
				{
					if (mPopup != null && mPopup.isShowing()) {
						mPopup.dismiss();
						mPopup = null;
						int n = lv_group.getSelectedItemPosition() ;
						if(n > INVALID_POSITION)
							TCLSpinner.this.setSelection(n);
					}
				}
				return false;
			}			
		});
		/*mouse supporting*/
        lv_group.setOnItemClickListener(new OnItemClickListener(){

			public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
					long arg3) {
				// TODO Auto-generated method stub
				if (mPopup != null && mPopup.isShowing()) {
					mPopup.dismiss();
					mPopup = null;
					if(arg2 > INVALID_POSITION)
						TCLSpinner.this.setSelection(arg2);
					
				}				
			}
			
		});
		lv_group.setLayoutParams(new LinearLayout.LayoutParams(this.getWidth()-22, LinearLayout.LayoutParams.WRAP_CONTENT));
		
		mPopup = new PopupWindow(view, this.getWidth()-22, LinearLayout.LayoutParams.WRAP_CONTENT,true);
		mPopup.setFocusable(true);
		mPopup.setOutsideTouchable(true);
		mPopup.setBackgroundDrawable(new BitmapDrawable());/*back key will be processed */	
//	    DisplayMetrics dm2 = getResources().getDisplayMetrics();
		mPopup.showAsDropDown(this, 11, -8);	
		return true;
	}
	
	@Override
	public void onClick(DialogInterface dialog, int which) {
		setSelection(which);
		dialog.dismiss();
		mPopup = null;
	}

	/*
	 * <p>Wrapper class for an Adapter. Transforms the embedded Adapter instance
	 * into a ListAdapter.</p>
	 */
	private static class DropDownAdapter implements ListAdapter, SpinnerAdapter {
		private SpinnerAdapter mAdapter;

		/**
		 * <p>
		 * Creates a new ListAddapter wrapper for the specified adapter.
		 * </p>
		 * 
		 * @param adapter
		 *            the Adapter to transform into a ListAdapter
		 */
		public DropDownAdapter(SpinnerAdapter adapter) {
			this.mAdapter = adapter;
		}

		public int getCount() {
			return mAdapter == null ? 0 : mAdapter.getCount();
		}

		public Object getItem(int position) {
			
			
			return mAdapter == null ? null : mAdapter.getItem(position);
		}

		public long getItemId(int position) {
			
			
			return mAdapter == null ? -1 : mAdapter.getItemId(position);
		}

		public View getView(int position, View convertView, ViewGroup parent) {
			return getDropDownView(position, convertView, parent);
		}

		public View getDropDownView(int position, View convertView,
				ViewGroup parent) {
			return mAdapter == null ? null : mAdapter.getDropDownView(position,
					convertView, parent);
		}

		public boolean hasStableIds() {
			return mAdapter != null && mAdapter.hasStableIds();
		}

		public void registerDataSetObserver(DataSetObserver observer) {
			if (mAdapter != null) {
				mAdapter.registerDataSetObserver(observer);
			}
		}

		public void unregisterDataSetObserver(DataSetObserver observer) {
			if (mAdapter != null) {
				mAdapter.unregisterDataSetObserver(observer);
			}
		}

		/**
		 * <p>
		 * Always returns false.
		 * </p>
		 * 
		 * @return false
		 */
		public boolean areAllItemsEnabled() {
			return true;
		}

		/**
		 * <p>
		 * Always returns false.
		 * </p>
		 * 
		 * @return false
		 */
		public boolean isEnabled(int position) {
			return true;
		}

		public int getItemViewType(int position) {
			return 0;
		}

		public int getViewTypeCount() {
			return 1;
		}

		public boolean isEmpty() {
			return getCount() == 0;
		}

	}
}
