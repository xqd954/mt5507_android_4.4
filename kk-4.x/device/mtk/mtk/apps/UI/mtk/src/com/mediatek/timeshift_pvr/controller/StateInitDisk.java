/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.controller;

import android.R.bool;
import android.R.integer;
import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnDismissListener;
import android.content.DialogInterface.OnKeyListener;
import android.os.Handler;
import android.os.Message;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager.LayoutParams;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TableLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.mediatek.timeshift_pvr.manager.Core;
import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.ui.R;
import com.mediatek.ui.util.KeyMap;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.math.BigDecimal;

/**
 * 
 */
public class StateInitDisk extends StateBase{

	enum EnumWizardSteps {
		WELCOME, FORMAT, FORMAT_CONFIRM, FORMATING, SET_TSHIFT_SIZE, CREATE_TSHIFT_FILE, SPEED_TESTING, SPEED_FAST_DISKINFO, SPEED_LOW_DISKINFO
	}

	private static StateInitDisk mStateSelf;
	private Dialog mInitDisk;
	private TableLayout tipbar;
	private ListView mBtnsList;
	private LinearLayout progressLayout;
	private ProgressBar progressBar;
	private TextView percentTextView;

	StateInitDisk mDiskWizard;
	private WizardBaseState currentStep;

	private final float WIDTH_SCALE = 0.5f;
	private final float HEIGHT_SCALE = 0.5f;

	private int[] tipbarValues;
	private final int _MB = 1024 * 1024;
	private final int _GB = 1024 * _MB;

	private int tshiftSizeKB = 100 * _MB;

	private MyHandler mHandler;

	private final static int FORMAT_DISK = 1 << 0;;
	private final static int CREATE_TSHIFT_FILE = 1 << 1;;
	private final static int SPEED_TEST = 1 << 2;;
	public final static int NEXT_STEP = 1 << 3;;
	private final static int Refresh_Progress = 100;
	public final static int DISK_REMOUNT = 101;
	public final static int EXIT = 102;
	private static boolean dissMissAble = true;
	private float maxSpeed = 0.0f;
	public int progressNum;

	public class MyHandler extends Handler {
		WeakReference<Activity> mActivity;

		MyHandler(Activity activity) {
			mActivity = new WeakReference<Activity>(activity); // reserve.
		}

		public void handleMessage(Message msg) {
			if (!(getManager().getState() instanceof StateInitDisk)) {
				return;
			}
			switch (msg.what) {
			case FORMAT_DISK:
			    dissMissAble = false;
				(new Thread(new FormatDiskRunable(mStateSelf))).start();
				break;
			case CREATE_TSHIFT_FILE:
			    dissMissAble = false;
				(new Thread(new CreateTshiftFileRunnable(mStateSelf))).start();
				break;
			case SPEED_TEST:
			    dissMissAble = false;
				(new Thread(new SpeedTestRunnable(mStateSelf))).start();
				break;
			case NEXT_STEP:
			    dissMissAble = true;
				mStateSelf.getCurrentStep().nextStep();
				break;
			case Refresh_Progress:
			    setProgressValue();
			    break;
			case DISK_REMOUNT:
				getManager().restoreToDefault(mStateSelf);
				break;
			
			case EXIT:
				getManager().restoreToDefault(mStateSelf);
				break;
				
			default:
				break;
			}
			super.handleMessage(msg);
		}
	}

	public StateInitDisk(Context mContext, TimeShiftManager manager) {
		super(mContext, manager);
		setType(StatusType.INITDISK);
		mStateSelf = this;
		mDiskWizard = this;

		initView();
		mHandler = new MyHandler(mStateSelf.getActivity());
		setMaxSpeed(0.0f);
	}

