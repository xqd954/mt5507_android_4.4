package com.mediatek.timeshift_pvr.ui;

import android.app.Activity;
import android.content.Context;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ImageView;
import android.widget.PopupWindow;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.mediatek.timeshift_pvr.controller.StateBase;
import com.mediatek.timeshift_pvr.manager.Core;
import com.mediatek.ui.util.MtkLog;

import com.mediatek.ui.R;


public class PVRPlayView extends PopupWindow{
    
    private ProgressBar mProgressBar;
    private TextView timeView,fastView,timeView_endtime;
    private ImageView playView,fbView;
    private Activity mContext;
    
    private TextView multiAudio,subTitle;
    public PVRPlayView(Activity mContext,StateBase state){
        super(mContext.getLayoutInflater().inflate(R.layout.pvr_playpvr_view, null),
                (int) (state.getManager().getTVWidth() * 0.5), (int) (state.getManager()
                .getTVHeight() * 0.2));
        this.mContext = mContext;
        
        initControlView();
    }
    
    public void initControlView(){
        mProgressBar = (ProgressBar)getContentView().findViewById(R.id.pvr_play_progress);
        fastView = (TextView)getContentView().findViewById(R.id.pvr_playfast_view);
        timeView = (TextView)getContentView().findViewById(R.id.pvr_playinfo);
        timeView_endtime = (TextView)getContentView().findViewById(R.id.pvr_playinfo_endtime);
        playView  = (ImageView)getContentView().findViewById(R.id.pvr_play_btn);
        fbView  = (ImageView)getContentView().findViewById(R.id.pvr_play_fb_btn);
        fbView.setVisibility(View.INVISIBLE);
        
        multiAudio = (TextView)getContentView().findViewById(R.id.pvr_play_multi_audio);
        subTitle = (TextView)getContentView().findViewById(R.id.pvr_play_subtitle);
    }
    
    public void show(){
        showAtLocation(mContext.findViewById(Core.ROOT_VIEW), Gravity.CENTER_HORIZONTAL
                | Gravity.BOTTOM, 50, 50);
    }
    
    public void setPlay(){
        fbView.setVisibility(View.INVISIBLE);
        playView.setImageResource(R.drawable.timeshift_play);
    }
    
    public void setFBPlay(){
        fbView.setVisibility(View.INVISIBLE);
        playView.setImageResource(R.drawable.timeshift_fb);
    }
    public void setPause(){
        fbView.setVisibility(View.INVISIBLE);
        playView.setImageResource(R.drawable.timshift_pasuse);
    }
    
    public void setFF(){
        fbView.setVisibility(View.VISIBLE);
        playView.setImageResource(R.drawable.timeshift_ff);
    }
    
    public void setFB(){
        fbView.setVisibility(View.VISIBLE);
        playView.setImageResource(R.drawable.timeshift_fb);
    }
    
    public void setFFNumView(int value){
    	if(value==View.GONE){
    		value=View.INVISIBLE;
    	}
        fbView.setVisibility(value);
    }
    
    public void setFFNum(int value){
    	if(value==View.GONE){
    		value=View.INVISIBLE;
    	}
        fbView.setImageResource(value);
    }
    
    public void dissmiss(){
        super.dismiss();
    }
    
    public void setProgressMax(int max) {
        mProgressBar.setMax(max);
    }
    
    public TextView getFastView(){
        return fastView;
    }

    public int getProgressMax(){
        return mProgressBar.getMax();
    }
    public void setProgress(int progress) {
        mProgressBar.setProgress(progress);
    }
    
    public void setCurrentTime(long mills) {
//        mills /= 1000;
        mills +=1;
        long minute = mills / 60;
        long hour = minute / 60;
        long second = mills % 60;
        minute %= 60;
        MtkLog.e("setCurrentTime", "setCurrentTime--:"+String.format("%02d:%02d:%02d", hour, minute,
                second));
        timeView.setText(String.format("%02d:%02d:%02d", hour, minute,
                second));
        
        //fix DTV00587181
        if(mills>=getProgressMax()){
        	setProgress(getProgressMax());
        }
    }
    public void setCurrentTimeZero() {
      timeView.setText(String.format("%02d:%02d:%02d", 0, 0,
              0));
  }
    public void setEndtime(int mills) {
//        mills /= 1000;
        int minute = mills / 60;
        int hour = minute / 60;
        int second = mills % 60;
        minute %= 60;
        //String text = "";
        try {
            timeView_endtime.setText(String.format("%02d:%02d:%02d", hour, minute, second));
            //text = String.format("%02d:%02d:%02d", hour, minute, second);
        } catch (Exception e) {
            timeView_endtime.setText("");
        }
        
    }
    public void setVisibility(boolean flag){
        if(flag){
            timeView.setVisibility(View.VISIBLE);
        }else{
            timeView.setVisibility(View.INVISIBLE);
        }
    }

    public void updateMultiAudioStr(String str){
    	multiAudio.setText(str);
    }
    
    public void updateSubtitleStr(String str){
    	subTitle.setText(str);
    }
}
