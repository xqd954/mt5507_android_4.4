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

public class TCLDialogToastEspecial extends Dialog{
	/**
	* The TCL_Info image,This is the default
	*/
	public static final int INFO_IMAGE = 0;

	/**
	* The TCL_Warning image
	*/
	public static final int WARNING_IMAGE = 1;
	/**
	* The TCL_Question image
	*/
	public static final int QUESTION_IMAGE = 2;

	static TCLDialogToastEspecial mDialog;

	public TCLDialogToastEspecial(Context context) {
		this(context, com.android.internal.R.style.TCLDialogToast);
	}

	public TCLDialogToastEspecial(Context context, int theme) {
		super(context,
				 (theme==0) ? com.android.internal.R.style.TCLDialogToast : theme);	
		// TODO Auto-generated constructor stub
	}
	
	public static TCLDialogToastEspecial makeText(Context context, CharSequence text){
		
		mDialog = new TCLDialogToastEspecial(context);	
		//setBackgroundWindow();
		LayoutInflater inflate = (LayoutInflater)
		context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		 
   
		View v = inflate.inflate(com.android.internal.R.layout.tcl_toast_especial, null);
        
		TextView tv = (TextView)v.findViewById(com.android.internal.R.id.tcl_especial_idmessage);     
      
       
		tv.setText(text);      
		mDialog.setContentView(v);
      
		return mDialog;		
	}
public static TCLDialogToastEspecial makeText(Context context, int resid){
	
	mDialog = new TCLDialogToastEspecial(context);	
	//setBackgroundWindow();
	LayoutInflater inflate = (LayoutInflater)
	context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
	 

	View v = inflate.inflate(com.android.internal.R.layout.tcl_toast_especial, null);
    
	TextView tv = (TextView)v.findViewById(com.android.internal.R.id.tcl_especial_idmessage);     
  
   
	tv.setText(resid);      
	mDialog.setContentView(v);
  
	return mDialog;		
}
	
	public static TCLDialogToastEspecial makePrompt(Context context, CharSequence text,int imagesrc){
		mDialog = new TCLDialogToastEspecial(context);	
		//setBackgroundWindow();
		LayoutInflater inflate = (LayoutInflater)
		context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);	
		View view=inflate.inflate(com.android.internal.R.layout.tcl_toast_img_especial,null);

		ImageView iv= (ImageView)view.findViewById(com.android.internal.R.id.tcl_especial_idpic);
		if(imagesrc==0){
			iv.setImageResource(com.android.internal.R.drawable.tcl_toast_info_especial);			
		}
		if(imagesrc==1){
			iv.setImageResource(com.android.internal.R.drawable.tcl_toast_warning_especial);			
		}
		if(imagesrc==2){
			iv.setImageResource(com.android.internal.R.drawable.tcl_toast_question_especial);			
		}
		if((imagesrc!=0)&&(imagesrc!=1)&&(imagesrc!=2)){
			iv.setImageResource(imagesrc);	
		}
		TextView tv=(TextView)view.findViewById(com.android.internal.R.id.tcl_especial_idmessage2);	
		tv.setText(text);		
		mDialog.setContentView(view);		
		return mDialog;
	}
   
	public static TCLDialogToastEspecial makePrompt(Context context, int resid,int imagesrc){
		mDialog = new TCLDialogToastEspecial(context);	
		//setBackgroundWindow();

		LayoutInflater inflate = (LayoutInflater)
		context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);	
		View view=inflate.inflate(com.android.internal.R.layout.tcl_toast_img_especial,null);

		ImageView iv= (ImageView)view.findViewById(com.android.internal.R.id.tcl_especial_idpic);
		if(imagesrc==0){
			iv.setImageResource(com.android.internal.R.drawable.tcl_toast_info_especial);			
		}
		if(imagesrc==1){
			iv.setImageResource(com.android.internal.R.drawable.tcl_toast_warning_especial);			
		}
		if(imagesrc==2){
			iv.setImageResource(com.android.internal.R.drawable.tcl_toast_question_especial);			
		}
		if((imagesrc!=0)&&(imagesrc!=1)&&(imagesrc!=2)){
			iv.setImageResource(imagesrc);	
		}
		TextView tv=(TextView)view.findViewById(com.android.internal.R.id.tcl_especial_idmessage2);	
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
		    public void start(final TCLDialogToastEspecial dialog) { 
		       timer.schedule(new TimerTask() { 
		           public void run() { 
		               closeDialog(dialog); 
		               timer.cancel();
		            } 
		           private void closeDialog(TCLDialogToastEspecial dialog) { 
		              dialog.dismiss();
		            } 
		       }, seconds * 1000);
		    } 
	}
   
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
