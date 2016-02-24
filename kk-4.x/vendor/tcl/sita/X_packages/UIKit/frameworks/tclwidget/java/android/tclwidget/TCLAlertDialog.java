/*
 * Copyright (C) 2006 The Android Open Source Project
 * 
 * ©2010-2013 TCL CORPORATION All Rights Reserved.
 */

package android.tclwidget;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.TableLayout.LayoutParams;


public class TCLAlertDialog extends Dialog{

	private static View mView;
	private static TCLAlertDialog dialog;
	private static RelativeLayout mTitleLayout;
	private static RelativeLayout mContentLayout;
	private static RelativeLayout mButtonLayout;
	private static RelativeLayout mLayout;

	private static View mButtonView;
	public static final int ONE_BUTTON = 1;
	public static final int TWO_BUTTON = 2;
	public static final int PULLDOWN_IMAGE = 0;
	public static final int QUESTION_IMAGE = 1;	
	public static final int INFO_IMAGE = 2;
	public static final int WARNING_IMAGE = 3;

	/**
	 * Create an TCL Alert Dialog window that uses a default dialog style.
     * @author TCL TVOS Team
	 * @param context The Context in which the Dialog should run. In particular, it
     *                uses the window manager and theme from this context to
     *                present its UI.
	 */
	public TCLAlertDialog(Context context) {
        this(context, com.android.internal.R.style.TCLAlertDialog);
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
	public TCLAlertDialog(Context context, int theme) {	
		super(context,
			 (theme==0) ? com.android.internal.R.style.TCLAlertDialog: theme);	
	}
	
	/**
	 * Creates a Dialog which has no title area.
     * @author TCL TVOS Team
	 * @param context The Context in which the Dialog should run. 
	 * 				  In particular, ituses the window manager and
	 * 				  theme from this context to present its UI.
	 * @param theme   A style resource describing the theme to use for the 
     * 				  window.If 0, the default dialog theme will be used.
     * 
     * return 		  The Object of the TCLAlertDialog.
	 */
	public static TCLAlertDialog makeNoTitle(Context context, int theme){
		 dialog = new TCLAlertDialog(context, theme);
		if(dialog != null){
			//setBackgroundWindow();
			setView(context, com.android.internal.R.layout.tcl_alert_notitle);
			mContentLayout = getLayout(com.android.internal.R.id.alert_content);
			mButtonLayout = getLayout(com.android.internal.R.id.alert_button);
			dialog.setContentView(mView);
			return dialog;
		}else{
		return null;
		}
	}
	
	/**
	 * Creates a Dialog which has no button area.
     * @author TCL TVOS Team
	 * @param context The Context in which the Dialog should run. 
	 * 				  In particular, ituses the window manager and
	 * 				  theme from this context to present its UI.
	 * @param theme   A style resource describing the theme to use for the 
     * 				  window.If 0, the default dialog theme will be used.
     * 
     * return 		  The Object of the TCLAlertDialog.
	 */
	 
	public static TCLAlertDialog makeNoButton(Context context, int theme){
		 dialog = new TCLAlertDialog(context, theme);
		if(dialog != null){
			//setBackgroundWindow();
			setView(context, com.android.internal.R.layout.tcl_alert_nobutton);
			
			mTitleLayout = getLayout(com.android.internal.R.id.alert_title);
			mContentLayout = getLayout(com.android.internal.R.id.alert_content_foot);
			dialog.setContentView(mView);
			return dialog;
		}else{
		return null;
		}
	}
	
	/**
	 * Creates a Dialog which has title, content and button area.
     * @author TCL TVOS Team
	 * @param context The Context in which the Dialog should run. 
	 * 				  In particular, ituses the window manager and
	 * 				  theme from this context to present its UI.
	 * @param theme   A style resource describing the theme to use for the 
     * 				  window.If 0, the default dialog theme will be used.
     * 
     * return 		  The Object of the TCLAlertDialog.
	 */
	public static TCLAlertDialog makeFullDialog(Context context, int theme){
		 dialog = new TCLAlertDialog(context, theme);
		if(dialog != null){
			//setBackgroundWindow();
			setView(context, com.android.internal.R.layout.tcl_alert_full);
			
			mTitleLayout = getLayout(com.android.internal.R.id.alert_title_full);
			mButtonLayout = getLayout(com.android.internal.R.id.alert_button_full);
			mContentLayout = getLayout(com.android.internal.R.id.alert_content_full);
			dialog.setContentView(mView);
			return dialog;
		}else{
		return null;
		}
	}

	private static CharSequence contentText;

	/**
	 * Create the default full dialog.
     * @author TCL TVOS Team
	 * @param context The Context in which the Dialog should run. 
	 * 				  In particular, ituses the window manager and
	 * 				  theme from this context to present its UI.
	 * @param theme A style resource describing the theme to use for the 
     * 				  window.If 0, the default dialog theme will be used.
	 * @param buttonStyle One button or Two button
	 * @param imageStyle TCLAlertDialog.QUESTION_IMAGE,TCLAlertDialog.INFO_IMAGE,TCLAlertDialog.PULLDOWN_IMAGE
	 * return The Object of the TCLAlertDialog.
	 */
	public static TCLAlertDialog makeDefaultFullDialog(Context context,int theme,int buttonStyle,int imageStyle){
		dialog = makeFullDialog(context,theme);
		RelativeLayout.LayoutParams lp =new RelativeLayout.LayoutParams(
    			RelativeLayout.LayoutParams.WRAP_CONTENT, RelativeLayout.LayoutParams.WRAP_CONTENT);
		//lp.topMargin = 10;
		CharSequence ASK = context.getString(com.android.internal.R.string.tcl_dialog_ask);
		CharSequence HINT = context.getString(com.android.internal.R.string.tcl_dialog_hint);
		CharSequence SELECT = context.getString(com.android.internal.R.string.tcl_dialog_select);
		if(imageStyle == TCLAlertDialog.QUESTION_IMAGE)
			dialog.setTitle(context, dialog, TCLAlertDialog.QUESTION_IMAGE, ASK, lp);
		else if(imageStyle == TCLAlertDialog.WARNING_IMAGE)
			dialog.setTitle(context, dialog, TCLAlertDialog.WARNING_IMAGE, HINT, lp);
		else if(imageStyle == TCLAlertDialog.PULLDOWN_IMAGE)
			dialog.setTitle(context, dialog, TCLAlertDialog.PULLDOWN_IMAGE, SELECT, lp);

		RelativeLayout.LayoutParams lp1 =new RelativeLayout.LayoutParams(
    			RelativeLayout.LayoutParams.WRAP_CONTENT, RelativeLayout.LayoutParams.WRAP_CONTENT);
		if(buttonStyle == TCLAlertDialog.TWO_BUTTON){
			dialog.setButton(context, TCLAlertDialog.TWO_BUTTON, lp1);
			Button bt1 = dialog.getLeftButton();
			bt1.setText(com.android.internal.R.string.tcl_dialog_ok1);
			Button bt2 = dialog.getRightButton();
			bt2.setText(com.android.internal.R.string.tcl_dialog_cancle);
		}else if(buttonStyle == TCLAlertDialog.ONE_BUTTON){
			dialog.setButton(context, TCLAlertDialog.ONE_BUTTON, lp1);
			Button bt3 =dialog.getLeftButton();
			bt3.setText(com.android.internal.R.string.tcl_dialog_ok1);
		}
		return dialog;
	}
	
	/**
	 * Set the content text of default dialog.
     * @author TCL TVOS Team
	 * @param context The Context in which the Dialog should run. 
	 * 				  In particular, ituses the window manager and
	 * 				  theme from this context to present its UI.
	 * @param text The text in default dialog content.
	 */	
	public void setContentText(Context context,CharSequence text){
		contentText=text;
		RelativeLayout.LayoutParams lp =new RelativeLayout.LayoutParams(
    			RelativeLayout.LayoutParams.WRAP_CONTENT, RelativeLayout.LayoutParams.WRAP_CONTENT);
		LayoutInflater inflate = (LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);	
		View view = inflate.inflate(com.android.internal.R.layout.tcl_alert_content_text,null);
		TextView tv = ((TextView)view.findViewById(com.android.internal.R.id.label));
		dialog.setCustomContentView(view,lp);
		tv.setText((contentText==null)?"test":contentText);
	}

	private static void setBackgroundWindow(){
		Window window = dialog.getWindow(); 
		//window.setFlags (WindowManager.LayoutParams.FLAG_BLUR_BEHIND, 
			//	WindowManager.LayoutParams.FLAG_BLUR_BEHIND );
		window.setFlags (WindowManager.LayoutParams.FLAG_DIM_BEHIND, 
				WindowManager.LayoutParams.FLAG_DIM_BEHIND );
	    WindowManager.LayoutParams lp = window.getAttributes(); 
	    lp.dimAmount=0.7f;    
	    window.setAttributes(lp);     
	}
	/**
	 * Set the title for the given object of TCLAlertDialog.
     * @author TCL TVOS Team
	 * @param context The Context in which the Dialog should run. 
	 * 				  In particular, ituses the window manager and
	 * 				  theme from this context to present its UI.
	 * @param dialog  The dialog which needs to set title
	 * @param resid   A resource id describing the title icon.
	 * 				  if 0, the pull-down icon will be used.
	 * 				  if 1, the question icon will be used.
	 * 				  Otherwise, the custom icon will be used.
	 * @param text    The title text of the given Dialog
	 * 				  if null, the default title will be used.
	 */
	public void setTitle(Context context, TCLAlertDialog dialog, int resid, CharSequence text,RelativeLayout.LayoutParams lp){
		LayoutInflater inflate = (LayoutInflater)
        context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);	
		View v = inflate.inflate(com.android.internal.R.layout.tcl_alert_title,null);
		ImageView imageView = (ImageView) v.findViewById(com.android.internal.R.id.alert_title_icon);
		switch(resid){
		case 0:
			resid = com.android.internal.R.drawable.tcl_alert_pulldown;
			break;
		case 1:
			resid = com.android.internal.R.drawable.tcl_question;
			break;
		case 2:
			resid = com.android.internal.R.drawable.tcl_info;
			break;
		case 3:
			resid = com.android.internal.R.drawable.tcl_warning;
			break;
		default:
			break;
		}
		imageView.setImageResource(resid);
		
		TextView textView = (TextView)v.findViewById(com.android.internal.R.id.alert_title_text);
		textView.setText((text == null) ? "test" : text);

		dialog.setCustomTitleView(v, lp);
		
	}
	
