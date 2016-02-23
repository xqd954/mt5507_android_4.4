package android.widget;

import android.app.Dialog;
import android.content.Context;
import android.widget.TCLButton;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.util.TypedValue;


public class TCLAlertDialogEspecial extends Dialog{

	private static View mView;
	private static TCLAlertDialogEspecial dialog;
	
	private static LinearLayout mContentLayout;
	private static LinearLayout mButtonLayout;
	private static LinearLayout mLayout;


	private static View mButtonView;
	public static final int ONE_BUTTON = 1;
	public static final int TWO_BUTTON = 2;
	public static final int THREE_BUTTON = 3;
	/**
	 * Create an TCL Alert Dialog window that uses a default dialog style.
	 * @param context The Context in which the Dialog should run. In particular, it
     *                uses the window manager and theme from this context to
     *                present its UI.
	 */
	
	public TCLAlertDialogEspecial(Context context) {
        this(context, com.android.internal.R.style.TCLAlertDialog);
      
    }

	/**
	 * Create an Alert Dialog window that uses a custom dialog style.
	 * @param context The Context in which the Dialog should run. In particular, it
     *                uses the window manager and theme from this context to
     *                present its UI.
	 * @param theme   A style resource describing the theme to use for the 
     * 				  window.If 0, the default dialog theme will be used.
	 */
	public TCLAlertDialogEspecial(Context context, int theme) {	
		super(context,
			 (theme==0) ? com.android.internal.R.style.TCLAlertDialog: theme);	
	}


	/**
	 * Creates a Dialog which has title, content and button area.
	 * @param context The Context in which the Dialog should run. 
	 * 				  In particular, ituses the window manager and
	 * 				  theme from this context to present its UI.
	 * @param theme   A style resource describing the theme to use for the 
     * 				  window.If 0, the default dialog theme will be used.
     * 
     * return 		  The Object of the TCLAlertDialog.
	 */
	public static TCLAlertDialogEspecial makeFullDialog(Context context, int theme){
		 dialog = new TCLAlertDialogEspecial(context, theme);
		if(dialog != null){
			//setBackgroundWindow();
			setView(context, com.android.internal.R.layout.tcl_alert_full_especial);
			
			mButtonLayout = getLayout(com.android.internal.R.id.tcl_especial_idalert_gray_full_button);
			mContentLayout = getLayout(com.android.internal.R.id.tcl_especial_idalert_gray_full_content);
			dialog.setContentView(mView);
			return dialog;
		}else{
		return null;
		}
	}
	
	//private static CharSequence contentText;
	
	/**
	 * Create the default full dialog.
	 * @param context The Context in which the Dialog should run. 
	 * 				  In particular, ituses the window manager and
	 * 				  theme from this context to present its UI.
	 * @param theme A style resource describing the theme to use for the 
     * 				  window.If 0, the default dialog theme will be used.
	 * @param buttonStyle One button or Two button
	 * @return The Object of the TCLAlertDialogGray.
	 */
	
	public static TCLAlertDialogEspecial makeDefaultFullDialog(Context context,int theme,int buttonStyle){
		dialog = TCLAlertDialogEspecial.makeFullDialog(context,theme);
		
		if(buttonStyle == TCLAlertDialogEspecial.TWO_BUTTON){
			dialog.setButton(context, TCLAlertDialogEspecial.TWO_BUTTON);
			Button bt1 = dialog.getLeftButton();
			bt1.setText(com.android.internal.R.string.tcl_dialog_ok);
			Button bt2 = dialog.getRightButton();
			bt2.setText(com.android.internal.R.string.tcl_dialog_cancle);
		}else if(buttonStyle == TCLAlertDialogEspecial.ONE_BUTTON){
			dialog.setButton(context, TCLAlertDialogEspecial.ONE_BUTTON);
			Button bt3 =dialog.getLeftButton();
			bt3.setText(com.android.internal.R.string.tcl_dialog_ok);
		}else if(buttonStyle == TCLAlertDialogEspecial.THREE_BUTTON){
			dialog.setButton(context, TCLAlertDialogEspecial.THREE_BUTTON);
			Button bt1 = dialog.getLeftButton();
			bt1.setText(com.android.internal.R.string.tcl_dialog_ok);
			Button bt2 = dialog.getRightButton();
			bt2.setText(com.android.internal.R.string.tcl_dialog_cancle);
			Button bt3 = dialog.getCenterButton();
			bt3.setText(com.android.internal.R.string.tcl_dialog_alonelistener);
		}
		return dialog;
	}
	
	
	/**
	 * Set the content text of default dialog.
	 * @param context The Context in which the Dialog should run. 
	 * 				  In particular, ituses the window manager and
	 * 				  theme from this context to present its UI.
	 * @param text The text in default dialog content.
	 */
	
