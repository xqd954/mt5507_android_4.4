/*
 * Copyright (C) 2006 The Android Open Source Project
 * 
 * ©2010-2013 TCL CORPORATION All Rights Reserved.
 */

package android.tclwidget;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.CheckBox;
import android.view.ContextThemeWrapper;

/**
 * <p>
 * A checkbox is a specific type of two-states button that can be either
 * checked or unchecked. A example usage of a checkbox inside your activity
 * would be the following:
 * </p>
 *
 * <pre class="prettyprint">
 * public class MyActivity extends Activity {
 *     protected void onCreate(Bundle icicle) {
 *         super.onCreate(icicle);
 *
 *         setContentView(R.layout.content_layout_id);
 *
 *         final CheckBox checkBox = (CheckBox) findViewById(R.id.checkbox_id);
 *         if (checkBox.isChecked()) {
 *             checkBox.setChecked(false);
 *         }
 *     }
 * }
 * </pre>
 *  
 * <p><strong>XML attributes</strong></p> 
 * <p>
 * See {@link android.R.styleable#CompoundButton CompoundButton Attributes}, 
 * {@link android.R.styleable#Button Button Attributes}, 
 * {@link android.R.styleable#TextView TextView Attributes}, 
 * {@link android.R.styleable#View View Attributes}
 * </p>
 */

public class TCLCheckBox extends CheckBox {
    public TCLCheckBox(Context context) {
        this(context, null);
    }
    
    public TCLCheckBox(Context context, AttributeSet attrs) {
        this(context, attrs, tvos.R.attr.tclcheckBoxStyle);
    }

    public TCLCheckBox(Context context, AttributeSet attrs, int defStyle) {
        //super(context, attrs, defStyle);
        super(new ContextThemeWrapper(context, tvos.R.style.Theme), attrs, defStyle);
    }
}
