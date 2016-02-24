package android.tclwidget;	

import android.app.Dialog;
import android.content.Context;
import android.graphics.drawable.AnimationDrawable;
import android.os.Bundle;
import android.util.AttributeSet;
import android.widget.ImageView;
import android.widget.TextView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import com.android.internal.R;

public class TCLDLabelEspecial extends Dialog
{
	private static TCLDLabelEspecial tcl_label;
	private static TextView textView;
	private static ImageView imgDance;
	private static AnimationDrawable animDance;
	private TCLDLabelEspecial(Context context)
	{
		super(context,R.style.TCLDLabelDialog);
	}

	public TCLDLabelEspecial setMessage(String text)
	{
		if(textView != null) {
			textView.setText(text);
		}
		return  this;
	}
	public TextView getTextView() {
		return textView;
	}
	public static TCLDLabelEspecial makeTCLDLabel(Context context){
		
		tcl_label = new TCLDLabelEspecial(context);
		LayoutInflater inflate = (LayoutInflater)
		context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
				 
		View v = inflate.inflate(R.layout.tcl_dlabel_especial, null);
		imgDance = (ImageView) v.findViewById(R.id.tcl_especial_idprogress);
		animDance = (AnimationDrawable) imgDance.getBackground();
		        
		textView = (TextView)v.findViewById(R.id.tcl_especial_idtext);     
		tcl_label.setContentView(v);
		animDance.start();
		return tcl_label;		
		
	}
	public static TCLDLabelEspecial makeTCLDLabel(Context context,String text){
        
		tcl_label = new TCLDLabelEspecial(context);
		LayoutInflater inflate = (LayoutInflater)
		context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                         
		View v = inflate.inflate(R.layout.tcl_dlabel_especial, null);
		imgDance = (ImageView) v.findViewById(R.id.tcl_especial_idprogress);
		animDance = (AnimationDrawable) imgDance.getBackground();
                
        textView = (TextView)v.findViewById(R.id.tcl_especial_idtext);     
        textView.setText(text);      
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
//	public void show(){
//		animDance.start();
//	}
}
	
	