	/**
	 * Sets the view to the Dialog.
     * @author TCL TVOS Team
	 * @param context The Context in which the Dialog should run. 
	 * 				  In particular, ituses the window manager and
	 * 				  theme from this context to present its UI.
	 * @param resid   The layout resource id.
	 */
	public static void setView(Context context, int resid){
		LayoutInflater inflate = (LayoutInflater)
        context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);	
		mView = inflate.inflate(resid, null);
		
	}
		
    /**
     * Gets the RelativeLayout Object of the given resource id.
     * @author TCL TVOS Team
     * @param resid Resource id of the layout which want to get.
     * @return  the Object of RelativeLayout
     */
	private static RelativeLayout getLayout(int resid){
		mLayout = (RelativeLayout)mView.findViewById(resid);
		return mLayout;
	}
	/**
	 * Adds custom Title view to title of AlertDialog.
     * @author TCL TVOS Team
	 * @param view  The desired title to display 
	 */
	public void setCustomTitleView(View view){
		if(mTitleLayout != null){
			mTitleLayout.removeAllViews();
			mTitleLayout.addView(view);
			
		}else
			return;
	}
	/**
	 * Adds custom Title view to title of AlertDialog.
     * @author TCL TVOS Team
	 * @param view  The desired title to display 
	 * @param lp the layout parameters to set on the child.
	 */
	public void setCustomTitleView(View view, RelativeLayout.LayoutParams lp){
		if(mTitleLayout != null){
			mTitleLayout.removeAllViews();
			mTitleLayout.addView(view, lp);
			
		}else
			return;
	}
	/**
	 * Adds custom button view to button of AlertDialog.
     * @author TCL TVOS Team
	 * @param view  The desired button to display 
	 */
	public void setCustomButtonView(View view){
		if(mButtonLayout != null){
			mButtonLayout.removeAllViews();
			mButtonLayout.addView(view);
		}else 
			return;
	}
	/**
	 * Adds custom button view to button of AlertDialog.
     * @author TCL TVOS Team
	 * @param view  The desired button to display 
	 * @param lp the layout parameters to set on the child.
	 */
	public void setCustomButtonView(View view, RelativeLayout.LayoutParams lp){
		if(mButtonLayout != null){
			mButtonLayout.removeAllViews();
			mButtonLayout.addView(view, lp);
		}else 
			return;
	}
	/**
	 * Adds custom content view to content of AlertDialog.
     * @author TCL TVOS Team
	 * @param view  The desired content to display
	 */
	public void setCustomContentView(View view){
		if(mContentLayout != null){
			mContentLayout.removeAllViews();
			mContentLayout.addView(view);
		}else
			return;
		
	}

	/**
	 * Adds custom content view to content of AlertDialog.
     * @author TCL TVOS Team
	 * @param view  The desired content to display
	 * @param lp the layout parameters to set on the child  
	 */
	public void setCustomContentView(View view, RelativeLayout.LayoutParams lp){
		if(mContentLayout != null){
			mContentLayout.removeAllViews();
			mContentLayout.addView(view, lp);
		}else
			return;
		
	}
	
	/**
	 * Add buttons to the Dialog which has button area.
     * @author TCL TVOS Team
	 * @param context
	 * @param buttonType The type of button that want to adds to Dialog. 
	 * 		  TCLAlertDialog.ONE_BUTTON : adds only one button.
	 * 		  TCLAlertDialog.TWO_BUTTON : adds two button.
	 * 		  Other : adds custom button.
	 */
	public View setButton(Context context, int buttonType, RelativeLayout.LayoutParams lp){
		LayoutInflater inflate = (LayoutInflater)
        context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);	
	
		switch(buttonType){
		case ONE_BUTTON:
			mButtonView = inflate.inflate(com.android.internal.R.layout.tcl_alert_button_one, null);
			break;
		case TWO_BUTTON:
			mButtonView = inflate.inflate(com.android.internal.R.layout.tcl_alert_button_two, null);
			break;
		
		default:
			mButtonView = inflate.inflate(buttonType, null);	
			break;
		}
		RelativeLayout layout = getLayout(mButtonLayout.getId());

		layout.addView(mButtonView, lp);		
		return mButtonView;
	}
	
	/**
	 * Gets the Object of Confirm button.
     * @author TCL TVOS Team
	 //	 * @return The Object of Confirm button.
	 */
	public TCLButton getLeftButton(){
		TCLButtonBlack btn = (TCLButtonBlack)mButtonView.findViewById(com.android.internal.R.id.button1);
		return btn;
	}
	
	/**
	 * Gets the Object of Reset button.
     * @author TCL TVOS Team
	 * @return The Object of Reset button.
	 */
	public TCLButton getRightButton(){
		TCLButtonBlack btn = (TCLButtonBlack)mButtonView.findViewById(com.android.internal.R.id.button2);
		return btn;
	}
	
	public void setAnimationMode(int mode){

	}
	
	
}
