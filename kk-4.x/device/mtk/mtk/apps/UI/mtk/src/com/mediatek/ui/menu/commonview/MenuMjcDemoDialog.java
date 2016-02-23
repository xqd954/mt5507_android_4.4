package com.mediatek.ui.menu.commonview;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;

import com.mediatek.tv.common.ConfigType;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.util.ScreenConstant;

public class MenuMjcDemoDialog extends Dialog {

	private Context mContext;

	public MenuMjcDemoDialog(Context context) {
		super(context, R.style.Theme_ScanDialog);
		mContext = context;
	}

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.menu_mjc_demo);

		Window window = getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();
		lp.width= ScreenConstant.SCREEN_WIDTH;
		lp.height= ScreenConstant.SCREEN_HEIGHT;
		window.setAttributes(lp);
		
		initData();
	}

	private void initData() {
		final MenuConfigManager mg = MenuConfigManager.getInstance(mContext);
		int value = ConfigType.CFG_MJC_DEMO_RIGHT;
		// set demo_partition right
		mg.setValue(MenuConfigManager.DEMO_PARTITION, value);
	}

}
