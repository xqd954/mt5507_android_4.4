package com.mediatek.ui.cec;

import android.content.Context;
import android.os.Bundle;

import com.mediatek.ui.R;

public class EPGProgressDialog extends android.app.ProgressDialog{
    
    public EPGProgressDialog(Context context, int theme) {
        super(context, theme);
        // TODO Auto-generated constructor stub
    }
    
    public EPGProgressDialog(Context mContext){
        super(mContext);
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
        setContentView(R.layout.epg_us_progressbar);
    }



}
