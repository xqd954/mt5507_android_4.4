/**   
 * @Description: TODO()
 * 
 * @date 2012-10-16 ����5:05:11  
 */
package  com.mediatek.ui.pippop;

/**
 * 
 * 
 */
public class PippopMode_Normal extends PippopModeBase implements PippopMode {

	/**
	 * @param pippopState
	 */
	public PippopMode_Normal(PippopState pippopState) {
		// TODO Auto-generated constructor stub
		super(pippopState);

		showMainFullScreen();
		initOutput();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.mediatek.pippopdemo.test.PippopMode#doAction_PIPPOP()
	 */
	@Override
	public boolean doAction_PIP_POP() {
		// TODO Auto-generated method stub

		if (supportPIP()) {
			mState.getmController().hiddenAllTVViews();

			mState.setState(new PipopoMode_PIP(mState));
			mState.getmController().getmLogic().changeOutputMode();
			return true;
		} else {
			// show no function window or not support?
			return true;
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.mediatek.pippopdemo.test.PippopMode#doAction_ENTER()
	 */
	@Override
	public boolean doAction_ENTER() {
		// TODO Auto-generated method stub

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

		return false;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.mediatek.pippopdemo.test.PippopMode#doAction_RIGHT()
	 */
	@Override
	public boolean doAction_RIGHT() {
		// TODO Auto-generated method stub

		return false;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.mediatek.pippopdemo.test.PippopMode#doAction_SWAP()
	 */
	@Override
	public boolean doAction_SWAP() {
		// TODO ??

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

	}

	/**
	 * 
	 */
	private void showMainFullScreen() {
		mState.getmController().getMainOutPutWindow().setFullScreen();

	}

	/**
	 * 
	 */
	private boolean supportPIP() {
		// TODO check if support PIP mode: inputSource, function
		// enable/disable...

		return true;
	}
}
