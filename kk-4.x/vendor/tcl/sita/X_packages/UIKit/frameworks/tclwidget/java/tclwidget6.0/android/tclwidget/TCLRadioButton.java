/*
 * Copyright (C) 2006 The Android Open Source Project
 * 
 * ©2010-2013 TCL CORPORATION All Rights Reserved.
 */

package android.tclwidget;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.RadioButton;
import android.view.ContextThemeWrapper;

public class TCLRadioButton extends RadioButton {
    
    public TCLRadioButton(Context context) {
        this(context, null);
    }
    
    public TCLRadioButton(Context context, AttributeSet attrs) {
        this(context, attrs, tvos.R.attr.tclradioButtonStyle);
    }

    public TCLRadioButton(Context context, AttributeSet attrs, int defStyle) {
        //super(context, attrs, defStyle);
        super(new ContextThemeWrapper(context, tvos.R.style.Theme), attrs, defStyle);
    }

}
