/**   
 * @Description: TODO()
 * 
 * @date 2012-10-16 ����5:05:11  
 */
package  com.mediatek.ui.pippop;

/**
 * 
 */
public class PippopMode_POP extends PippopModeBase implements PippopMode {

	private boolean subIsFocused = false;

	public PippopMode_POP(PippopState pippopState) {
		super(pippopState);

		showPopWindow();
		initOutput();
		
//		mState.getmController().setSubHasFocus(false);
		reDrawFocus();
		
	}

	/**
	 * 
	 */
	private void showPopWindow() {
		// TODO Auto-generated method stub
		mMainTVwindow.setAsPOPLeftWindow();
		mSubTVwindow.setAsPOPRightWindow();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.mediatek.pippopdemo.test.PippopMode#doAction_PIPPOP()
	 */
	@Override
	public boolean doAction_PIP_POP() {
		// TODO Auto-generated method stub
		mState.getmController().hiddenAllTVViews();
		mState.mNavSundryImplement.setScreenModeExitPOP();
		mState.getmController().getmLogic().changeOutputMode();
		mState.getmController().releaseFocus();
		mState.setState(new PippopMode_Normal(mState));
		
		return true;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.mediatek.pippopdemo.test.PippopMode#doAction_ENTER()
	 */
	@Override
	public boolean doAction_ENTER() {
		// TODO ??

		return false;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.mediatek.pippopdemo.test.PippopMode#doAction_LEFT()
	 */
	@Override
	public boolean doAction_LEFT() {
		// TODO Auto-generated method stub
		switchFocus();

		return true;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.mediatek.pippopdemo.test.PippopMode#doAction_RIGHT()
	 */
	@Override
	public boolean doAction_RIGHT() {
		// TODO Auto-generated method stub
		switchFocus();
		return true;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.mediatek.pippopdemo.test.PippopMode#doAction_SWAP()
	 */
	@Override
	public boolean doAction_SWAP() {
		// TODO Auto-generated method stub
		mController.getmLogic().swapInputSource();
//		mState.releaseFocus();

		return true;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.mediatek.pippopdemo.test.PippopMode#doAction_PIP_POS()
	 */
	@Override
	public boolean doAction_PIP_POS() {
		// TODO Auto-generated method stub

		return true;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.mediatek.pippopdemo.test.PippopMode#doAction_PIP_SIZE()
	 */
	@Override
	public boolean doAction_PIP_SIZE() {
		// TODO Auto-generated method stub

		return true;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.mediatek.pippopdemo.test.PippopMode#doAction_TIMEOUT()
	 */
	@Override
	public void doAction_TIMEOUT() {
		// TODO Auto-generated method stub

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.mediatek.pippopdemo.test.PippopMode#changeState()
	 */
	@Override
	public void changeState() {
		// TODO Auto-generated method stub

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.mediatek.pippopdemo.test.PippopMode#setFocusASDefault()
	 */
	@Override
	public void setFocusASDefault() {
		// TODO Auto-generated method stub
		subIsFocused = false;
	}

	/**
	 * 
	 */
	private void switchFocus() {
		// TODO Auto-generated method stub
//		subIsFocused = !subIsFocused;
//
//		mState.setSubHasFocus(subIsFocused);
		if (mState.reShowFocus()) {
			subIsFocused = !subIsFocused;

			mState.setSubHasFocus(subIsFocused);
			}
	}

	private void reDrawFocus(){
		String focus =mState.getmController().getmLogic().getOutputFocus();
		
		if(focus.equalsIgnoreCase(Core.MAIN)){
			subIsFocused=false;
		}else
		{
			subIsFocused=true;
		}
		
		if(mState.getmController().getmFocusLabel()!=null){
			mState.setSubHasFocus(subIsFocused);
		}
		
	}
	
}
