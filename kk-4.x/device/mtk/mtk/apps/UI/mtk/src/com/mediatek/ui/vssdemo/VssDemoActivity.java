package com.mediatek.ui.vssdemo;

import com.mediatek.tvcm.TVInputManager;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.menu.MenuMain;
import com.mediatek.ui.nav.NavIntegration;
import com.opengl.jni.GLJniLib;

import com.mediatek.ui.util.DestroyApp;
import com.mediatek.ui.util.KeyMap;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import com.opengl.jni.GLJniLib;
import com.mediatek.tvcommon.TVCommonManager;
import com.mediatek.tvcommon.TVInputCommon;
import com.mediatek.tvcommon.TVInputCommon.TVInputSourceListener;

public class VssDemoActivity extends Activity{
	
	public VssDemoView mVssDemoView;
	private TVCommonManager tvCommon;
	
	public 
	@Override
	void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		
		mVssDemoView = new VssDemoView(this);
		setContentView(mVssDemoView);
		tvCommon = TVCommonManager.getInstance(this);
		tvCommon.changeInputSource("main",tvCommon.getCurrentInputSource());
	}
	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		super.onPause();
		//if (SystemProperties.getInt("mtk.vss.demo", 0) == 1) {
		if(mVssDemoView.bOnVssDemo){
			mVssDemoView.mRenderer.onPause();
			mVssDemoView.bOnVssDemo = false;
			Log.i("VssDemo","mRenderer.onPause()....");
			//finish();
		}
		//	GLJniLib.nativeUnInit();
		//}	
	}
	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
		this.mVssDemoView.onDraw();
		
	}
	@Override
	protected void onStop() {
		// TODO Auto-generated method stub
		super.onStop();
		GLJniLib.nativeUnInit();
		Log.i("VssDemo","mRenderer.onStop()....");
	}
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		if((KeyEvent.KEYCODE_BACK == event.getKeyCode()||KeyMap.KEYCODE_MTKIR_GREEN == event.getKeyCode())&&// 
				   KeyEvent.ACTION_DOWN == event.getAction()){
					if(mVssDemoView.bOnVssDemo){
						mVssDemoView.mRenderer.onPause();
						mVssDemoView.bOnVssDemo = false;
					//	finish();
						Log.i("VssDemo","mRenderer.onKeyDown()....====");
						Intent intent = new Intent(VssDemoActivity.this,
								TurnkeyUiMainActivity.class);
						intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
						startActivityForResult(intent, 0);
						return true;
						//finish();
					}else{
						Log.i("VssDemo","Do nothing for Keycode_MTKIR_Green Key...");
						return false;
					}
					
				}
				return mVssDemoView.mRenderer.setKeyEvent(event);
	}
	
	
	
}



class VssDemoView extends GLSurfaceView {
	public GLRenderer mRenderer;
	public boolean bOnVssDemo = false;
    public VssDemoView(Context context) {
        super(context);
        mRenderer = new GLRenderer(context);
        setRenderer(mRenderer);
    }
    
    public void onDraw(){
    	mRenderer.onDraw();
		bOnVssDemo = true;
		Log.i("VssDemo","set the bOnVssDemo to true");
    }
   
}