	public void setContentText(Context context,CharSequence text){
		//contentText=text;
		LinearLayout.LayoutParams lp =new LinearLayout.LayoutParams(
				LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
		LayoutInflater inflate = (LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);	
		View view = inflate.inflate(com.android.internal.R.layout.tcl_alert_content_text_especial,null);
		TextView tv = ((TextView)view.findViewById(com.android.internal.R.id.tcl_especial_idlabel));
		dialog.setCustomContentView(view/*,lp*/);
		tv.setText((text==null)?"test":text);

		//add by gaodw.
		//tv.setTextAlignment(TextView.TEXT_ALIGNMENT_CENTER);
		//tv.setTextSize(TypedValue.COMPLEX_UNIT_SP, 30);
		//end

	}
	

	/**
	 * Sets the view to the Dialog.
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
    * Gets the LinearLayout Object of the given resource id 
    * @param resid Resource id of the layout which want to get.
    * @return  the Object of LinearLayout
    */
	private static LinearLayout getLayout(int resid){
		mLayout = (LinearLayout)mView.findViewById(resid);
		return mLayout;
	}

	/**
	 * Adds custom button view to button of AlertDialog.
	 * 
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
	 * 
	 * @param view  The desired button to display 
	 * @param lp the layout parameters to set on the child.
	 */
	public void setCustomButtonView(View view, LinearLayout.LayoutParams lp){
		if(mButtonLayout != null){
			mButtonLayout.removeAllViews();
			mButtonLayout.addView(view, lp);
		}else 
			return;
	}
	/**
	 * Adds custom content view to content of AlertDialog.
	 * 
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
	 * 
	 * @param view  The desired content to display
	 * @param lp the layout parameters to set on the child  
	 */
	public void setCustomContentView(View view, LinearLayout.LayoutParams lp){
		if(mContentLayout != null){
			mContentLayout.removeAllViews();
			mContentLayout.addView(view, lp);
		}else
			return;
		
	}
	
	/**
	 * Add buttons to the Dialog which has button area.
	 * @param context
	 * @param buttonType The type of button that want to adds to Dialog. 
	 * 		  TCLAlertDialog.ONE_BUTTON : adds only one button.
	 * 		  TCLAlertDialog.TWO_BUTTON : adds two button.
	 * 		  Other : adds custom button.
	 */
	public View setButton(Context context, int buttonType){
		LayoutInflater inflate = (LayoutInflater)
        context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);	
		 LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
					LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
		 
		switch(buttonType){
		case ONE_BUTTON:
			//mButtonView = inflate.inflate(com.android.internal.R.layout.tcl_alert_button_one_especial, null);
			break;
		case TWO_BUTTON:
			mButtonView = inflate.inflate(com.android.internal.R.layout.tcl_alert_button_two_especial, null);
			break;
		case THREE_BUTTON:
			mButtonView = inflate.inflate(com.android.internal.R.layout.tcl_alert_button_three_especial, null);
			break;
		default:
			mButtonView = inflate.inflate(buttonType, null);	
			break;
		}
		LinearLayout layout = getLayout(mButtonLayout.getId());

		lp.width = 700; //add by gaodw.
		//lp.height = 150; //add by gaodw.

		layout.addView(mButtonView, lp);		
		return mButtonView;
	}
	/**
	 * Gets the Object of Confirm button.
	 * @return The Object of Confirm button.
	 */
	public TCLButtonEspecial getLeftButton(){
		TCLButtonEspecial btn = (TCLButtonEspecial)mButtonView.findViewById(com.android.internal.R.id.tcl_especial_idbutton1);
		return btn;
	}
	
	/**
	 * Gets the Object of Reset button.
	 * @return The Object of Reset button.
	 */
	public TCLButtonEspecial getRightButton(){
		TCLButtonEspecial btn = (TCLButtonEspecial)mButtonView.findViewById(com.android.internal.R.id.tcl_especial_idbutton2);
		return btn;
	}

	/**
	 * Gets the Object of Center button.

	 * @return The Object of Center button.
	 */
	public TCLButtonEspecial getCenterButton(){
		TCLButtonEspecial btn = (TCLButtonEspecial)mButtonView.findViewById(com.android.internal.R.id.tcl_especial_idbutton3);
		return btn;
	}	
	public void setAnimationMode(int mode){

	}
	
	
}
