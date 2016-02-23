package com.mediatek.ui.menu.commonview;

import com.mediatek.ui.R;
import com.mediatek.ui.menu.util.TVCommonInterface;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.widget.LinearLayout;
import android.widget.TextView;

/**
 * setup_version_info
 * 
 * @author hs_haosun
 * 
 */
public class VersionInfoView extends LinearLayout {

	private TextView modelNameShow, versionShow, serialNumShow;

	private TVCommonInterface comInter;

	private Context context;
	private LayoutInflater mInflater;

	public VersionInfoView(Context context) {
		super(context);
		this.context = context;
		init();

	}

	public VersionInfoView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		mInflater = LayoutInflater.from(context);
		mInflater.inflate(R.layout.menu_version_info, this);
		init();
	}

	public void init() {
		comInter = TVCommonInterface.getInstance(context);

		modelNameShow = (TextView) findViewById(R.id.common_versioninfo_name_r);
		versionShow = (TextView) findViewById(R.id.common_versioninfo_ver_r);
		serialNumShow = (TextView) findViewById(R.id.common_versioninfo_num_r);

		setValue();

	}

	/**
	 * read Model Name, Version and Serial Number from common logic
	 */
	public void setValue() {
		modelNameShow.setText(comInter.getVerModelName());
		versionShow.setText(comInter.getVer());
		serialNumShow.setText(comInter.getVerSerialNum());
	}

}
