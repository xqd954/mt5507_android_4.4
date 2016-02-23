package com.mediatek.ui;

import java.util.ArrayList;
import java.util.List;

import com.mediatek.ui.commonmenu.CChangeValueItem;
import com.mediatek.ui.commonmenu.CDataItem;
import com.mediatek.ui.commonmenu.CListItem;
import com.mediatek.ui.commonmenu.CMenuAdapter;
import com.mediatek.ui.commonmenu.CMenuIntegration;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.ScreenConstant;

//import com.mediatek.ui.mmp.util.LogicManager;

import android.app.Activity;

import android.os.Bundle;

import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;

import android.view.WindowManager;

import android.widget.LinearLayout.LayoutParams;

import android.widget.ListView;
import android.widget.TextView;
import android.view.Window;





public class CommonMenuActivity extends Activity {
	private CMenuAdapter mAdapter;
	private TextView mTitle;
	private ListView mList;
	private static final String TAG = CommonMenuActivity.class.getSimpleName();
 private ArrayList<String> ss = new ArrayList<String>();
	
//	private TextView mEnter;
	private TextView mExit;
	private TextView mMenuSelect;
//	
//	private ImageView mMenuSelectImg;
//	private ImageView mEnterImg;
//	private ImageView mExitImg;
	//protected LogicManager mLogicManager;
	protected CMenuIntegration mCMenuIntegration;
	private CChangeValueItem mChangeValueItem;
	private Window mWindow;

	//private ImageView mItemImg;
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

		mWindow = getWindow();

		if(mWindow != null){
        
		        WindowManager.LayoutParams lp=mWindow.getAttributes();
				if(lp != null){
		        		lp.gravity=Gravity.LEFT|Gravity.CENTER;
					}				
		        mWindow.setAttributes(lp);
			}
        
        View mainView=LayoutInflater.from(this).inflate(R.layout.thr_menu_main, null);
        int mWidth=ScreenConstant.SCREEN_WIDTH;
        int mHeight=ScreenConstant.SCREEN_HEIGHT;
        setContentView(mainView,new LayoutParams(mWidth*7/24, mHeight/2 /**3/5*/));
        
        initView();
        initData();      
    }
    
    private void initView(){
    	mTitle=(TextView)findViewById(R.id.thr_menu_title);
    	mList=(ListView)findViewById(R.id.thr_menu_list);
    	
    	mMenuSelect = (TextView)findViewById(R.id.thr_select);
    	mMenuSelect.setText(R.string.thr_menu_select);
//    	mEnter = (TextView) findViewById(R.id.thr_menu_confirm);
//    	mEnter.setText(R.string.thr_menu_confim);
    	mExit = (TextView) findViewById(R.id.thr_menu_cancal);
    	mExit.setText(R.string.thr_menu_cancal);
    	
 //   	mMenuSelectImg = (ImageView) findViewById(R.id.thr_select_img);
 //   	mMenuSelectImg.setBackgroundResource(R.drawable.tk_cm_key_select_updown);
//    	mEnterImg = (ImageView) findViewById(R.id.thr_menu_confirm_img);
//    	mEnterImg.setBackgroundResource(R.drawable.tk_cm_key_enter);
 //   	mExitImg = (ImageView) findViewById(R.id.thr_menu_cancal_img);
 //   	mExitImg.setBackgroundResource(R.drawable.tk_cm_key_exit);

    	
}    
    private void initData(){    	   	
    	//mLogicManager = LogicManager.getInstance(this);
    	//maxVolume = mLogicManager.getMaxVolume();
    	mCMenuIntegration = CMenuIntegration.getInstance(this);
    	mChangeValueItem = new CChangeValueItem(this);
    	
    	System.out.println("CMenuIntegration.CONFIG_PICTURE_EFFECT=== "+CMenuIntegration.CONFIG_PICTURE_EFFECT);
    	System.out.println("CMenuIntegration.CONFIG_TRACK=== "+CMenuIntegration.CONFIG_TRACK);
    	System.out.println("CMenuIntegration.CONFIG_SOUND_EFFECT=== "+CMenuIntegration.CONFIG_SOUND_EFFECT);
    	System.out.println("CMenuIntegration.CONFIG_VOLUME=== "+CMenuIntegration.CONFIG_VOLUME);
    	
    	CDataItem data;    	
    	List<CDataItem> list=new ArrayList<CDataItem>();
    	String title=getResources().getString(R.string.thr_menu_picture_mode);
    	String[] values=getResources().getStringArray(R.array.menu_pictrue_mode_list);
    	
		data=new CDataItem(/*"grp_video_picture_mode"*/CMenuIntegration.CONFIG_PICTURE_EFFECT,title,values,mCMenuIntegration.getPictureEffect(),3,0,CDataItem.CHANGEITEM);
		list.add(data);
		
    	title=getResources().getString(R.string.thr_menu_track);
    	values=getResources().getStringArray(R.array.menu_track_mode_list);
    	data=new CDataItem(/*"config_track"*/CMenuIntegration.CONFIG_TRACK,title,values,1,1,0,CDataItem.CHANGEITEM);
		list.add(data);
		
		title=getResources().getString(R.string.thr_menu_Equalizer);
    	values=getResources().getStringArray(R.array.menu_audio_equalizer_list);
    	data=new CDataItem(/*"grp_audio_equalize"*/CMenuIntegration.CONFIG_SOUND_EFFECT,title,values,mCMenuIntegration.getSoundEffect(),2,0,CDataItem.CHANGEITEM);
		list.add(data);  
		
		title=getResources().getString(R.string.thr_menu_volume);
    	data=new CDataItem(/*"config_volume"*/CMenuIntegration.CONFIG_VOLUME,title,null,mCMenuIntegration.getCurrentVolume()/*10*/,100,0,CDataItem.PROGRESSITEM);
    	list.add(data);
    	
    	mAdapter=new CMenuAdapter(this, list);
    	mList.setAdapter(mAdapter);
    }
    @Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		View view = mList.getSelectedView();
		
		switch (keyCode) {
		
		case KeyMap.KEYCODE_DPAD_LEFT:
			((CListItem)view).onPressLeft();
			break;
		case KeyMap.KEYCODE_DPAD_RIGHT:
			((CListItem)view).onPressRigth();
			break;
//		case KeyMap.KEYCODE_MTKIR_CC:
//			finish();
//			break;
		default:
			break;
		}
		
		return super.onKeyDown(keyCode, event);
	}

}