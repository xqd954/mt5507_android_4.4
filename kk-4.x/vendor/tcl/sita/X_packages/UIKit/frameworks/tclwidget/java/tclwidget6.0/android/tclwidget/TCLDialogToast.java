/*
 * Copyright (C) 2006 The Android Open Source Project
 * 
 * ©2010-2013 TCL CORPORATION All Rights Reserved.
 */

package android.tclwidget;

import java.util.Timer;
import java.util.TimerTask;

import android.app.Dialog;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.TextView;

/**
 * A toast is a view containing a quick little message for the user.  The toast class
 * helps you create and show those.
 * {@more}
 *
 * <p>
 * When the view is shown to the user, appears as a floating view over the
 * application.  It will never receive focus.  The user will probably be in the
 * middle of typing something else.  The idea is to be as unobtrusive as
 * possible, while still showing the user the information you want them to see.
 * Two examples are the volume control, and the brief message saying that your
 * settings have been saved.
 * <p>
 * The easiest way to use this class is to call one of the static methods that constructs
 * everything you need and returns a new Toast object.
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For information about creating Toast notifications, read the
 * <a href="{@docRoot}guide/topics/ui/notifiers/toasts.html">Toast Notifications</a> developer
 * guide.</p>
 * </div>
 */ 

public class TCLDialogToast extends Dialog{
	/**
	 * The TCL_Info image,This is the default.
     * @author TCL TVOS Team
	 */
	public static final int INFO_IMAGE = 0;

	/**
	 * The TCL_Warning image.
     * @author TCL TVOS Team
	 */
	public static final int WARNING_IMAGE = 1;
	/**
	 * The TCL_Question image
     * @author TCL TVOS Team
	 */
	public static final int QUESTION_IMAGE = 2;

	static TCLDialogToast mDialog;

	public TCLDialogToast(Context context) {
		this(context, tvos.R.style.TCLDialogToast);
	}

	public TCLDialogToast(Context context, int theme) {
		super(context,
				 (theme==0) ? tvos.R.style.TCLDialogToast : theme);	
		// TODO Auto-generated constructor stub
	}
	
    /**
     * Make a standard toast that just contains a text view with the text from a CharSequence.
     * @author TCL TVOS Team
     * @param context  The context to use.  Usually your {@link android.app.Application}
     *                 or {@link android.app.Activity} object.
     * @param text    The text of the string resource to use.
     */
	public static TCLDialogToast makeText(Context context, CharSequence text){
		mDialog = new TCLDialogToast(context);	
		//setBackgroundWindow();
		LayoutInflater inflate = (LayoutInflater)
        context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View v = inflate.inflate(tvos.R.layout.tcl_toast, null);
        TextView tv = (TextView)v.findViewById(tvos.R.id.message);     
        tv.setText(text);      
        mDialog.setContentView(v);
		return mDialog;		
	}

    /**
     * Make a standard toast that just contains a text view with the text from a resource.
     * @author TCL TVOS Team
     * @param context  The context to use.  Usually your {@link android.app.Application}
     *                 or {@link android.app.Activity} object.
     * @param resId    The resource id of the string resource to use.  Can be formatted text.
     */
	public static TCLDialogToast makeText(Context context, int resid){
		mDialog = new TCLDialogToast(context);	
		//setBackgroundWindow();
		LayoutInflater inflate = (LayoutInflater)
		context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		View v = inflate.inflate(tvos.R.layout.tcl_toast, null);
		TextView tv = (TextView)v.findViewById(tvos.R.id.message);     
	    tv.setText(resid);      
    	mDialog.setContentView(v);
		return mDialog;		
	}
	
    /**
     * Make a standard toast that just contains a text view with the text and image from a CharSequence.
     * @author TCL TVOS Team
     * @param context  The context to use.  Usually your {@link android.app.Application}
     *                 or {@link android.app.Activity} object.
     * @param text    The text of the string resource to use.
     * @param imagesrc    The resource id of the image resource to use.  Can be formatted image resource.
     */
	public static TCLDialogToast makePrompt(Context context, CharSequence text,int imagesrc){
		mDialog = new TCLDialogToast(context);	
		//setBackgroundWindow();
		LayoutInflater inflate = (LayoutInflater)
         context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);	
		View view=inflate.inflate(tvos.R.layout.tcl_toast_img,null);

		ImageView iv= (ImageView)view.findViewById(tvos.R.id.pic);
		if(imagesrc==1){
			iv.setImageResource(tvos.R.drawable.tcl_warning);			
		}
		if(imagesrc==2){
			iv.setImageResource(tvos.R.drawable.tcl_question);			
		}
		if((imagesrc!=0)&&(imagesrc!=1)&&(imagesrc!=2)){
			iv.setImageResource(imagesrc);	
		}
		TextView tv=(TextView)view.findViewById(tvos.R.id.message2);	
		tv.setText(text);		
		mDialog.setContentView(view);		
		return mDialog;
	}
   
    /**
     * Make a standard toast that just contains a text view with the text and image from a resource.
     * @author TCL TVOS Team
     * @param context  The context to use.  Usually your {@link android.app.Application}
     *                 or {@link android.app.Activity} object.
     * @param resId    The resource id of the string resource to use.  Can be formatted text.
     * @param imagesrc    The resource id of the image resource to use.  Can be formatted image resource.
     */
	public static TCLDialogToast makePrompt(Context context, int resid,int imagesrc){
		mDialog = new TCLDialogToast(context);	
		//setBackgroundWindow();

		LayoutInflater inflate = (LayoutInflater)
         context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);	
		View view=inflate.inflate(tvos.R.layout.tcl_toast_img,null);

		ImageView iv= (ImageView)view.findViewById(tvos.R.id.pic);
		if(imagesrc==1){
			//iv.setImageResource(com.android.internal.R.drawable.tcl_warning);			
			iv.setImageResource(tvos.R.drawable.tcl_warning);			
		}
		if(imagesrc==2){
			iv.setImageResource(tvos.R.drawable.tcl_question);			
		}
		if((imagesrc!=0)&&(imagesrc!=1)&&(imagesrc!=2)){
			iv.setImageResource(imagesrc);	
		}
		TextView tv=(TextView)view.findViewById(tvos.R.id.message2);	
		tv.setText(resid);		
		mDialog.setContentView(view);		
		return mDialog;
	}

	public static class TCLTimer {
		  private final Timer timer = new Timer();
		    private final int seconds;
		    public TCLTimer(int seconds) { 
		       this.seconds = seconds;
		    }
		    public void start(final TCLDialogToast dialog) { 
		       timer.schedule(new TimerTask() { 
		           public void run() { 
		               closeDialog(dialog); 
		               timer.cancel();
		            } 
		           private void closeDialog(TCLDialogToast dialog) { 
		              dialog.dismiss();
		            } 
		       }, seconds * 1000);
		    } 
	}
   
	/**
     * The toast will dismiss after the set seconds.
     * @author TCL TVOS Team
     * @param seconds  The seconds in milliseconds. Remove the toast until dismissToast() is called.
     */
	public void dismissToast(int seconds){
		TCLTimer tclTimer = new TCLTimer(seconds);
		tclTimer.start(mDialog);
	}
	
	private static void setBackgroundWindow(){
		Window window = mDialog.getWindow(); 
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
