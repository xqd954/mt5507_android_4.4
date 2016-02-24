/*
 * Copyright (C) 2006 The Android Open Source Project
 * 
 * ©2010-2013 TCL CORPORATION All Rights Reserved.
 */

package android.tclwidget;	

import android.app.Dialog;
import android.content.Context;
import android.graphics.drawable.AnimationDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.ImageView;
import android.widget.TextView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

public class TCLDLabel extends Dialog {
	private static TCLDLabel tcl_label;
	private static String text;
	private static TextView textview;
	private final static String TAG = "TCLDLabel"; 
	  
    private static ImageView imgDance;
	private boolean bShowDLabel = false;
    
    private static AnimationDrawable animDance;

	public TCLDLabel(Context context) {
		super(context,tvos.R.style.TCLDLabelDialog);
	}

	//	protected void onCreate(Bundle savedInstanceState)
	//	{
	//		super.onCreate(savedInstanceState);
	//		setContentView(R.layout.tcl_dlabel);
	//		imgDance = (ImageView) super.findViewById(R.id.progress);
	//		 
	//        animDance = (AnimationDrawable) imgDance.getBackground();
	//		textView = (TextView)findViewById(R.id.text);
	//		if(text != null){
	//			textView.setText(text);
	//		}
	//	}

    /**
     * Make a standard dlabel that just contains a text view with the text from a String.
     * @author TCL TVOS Team
     * @param text    The text of the string resource to use.
     */
	public TCLDLabel setMessage(String text) {
		//{{tj@tcl
		if(textview != null)
		//}}
		textview.setText(text);
		return  this;
	}

    /**
     * Make a standard dlabel that just contains a text view with the text from a String.
     * @author TCL TVOS Team
     * @param context  The context to use.  Usually your {@link android.app.Application}
     *                 or {@link android.app.Activity} object.
     */
	public static TCLDLabel makeTCLDLabel(Context context){
		tcl_label = new TCLDLabel(context);
		LayoutInflater inflate = (LayoutInflater)
		context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);

		View v = inflate.inflate(tvos.R.layout.tcl_dlabel, null);
		imgDance = (ImageView) v.findViewById(tvos.R.id.progress);

		animDance = (AnimationDrawable) imgDance.getBackground();

		textview = (TextView)v.findViewById(tvos.R.id.text);           
		tcl_label.setContentView(v);
		animDance.start();
		return tcl_label;		
	}

    /**
     * Make a standard dlabel that just contains a text view with the text from a String.
     * @author TCL TVOS Team
     * @param context  The context to use.  Usually your {@link android.app.Application}
     *                 or {@link android.app.Activity} object.
     * @param text    The text of the string resource to use.
     */
	public static TCLDLabel makeTCLDLabel(Context context,String text){
        tcl_label = new TCLDLabel(context);
        LayoutInflater inflate = (LayoutInflater)
		context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
			 
		View v = inflate.inflate(tvos.R.layout.tcl_dlabel, null);
		imgDance = (ImageView) v.findViewById(tvos.R.id.progress);
			 
		animDance = (AnimationDrawable) imgDance.getBackground();
		//{{tj@tcl, getTCLDLabelTextView
		textview = (TextView)v.findViewById(tvos.R.id.text);     
		textview.setText(text);      
		//}}
		tcl_label.setContentView(v);
		animDance.start();
		return tcl_label;                
	}

	private static void setBackgroundWindow(){
		Window window = tcl_label.getWindow(); 
		//window.setFlags (WindowManager.LayoutParams.FLAG_BLUR_BEHIND, 
		//WindowManager.LayoutParams.FLAG_BLUR_BEHIND );
		window.setFlags (WindowManager.LayoutParams.FLAG_DIM_BEHIND, 
		WindowManager.LayoutParams.FLAG_DIM_BEHIND );
		WindowManager.LayoutParams lp = window.getAttributes(); 
		lp.dimAmount=0.7f;    
		window.setAttributes(lp);     
	 }

	public void setAnimationMode(int mode){

	}

	//public void show(){
	//	animDance.start();
	//}
	
	//{{tj@tcl, getTCLDLabelTextView
	public TextView getTextView(){
		return textview;
	}
	//}}
	
	public void cancel() {
		synchronized (this) {
			bShowDLabel = false;
		}
		new Handler().postDelayed(new Runnable() {
			public void run() {
				try {
					Log.i(TAG, "Cancel=============");
					TCLDLabel.super.cancel();
				} catch (Exception e) {
				}
			}
		}, 1000);
	}

	public void dismiss() {
		synchronized (this) {
			bShowDLabel = false;
		}
		new Handler().postDelayed(new Runnable() {
			public void run() {
				try {
					Log.i(TAG, "Dismiss=============");
					TCLDLabel.super.dismiss();
				} catch (Exception e) {
				}
			}
		}, 1000);
	}

	public boolean isShowing() {
		synchronized (this) {
			bShowDLabel = false;
		}
		Log.i(TAG, "isShowing=============");
		return super.isShowing();
	}

	public void show() {
		bShowDLabel = true;
		new Handler().postDelayed(new Runnable() {
			public void run() {
				synchronized (TCLDLabel.this) {
					Log.i(TAG, "Show=============" + bShowDLabel);
					if(bShowDLabel) {
						try {
							TCLDLabel.super.show();
						} catch (Exception e) {
						}
					}
				}
			}
		}, 1000);
	}
}
