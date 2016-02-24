/*
 * Copyright (C) 2006 The Android Open Source Project
 * 
 * ©2010-2013 TCL CORPORATION All Rights Reserved.
 */

package android.tclwidget;

import android.app.Dialog;
import android.content.Context;

import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.RelativeLayout;
import android.widget.TextView;


public class TCLListDialog extends Dialog{

	private static TCLListDialog dialog;
	/**
	 * Create an TCL Alert Dialog window that uses a default dialog style.
     * @author TCL TVOS Team
	 * @param context The Context in which the Dialog should run. In particular, it
     *                uses the window manager and theme from this context to
     *                present its UI.
	 */
	public TCLListDialog(Context context) {
        this(context, tvos.R.style.TCLAlertDialog);
    }

	/**
	 * Create an Alert Dialog window that uses a custom dialog style.
     * @author TCL TVOS Team
	 * @param context The Context in which the Dialog should run. In particular, it
     *                uses the window manager and theme from this context to
     *                present its UI.
	 * @param theme   A style resource describing the theme to use for the 
     * 				  window.If 0, the default dialog theme will be used.
	 */
	public TCLListDialog(Context context, int theme) {	
		super(context,
			 (theme==0) ? tvos.R.style.TCLAlertDialog : theme);	
	}
	
	
	/**
	 * Creates a Dialog which has no button area.
     * @author TCL TVOS Team
	 * @param context The Context in which the Dialog should run. 
	 * 				  In particular, ituses the window manager and
	 * 				  theme from this context to present its UI.
	 * @param theme   A style resource describing the theme to use for the 
	 * 				  window.If 0, the default dialog theme will be used.
	 * @param text        set text to title.
	 * @param width        set width for title view.
	 * @param height        set height for title view.
	 * @param view        set view to content.
	 * @param lp        set LayoutParams for content view
	 * return 		  The Object of the TCLListtDialog.
	 */
	public static TCLListDialog makeListDialog(Context context, int theme,CharSequence text,int width,int height,View view,RelativeLayout.LayoutParams lp){
		dialog = new TCLListDialog(context, theme);
		//setBackgroundWindow();
		LayoutInflater inflate = (LayoutInflater)
        context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);	
		View mView = inflate.inflate(tvos.R.layout.tcl_listdialog,null);
		TextView tv = (TextView)mView.findViewById(tvos.R.id.list_title);
		tv.setText(text);
		tv.setWidth(width);
		tv.setHeight(height);
		if(view != null)
		{
			RelativeLayout rl = (RelativeLayout)mView.findViewById(tvos.R.id.list_content_foot);
			rl.addView(view);
		}
		dialog.setContentView(mView,lp);
		return dialog;
	}
	
	private static void setBackgroundWindow(){
		Window window = dialog.getWindow(); 
		//window.setFlags (WindowManager.LayoutParams.FLAG_BLUR_BEHIND, 
		//		WindowManager.LayoutParams.FLAG_BLUR_BEHIND );
		window.setFlags (WindowManager.LayoutParams.FLAG_DIM_BEHIND, 
				WindowManager.LayoutParams.FLAG_DIM_BEHIND );
	    WindowManager.LayoutParams lp = window.getAttributes(); 
	    lp.dimAmount=0.7f;    
	    window.setAttributes(lp);     
	}

	public void setAnimationMode(int mode){

	}
}
