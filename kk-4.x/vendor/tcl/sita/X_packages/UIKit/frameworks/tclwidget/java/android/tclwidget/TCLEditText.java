/*
 * Copyright (C) 2006 The Android Open Source Project
 * 
 * ©2010-2013 TCL CORPORATION All Rights Reserved.
 */

package android.tclwidget;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.EditText;

public class TCLEditText extends EditText{
	public TCLEditText(Context context) {
 		this(context, null);
	}

	public TCLEditText(Context context, AttributeSet attrs) {
		this(context, attrs, com.android.internal.R.attr.tcleditTextStyle);
	}
	
	public TCLEditText(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

}
