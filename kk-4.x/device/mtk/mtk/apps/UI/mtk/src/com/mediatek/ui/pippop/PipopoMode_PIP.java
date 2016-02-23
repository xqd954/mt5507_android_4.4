/**   
 * @Description: TODO()
 * 
 * @date 2012-10-16 ����5:04:47  
 */
package  com.mediatek.ui.pippop;

import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;

/**
 * PIP mode.
 * 
 */
public class PipopoMode_PIP extends PippopModeBase implements PippopMode {

//	private boolean isBigger = false;
	private Size mSize=Size.SMALL;
	private boolean subIsFocused = false;

	private SubWinLocation[] mLocation = { SubWinLocation.RIGHT_BOTTOM,
			SubWinLocation.CENTER, SubWinLocation.RIGHT_TOP,
			SubWinLocation.LEFT_TOP, SubWinLocation.LEFT_BOTTOM, };

	private int mLocationIndex = 0;

	public PipopoMode_PIP(PippopState pippopState) {
		super(pippopState);

		showMainFullScreen();
		showSubATRight_Bottom();

		modifyMainOutput();
		modifySubOutput();

		mState.getmController().setSubHasFocus(false);
		// initOutput();
//		modifySubOutput();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.mediatek.pippopdemo.test.PippopMode#doAction_PIPPOP()
	 */
	@Override
	public boolean doAction_PIP_POP() {
		// TODO Auto-generated method stub

		if (mState.getmController().getmFocusLabel() != null
				&& (mState.getmController().getmFocusLabel().getVisibility() != View.VISIBLE)) {
			mState.getmController().reDrawFocus();
		} else {
			mState.getmController().hiddenAllTVViews();
			mState.getmController().getmLogic().changeOutputMode();
			mState.mNavSundryImplement.setScreenModeForPOP();
			mState.setState(new PippopMode_POP(mState));
			
			modifySubOutput();
		}

		// mState.getmController().releaseFocus();
		// setFocusASDefault();
		// mState.getmController().getmLogic().setOutputFocus(Core.MAIN);

		return true;
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
		// TODO swap the source main:sub
		mController.getmLogic().swapInputSource();
		// mState.releaseFocus();

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
		// mLocation[mLocationIndex];
		mLocationIndex = (mLocationIndex + 1) % mLocation.length;

		SubWinLocation location = mLocation[mLocationIndex];
		switch (location) {
		case RIGHT_TOP:
			showSubATRight_Top();
			break;
		case LEFT_TOP:
			showSubATLeft_Top();
			break;
		case LEFT_BOTTOM:
			showSubATLeft_Bottom();
			break;
		case RIGHT_BOTTOM:
			showSubATRight_Bottom();
			break;
		case CENTER:
			showSubATCenter();
			break;

		default:
			break;
		}

		modifySubOutput();
		refreshFocus();
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

//		if (isBigger) {
//			width = (int) (screenW * Core.SCALE_SMALL);
//			height = (int) (screenH * Core.SCALE_SMALL);
//		} else {
//			width = (int) (screenW * Core.SCALE_MEDIUM);
//			height = (int) (screenH * Core.SCALE_MEDIUM);
//		}
//		isBigger = !isBigger;
		
		switch(mSize){
		case SMALL:
			width = (int) (screenW * Core.SCALE_MEDIUM);
			height = (int) (screenH * Core.SCALE_MEDIUM);
			
			mSize=Size.MEDIUM;
			break;
		case MEDIUM:
			width = (int) (screenW * Core.SCALE_BIG);
			height = (int) (screenH * Core.SCALE_BIG);
			
			mSize=Size.BIG;
			break;
		case BIG:
			width = (int) (screenW * Core.SCALE_SMALL);
			height = (int) (screenH * Core.SCALE_SMALL);
			
			mSize=Size.SMALL;
			break;
		default:
			width = (int) (screenW * Core.SCALE_SMALL);
			height = (int) (screenH * Core.SCALE_SMALL);
			
			mSize=Size.SMALL;
			break;
		}
		
		mSubTVwindow.setWindowSize(width, height);

		updateLocation();
		refreshFocus();
		return true;
	}

	/**
	 * 
	 */
	private void updateLocation() {
		// TODO Auto-generated method stub
		SubWinLocation location = mLocation[mLocationIndex];
		switch (location) {
		case RIGHT_TOP:
			mSubTVwindow.setTVLocation(new float[] { screenW - offsetW - width,
					offsetH });
			break;
		case LEFT_TOP:
			mSubTVwindow.setTVLocation(new float[] { offsetW, offsetH });
			break;
		case LEFT_BOTTOM:
			mSubTVwindow.setTVLocation(new float[] { offsetW,
					screenH - offsetH - height });
			break;
		case RIGHT_BOTTOM:
			mSubTVwindow.setTVLocation(new float[] { screenW - offsetW - width,
					screenH - offsetH - height });
			break;
		case CENTER:
			mSubTVwindow.setTVLocation(new float[] { screenW / 2 - width / 2,
					screenH / 2 - height / 2 });
			break;
		default:
			break;
		}
		modifySubOutput();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.mediatek.pippopdemo.test.PippopMode#doAction_TIMEOUT()
	 */
	@Override
	public void doAction_TIMEOUT() {
		// TODO Auto-generated method stub
		mState.setState(new PippopMode_Normal(mState));
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.mediatek.pippopdemo.test.PippopMode#changeState()
	 */
	@Override
	public void changeState() {
		// TODO Auto-generated method stub
		mState.setState(null);
	}

	/**
	 * 
	 */
	private void showMainFullScreen() {
		// TODO Auto-generated method stub
		mMainTVwindow.setFullScreen();
	}

	/**
	 * 
	 */
	private void showSubATRight_Top() {
		// TODO Auto-generated method stub
		WindowManager.LayoutParams wmParams = mSubTVwindow.getWmParams();

		wmParams.gravity = Gravity.RIGHT | Gravity.TOP;
		mSubTVwindow.setWindowSize(width, height);

		mSubTVwindow.setWindowPadding(offsetW, offsetH);

		mSubTVwindow.setTVLocation(new float[] { screenW - offsetW - width,
				offsetH });
	}

	/**
	 * 
	 */
	private void showSubATLeft_Top() {
		// TODO Auto-generated method stub
		mSubTVwindow.setWindowGravity(Gravity.LEFT | Gravity.TOP);

		mSubTVwindow.setTVLocation(new float[] { offsetW, offsetH });
	}

	/**
	 * 
	 */
	private void showSubATLeft_Bottom() {
		// TODO Auto-generated method stub
		mSubTVwindow.setWindowGravity(Gravity.LEFT | Gravity.BOTTOM);

		mSubTVwindow.setTVLocation(new float[] { offsetW,
				screenH - offsetH - height });
	}

	/**
	 * 
	 */
	private void showSubATRight_Bottom() {
		// TODO Auto-generated method stub
		mSubTVwindow.setWindowGravity(Gravity.RIGHT | Gravity.BOTTOM);
		Log.d("xiaojie","showSubATRight_Bottom()==="+width+","+height);
		mSubTVwindow.setWindowSize(width, height);  
		mSubTVwindow.setTVLocation(new float[] { screenW - offsetW - width,
				screenH - offsetH - height });
	}

	/**
	 * 
	 */
	private void showSubATCenter() {
		// TODO Auto-generated method stub
		mSubTVwindow.setWindowGravity(Gravity.CENTER);
		Log.d("xiaojie","showSubATCenter==="+width+","+height);
		mSubTVwindow.setWindowPadding(0, 0);
		mSubTVwindow.setTVLocation(new float[] { screenW / 2 - width / 2,
				screenH / 2 - height / 2 });
	}

	/**
	 * 
	 */
	private void switchFocus() {
		// TODO Auto-generated method stub
		if (mState.reShowFocus()) {
			subIsFocused = !subIsFocused;
			mState.setSubHasFocus(subIsFocused);
		}
	}

	/**
	 * 
	 */
	private void refreshFocus() {
		if (mState.getmController().getmFocusLabel() != null) {
			if (mState.reShowFocus())
				mState.setSubHasFocus(subIsFocused);
		}
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
}

enum SubWinLocation {
	RIGHT_TOP, LEFT_TOP, LEFT_BOTTOM, RIGHT_BOTTOM, CENTER;
}
enum Size {
	SMALL, MEDIUM, BIG,
}
