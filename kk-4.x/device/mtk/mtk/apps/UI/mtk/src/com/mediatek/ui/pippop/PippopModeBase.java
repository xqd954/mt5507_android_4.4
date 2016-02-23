/**   
 * @Description: TODO()
 * 
 * @date 2012-10-17 ����11:55:07  
 */
package com.mediatek.ui.pippop;

/**
 * 
 */
public class PippopModeBase {

	PippopState mState;
	TVWindowView mSubTVwindow;
	TVWindowView mMainTVwindow;
	TVController mController;

	int offsetW = 0;
	int offsetH = 0;

	int screenW = 0;
	int screenH = 0;

	int width = 0;
	int height = 0;

	public PippopModeBase(PippopState pippopState) {
		super();
		mState = pippopState;
		mMainTVwindow = mState.getmController().getMainOutPutWindow();
		mSubTVwindow = mState.getmController().getSubOutPutWindow();
		initWH();

		mController = mState.getmController();
//		mState.releaseFocus();
	}

	/**
	 * 
	 */
	private void initWH() {
		// TODO Auto-generated method stub
		screenW = mSubTVwindow.getmScreenWidth();
		screenH = mSubTVwindow.getmScreenHeight();

		offsetW = mSubTVwindow.getmScreenWidth() / 16;
		offsetH = mSubTVwindow.getmScreenHeight() / 16;

		width = (int) (screenW * Core.SCALE_SMALL);
		height = (int) (screenH * Core.SCALE_SMALL);

		Util.showDLog("initWH: -----------------------------");
		Util.showDLog("screenW: " + screenW);
		Util.showDLog("screenH: " + screenH);
		Util.showDLog("offsetW: " + offsetW);
		Util.showDLog("offsetH: " + offsetH);
		Util.showDLog("width: " + width);
		Util.showDLog("height: " + height);
	}

	/**
	 * The main output window has changed,so modify the output location and
	 * size.
	 */
	protected void modifyMainOutput() {
		mController.modifyMainOutput();
	}

	/**
	 * The sub output window has changed,so modify the output location and size.
	 */
	protected void modifySubOutput() {
		mController.modifySubOutput();
	}

	/**
	 * init the output region.
	 */
	protected void initOutput() {
		modifyMainOutput();
		modifySubOutput();
	}

}
