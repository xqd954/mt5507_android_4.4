package android.tclwidget;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.drawable.Drawable;

import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.util.DisplayMetrics;
import android.util.TypedValue;

public class TCLFloatMenu extends Dialog {
	private static View mView;
	private static TCLFloatMenu floatMenu;
	private static  View leftLayout;
	private static  View leftLayout2;
	private static View midLayout;
	private static  View rightLayout;
	private static  View rightLayout2;
	private static RelativeLayout menuLayout;
	public static final int TWO_ITEM = 2;
	public static final int THREE_ITEM = 3;
	public static final int FOUR_ITEM = 4;
	public static final int FIVE_ITEM = 5;
	public Window window = null;
	
	//added by xufei@tcl.com begin for 1080p and 720p compatible.
	DisplayMetrics mMetrics = new DisplayMetrics();
	//added by xufei@tcl.com end
	
	public TCLFloatMenu(Context context, int theme) {
		this(context, theme, -173);
	}

	public TCLFloatMenu(Context context,int theme,/*int x,*/int posY){
		super(context,theme);
		//added by xufei begin for 1080p and 720p compatible.
		mMetrics.setToDefaults();
		//added by xufei end
		window = getWindow(); 
        WindowManager.LayoutParams wl = window.getAttributes();
        
      	// wl.x = x; 
        wl.y = getPx(posY);   
        window.setAttributes(wl);
	}
	
