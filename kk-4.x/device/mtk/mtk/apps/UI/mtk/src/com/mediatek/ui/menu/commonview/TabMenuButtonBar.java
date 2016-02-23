package com.mediatek.ui.menu.commonview;

import com.mediatek.ui.R;
import android.app.Activity;
import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;

/**
 * This LinearLayout includes six buttons, it is on the top of main xml. The
 * first one is show the current button.you can press LEFT or RIGHT key to
 * change the focus of button
 * 
 * @author hs_yaoxiao
 */

public class TabMenuButtonBar extends LinearLayout {

	public Button mButtonShow;
	public Button mButtonVideo;
	public Button mButtonAudio;
	public Button mButtonTV;
	public Button mButtonSetup;
	public Button mButtonParental;

	public TabMenuButtonBar(Context context) {
		super(context);
	}

	public TabMenuButtonBar(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	public void onFinishInflate() {
		super.onFinishInflate();
		((Activity) getContext()).getLayoutInflater().inflate(
				R.layout.menu_tab_menu_button, this);
		mButtonShow = (Button) findViewById(R.id.widget_buttonShow);
		mButtonVideo = (Button) findViewById(R.id.widget_buttonVideo);
		mButtonAudio = (Button) findViewById(R.id.widget_buttonAudio);
		mButtonTV = (Button) findViewById(R.id.widget_buttonTV);
		mButtonSetup = (Button) findViewById(R.id.widget_buttonSetUp);
		mButtonParental = (Button) findViewById(R.id.widget_buttonParental);
	}
	public void tvSourceTabChanngeGone(){
		mButtonSetup.setVisibility(View.GONE);
		mButtonTV.setNextFocusRightId(R.id.widget_buttonParental);
		mButtonParental.setNextFocusLeftId(R.id.widget_buttonTV);
	}
	public void tvSourceTabChanngeVisible(){
		mButtonSetup.setVisibility(View.VISIBLE);
		mButtonSetup.setNextFocusLeftId(R.id.widget_buttonTV);
		mButtonSetup.setNextFocusRightId(R.id.widget_buttonParental);
		mButtonParental.setNextFocusLeftId(R.id.widget_buttonSetUp);
		mButtonTV.setNextFocusRightId(R.id.widget_buttonSetUp);
		
	}

}
