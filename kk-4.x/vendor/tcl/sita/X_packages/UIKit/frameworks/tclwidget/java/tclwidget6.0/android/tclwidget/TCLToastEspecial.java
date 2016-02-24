package android.tclwidget;


import android.content.Context;
import android.graphics.drawable.Drawable;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

public class TCLToastEspecial extends Toast{
    /**
     * Show the view or text notification for a short period of time.  This time
     * could be user-definable.  This is the default.
     */
    public static final int LENGTH_SHORT = 0;

    /**
     * Show the view or text notification for a long period of time.  This time
     * could be user-definable.
     */
    public static final int LENGTH_LONG = 1;
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


	public TCLToastEspecial(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}
	
	public static TCLToastEspecial makeText(Context context, CharSequence text, int duration){
		
		TCLToastEspecial tclToast = new TCLToastEspecial(context);
	
		LayoutInflater inflate = (LayoutInflater)
		context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);		
		View v = inflate.inflate(tvos.R.layout.tcl_toast_especial, null);
		TextView tv = (TextView)v.findViewById(tvos.R.id.tcl_especial_idmessage);     
       
		tv.setText(text);      
		tclToast.setView(v);
		tclToast.setDuration(duration);
		tclToast.setGravity(Gravity.CENTER, 0, 0);
		return tclToast;		
	}
	public static TCLToastEspecial makeText(Context context, int textid, int duration){
		
		TCLToastEspecial tclToast = new TCLToastEspecial(context);
		
		LayoutInflater inflate = (LayoutInflater)
		context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);		
		View v = inflate.inflate(tvos.R.layout.tcl_toast_especial, null);
		TextView tv = (TextView)v.findViewById(tvos.R.id.tcl_especial_idmessage);     
       
		tv.setText(textid);      
		tclToast.setView(v);
		tclToast.setDuration(duration);
		tclToast.setGravity(Gravity.CENTER, 0, 0);
		return tclToast;		
	}
	
	public static TCLToastEspecial makePrompt(Context context, CharSequence text, int imagesrc,int duration){
		TCLToastEspecial tclToast = new TCLToastEspecial(context);
			
		LayoutInflater inflate = (LayoutInflater)
		context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);	
		View view=inflate.inflate(tvos.R.layout.tcl_toast_img_especial,null);
		ImageView iv= (ImageView)view.findViewById(tvos.R.id.tcl_especial_idpic);
		if(imagesrc==0){
			iv.setImageResource(tvos.R.drawable.tcl_toast_info_especial);			
		}		
		if(imagesrc==1){
			iv.setImageResource(tvos.R.drawable.tcl_toast_warning_especial);			
		}
		if(imagesrc==2){
			iv.setImageResource(tvos.R.drawable.tcl_toast_question_especial);			
		}
		if((imagesrc!=0)&&(imagesrc!=1)&&(imagesrc!=2)){
			iv.setImageResource(imagesrc);	
		}

		TextView tv=(TextView)view.findViewById(tvos.R.id.tcl_especial_idmessage2);
		tv.setText(text);
		tclToast.setView(view);
		tclToast.setDuration(duration);
		tclToast.setGravity(Gravity.CENTER, 0, 0);
		return tclToast;
	}
	public static TCLToastEspecial makePrompt(Context context, int textid, int imagesrc,int duration){
		TCLToastEspecial tclToast = new TCLToastEspecial(context);
		LayoutInflater inflate = (LayoutInflater)
		context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);	
		View view=inflate.inflate(tvos.R.layout.tcl_toast_img_especial,null);
		ImageView iv= (ImageView)view.findViewById(tvos.R.id.tcl_especial_idpic);
		if(imagesrc==0){
			iv.setImageResource(tvos.R.drawable.tcl_toast_info_especial);			
		}
		if(imagesrc==1){
			iv.setImageResource(tvos.R.drawable.tcl_toast_warning_especial);			
		}
		if(imagesrc==2){
			iv.setImageResource(tvos.R.drawable.tcl_toast_question_especial);			
		}
		if((imagesrc!=0)&&(imagesrc!=1)&&(imagesrc!=2)){
			iv.setImageResource(imagesrc);	
		}

		TextView tv=(TextView)view.findViewById(tvos.R.id.tcl_especial_idmessage2);
		tv.setText(textid);
		tclToast.setView(view);
		tclToast.setDuration(duration);
		tclToast.setGravity(Gravity.CENTER, 0, 0);
		return tclToast;
	}

	public void setAnimationMode(int mode){

	}

}
