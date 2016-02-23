package com.mediatek.ui.cec;

import android.app.Dialog;
import android.content.Context;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ListView;

import android.os.Handler;
import android.os.Message;
import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.cec.CecListviewAdapter.CecDataItem;
import com.mediatek.ui.cec.CecListviewAdapter.CecDataItem.DataType;
import com.mediatek.ui.cec.CecMainDialog.ListListener;
import com.mediatek.ui.menu.commonview.RespondedKeyEvent;
import com.mediatek.ui.menu.commonview.ToastDialog;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

import java.util.List;

public class CecSecondDialog extends Dialog implements android.view.View.OnKeyListener{
    private float wScale = 0.2f;
    private float hScale = 0.3f;
    
    private ListView configListView;
    private Context mContext;
    private RespondedKeyEvent selectedView;
    
    private CecDataItem selectedItem;
    private int selectedPosition;
    
    public static final int MSG_DISMISS = 1;
  	public static final int MSG_DISMISS_DELAY = 10000;

    public CecSecondDialog(Context context) {
        super(context, R.style.Theme_TurnkeyCommDialog);
        // TODO Auto-generated constructor stub
        setContentView(R.layout.cec_dialog_second);
        mContext = context;
        this.getWindow().setLayout((int) (CecManager.tvWidth * wScale),(int) (CecManager.tvHeight* hScale));
        setPositon(0,200);
        initViews();
    }
    
    private void initViews(){
        configListView = (ListView)findViewById(R.id.cec_second_list);
    }
    
    public void setAdapter(List<CecListviewAdapter.CecDataItem> dataGroup){
       
        configListView.setAdapter(new CecListviewAdapter(mContext,dataGroup));
        configListView.setOnKeyListener(this);
//        configListView.setOnKeyListener(this);
    }
    
    public void setPositon(int xoff, int yoff) {
        Window window = this.getWindow();
        WindowManager.LayoutParams lp = window.getAttributes();
        lp.x = xoff ;//* ScreenConstant.SCREEN_WIDTH/CecManager.tvWidth;
        lp.y = yoff;//*ScreenConstant.SCREEN_HEIGHT/CecManager.tvHeight;
        window.setAttributes(lp);
    }

    @Override
    public boolean onKey(View v, int keyCode, KeyEvent event) {
    	CecManager.getInstance().removeMessage();
        mHandler.removeMessages(MSG_DISMISS);
		mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);
        if (event.getAction()==KeyEvent.ACTION_DOWN) {
            selectedPosition = configListView.getSelectedItemPosition();
            selectedItem = (CecDataItem)configListView.getItemAtPosition(selectedPosition);
            selectedView = (RespondedKeyEvent)configListView.getSelectedView();
            if (selectedView!=null) {
                selectedView = (RespondedKeyEvent) configListView
                        .getChildAt(configListView.getSelectedItemPosition());
            }
            switch (keyCode) {
                case KeyMap.KEYCODE_DPAD_CENTER:
                case KeyMap.KEYCODE_DPAD_RIGHT:
                    if (selectedItem.mItemID.endsWith(CecManager.CEC_Standby_ID)) {
                        CecManager.getInstance().setStandby();
                    }
                    if (selectedItem.mItemID.endsWith(CecManager.CEC_Poweron_ID)) {
                        CecManager.getInstance().setPowerOn();
                    }
                    if (selectedItem.mItemID.endsWith(CecManager.CEC_Able_ID)) {
                        selectedView.onKeyEnter();
                    }
                    if (selectedItem.mItemID.equals(MenuConfigManager.CEC_DEVICE_DISCOVERY)) {
                        CecManager.getInstance().discoveryDevice();
                    }
                    if (keyCode==KeyMap.KEYCODE_DPAD_RIGHT) {
                        if (selectedItem.getDataType().equals(DataType.OPTIONVIEW)) {
                            selectedView.onKeyRight();
                        }
                    }
                    break;
                case KeyMap.KEYCODE_DPAD_LEFT:
                  if (selectedItem.getDataType().equals(DataType.OPTIONVIEW)) {
                      selectedView.onKeyLeft();
                  }
                  break;
                case KeyMap.KEYCODE_BACK:
                    dismiss();
                    break;
                case KeyMap.KEYCODE_MTKIR_TITLEPBC:
                case KeyMap.KEYCODE_VOLUME_UP:
                case KeyMap.KEYCODE_VOLUME_DOWN:
                    dismiss();
                    CecManager.getInstance().onKeyDown(KeyMap.KEYCODE_BACK);
                    break;
                default:
                    break;
            }
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
		
        return false;
    }
    private Handler mHandler = new Handler() {

		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			switch (msg.what) {
			case MSG_DISMISS: {
				 dismiss();
				 mHandler.removeMessages(MSG_DISMISS);
                // CecManager.getInstance().onKeyDown(KeyMap.KEYCODE_BACK);
				break;
			}
			default:
				break;
			}

		}

	};
}
