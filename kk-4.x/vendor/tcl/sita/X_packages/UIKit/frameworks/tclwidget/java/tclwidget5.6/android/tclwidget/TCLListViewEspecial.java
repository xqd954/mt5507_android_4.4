package android.tclwidget;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.ListView;

public class TCLListViewEspecial extends ListView{
    public TCLListViewEspecial(Context context) {
        this(context, null);
    }

    public TCLListViewEspecial(Context context, AttributeSet attrs) {
        this(context, attrs, com.android.internal.R.attr.tclListViewEspecialStyle);
    }

    public TCLListViewEspecial(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }
}
