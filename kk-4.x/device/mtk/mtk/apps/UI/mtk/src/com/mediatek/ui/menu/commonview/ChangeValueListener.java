package com.mediatek.ui.menu.commonview;

import android.view.View;

public interface ChangeValueListener {
	public void setValue(View mView, int vaule);

	public void setValues(View mView, int parentValue, int[] childIds,
			int vaules[]);
}
