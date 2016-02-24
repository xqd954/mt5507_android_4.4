/*
 * Copyright (C) 2007 The Android Open Source Project
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


import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.res.TypedArray;
import android.database.DataSetObserver;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Spinner;


/**
 * A view that displays one child at a time and lets the user pick among them.
 * The items in the Spinner come from the associated with
 * this view.
 * 
 * @attr ref android.R.styleable#Spinner_prompt
 */
//@Widget
public class TCLSpinner extends Spinner  {
    
    private CharSequence mPrompt;
    private AlertDialog mPopup;
	private String text;

    public TCLSpinner(Context context) {
        this(context, null);
    }

    public TCLSpinner(Context context, AttributeSet attrs) {
        this(context, attrs, com.android.internal.R.attr.tclspinnerStyle);
    }

    public TCLSpinner(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
	}
 
	
}
 
