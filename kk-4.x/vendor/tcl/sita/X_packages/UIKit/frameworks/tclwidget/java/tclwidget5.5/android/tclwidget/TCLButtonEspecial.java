/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.tclwidget;

import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.GradientDrawable;
import android.tclwidget.TCLButton;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.KeyEvent;
import android.view.View;
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
public class TCLButtonEspecial extends TCLButton {
    public TCLButtonEspecial(Context context) {
        this(context, null);
    }

    public TCLButtonEspecial(Context context, AttributeSet attrs) {
        this(context, attrs, com.android.internal.R.attr.tclButtonEspecialStyle);

    }

    public TCLButtonEspecial(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        init();
    }
    private void init() {
    	if(!isClickable() || !isFocusable()) {
    		setTextColor(Color.BLACK);
    	} else if(isFocusable() && hasFocus()) {
    		setTextColor(Color.WHITE);
    	} else {
    		setTextColor(Color.BLACK);
    	}
    	
    	setOnFocusChangeListener(new OnFocusChangeListener() {
			
		public void onFocusChange(View v, boolean hasFocus) {
			// TODO Auto-generated method stub
			if(hasFocus) {
				setTextColor(Color.WHITE);
			} else {
				setTextColor(Color.BLACK);
			}
		}
	});
    }
    
}
