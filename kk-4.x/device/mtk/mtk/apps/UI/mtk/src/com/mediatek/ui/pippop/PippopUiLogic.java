package com.mediatek.ui.pippop;

import java.util.List;

import android.content.Context;
import android.graphics.RectF;

import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcommon.ITVCommon;
import com.mediatek.tvcommon.TVCommonManager;
import com.mediatek.tvcommon.TVOutputCommon;

/**
 * this class use to abstract logic from UI.
 * 
 */
public class PippopUiLogic {
	private static PippopUiLogic instance;

	public static final int MODE_NORMAL = ITVCommon.OUTPUT_MODE_NORMAL;
	public static final int MODE_PIP = ITVCommon.OUTPUT_MODE_PIP;
	public static final int MODE_POP = ITVCommon.OUTPUT_MODE_POP;

	private TVCommonManager mTVCMManager;
	private TVOutputCommon mTVoutputCM;

	private PippopUiLogic(Context context) {

		mTVCMManager = TVCommonManager.getInstance(context);
		mTVoutputCM = TVOutputCommon.getInstance(context);

	}

	public static PippopUiLogic getInstance(Context context) {
		if (instance == null) {
			instance = new PippopUiLogic(context);
		}
		return instance;
	}

	public int getCurrentMode() {
		return mTVCMManager.getCurrentOutputMode();
	}

	/**
	 * Set focus on the output, then UI do operation base on it.
	 * 
	 * @param outputName
	 *            "main" or "sub"
	 */
	public void setOutputFocus(String outputName) {
		mTVCMManager.setDefaultOutput(outputName);
		Util.showDLog("setOutputFocus: " + outputName);
	}

	public String getOutputFocus() {
		return mTVCMManager.getCurrentOutput();
	}

	/**
	 * change output mode
	 */
	public void changeOutputMode() {
		mTVCMManager.switchOutputMode();
	}

	public int changeFocusTo(String outputName) {
		int ret = mTVCMManager.changeFocusTo(outputName);
		Util.showDLog("changeFocusTo ret: " + ret);
		return ret;
	}

	/**
	 * set the output region
	 * 
	 * @param outputName
	 *            "main" or "sub"
	 * @param rect
	 */
	public void setOutputPosition(String outputName, RectF rect) {
		mTVoutputCM.setScreenRectangle(outputName, rect);
	}

	/**
	 * swap input source between main output and sub output.
	 */
	public void swapInputSource() {
		mTVCMManager.swapInputSource();
	}

	/**
	 * swap input source between main output and sub output.
	 */
	public void changeInputSource(String outputName, String sourceName) {
		mTVCMManager.changeInputSource(outputName, sourceName);
	}

	public String[] getConnectableInputsourceList() {
		return mTVCMManager.getInputSourceArray();
	}

	public List<String> getConflictInputs() {
		return mTVCMManager.getConflictInputs();
	}

	/**
	 * get main output.
	 */
	public String getMainOutputSourceName() {
		return mTVoutputCM.getInput("main");
	}

	/**
	 * get sub output.
	 */
	public String getSubOutputSourceName() {
		return mTVoutputCM.getInput("sub");
	}

	/**
	 * get output source name.
	 */
	public String getOutputSourceName(String mainOrSub) {
		return mTVoutputCM.getInput(mainOrSub);
	}

	/**
	 * current mode is PIP/POP or not
	 */
	public boolean isPipOrPopState() {
		if (MODE_PIP == getCurrentMode() || MODE_POP == getCurrentMode()) {
			return true;
		} else {
			return false;
		}
	}
	
	public boolean isPopState() {
		if (MODE_POP == getCurrentMode()) {
			return true;
		} else {
			return false;
		}
	}
	
	public boolean isPipState() {
		if (MODE_PIP == getCurrentMode()) {
			return true;
		} else {
			return false;
		}
	}
	public boolean isNormalState(){
		if(MODE_NORMAL == getCurrentMode()){
			return true;
		}else{
			return false;
		}
	}

}