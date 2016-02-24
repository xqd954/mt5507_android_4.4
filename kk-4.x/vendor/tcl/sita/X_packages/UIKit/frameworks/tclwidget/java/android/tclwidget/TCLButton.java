/*
 * Copyright (C) 2006 The Android Open Source Project
 * 
 * ©2010-2013 TCL CORPORATION All Rights Reserved.
 */

package android.tclwidget;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.KeyEvent;
import android.widget.RemoteViews.RemoteView;
import android.widget.Button;

/**
 * <p>
 * <code>Button</code> represents a push-button widget. Push-buttons can be
 * pressed, or clicked, by the user to perform an action. A typical use of a
 * push-button in an activity would be the following:
 * </p>
 *
 * <pre class="prettyprint">
 * public class MyActivity extends Activity {
 *     protected void onCreate(Bundle icicle) {
 *         super.onCreate(icicle);
 *
 *         setContentView(R.layout.content_layout_id);
 *
 *         final Button button = (Button) findViewById(R.id.button_id);
 *         button.setOnClickListener(new View.OnClickListener() {
 *             public void onClick(View v) {
 *                 // Perform action on click
 *             }
 *         });
 *     }
 * }
 * </pre>
 *
 * <p><strong>XML attributes</strong></p>
 * <p> 
 * See {@link android.R.styleable#Button Button Attributes}, 
 * {@link android.R.styleable#TextView TextView Attributes},  
 * {@link android.R.styleable#View View Attributes}
 * </p>
 */

@RemoteView
public class TCLButton extends Button {
    public TCLButton(Context context) {
        this(context, null);
    }

    public TCLButton(Context context, AttributeSet attrs) {
        this(context, attrs, com.android.internal.R.attr.tclbuttonStyle);
    }

    public TCLButton(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }
}
