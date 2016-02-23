package com.mediatek.ui.menu.commonview;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.AbsSeekBar;

import android.widget.SeekBar;


public class MyProgressBar extends AbsSeekBar {

	public MyProgressBar(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}

	public MyProgressBar(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		// TODO Auto-generated constructor stub
	}

	public MyProgressBar(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
	}
	  public interface OnProgressBarChangeListener {
	      
	        void onProgressChanged(MyProgressBar myProgressBar, int progress, boolean fromUser);
	    
	      
	        void onStartTrackingTouch(MyProgressBar myProgressBar);
	        
	       
	        void onStopTrackingTouch(MyProgressBar myProgressBar);
	    }

	    private OnProgressBarChangeListener mOnProgressBarChangeListener;
	
	    void onProgressRefresh(float scale, boolean fromUser) {
//	          onProgressRefresh(scale, fromUser);
             if (mOnProgressBarChangeListener != null) {
	            mOnProgressBarChangeListener.onProgressChanged(this, getProgress(), fromUser);
	        }
	    }

	   
	    public void setOnProgressBarChangeListener(OnProgressBarChangeListener l) {
	        mOnProgressBarChangeListener = l;
	    }
	    
	   
	    void onStartTrackingTouch() {
	        if (mOnProgressBarChangeListener != null) {
	            mOnProgressBarChangeListener.onStartTrackingTouch(this);
	        }
	    }
	    
	  
	    void onStopTrackingTouch() {
	        if (mOnProgressBarChangeListener != null) {
	            mOnProgressBarChangeListener.onStopTrackingTouch(this);
	        }
	    }
}
