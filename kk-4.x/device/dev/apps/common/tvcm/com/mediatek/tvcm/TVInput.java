package com.mediatek.tvcm;

import android.content.Context;

import com.mediatek.tvcommon.TVInputCommon;

public class TVInput {
	
	private String name;
	private Context mContext;
	private TVInputCommon mTVInputCommon;
	
	public TVInput(Context context, String name) {
		mContext = context;
		this.name = name;
		mTVInputCommon = TVInputCommon.getInstance(mContext);
	}
	
	public String getName() {
		return name;
	}
	
}
