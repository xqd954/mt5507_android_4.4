package com.mediatek.ui.cec;

import android.R.integer;
import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.app.Dialog;
import android.content.Context;
import android.graphics.Rect;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ListView;

import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.cec.CecListviewAdapter.CecDataItem;
import com.mediatek.ui.menu.commonview.ToastDialog;
import com.mediatek.ui.nav.util.CheckLockSignalChannelState;
import com.mediatek.ui.util.AnimationManager;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

import java.util.List;

public class CecMainDialog extends Dialog implements android.view.View.OnKeyListener{
    private float wScale = 0.2f;
    private float hScale = 0.35f;
    private ListView deviceListView;
    private Context mContext;
    private int xxxx=0;
    private int yyyy=0;
    
    private CecDataItem selectedItem;
    private int selectedPosition;
    private CecSecondDialog sDialog =null;
    public static final int MSG_DISMISS = 1;
	public static final int MSG_DISMISS_DELAY = 10000;
	private CecListviewAdapter cecListviewAdapter;

    public CecMainDialog(Context context) {
        super(context, R.style.Theme_TurnkeyCommDialog);
        setContentView(R.layout.cec_dialog_main);
        this.mContext = context;
        this.getWindow().setLayout((int) (CecManager.tvWidth * wScale),(int) (CecManager.tvHeight* hScale));
//        setPositon(50,200);
        initViews();
    }
    
    private void initViews(){
        deviceListView = (ListView)findViewById(R.id.cec_device_list);
    }
    public void setAdapter(List<CecListviewAdapter.CecDataItem> dataGroup){
    	cecListviewAdapter = new CecListviewAdapter(mContext,dataGroup);
        deviceListView.setAdapter(cecListviewAdapter);
        deviceListView.setOnItemSelectedListener(new ListListener());
        deviceListView.setOnKeyListener(this);
    }
    
    public void setPositon(int xoff, int yoff) {
        Window window = CecMainDialog.this.getWindow();
        WindowManager.LayoutParams lp = window.getAttributes();
        lp.x = xoff;// * ScreenConstant.SCREEN_WIDTH/CecManager.tvWidth;
        lp.y = yoff*ScreenConstant.SCREEN_HEIGHT/CecManager.tvHeight;
        window.setAttributes(lp);
    }
    public void setPositon() {
        Window window = CecMainDialog.this.getWindow();
        WindowManager.LayoutParams lp = window.getAttributes();
        lp.x = xxxx;// * ScreenConstant.SCREEN_WIDTH/CecManager.tvWidth;
        lp.y = yyyy;//yoff*ScreenConstant.SCREEN_HEIGHT/CecManager.tvHeight;
        Log.e("CEC", "cec:xxxx:"+xxxx+"_yyyy:"+yyyy);
        window.setAttributes(lp);
    }
    
    public class ListListener implements OnItemSelectedListener{

        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            // TODO Auto-generated method stub
           
        }

        @Override
        public void onNothingSelected(AdapterView<?> parent) {
            // TODO Auto-generated method stub
            
        }
        
    }
    
