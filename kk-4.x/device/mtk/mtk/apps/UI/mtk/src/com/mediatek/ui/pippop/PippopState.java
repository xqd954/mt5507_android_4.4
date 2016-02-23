/**   
 * @Description: TODO()
 * 
 * @date 2012-10-16 ����5:10:11  
 */
package com.mediatek.ui.pippop;

import com.mediatek.tv.common.ConfigType;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcommon.TVConfigurer;
import com.mediatek.tvcommon.TVOptionRange;
import com.mediatek.tvcommon.TVOutputCommon;
import com.mediatek.ui.nav.util.NavSundryImplement;

import android.content.Context;
import android.graphics.RectF;
import android.os.Debug;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;

/**
 * 
 */
public class PippopState {

	private PippopMode mMode;
	private TVController mController;
	public NavSundryImplement mNavSundryImplement;
	private static PippopState insPippopState = null;

	public PippopState(Context context) {
		super();
		mController = new TVController(context);
		mNavSundryImplement = NavSundryImplement.getInsNavSundryImplement(context);

		int mode = mController.getmLogic().getCurrentMode();

		switch (mode) {

		case PippopUiLogic.MODE_NORMAL:
			setState(new PippopMode_Normal(this));
			break;
		case PippopUiLogic.MODE_PIP:
			setState(new PipopoMode_PIP(this));
			break;
		case PippopUiLogic.MODE_POP:
			setState(new PippopMode_POP(this));
			break;
		default:
			setState(new PippopMode_Normal(this));
			break;
		}

	}

	public static PippopState getPippopStateInstance(Context context){
		if(null == insPippopState){
			insPippopState = new PippopState(context);
		}
		return insPippopState;
	}
	
	public void setState(PippopMode mode) {
		this.mMode = mode;
	}

	public boolean dispatchAction(int keyCode) {

		// if (keyCode != KeyEvent.KEYCODE_DPAD_LEFT
		// && keyCode != KeyEvent.KEYCODE_DPAD_RIGHT) {
		// releaseFocus();
		// mMode.setFocusASDefault();
		// }
		if (checkFocus(keyCode)) {
			return true;
		}

		Util.showDLog("PippopState->dispatchAction(int keycode): " + keyCode);
		switch (keyCode) {
		case KeyEvent.KEYCODE_MTKIR_PIPPOP:
			return mMode.doAction_PIP_POP();

		case KeyEvent.KEYCODE_MTKIR_PIPSIZE:
			return mMode.doAction_PIP_SIZE();

		case KeyEvent.KEYCODE_MTKIR_POPPOS:
			return mMode.doAction_PIP_POS();

		case KeyEvent.KEYCODE_DPAD_LEFT:
			return mMode.doAction_LEFT();

		case KeyEvent.KEYCODE_DPAD_RIGHT:
			return mMode.doAction_RIGHT();

		case KeyEvent.KEYCODE_MTKIR_SWAP:
			return mMode.doAction_SWAP();
		default:
			break;
		}

		return false;
	}

	private void debugRect() {
		TVOutputCommon common = TVOutputCommon.getInstance(null);

		RectF rect = common.getScreenRectangle("main");
		Log.d("main", "rect:left" + rect.left + "top:" + rect.top + "right:"
				+ rect.right + "bottom:" + rect.bottom);
	}

	public TVController getmController() {
		return mController;
	}

	public void setmController(TVController mController) {
		this.mController = mController;
	}

	/**
	 * @param subIsFocused
	 */
	public void setSubHasFocus(boolean subIsFocused) {
		// TODO Auto-generated method stub
		mController.setSubHasFocus(subIsFocused);

		if (subIsFocused) {
			mController.getmLogic().setOutputFocus(mController.getSubSource());
		} else {
			mController.getmLogic().setOutputFocus(mController.getMainSource());
		}

		/* start PIP/POP test */
		String currOutputNameInPIPPOP = "unknown";
		currOutputNameInPIPPOP = mController.getmLogic().getOutputFocus();
		Log.i("TEST PIP/POP", "TEST currOutput: " + currOutputNameInPIPPOP);
	
		if (null != currOutputNameInPIPPOP) {
			Log.i("TEST PIP/POP", "TEST change focus to " + currOutputNameInPIPPOP);
			int ret = mController.getmLogic().changeFocusTo(currOutputNameInPIPPOP);
			Log.i("TEST PIP/POP", "TEST ret: " + ret);
		}
		/* end PIP/POP test */
	}

	/**
	 */
	public boolean reShowFocus() {
		return (mController.reDrawFocus());
	}

	/**
	 * @param subIsFocused
	 */
	public void releaseFocus() {
		// TODO Auto-generated method stub
		mController.releaseFocus();
		// mController.getmLogic().setOutputFocus(mController.getMainSource());
	}

	public void onPause() {
		mController.hiddenFocus(true);
	}

	public void onResume() {
		mController.hiddenFocus(false);
	}

	private boolean checkFocus(int keycode) {
		switch (keycode) {
		case KeyEvent.KEYCODE_MTKIR_PIPPOP:
		case KeyEvent.KEYCODE_MTKIR_PIPSIZE:
		case KeyEvent.KEYCODE_MTKIR_POPPOS:
		case KeyEvent.KEYCODE_DPAD_LEFT:
		case KeyEvent.KEYCODE_DPAD_RIGHT:
		case KeyEvent.KEYCODE_MTKIR_SWAP:
			if (mController.getmFocusLabel() != null
					&& mController.getmFocusLabel().getVisibility() != View.VISIBLE) {
				mController.getmFocusLabel().show();
				return true;
			} else {
				return false;
			}

		default:
			return false;
		}
	}
}