	/**
	 * 
	 */
	private void initView() {
		mInitDisk = new Dialog(getActivity(), R.style.MTK_Dialog_bg);

		int height = (int) (getManager().getTVHeight() * HEIGHT_SCALE);
		int width = (int) (getManager().getTVWidth() * WIDTH_SCALE);

		mInitDisk.setContentView(R.layout.pvr_tshfit_disksetupwizard);
		LayoutParams parms = mInitDisk.getWindow().getAttributes();
		parms.height = height;
		parms.width = width;
		parms.dimAmount = 0.0f;

		Util.showDLog("parms.height: " + parms.height);
		Util.showDLog("parms.width: " + parms.width);

		mInitDisk.getWindow().setAttributes(parms);

		setListener(mInitDisk);
		mInitDisk.show();

		tipbar = (TableLayout) mInitDisk.findViewById(R.id.disk_wizard_hintbar);
		mBtnsList = (ListView) mInitDisk.findViewById(R.id.disk_wizard_op_list);
		progressLayout = (LinearLayout)mInitDisk.findViewById(R.id.disk_progress_layout);
		progressBar = (ProgressBar)mInitDisk.findViewById(R.id.disk_progress_progressbar);
		progressBar.setMax(100);
		progressBar.setProgress(0);
		percentTextView = (TextView)mInitDisk.findViewById(R.id.disk_progress_percent);
		mBtnsList.setDivider(null);
		mBtnsList.setOnKeyListener(new android.view.View.OnKeyListener(){

			@Override
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				// TODO Auto-generated method stub
				if(event.getAction()==KeyEvent.ACTION_DOWN){
					switch(keyCode){
					case KeyMap.KEYCODE_BACK:
						return exit();
					}
				}
			
				return false;
			}
		});
		mDiskWizard.setCurrentStep(new Welcome(mInitDisk));

	}
	
	public void showProgressView(){
	    if (null!=progressLayout) {
	        progressLayout.setVisibility(View.VISIBLE);
        }
	    
	}
	
	public void dissmissProgressView(){
	    progressLayout.setVisibility(View.GONE);
	}
	

	/**
	 * 
	 * @param step
	 */
	private void resetHinbarIcons(EnumWizardSteps step) {
		// hintbarValues[4]={pageup,select,pagedown,exit}

		switch (step) {
		case WELCOME:
		case SET_TSHIFT_SIZE:
			tipbarValues = new int[] { View.GONE, View.VISIBLE, View.VISIBLE,
					View.VISIBLE };
			break;
		case FORMAT:
		case FORMAT_CONFIRM:
			tipbarValues = new int[] { View.VISIBLE, View.VISIBLE,
					View.VISIBLE, View.VISIBLE };
			break;
		case FORMATING:
		case CREATE_TSHIFT_FILE:
		case SPEED_TESTING:
			tipbarValues = new int[] { View.INVISIBLE, View.INVISIBLE,
					View.INVISIBLE, View.INVISIBLE };
			break;

		case SPEED_FAST_DISKINFO:
		case SPEED_LOW_DISKINFO:
			tipbarValues = new int[] { View.INVISIBLE, View.INVISIBLE,
					View.INVISIBLE, View.VISIBLE };
			break;
		default:
			break;
		}

		ViewGroup viewG = (ViewGroup) tipbar.getChildAt(0);

		for (int i = 0; i < tipbarValues.length; i++) {
			try {
				viewG.getChildAt(i * 2).setVisibility(tipbarValues[i]);
				viewG.getChildAt(i * 2 + 1).setVisibility(tipbarValues[i]);
			} catch (Exception e) {
				Util.showELog(e.toString());
			}

		}

		Util.showDLog("tipbar class...."
				+ tipbar.getChildAt(0).getClass().toString());
	}

	/**
	 * @param initDisk
	 */
	private void setListener(Dialog initDisk) {
		if (initDisk != null) {
			initDisk.setOnDismissListener(new OnDismissListener() {

				@Override
				public void onDismiss(DialogInterface dialog) {
					getManager().restoreToDefault(mStateSelf);
				}
			});

			initDisk.setOnKeyListener(new OnKeyListener() {

				@Override
				public boolean onKey(DialogInterface dialog, int keyCode,
						KeyEvent event) {
					if (event.getAction() != KeyEvent.ACTION_DOWN) {
						return true;
					}

					if (mDiskWizard.onKeyDown(keyCode)) {
						return true;
					} else {
						return mDiskWizard.getCurrentStep().fliterKeyEvent(
								keyCode);
					}

				}
			});

		}
	}

	/*
	 * (non-Javadoc)
	 */
	@Override
	public boolean onKeyDown(int keycode) {
		// TODO Auto-generated method stub
	    switch (keycode) {
	        case KeyEvent.KEYCODE_MENU:
	            if (dissMissAble) {
	                onStop();
	                onRelease();
	                return false;
	            }else {
	                return true;
	            }
	            
		case KeyMap.KEYCODE_BACK:
			mStateSelf.getManager().restoreToDefault(mStateSelf);
			return true;
	        default:
	            break;
	        }

		return false;
	}
	private boolean exit() {
		if (dissMissAble) {
			return true;
		} else {
			return true;
		}
	}

	/*
	 * 
	 */
	@Override
	public void onResume() {
		// TODO Auto-generated method stub

	}

	/* 
	 */
	@Override
	public void onPause() {
		// TODO Auto-generated method stub
	}

	/* 
	 */
	@Override
	public void onStop() {
		// TODO Auto-generated method stub

	}

	/* 
	 */
	@Override
	public void onRelease() {
		// TODO Auto-generated method stub
		mInitDisk.setOnDismissListener(null);
		mInitDisk.dismiss();
		if(getCurrentStep()!=null){
			getCurrentStep().onlyExit();
		}
	}

	public WizardBaseState getCurrentStep() {
		return currentStep;
	}

	public void setCurrentStep(WizardBaseState mCurrentStep) {
		this.currentStep = mCurrentStep;

		resetHinbarIcons(currentStep.getLabel());
		refreshOpBtnsList(currentStep.getLabel());
	}

	public ListView getBtnsList() {
		return mBtnsList;
	}

	public void hiddenBtnsList() {
		getBtnsList().setVisibility(View.GONE);
	}

	/**
	 * @return unit: KB
	 */
	public int getTshiftKBSize() {
		return tshiftSizeKB;
	}

	/**
	 * @return unit: KB
	 */
	public void setTshiftKBSize(int mTshiftFileSize) {
		this.tshiftSizeKB = mTshiftFileSize;
		getManager().setTshiftFileSize(tshiftSizeKB);
	}

	/**
	 * 
	 */
	private void refreshOpBtnsList(EnumWizardSteps step) {
		// TODO Auto-generated method stub
		// mBtnsList
		switch (step) {
		case WELCOME:
		case FORMAT:
		case FORMAT_CONFIRM:
		case SET_TSHIFT_SIZE:
			mBtnsList.setVisibility(View.VISIBLE);
			break;

		default:
			mBtnsList.setVisibility(View.INVISIBLE);
			break;
		}
	}

	private void dumpDubuginfo() {
		LayoutParams lp = mInitDisk.getWindow().getAttributes();
		Util.showDLog("lp.width: " + lp.width);
		Util.showDLog("lp.height: " + lp.height);
	}

	void sleep() {
		try {
			Thread.sleep(10 * 1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	void sleep(int time) {
		try {
			Thread.sleep(time * 1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void enableNext() {
		getCurrentStep().mNext = true;
	}
	public void setProgressMessage(int progress){
	    progressNum = progress;
	    mHandler.sendEmptyMessage(Refresh_Progress);
	   
	}
	public void setProgressValue(){
	    progressBar.setProgress(progressNum);
        percentTextView.setText(progressNum+"%");
	}

	/**
	 * @return the maxSpeed
	 */
	public float getMaxSpeed() {
		return maxSpeed;
	}

	/**
	 * @param maxSpeed
	 *            the maxSpeed to set
	 */
	public void setMaxSpeed(float maxSpeed) {
		this.maxSpeed = maxSpeed;
		getManager().setDiskSpeed(maxSpeed);
	}
	// ================================================================
	/**
	 * include all operation of init wizard.
	 */
	interface WizardInterface {

		void previousStep();

		void nextStep();

		void exit();

		boolean fliterKeyEvent(int keyCode);

		void refreshView();

		void setFocus();

		EnumWizardSteps getLabel();

	}

	class WizardBaseState implements WizardInterface, onItemClickListener {

		Dialog wizardDialog;
		PvrLoading mHintMessage;

		private boolean autoProcess = false;
		public boolean mNext = false; // debugging: always equal true now.

		public boolean isAutoProcess() {
			return autoProcess;
		}

		public void setAutoProcess(boolean autoProcess) {
			this.autoProcess = autoProcess;
		}

		public WizardBaseState(Dialog wizardDialog) {
			super();
			Util.showDLog("class :"+getClass().getSimpleName());
			this.wizardDialog = wizardDialog;

			refreshView();
			mDiskWizard.dumpDubuginfo();
		}

		private boolean onKeyDown(int keyCode) {
			if (wizardDialog == null || !wizardDialog.isShowing()) {
				return false;
			}

			switch (keyCode) {
			case KeyEvent.KEYCODE_DPAD_RIGHT: // Page Down.
				if (!isAutoProcess())
					nextStep();
				return true;
			case KeyEvent.KEYCODE_DPAD_LEFT: // Page Up.
				if (!isAutoProcess())
					previousStep();
				return true;
			case KeyEvent.KEYCODE_BACK: // Exit.
				exit();
				return true;
			default:
				return false;
			}
		}

		/*
		 * 
		 */
		@Override
		public void previousStep() {
			// TODO Auto-generated method stub

		}

		/*
		 * 
		 */
		@Override
		public void nextStep() {
			// TODO Auto-generated method stub

		}

		/*
		 * 
		 */
		@Override
		public void exit() {
			// TODO Auto-generated method stub
			onlyExit();
			getManager().restoreToDefault(mStateSelf);
		}
		
		/*
		 * 
		 */
		public void onlyExit() {
			// TODO Auto-generated method stub
			if (wizardDialog != null) {
				wizardDialog.setOnDismissListener(null);
				wizardDialog.dismiss();
			}
		}

		/*
		 * 
		 */
		@Override
		public boolean fliterKeyEvent(int keyCode) {
			if (keyCode == KeyEvent.KEYCODE_DPAD_CENTER) {
				onCick(getBtnsList().getSelectedItemPosition());

			}
			return onKeyDown(keyCode);
		}

		/*
		 * 
		 */
		@Override
		public void refreshView() {
			// TODO Auto-generated method stub
			mHintMessage = (PvrLoading) wizardDialog
					.findViewById(R.id.disk_wizard_summary);

			try {
				mHintMessage.stopDraw();
			} catch (Exception e) {

			}
		}

		/*
		 * 
		 */
		@Override
		public void setFocus() {
			// TODO Auto-generated method stub

		}

		/*
		 * 
		 */
		@Override
		public EnumWizardSteps getLabel() {
			// TODO Auto-generated method stub
			return null;
		}

		/*
		 * 
		 */
		@Override
		public void onCick(int position) {
			// TODO Auto-generated method stub

		}

	}

	/**
	 * State: WELCOME
	 */
	class Welcome extends WizardBaseState {

		final int AUTO = 0;
		final int MANUAL = 1;

		/**
		 * @param wizardDialog
		 */
		public Welcome(Dialog wizardDialog) {
			super(wizardDialog);
			// TODO Auto-generated constructor stub
		}

		/*
		 * 
		 */
		@Override
		public void nextStep() {
			// TODO Auto-generated method stub
			int position = getBtnsList().getSelectedItemPosition();
			switch (position) {

			case MANUAL:
				mDiskWizard.setCurrentStep(new Format(mInitDisk));
				break;
			case AUTO:
			default:
				autoInitDisk();
				break;
			}
		}

		/*
		 * 
		 */
		@Override
		public void refreshView() {
			super.refreshView();
			dissmissProgressView();
			mHintMessage.setTextValue(mDiskWizard.getResource().getString(
					R.string.disk_wizard_welcome_summary));

			String[] btns = getResource().getStringArray(
					R.array.pvr_tshift_initdisk_step1);

			getBtnsList()
					.setAdapter(
							new StateInitDiskBtnAdapter(mDiskWizard
									.getActivity(), btns));

			getBtnsList().setSelection(0);
		}

		/*
		 * 
		 */
		@Override
		public void setFocus() {
			// TODO Auto-generated method stub

		}

		/*
		 * 
		 */
		@Override
		public EnumWizardSteps getLabel() {
			// TODO Auto-generated method stub
			return EnumWizardSteps.WELCOME;
		}

		/*
		 * 
		 */
		@Override
		public void onCick(int position) {
			nextStep();
		}

		private void autoInitDisk() {
			Long freeSize = mDiskWizard.getManager().getDiskFreesize(
					getManager().getPvrMP());
			String[] btns = mDiskWizard.getManager()
					.getSizeList(true, freeSize);
			Util.showDLog("auto,"+"btns:" + btns);
			if (btns != null) {
				setTshiftKBSize(1024 * 1024); // kb
				mDiskWizard.setCurrentStep(new CreateTshiftFile(mInitDisk));
			} else {
				Toast.makeText(getActivity(), "There is no enough space!",
						Toast.LENGTH_SHORT).show();
			}
		}

		@Override
		public boolean fliterKeyEvent(int keyCode) {
			// TODO Auto-generated method stub
			switch(keyCode){
			case KeyMap.KEYCODE_BACK:
				exit();
				break;
			default:
				break;
			}
			return super.fliterKeyEvent(keyCode);
		}
		
		
	}

	/**
	 * State: FORMAT
	 */
	class Format extends WizardBaseState {

		final int FORMAT = 0;
		final int SKIP = 1;

		/**
		 * @param wizardDialog
		 */
		public Format(Dialog wizardDialog) {
			super(wizardDialog);
			// TODO Auto-generated constructor stub
		}

		/*
		 * 
		 */
		@Override
		public void previousStep() {
			mDiskWizard.setCurrentStep(new Welcome(mInitDisk));
		}

		/*
		 * 
		 */
		@Override
		public void nextStep() {
			int position = getBtnsList().getSelectedItemPosition();
			switch (position) {
			case SKIP:
				mDiskWizard.setCurrentStep(new SetTshiftSize(mInitDisk));
				break;

			case FORMAT:
			default:
				mDiskWizard.setCurrentStep(new FormatConfirm(mInitDisk));
				break;
			}
		}

		/*
		 * 
		 */
		@Override
		public void refreshView() {
			super.refreshView();
			mHintMessage.setTextValue(mDiskWizard.getResource().getString(
					R.string.disk_wizard_format_summary));

			String[] btns = getResource().getStringArray(
					R.array.pvr_tshift_initdisk_step_format);

			getBtnsList()
					.setAdapter(
							new StateInitDiskBtnAdapter(mDiskWizard
									.getActivity(), btns));
		}

		/*
		 * 
		 */
		@Override
		public void setFocus() {
			// TODO Auto-generated method stub

		}

		/*
		 * 
		 */
		@Override
		public EnumWizardSteps getLabel() {
			// TODO Auto-generated method stub
			return EnumWizardSteps.FORMAT;
		}

		/*
		 * 
		 */
		@Override
		public void onCick(int position) {
			switch (position) {
			case FORMAT:
				nextStep();
				break;
			case SKIP:
				mDiskWizard.setCurrentStep(new SetTshiftSize(mInitDisk));
				break;
			default:
				break;
			}
		}
	}

	/**
	 * State: FORMAT_CONFIRM
	 */
	class FormatConfirm extends WizardBaseState {

		final int FORMAT = 0;
		final int CANCEL = 1;

		/**
		 * @param wizardDialog
		 */
		public FormatConfirm(Dialog wizardDialog) {
			super(wizardDialog);
			// TODO Auto-generated constructor stub
		}

		/*
		 * 
		 */
		@Override
		public void previousStep() {
			mDiskWizard.setCurrentStep(new Format(mInitDisk));
		}

		/*
		 * 
		 */
		@Override
		public void nextStep() {
			int position = getBtnsList().getSelectedItemPosition();

			switch (position) {

			case CANCEL:
				mDiskWizard.setCurrentStep(new SetTshiftSize(mInitDisk));
				break;
			case FORMAT:
			default:
				mDiskWizard.setCurrentStep(new Formating(mInitDisk));
				break;
			}
		}

		/*
		 * 
		 */
		@Override
		public void refreshView() {
			super.refreshView();
			mHintMessage.setTextValue(mDiskWizard.getResource().getString(
					R.string.disk_wizard_formatconfirm_summary));

			String[] btns = getResource().getStringArray(
					R.array.pvr_tshift_initdisk_step_formatconfirm);

			getBtnsList()
					.setAdapter(
							new StateInitDiskBtnAdapter(mDiskWizard
									.getActivity(), btns));

			getBtnsList().setOnItemClickListener(new OnItemClickListener() {

				@Override
				public void onItemClick(AdapterView<?> parent, View view,
						int position, long id) {
					switch (position) {
					case FORMAT:
						nextStep();
						break;
					case CANCEL:
						mDiskWizard
								.setCurrentStep(new SetTshiftSize(mInitDisk));
						break;
					default:
						break;
					}
				}
			});

		}

		/*
		 * 
		 */
		@Override
		public void setFocus() {
			// TODO Auto-generated method stub

		}

		/*
		 * 
		 */
		@Override
		public EnumWizardSteps getLabel() {
			// TODO Auto-generated method stub
			return EnumWizardSteps.FORMAT_CONFIRM;
		}

		/*
		 * 
		 */
		@Override
		public void onCick(int position) {
			nextStep();
		}
	}

	/**
	 * State: FORMATING
	 */
	class Formating extends WizardBaseState {
		/**
		 * @param wizardDialog
		 */
		public Formating(Dialog wizardDialog) {
			super(wizardDialog);
			mHandler.sendEmptyMessage(FORMAT_DISK);
		}

		/*
		 * 
		 */
		@Override
		public void previousStep() {
			mDiskWizard.setCurrentStep(new FormatConfirm(mInitDisk));

		}

		/*
		 * 
		 */
		@Override
		public void nextStep() {
			mDiskWizard.setCurrentStep(new SetTshiftSize(mInitDisk));

		}

		/*
		 * 
		 */
		@Override
		public void refreshView() {
			super.refreshView();
			mHintMessage.setTextValue(mDiskWizard.getResource().getString(
					R.string.disk_wizard_formating_summary));
			mHintMessage.drawLoading();
		}

		/*
		 * 
		 */
		@Override
		public void setFocus() {
			// TODO Auto-generated method stub

		}

		/*
		 * 
		 */
		@Override
		public EnumWizardSteps getLabel() {
			// TODO Auto-generated method stub
			return EnumWizardSteps.FORMATING;
		}
	}

	/**
	 * State: SET_TSHIFT_SIZE
	 */
	class SetTshiftSize extends WizardBaseState {

		String[] btns;
		String notEnoughSpace="No Enough Space";
		Long[] sizeArray = new Long[] { 512L, 1024L, 1536L, 2048L, 2560L, };

		/**
		 * @param wizardDialog
		 */
		public SetTshiftSize(Dialog wizardDialog) {
			super(wizardDialog);
			// TODO Auto-generated constructor stub
			mStateSelf.dissmissProgressView();
		}

		/*
		 * 
		 */
		@Override
		public void previousStep() {

		}

		/*
		 * 
		 */
		@Override
		public void nextStep() {
			mDiskWizard.setCurrentStep(new CreateTshiftFile(mInitDisk));
		}

		/*
		 * 
		 */
		@Override
		public void refreshView() {
			super.refreshView();
			mHintMessage.setTextValue(mDiskWizard.getResource().getString(
					R.string.disk_wizard_set_tshift_size_summary));
			Long freeSize = mDiskWizard.getManager().getDiskFreesize(getManager().getPvrMP());
			btns = mDiskWizard.getManager().getSizeList(false,freeSize);

			if (btns == null) {
				btns = new String[1];
				btns[0] = notEnoughSpace;
			}
			getBtnsList()
					.setAdapter(
							new StateInitDiskBtnAdapter(mDiskWizard
									.getActivity(), btns));
		}

		/*
		 * 
		 */
		@Override
		public void setFocus() {
			// TODO Auto-generated method stub

		}

		/*
		 * 
		 */
		@Override
		public EnumWizardSteps getLabel() {
			// TODO Auto-generated method stub
			return EnumWizardSteps.SET_TSHIFT_SIZE;
		}

		/*
		 * 
		 */
		@Override
		public void onCick(int position) {
			switch (position) {
			default:
				if(btns[0].equalsIgnoreCase(notEnoughSpace)){
					exit();
				}else
				{
					setTshiftKBSize((position+1)*512*1024);	//kb
					nextStep();
				}
				break;
			}
		}
	}

	/**
	 * State: CREATE_TSHIFT_FILE
	 */
	class CreateTshiftFile extends WizardBaseState {
		/**
		 * @param wizardDialog
		 */
		public CreateTshiftFile(Dialog wizardDialog) {
			super(wizardDialog);
			mNext = false;
			mHandler.sendEmptyMessage(CREATE_TSHIFT_FILE);
		}

		/*
		 * 
		 */
		@Override
		public void previousStep() {

		}

		/*
		 * 
		 */
		@Override
		public void nextStep() {
			mDiskWizard.setCurrentStep(new SpeedTesting(mInitDisk));
		}

		/*
		 * 
		 */
		@Override
		public void refreshView() {
			super.refreshView();
			showProgressView();
			setProgressMessage(0);
			mHintMessage.setTextValue(mDiskWizard.getResource().getString(
					R.string.disk_wizard_create_tshift_file_summary));

			hiddenBtnsList();
			mHintMessage.drawLoading();
		}

		/*
		 * 
		 */
		@Override
		public void setFocus() {
			// TODO Auto-generated method stub

		}

		/*
		 * 
		 */
		@Override
		public EnumWizardSteps getLabel() {
			// TODO Auto-generated method stub
			return EnumWizardSteps.CREATE_TSHIFT_FILE;
		}
	}

	/**
	 * State: SPEED_TESTING
	 */
	class SpeedTesting extends WizardBaseState {

		boolean isFastUDisk = false;

		/**
		 * @param wizardDialog
		 */
		public SpeedTesting(Dialog wizardDialog) {
			super(wizardDialog);
			mHandler.sendEmptyMessage(SPEED_TEST);
		}

		/*
		 * 
		 */
		@Override
		public void previousStep() {

		}

		/*
		 * 
		 */
		@Override
		public void nextStep() {
			Util.showDLog("getMaxSpeed():"+getMaxSpeed());
			if (getMaxSpeed() >= Core.AVERAGE_DISK_SPEED) {
				mDiskWizard.setCurrentStep(new SpeedFastDiskInfo(mInitDisk));
			} else {
				mDiskWizard.setCurrentStep(new SpeedSlowDiskInfo(mInitDisk));
			}
		}

		/*
		 * 
		 */
		@Override
		public void refreshView() {
			super.refreshView();
			mHintMessage.setTextValue(mDiskWizard.getResource().getString(
					R.string.disk_wizard_speed_testing_summary));

			hiddenBtnsList();
			mHintMessage.drawLoading();
		}

		/*
		 * 
		 */
		@Override
		public void setFocus() {
			// TODO Auto-generated method stub

		}

		/*
		 * 
		 */
		@Override
		public EnumWizardSteps getLabel() {
			// TODO Auto-generated method stub
			return EnumWizardSteps.SPEED_TESTING;
		}
	}

	/**
	 * State: SPEED_FAST_DISKINFO
	 */
	class SpeedFastDiskInfo extends WizardBaseState {
		/**
		 * @param wizardDialog
		 */
		public SpeedFastDiskInfo(Dialog wizardDialog) {
			super(wizardDialog);
			// TODO Auto-generated constructor stub
		}

		/*
		 * 
		 */
		@Override
		public void previousStep() {
			exit();
		}

		/*
		 * 
		 */
		@Override
		public void nextStep() {
			exit();
		}

		/*
		 * 
		 */
		@Override
		public void refreshView() {
			super.refreshView();
			//mHintMessage.setTextValue(mDiskWizard.getResource().getString(
			//		R.string.disk_wizard_speed_fast_disk_result_summary));

			String str = String
					.format(mDiskWizard
							.getResource()
							.getString(
									R.string.disk_wizard_speed_fast_disk_result_summary),
							getMaxSpeed());
			mHintMessage.setTextValue(str);
			hiddenBtnsList();
		}

		/*
		 * 
		 */
		@Override
		public void setFocus() {
			// TODO Auto-generated method stub

		}

		/*
		 * 
		 */
		@Override
		public EnumWizardSteps getLabel() {
			// TODO Auto-generated method stub
			return EnumWizardSteps.SPEED_FAST_DISKINFO;
		}

		@Override
		public boolean fliterKeyEvent(int keyCode) {
			// TODO Auto-generated method stub
			exit();
			return false;
		}

	}

	/**
	 * State: SPEED_LOW_DISKINFO
	 */
	class SpeedSlowDiskInfo extends WizardBaseState {
		/**
		 * @param wizardDialog
		 */
		public SpeedSlowDiskInfo(Dialog wizardDialog) {
			super(wizardDialog);
			// TODO Auto-generated constructor stub
		}

		/*
		 * 
		 */
		@Override
		public void previousStep() {
			exit();
		}

		/*
		 * 
		 */
		@Override
		public void nextStep() {
			exit();
		}

		/*
		 * 
		 */
		@Override
		public void refreshView() {
			super.refreshView();
			//mHintMessage.setTextValue(mDiskWizard.getResource().getString(
			//		R.string.disk_wizard_speed_slow_disk_result_summary));
			
			String str = String
					.format(mDiskWizard
							.getResource()
							.getString(
									R.string.disk_wizard_speed_slow_disk_result_summary),
							getMaxSpeed());
			mHintMessage.setTextValue(str);
			
			hiddenBtnsList();
		}

		/*
		 * 
		 */
		@Override
		public void setFocus() {
			// TODO Auto-generated method stub

		}

		/*
		 * 
		 */
		@Override
		public EnumWizardSteps getLabel() {
			// TODO Auto-generated method stub
			return EnumWizardSteps.SPEED_LOW_DISKINFO;
		}

		@Override
		public boolean fliterKeyEvent(int keyCode) {
			// TODO Auto-generated method stub
			exit();
			return false;
		}
	}

	public MyHandler getmHandler() {
		return mHandler;
	}

	public void setmHandler(MyHandler mHandler) {
		this.mHandler = mHandler;
	}

}

interface onItemClickListener {
	void onCick(int position);
}

class FormatDiskRunable implements Runnable {

	private StateInitDisk mState;

	public FormatDiskRunable(StateInitDisk state) {
		super();
		this.mState = state;
		mState.showProgressView();
	}

	/*
	 * 
	 */
	@Override
	public void run() {
//		mState.sleep();
	    mState.setProgressMessage(0);
		Util.fomatDisk(null);
		for (int i = 0; i < 101; i++) {
            try {
              Thread.sleep(30);
                if (i < 101) {
                    mState.setProgressMessage(i);
                }
            } catch (Exception e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
		}
		mState.getmHandler().sendEmptyMessage(StateInitDisk.NEXT_STEP);
	}

}

class CreateTshiftFileRunnable implements Runnable {
	private StateInitDisk mState;

	public CreateTshiftFileRunnable(StateInitDisk state) {
		super();
		this.mState = state;
	}

	/*
	 * 
	 */
	@Override
	public void run() {

		mState.getController().preparePvr(false, true);
		do{
			mState.sleep(2);
		}while(!mState.getCurrentStep().mNext);

		mState.getmHandler().sendEmptyMessage(StateInitDisk.NEXT_STEP);
	}

}

class SpeedTestRunnable implements Runnable {

	private StateInitDisk mState;

	public SpeedTestRunnable(StateInitDisk state) {
		super();
		this.mState = state;
	}

	/*
	 * 
	 */
	@Override
	public void run() {
//		mState.sleep(5);
//
//		float maxSpeed = Util.speedTest(mState.getManager());
//		mState.setMaxSpeed(maxSpeed);
//		mState.getmHandler().sendEmptyMessage(StateInitDisk.NEXT_STEP);
	    


        TimeShiftManager manager = mState.getManager();
        int index = (int) (Math.random() * 1000);

        if (manager.getPvrMP() == null) {
            manager.setPvrMP(manager.getDeviceList().get(0));

        }
        String path = String.format(
                manager.getPvrMP().mMountPoint + "/"
                        + "speedTest%d.dat", index);
        Util.showDLog("path:" + path);
        File testFile = new File(path);

        float maxSpeed = 0.0f;
        mState.setMaxSpeed(maxSpeed);

        Long MinTime = Long.MAX_VALUE;

        if (testFile.exists()) {
            testFile.delete();
        }
        try {
            testFile.createNewFile();
        } catch (IOException e) {
            return;
        }

        int bufferSize = 1024 * 120; // 7.7
        final float defaultCount = 300L;
        float counts = defaultCount;

        byte[] writeByte = new byte[(int) (bufferSize)];

        FileOutputStream fis = null;
        Long startTime = 0L;
        Long endTime;
        try {
            fis = new FileOutputStream(testFile);
        } catch (FileNotFoundException e1) {
            return;
        }
        startTime = System.currentTimeMillis();
        Long startTime1 = 0L;
        Long startTime2 = 0L;

        int progress = 0;
        try {

            while (counts > 0) {
                startTime1 = System.currentTimeMillis();
                fis.write(writeByte);
                startTime2 = System.currentTimeMillis();

                if (MinTime > startTime2 - startTime1
                        && (startTime2 - startTime1) > 0) {
                    MinTime = startTime2 - startTime1;
                }

                counts--;
                progress = (int) (((defaultCount - counts) / defaultCount) * 100);
                //Util.showDLog("Progress:" + progress);
                if (progress != 100) {
                    mState.setProgressMessage(progress);
                }
            }
            fis.close();
        } catch (FileNotFoundException e) {
			bufferSize=0;
        } catch (IOException e) {
			bufferSize=0;
        } finally {
            testFile.delete();
        }

        endTime = System.currentTimeMillis();

        maxSpeed = (float) (bufferSize * 1000f / MinTime / 1024 / 1024);
        maxSpeed = (new BigDecimal(maxSpeed).setScale(1,
                BigDecimal.ROUND_HALF_UP)).floatValue();

        float average = bufferSize * defaultCount * 1000L
                / (endTime - startTime) / 1024 / 1024;
        System.out.println(String.format("Speed:%3.1f MB/s ",
                average));
        
      mState.setMaxSpeed(average);
      mState.getmHandler().sendEmptyMessage(StateInitDisk.NEXT_STEP);

      mState.setProgressMessage(100);

    
	}

}