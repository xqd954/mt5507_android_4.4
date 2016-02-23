package com.mediatek.ui.menu.commonview;

import android.app.Activity;
import android.content.Context;
import android.util.AttributeSet;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.mediatek.ui.R;

public class ChannelMenuViewBottom extends LinearLayout {
	private TextView tSwap, tInsert, tDelete;
//	private ImageView pSwap,pInsert,pDelete;
	
	private Context mContext;
    public ChannelMenuViewBottom(Context context) {
		super(context);
		this.mContext =context;
	}
    
	public ChannelMenuViewBottom(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.mContext =context;
//	    requestFocus(View.FOCUS_RIGHT);
	}
	
	protected void onFinishInflate() {
		super.onFinishInflate();
		((Activity)getContext()).getLayoutInflater().inflate(R.layout.menu_channelpage_bottom, this);
		
		tSwap = (TextView)findViewById(R.id.common_menuview_text_swap);
		tInsert = (TextView)findViewById(R.id.common_menuview_text_insert);
		tDelete = (TextView)findViewById(R.id.common_menuview_text_delete);
//		pSwap = (ImageView)findViewById(R.id.common_menuview_pic_swap);
//		pInsert = (ImageView)findViewById(R.id.common_menuview_pic_insert);
//		pDelete = (ImageView)findViewById(R.id.common_menuview_pic_delete);
	}

}