	//added by xufei begin for 1080p and 720p compatible.
	public int getPx(int dp) {
		return (int)TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, dp, mMetrics);
	}
	//added by xufei end
    
	/**
	 * Create a Float Dialog window that uses a custom dialog style.
	 * @author TCL TVOS Team
	 * @param context The Context in which the Dialog should run. In particular, it
     *                uses the window manager and theme from this context to
     *                present its UI.
	 * @param theme   A style resource describing the theme to use for the 
     * 				  window.If 0, the default dialog theme will be used.
	 * @return        The Object of Float Dialog
	 */
	public static TCLFloatMenu makeFloatMenu(Context context, int theme, int itemNo){		
		if(theme == 0){
			floatMenu = new TCLFloatMenu(context, tvos.R.style.TCLFloatMenu);  
		}else{
			floatMenu = new TCLFloatMenu(context, theme);
		}
		
		//setBackgroundWindow();
		switch(itemNo){
			case 2:{
				setView(context, tvos.R.layout.tcl_menu_2);
				floatMenu.setContentView(mView);
				if(floatMenu != null){			
					leftLayout = floatMenu.findViewById(tvos.R.id.left_RelativeLayout);
					rightLayout = floatMenu.findViewById(tvos.R.id.right_RelativeLayout);
				    return floatMenu;
				}			
			}
			break;
			case 3:{
				setView(context, tvos.R.layout.tcl_menu_3);
				floatMenu.setContentView(mView);
				if(floatMenu != null){			
					leftLayout = floatMenu.findViewById(tvos.R.id.left_RelativeLayout);
					midLayout =  floatMenu.findViewById(tvos.R.id.mid_RelativeLayout);
					rightLayout = floatMenu.findViewById(tvos.R.id.right_RelativeLayout);
					
				    return floatMenu;
				}
			}
			case 4:{
				setView(context, tvos.R.layout.tcl_menu_4);
				floatMenu.setContentView(mView);
				if(floatMenu != null){			
					leftLayout = floatMenu.findViewById(tvos.R.id.left_RelativeLayout);
					leftLayout2 = floatMenu.findViewById(tvos.R.id.left_RelativeLayout2);
					rightLayout = floatMenu.findViewById(tvos.R.id.right_RelativeLayout);
					rightLayout2 = floatMenu.findViewById(tvos.R.id.right_RelativeLayout2);
				    return floatMenu;
				}			
			}
			break;
			case 5:{
				setView(context, tvos.R.layout.tcl_menu_5);
				floatMenu.setContentView(mView);
				if(floatMenu != null){			
					leftLayout = floatMenu.findViewById(tvos.R.id.left_RelativeLayout);
					leftLayout2 = floatMenu.findViewById(tvos.R.id.left_RelativeLayout2);
					midLayout =  floatMenu.findViewById(tvos.R.id.mid_RelativeLayout);
					rightLayout = floatMenu.findViewById(tvos.R.id.right_RelativeLayout);
					rightLayout2 = floatMenu.findViewById(tvos.R.id.right_RelativeLayout2);
				    return floatMenu;
				}
			}
			break;
			default:return null;
		}
		return null;
	}
	
	/**
	 * Gets the layout of the TCLFloatMenu's left item
	 * @author TCL TVOS Team
	 * @return View The view of the TCLFloatMenu's left layout
	 */
	public View getLeftLayout(){
	    return (leftLayout == null) ? null : leftLayout;
	}

	public View getSecondLeftLayout(){
	    return (leftLayout2 == null) ? null : leftLayout2;
	}
	
	/**
	 * Gets the layout of the TCLFloatMenu's middle item
	 * @author TCL TVOS Team
	 * @return View The view of the TCLFloatMenu's middle layout
	 */
	public View getMidLayout(){
		return (midLayout == null) ? null : midLayout;
		
	}

	/**
	 * Gets the layout of the TCLFloatMenu's right item
	 * @author TCL TVOS Team
	 * @return View The view of the TCLFloatMenu's right layout
	 */
	public View getRightLayout(){
		return (rightLayout == null) ? null : rightLayout;
	}

	public View getSecondRightLayout(){
		return (rightLayout == null) ? null : rightLayout2;
	}

	/**
	 * Sets the text of the TCLFloatMenu's left item
	 * @author TCL TVOS Team
	 * @param resId The resource id of the text resource to use. 
	 */
	public void setLeftText(int resId){
		TextView tv =(TextView) mView.findViewById(tvos.R.id.left_text);
		tv.setText(resId);
	}

	public void setSecondLeftText(int resId){
		TextView tv =(TextView) mView.findViewById(tvos.R.id.left_text2);
		tv.setText(resId);
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
	 * Sets the text of the TCLFloatMenu's left item
	 * @author TCL TVOS Team
	 * @param text The text of the CharSequence resource to use.
	 */
	public void setLeftText(CharSequence text){
		TextView tv =(TextView) mView.findViewById(tvos.R.id.left_text);
		tv.setText(text);
	}

	public void setSecondLeftText(CharSequence text){
		TextView tv =(TextView) mView.findViewById(tvos.R.id.left_text2);
		tv.setText(text);
	}
	
	/**
	 * Sets the image resource of the TCLFloatMenu's left item
	 * @author TCL TVOS Team
	 * @param resId The resource id of the image resource to use. 
	 */
	public void setLeftImageResource(int resId){
		ImageView icon = (ImageView)mView.findViewById(tvos.R.id.left_icon);
		icon.setImageResource(resId);	
	}

	public void setSecondLeftImageResource(int resId){
		ImageView icon = (ImageView)mView.findViewById(tvos.R.id.left_icon2);
		icon.setImageResource(resId);	
	}
	
	/**
	 * Sets the image of the TCLFloatMenu's left item
	 * @author TCL TVOS Team
	 * @param drawable The resource of the image to use. 
	 */
	public void setLeftImageDrawable(Drawable drawable){
		ImageView icon = (ImageView)mView.findViewById(tvos.R.id.left_icon);
		icon.setImageDrawable(drawable);
	}

	public void setSecondLeftImageDrawable(Drawable drawable){
		ImageView icon = (ImageView)mView.findViewById(tvos.R.id.left_icon2);
		icon.setImageDrawable(drawable);
	}
	
	/**
	 * Sets the text of the TCLFloatMenu's middle item
	 * @author TCL TVOS Team
	 * @param resId The resource id of the text resource to use. 
	 */
	public void setMidText(int resId){
		TextView tv = (TextView)mView.findViewById(tvos.R.id.mid_text);
		tv.setText(resId);
	}

	/**
	 * Sets the text of the TCLFloatMenu's middle item
	 * @author TCL TVOS Team
	 * @param text The text of the CharSequence resource to use.
	 */
	public void setMidText(CharSequence text){
		TextView tv = (TextView)mView.findViewById(tvos.R.id.mid_text);
		tv.setText(text);
	}

	/**
	 * Sets the image of the TCLFloatMenu's middle item
	 * @author TCL TVOS Team
	 * @param drawable The resource of the image to use. 
	 */
	public void setMidImageDrawable(Drawable drawable){
		ImageView icon = (ImageView)mView.findViewById(tvos.R.id.mid_icon);
		icon.setImageDrawable(drawable);
	}
	
	/**
	 * Sets the image of the TCLFloatMenu's middle item
	 * @author TCL TVOS Team
	 * @param resId The resource id of the image resource to use. 
	 */
	public void setMidImageResource(int resId){
		ImageView icon = (ImageView)mView.findViewById(tvos.R.id.mid_icon);
		icon.setImageResource(resId);
	}
	
	/**
	 * Sets the text of the TCLFloatMenu's left item
	 * @author TCL TVOS Team
	 * @param resId The resource id of the text resource to use. 
	 */
	public void setRightText(int resId){
		TextView tv = (TextView)mView.findViewById(tvos.R.id.right_text);
		tv.setText(resId);
	}

	public void setSecondRightText(int resId){
		TextView tv = (TextView)mView.findViewById(tvos.R.id.right_text2);
		tv.setText(resId);
	}

	/**
	 * Sets the text of the TCLFloatMenu's right item
	 * @author TCL TVOS Team
	 * @param text The text of the CharSequence resource to use.
	 */
	public void setRightText(CharSequence text){
		TextView tv = (TextView)mView.findViewById(tvos.R.id.right_text);
		tv.setText(text);
	}

	public void setSecondRightText(CharSequence text){
		TextView tv = (TextView)mView.findViewById(tvos.R.id.right_text2);
		tv.setText(text);
	}

	/**
	 * Sets the image of the TCLFloatMenu's right item
	 * @author TCL TVOS Team
	 * @param drawable The resource of the image to use. 
	 */
	public void setRightImageDrawable(Drawable drawable){
		ImageView icon = (ImageView)mView.findViewById(tvos.R.id.right_icon);
		icon.setImageDrawable(drawable);
	}

	public void setSecondRightImageDrawable(Drawable drawable){
		ImageView icon = (ImageView)mView.findViewById(tvos.R.id.right_icon2);
		icon.setImageDrawable(drawable);
	}
	
	/**
	 * Sets the image of the TCLFloatMenu's right item
	 * @author TCL TVOS Team
	 * @param resId The resource id of the image resource to use. 
	 */
	public void setRightImageResource(int resId){
		ImageView icon = (ImageView)mView.findViewById(tvos.R.id.right_icon);
		icon.setImageResource(resId);
	}

	public void setSecondRightImageResource(int resId){
		ImageView icon = (ImageView)mView.findViewById(tvos.R.id.right_icon2);
		icon.setImageResource(resId);
	}

	private static void setBackgroundWindow(){
		Window window = floatMenu.getWindow(); 
		//window.setFlags (WindowManager.LayoutParams.FLAG_BLUR_BEHIND, 
		//		WindowManager.LayoutParams.FLAG_BLUR_BEHIND );
		window.setFlags (WindowManager.LayoutParams.FLAG_DIM_BEHIND, 
				WindowManager.LayoutParams.FLAG_DIM_BEHIND );
	    WindowManager.LayoutParams lp = window.getAttributes(); 
	    lp.dimAmount=0.7f;    
	    window.setAttributes(lp);     
	}
    
	/**
	 * Sets the clicklistener of the TCLFloatMenu's left item.
	 * @author TCL TVOS Team
	 * @param View.OnClickListener the clicklistener of the TCLFloatMenu's left item.
	 */
	public void setLeftOnClickListener(View.OnClickListener listener){
		View vLeft = getLeftLayout();
		vLeft.setOnClickListener(listener);
	}
	
	public void setSecondLeftOnClickListener(View.OnClickListener listener){
		View vSecLeft = getSecondLeftLayout();
		vSecLeft.setOnClickListener(listener);
	}

	/**
	 * Sets the clicklistener of the TCLFloatMenu's middle item.
	 * @author TCL TVOS Team
	 * @param View.OnClickListener the clicklistener of the TCLFloatMenu's middle item.
	 */
	public void setMidOnClickListener(View.OnClickListener listener){
		View vMid = getMidLayout();
		vMid.setOnClickListener(listener);
	}
	
	/**
	 * Sets the clicklistener of the TCLFloatMenu's right item.
	 * @author TCL TVOS Team
	 * @param View.OnClickListener the clicklistener of the TCLFloatMenu's right item.
	 */
	public void setRightOnClickListener(View.OnClickListener listener){
		View vRight = getRightLayout();
		vRight.setOnClickListener(listener);
	}

	public void setSecondRightOnClickListener(View.OnClickListener listener){
		View vSecRight = getSecondRightLayout();
		vSecRight.setOnClickListener(listener);
	}

	public void setAnimationMode(int mode){

	}

	@Override
	public void dismiss() {
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
