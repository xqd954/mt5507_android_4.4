package com.mediatek.ui.menu.commonview;

import android.app.Activity;
import android.content.Context;
import android.util.AttributeSet;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.mediatek.ui.R;

public class ChannelMenuView extends LinearLayout {
	private TextView text1, text2, text3, text4, text5, text6;
	private Context mContext;
    public ChannelMenuView(Context context) {
		super(context);
		this.mContext =context;
	}
    
	public ChannelMenuView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.mContext =context;
//	    requestFocus(View.FOCUS_RIGHT);
	}
	
	protected void onFinishInflate() {
		super.onFinishInflate();
		((Activity)getContext()).getLayoutInflater().inflate(R.layout.menu_channelpage_head, this);
		
		text1 = (TextView)findViewById(R.id.common_menuview_text1);
		text2 = (TextView)findViewById(R.id.common_menuview_text2);
		text3 = (TextView)findViewById(R.id.common_menuview_text3);
		text4 = (TextView)findViewById(R.id.common_menuview_text4);
		text5 = (TextView)findViewById(R.id.common_menuview_text5);
		text6 = (TextView)findViewById(R.id.common_menuview_text6);
	}

}