//    @Override
//    public boolean onKeyDown(int keyCode, KeyEvent event) {
//        // TODO Auto-generated method stub
//        switch (keyCode) {
//            case KeyMap.KEYCODE_DPAD_DOWN:
//                xxxx=xxxx-20;
//                break;
//            case KeyMap.KEYCODE_DPAD_UP:
//                xxxx=xxxx+20;
//                break;
//            case KeyMap.KEYCODE_DPAD_LEFT:
//                yyyy=yyyy-20;
//                break;
//            case KeyMap.KEYCODE_DPAD_RIGHT:
//                yyyy=yyyy+20;
//                break;
//            default:
//                break;
//        }
//        setPositon();
//        return super.onKeyDown(keyCode, event);
//    }


    @Override
    public boolean onKey(View v, int keyCode, KeyEvent event) {
    	mHandler.removeMessages(MSG_DISMISS);
		mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);
        if (event.getAction()==KeyEvent.ACTION_DOWN) {        
            //Log.e("chengcl", "onkey");
            switch (keyCode) {
                case KeyMap.KEYCODE_DPAD_CENTER:
                case KeyEvent.KEYCODE_ENTER:
                case KeyEvent.KEYCODE_DPAD_RIGHT:
                    List<CecListviewAdapter.CecDataItem> secondDataGroup = null;
                    selectedPosition = deviceListView.getSelectedItemPosition();
                    sDialog = new CecSecondDialog(mContext);
                    selectedItem = (CecDataItem)deviceListView.getItemAtPosition(selectedPosition);
                    if (selectedItem.mItemID.equals(CecManager.CEC_Setup)) {
                        secondDataGroup =CecManager.getInstance().loadSetupDataGroup();
//                        secondDataGroup = CecManager.getInstance().getSetupDataGroup();
                    }else {
                        CecManager.getInstance().setSelectedDevice(selectedPosition);
                        int deviceType = CecManager.getInstance().getDeviceType(selectedItem.getDeviceType());
                        CecManager.getInstance().loadAttributeDataGroup(deviceType);
                        secondDataGroup = CecManager.getInstance().getAttributeDataGroup();
                        Log.e("cecTest", "cecTest:select device_type:"+deviceType);
                    }
                    Log.e("cecTest", "cecTest:dataGroup.size:"+secondDataGroup.size());
                    sDialog.setAdapter(secondDataGroup);
                    sDialog.show();
                    break;
                case KeyMap.KEYCODE_MTKIR_TITLEPBC:
                case KeyMap.KEYCODE_BACK:
                case KeyMap.KEYCODE_VOLUME_UP:
                case KeyMap.KEYCODE_VOLUME_DOWN:
                    dismiss();
                    break;
                default:
                    break;
            }
            
            if(keyCode == KeyMap.KEYCODE_DPAD_CENTER){
    			return true;
    		}
            if (null != mContext && mContext instanceof TurnkeyUiMainActivity) {
    			((TurnkeyUiMainActivity) mContext).onKeyDown(keyCode, event);
    			if (keyCode == KeyMap.KEYCODE_MTKIR_MUTE) {
    				return true;
    			}
    		}	
    		if (keyCode == KeyMap.KEYCODE_VOLUME_UP
    				|| keyCode == KeyMap.KEYCODE_VOLUME_DOWN) {
    			return true;
    		}
    		
        }
        return false;
    } 
    @Override
    public  void dismiss() {
        // TODO Auto-generated method stub
    	mHandler.removeMessages(MSG_DISMISS);
    	if(sDialog!=null && sDialog.isShowing()){
    		sDialog.dismiss();
    	}
        super.dismiss();
        TurnkeyUiMainActivity.getInstance().resume();
    }
    public Handler mHandler = new Handler() {

		public void handleMessage(Message msg) {
			super.handleMessage(msg);

			switch (msg.what) {
			case MSG_DISMISS: {
				exit();
//				CheckLockSignalChannelState.getInstance(mContext)
//						.checkLockedSignStateOrHasChannel(false);
//				
				break;
			}
			default:
				break;
			}

		}

	};
	public void exit(){
		/*if(AnimationManager.getInstance().getIsAnimation()){
		AnimationManager.getInstance().channelListExitAnimation(findViewById(R.id.nav_ll_sourceList_model) , new AnimatorListenerAdapter() {
			@Override
			public void onAnimationEnd(Animator animation) {
				super.onAnimationEnd(animation);
				dismiss();
			}
		});
		}else{*/
			dismiss();
		//}
	}
	public Handler getHandler(){
		if(mHandler!=null){
			return mHandler;
		}else{
			return null;
		}
	}
	public ListView getDeviceListView(){
		if(deviceListView!=null ){
			return deviceListView;
		}
		
		return null;
	}
	public CecListviewAdapter getCecListviewAdapter() {
		return cecListviewAdapter;
	}
}
