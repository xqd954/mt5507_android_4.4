package com.mediatek.ui.menu.commonview;

import android.content.Context;
import android.view.View;
import android.widget.LinearLayout;

/**
 * @usePart setup_network_network update ,implement Runnable()
 * @author hs_haosun
 * 
 */

public class NetUpdateGider implements Runnable {
	// to display the dialog
	private TurnkeyCommDialog showDialog;
	// to disappear the dialog
	private TurnkeyCommDialog dissMissDiag;
	// identify the type of dialog
	private String type = "";
	private LinearLayout layout;

	public NetUpdateGider(Context context) {
		super();
	}

	public NetUpdateGider(Context context, TurnkeyCommDialog t) {
		dissMissDiag = t;
		showDialog = new TurnkeyCommDialog(context, 4);
	}

	public NetUpdateGider(Context context, TurnkeyCommDialog t, String type,
			LinearLayout layout) {
		dissMissDiag = t;
		this.type = type;
		this.layout = layout;
	}

	public void run() {
		dissMissDiag.dismiss();
		if (type.equals("numRepeatDialog")) {
			layout.setVisibility(View.VISIBLE);
		} else {
			showDialog.show();
		}
	}

	public TurnkeyCommDialog getShowDialog() {
		return showDialog;
	}

	public void setShowDialog(TurnkeyCommDialog showDialog) {
		this.showDialog = showDialog;
	}

	public TurnkeyCommDialog getDissMissDiag() {
		return dissMissDiag;
	}

	public void setDissMissDiag(TurnkeyCommDialog dissMissDiag) {
		this.dissMissDiag = dissMissDiag;
	}

}
