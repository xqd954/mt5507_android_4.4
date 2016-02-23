package com.mediatek.ui.menu.commonview;


import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.os.Handler;
import android.os.Message;
import android.text.InputFilter;
import android.text.InputType;
import android.text.SpannableStringBuilder;
import android.text.Spanned;
import android.text.method.NumberKeyListener;
import android.text.style.ForegroundColorSpan;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.TextView.BufferType;

import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcommon.TVCAMManager;
import com.mediatek.tvcommon.TVCAMManager.CAMEnquiry;
import com.mediatek.tvcommon.TVCAMManager.CAMMenu;
import com.mediatek.tvcommon.TVCAMManager.MenuEnqUpdateListener;
import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.menu.MenuMain;
import com.mediatek.ui.menu.adapter.CIListAdapter;
import com.mediatek.ui.nav.util.CICardDelayNotification;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVChannelState.ChannelListener;

/*
 * the view for CI
 */

public class CIMainView extends LinearLayout implements ChannelListener {

	private static final String TAG = "CIMainView";
	private TextView mCiNoCard, mCiMenuName, mCiMenuSubtitle, mCiMenuBottom,
			mCiMenuTitle;
	private TextView mCiEnqTitle, mCiEnqName, mCiEnqSubtitle;
	private EditText mCiEnqInput;
	private ListView mCiMenuList;
	private LinearLayout mCiNoCardLayout, mCiMenuLayout, mCiEnqLayout;
	private Context context;
	private LayoutInflater mInflater;
	private CIListAdapter mCiAdapter;
	private String[] mCIGroup;
	private TVCAMManager mCAMManager;

	// for password
	private String mEditStr = "";// real string
	private int mCurrentIndex = 0; // index of string
	private char[] tempChar;// temp char
	private char num;// input num
	private byte length;// editText length
	private String mPreEditStr = ""; // previous string
	private RelativeLayout mLeftLayout;
	private TextView mFirstTextView;
	private TextView mSecondTextView;
	private TextView mThirdTextView;
	private TextView mLastTextView;
	private boolean mFirstShow = false;// the first enter the password eidttext
	private boolean mInputCharChange = true;// whether up or down key
	private int mEnqType = 1;// whether password or display num
	private MyMenuEnqUpdateListener enqlistener;
	public static final String ACTION_NO_SIGNAL = "NO_SIGNAL";
	private boolean isRegister=false;

	public CIMainView(Context context) {
		super(context);
		this.context = context;
		mInflater = LayoutInflater.from(context);
		init();
	}

	public CIMainView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		mInflater = LayoutInflater.from(context);
		init();
	}
	
	public class NoSignalReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {
			// TODO Auto-generated method stub
			String action = intent.getAction();			
			if(action != null && action.equalsIgnoreCase(ACTION_NO_SIGNAL)){
				((MenuMain)CIMainView.this.context).finish();
			}
		}
	}
	
	private NoSignalReceiver noSignalReceiver=new NoSignalReceiver();

	private void init() {
		mCiNoCardLayout = (LinearLayout) mInflater.inflate(
				R.layout.menu_ci_no_card, null);
		mCiMenuLayout = (LinearLayout) mInflater.inflate(R.layout.menu_ci_main,
				null);
		mCiEnqLayout = (LinearLayout) mInflater.inflate(R.layout.menu_ci_enq,
				null);
		mCiAdapter = new CIListAdapter(context);
		mCAMManager = TVContent.getInstance(context).getCAMManager();
		enqlistener = new MyMenuEnqUpdateListener();
		mCAMManager.registerMenuEnqUpdateListener(enqlistener);
//		NavIntegration.getInstance(context).iRegisteChSelectListener(
//				mChannelSelectorListener);

		mCiNoCard = (TextView) mCiNoCardLayout
				.findViewById(R.id.menu_ci_no_card);
		mCiNoCard.requestFocus();
		mCiNoCard.setOnKeyListener(mCardNameListener);

		mCiMenuTitle = (TextView) mCiMenuLayout
				.findViewById(R.id.menu_ci_main_title);
		mCiMenuName = (TextView) mCiMenuLayout
				.findViewById(R.id.menu_ci_main_name);
		mCiMenuSubtitle = (TextView) mCiMenuLayout
				.findViewById(R.id.menu_ci_main_subtitle);
		mCiMenuBottom = (TextView) mCiMenuLayout
				.findViewById(R.id.menu_ci_main_bottom);
		mCiMenuList = (ListView) mCiMenuLayout
				.findViewById(R.id.menu_ci_main_list);
		mCiMenuList.setOnKeyListener(mMenuListKeyListener);
		mCiEnqTitle = (TextView) mCiEnqLayout
				.findViewById(R.id.menu_ci_enq_title);
		mCiEnqName = (TextView) mCiEnqLayout
				.findViewById(R.id.menu_ci_enq_name);
		mCiEnqSubtitle = (TextView) mCiEnqLayout
				.findViewById(R.id.menu_ci_enq_subtitle);
		mCiEnqInput = (EditText) mCiEnqLayout
				.findViewById(R.id.menu_ci_enq_input);

		// hide keyboard
		mCiEnqInput.setInputType(InputType.TYPE_NULL);
		mCiEnqInput.setTextSize(20);
		mCiEnqInput.setOnKeyListener(mEnqInputKeyListener);
	}
	
	public void registerNosignalReceiver(){
		IntentFilter filter = new IntentFilter();
		filter.addAction(ACTION_NO_SIGNAL);		
		context.registerReceiver(noSignalReceiver, filter);
		isRegister=true;
	}
	
	public void unRegisterNosignalReceiver(){
		if(isRegister){
			context.unregisterReceiver(noSignalReceiver);
		}
	}

	// different type, show different layout
	public void showChildView(int dataType) {
		LayoutParams layoutParams = new LayoutParams(LayoutParams.FILL_PARENT,
				LayoutParams.FILL_PARENT);
		switch (dataType) {

		case MessageType.CI_DATA_TYPE_NO_CARD:
			removeAllViews();
			setFocusable(false);
			mCiNoCardLayout.setFocusable(false);
			addView(mCiNoCardLayout, layoutParams);
			MenuMain.getInstance().requestFocus();
			break;

		case MessageType.CI_DATA_TYPE_MENU:
			removeAllViews();
	//		setFocusable(false);
		//	mCiMenuLayout.setFocusable(false);
			addView(mCiMenuLayout, layoutParams);
			//MenuMain.getInstance().requestFocus();
			break;

		case MessageType.CI_DATA_TYPE_ENQ:
			removeAllViews();
			setFocusable(false);
			mCiEnqLayout.setFocusable(false);
			addView(mCiEnqLayout, layoutParams);
			MenuMain.getInstance().requestFocus();
			break;

		default:
			break;
		}
	}

	// for no card or card name
	public void showNoCardInfo(String cardName) {
		if (cardName == null) {
			cardName = "";
		}
		mCiNoCard.setText(cardName.trim());
		mFirstTextView.setVisibility(View.INVISIBLE);
		mSecondTextView.setVisibility(View.INVISIBLE);
		mLastTextView.setVisibility(View.VISIBLE);
		mLastTextView.setText(context.getString(R.string.menu_back));
		// no card
		if (cardName.trim().equals(
				context.getString(R.string.menu_setup_ci_no_card))) {
			mThirdTextView.setVisibility(View.INVISIBLE);
		} else {
			mThirdTextView.setVisibility(View.VISIBLE);
			mThirdTextView.setText(context.getString(R.string.menu_ok));
		}
	}

	// for data_menu
	public void showCiMenuInfo(String cardTitle, String cardName,
			String cardSubtitle, String cardBottom, String[] cardListData) {
		if (cardTitle == null) {
			cardTitle = "";
		}
		if (cardName == null) {
			cardName = "";
		}
		if (cardSubtitle == null) {
			cardSubtitle = "";
		}
		if (cardBottom == null) {
			cardBottom = "";
		}
		mCiMenuTitle.setText(cardTitle.trim());
		mCiMenuName.setText(cardName.trim());
		mCiMenuSubtitle.setText(cardSubtitle.trim());
		mCiMenuBottom.setText(cardBottom.trim());

		mCIGroup = cardListData;
		/*
		 * if (mCIGroup != null && mCIGroup.length > 0) { List<String> ciList =
		 * Arrays.asList(mCIGroup); mCiMenuList.initData(ciList, ciList.size());
		 * }
		 */
		mCiAdapter.setCIGroup(mCIGroup);
		mCiMenuList.setAdapter(mCiAdapter);
		mCiMenuList.setFocusable(true);
		mCiMenuList.requestFocus();
		showMenuLeftView();
	}

	// for data_enq
	private void showCiEnqInfo(String cardTitle, String cardName,
			String cardSubtitle) {
		if (cardTitle == null) {
			cardTitle = "";
		}
		if (cardName == null) {
			cardName = "";
		}
		if (cardSubtitle == null) {
			cardSubtitle = "";
		}
		mHandler.removeMessages(1);
		mCiEnqTitle.setText(cardTitle.trim());
		mCiEnqName.setText(cardName.trim());
		mCiEnqSubtitle.setText(cardSubtitle.trim());
		mCiEnqInput.setText("");
		mCiEnqInput.setTextSize(20);
		mEditStr = "";
		mCurrentIndex = 0;
		mInputCharChange = false;
		if (mCAMManager.getCamEnquiry().isBlindAns()) {
			mEnqType = 1;
			mPreEditStr = "_";
			mCiEnqInput.setText(getSpan(mPreEditStr, 0));
		} else {
			mEnqType = 0;
			mPreEditStr = "_";
			mCiEnqInput.setText(getSpan(mPreEditStr, 0));
		}
		mFirstShow = true;
		// reset text
		mCiEnqInput.setFocusable(true);
		mCiEnqInput.requestFocus();
		if ((mCAMManager.getCamEnquiry().getAnsTextLen() & (byte) 0xf0) == 0) {
			length = mCAMManager.getCamEnquiry().getAnsTextLen();
		} else {
			length = (byte) 0x0f;
		}
		mCiEnqInput
				.setFilters(new InputFilter[] { new InputFilter.LengthFilter(
						length) });

		MtkLog.d(TAG, "---------length------" + length);
		showMenuLeftFourView();
	}

	// listener for enq
	private OnKeyListener mEnqInputKeyListener = new OnKeyListener() {

		public boolean onKey(View v, int keyCode, KeyEvent event) {
			if (event.getAction() == KeyEvent.ACTION_DOWN) {

				switch (keyCode) {
				case KeyEvent.KEYCODE_DPAD_CENTER:
				case KeyEvent.KEYCODE_ENTER:
					MtkLog.d(TAG, "mCardInputKeyListener==== mEditStr"
							+ mEditStr);
					mCAMManager.answerEnquiry(mCAMManager.getCamEnquiry()
							.isBlindAns(), mEditStr);
					break;

				case KeyEvent.KEYCODE_BACK:
					mCAMManager.answerEnquiry(false, "");
					return true;

				case KeyEvent.KEYCODE_DPAD_DOWN:
					changeDown();

					break;

				case KeyEvent.KEYCODE_DPAD_UP:
					changeUp();
					break;

				case KeyEvent.KEYCODE_DPAD_LEFT:
					leftEdit();
					break;

				case KeyEvent.KEYCODE_DPAD_RIGHT:
					rightEdit();
					break;

				case KeyEvent.KEYCODE_0:
					num = '0';
					inputNum(num);
					break;

				case KeyEvent.KEYCODE_1:
					num = '1';
					inputNum(num);
					break;

				case KeyEvent.KEYCODE_2:
					num = '2';
					inputNum(num);
					break;

				case KeyEvent.KEYCODE_3:
					num = '3';
					inputNum(num);
					break;

				case KeyEvent.KEYCODE_4:
					num = '4';
					inputNum(num);
					break;

				case KeyEvent.KEYCODE_5:
					num = '5';
					inputNum(num);
					break;

				case KeyEvent.KEYCODE_6:
					num = '6';
					inputNum(num);
					break;

				case KeyEvent.KEYCODE_7:
					num = '7';
					inputNum(num);
					break;

				case KeyEvent.KEYCODE_8:
					num = '8';
					inputNum(num);
					break;

				case KeyEvent.KEYCODE_9:
					num = '9';
					inputNum(num);
					break;

				default:
					break;
				}
			}
			return false;
		}
	};

	// listener for cardListView
	private OnKeyListener mMenuListKeyListener = new OnKeyListener() {
		public boolean onKey(View v, int keyCode, KeyEvent event) {
			if (event.getAction() == KeyEvent.ACTION_DOWN) {

				int position = mCiMenuList.getSelectedItemPosition();
				switch (keyCode) {
				case KeyEvent.KEYCODE_DPAD_CENTER:
				case KeyEvent.KEYCODE_ENTER:
					if (position < 0) {
						position = 0;
					}
					mCAMManager.selectMenuItem((char) position);
					break;

				case KeyEvent.KEYCODE_BACK:
					mCAMManager.cancelCurrMenu();
					return true;

				case KeyEvent.KEYCODE_DPAD_DOWN:
					if (position == mCiMenuList.getCount() - 1) {
						position = 0;
						mCiMenuList.setSelection(position);
					}
					break;

				case KeyEvent.KEYCODE_DPAD_UP:
					if (position == 0) {
						position = mCiMenuList.getCount() - 1;
						mCiMenuList.setSelection(position);
					}
					break;
				}
			}
			return false;
		}
	};

	class MyMenuEnqUpdateListener implements MenuEnqUpdateListener {

		public void enqReceived(final CAMEnquiry enquiry) {
			if(CICardDelayNotification.getInstance().isPushToQueue()){
				CICardDelayNotification.getInstance().addDelayNotifications(new Runnable() {
					
					@Override
					public void run() {
						enqReceived(enquiry);
					}
				});
				return ;
			}
			showChildView(MessageType.CI_DATA_TYPE_ENQ);
			/*
			 * showCiEnqInfo(mCAMManager.getCamName(), mCAMManager.getCamName(),
			 * enquiry.getText());
			 */
			showCiEnqInfo(mCAMManager.getCamName(), "", enquiry.getText());
		}

		public void menuEnqClosed(final byte mmiCloseDelay) {
			MenuMain.getInstance().unRegisterNoSignal();
			if(CICardDelayNotification.getInstance().isPushToQueue()){
				CICardDelayNotification.getInstance().addDelayNotifications(new Runnable() {
					
					@Override
					public void run() {
						menuEnqClosed(mmiCloseDelay);
					}
				});
				return;
			}
			showChildView(MessageType.CI_DATA_TYPE_NO_CARD);
			showNoCardInfo(mCAMManager.getCamName());
			mCiNoCard.requestFocus();
		}

		public void menuReceived(final CAMMenu menu) {
			if(CICardDelayNotification.getInstance().isPushToQueue()){
				CICardDelayNotification.getInstance().addDelayNotifications(new DelayMMIMenuReceived() {
					
					@Override
					public void run() {
						
						menuReceived(menu);
					}
				});
				return;
			}
			showChildView(MessageType.CI_DATA_TYPE_MENU);
			showCiMenuInfo(mCAMManager.getCamName(), menu.getTitle(), menu
					.getSubtitle(), menu.getBottom(), menu.getItemList());
			//MenuMain.getInstance().requestFocus();
		}
	}
	interface DelayMMIMenuReceived extends Runnable {
	}
	public void setCIOnKyListener(){
		mCiNoCard.requestFocus();
	}

	private OnKeyListener mCardNameListener = new OnKeyListener() {

		public boolean onKey(View v, int keyCode, KeyEvent event) {
			if (event.getAction() == KeyEvent.ACTION_DOWN) {
				if (keyCode == KeyEvent.KEYCODE_ENTER
						|| keyCode == KeyEvent.KEYCODE_DPAD_CENTER) {
				
					// has card
					if (TurnkeyUiMainActivity.mCIStatus == true
							|| mCAMManager.isSlotActive() == true) {
						
						MenuMain.getInstance().registerNoSignal();
						mCAMManager.enterCAMMainMenu();
						showChildView(MessageType.CI_DATA_TYPE_MENU);

						CAMMenu mCIManager = mCAMManager.getCamMenu();
						showChildView(MessageType.CI_DATA_TYPE_MENU);

						showCiMenuInfo(mCAMManager.getCamName(), mCIManager
								.getTitle(), mCIManager.getSubtitle(),
								mCIManager.getBottom(), mCIManager
										.getItemList());

						MenuMain.mCINav = false;
					}
					return true;
				}
			}
			return false;
		}
	};

	private Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			setPassword();
		}
	};

	// handler set password
	private void setPassword() {
		if (mEnqType == 1) {
			mInputCharChange = false;
			mEditStr = mPreEditStr;

			if (mCurrentIndex <= mEditStr.length() - 1
					&& mEditStr.length() < length) {
				mCurrentIndex++;
				if (mCurrentIndex > mEditStr.length() - 1) {
					mPreEditStr = mPreEditStr + "_";
					tempChar = mPreEditStr.toCharArray();
					for (int i = 0; i < tempChar.length - 1; i++) {
						tempChar[i] = '*';
					}
					tempChar[tempChar.length - 1] = '_';
				} else {
					tempChar = mEditStr.toCharArray();
					for (int i = 0; i < mEditStr.length(); i++) {
						tempChar[i] = '*';
					}
				}

			} else if (mCurrentIndex > mEditStr.length() - 1
					&& mEditStr.length() < length) {
				mCurrentIndex++;
				if (mCurrentIndex <= length - 1) {
					mPreEditStr = mPreEditStr + "_";
					tempChar = mPreEditStr.toCharArray();
					for (int i = 0; i < tempChar.length - 1; i++) {
						tempChar[i] = '*';
					}
					tempChar[tempChar.length - 1] = '_';
				} else {
					mCurrentIndex = length - 1;
					mPreEditStr = mPreEditStr + "_";
					tempChar = mPreEditStr.toCharArray();
					for (int i = 0; i < tempChar.length; i++) {
						tempChar[i] = '*';
					}
				}
			} else {
				mCurrentIndex++;
				if (mCurrentIndex > length - 1) {
					mCurrentIndex = length - 1;
				}
				tempChar = mEditStr.toCharArray();
				for (int i = 0; i < mEditStr.length(); i++) {
					tempChar[i] = '*';
				}
			}
			mCiEnqInput
					.setText(getSpan(String.valueOf(tempChar), mCurrentIndex));
		}
	}

	// set font color
	SpannableStringBuilder getSpan(String str, int indext) {
		SpannableStringBuilder sp = new SpannableStringBuilder();
		sp.append(str);
		sp.setSpan(new ForegroundColorSpan(Color.YELLOW), indext, indext + 1,
				Spanned.SPAN_COMPOSING);
		return sp;
	}

	// press num key
	void inputNum(char num) {
		mHandler.removeMessages(1);
		mFirstShow = false;
		mCiEnqInput.setKeyListener(numberKeyListener);
		if (mCurrentIndex <= mEditStr.length() - 1) {
			tempChar = mEditStr.toCharArray();
			tempChar[mCurrentIndex] = num;
			mEditStr = String.valueOf(tempChar);
			mCurrentIndex++;
		} else {
			mEditStr = mEditStr + num;
			mCurrentIndex++;
		}
		if (mCurrentIndex >= length) {
			mCurrentIndex = length - 1;
		}
		if (mCurrentIndex <= mEditStr.length() - 1
				|| mEditStr.length() == length) {
			mPreEditStr = mEditStr;
		} else {
			mPreEditStr = mEditStr + "_";
		}
		if (mEnqType == 1) {
			tempChar = mPreEditStr.toCharArray();
			if ("_".equals(String.valueOf(mPreEditStr.charAt(mPreEditStr
					.length() - 1)))) {
				for (int i = 0; i < tempChar.length - 1; i++) {
					tempChar[i] = '*';
				}
				tempChar[tempChar.length - 1] = '_';
			} else {
				for (int i = 0; i < tempChar.length; i++) {
					tempChar[i] = '*';
				}
			}
			mCiEnqInput
					.setText(getSpan(String.valueOf(tempChar), mCurrentIndex));
		} else {
			mCiEnqInput.setText(getSpan(mPreEditStr, mCurrentIndex));
		}
		MtkLog.d(TAG, "-inputNum----------mEditStr------->" + mEditStr);
	}

	// press up key
	void changeUp() {
		char c = '.';
		if (mFirstShow) {
			c = '.';
		}
		if (mCurrentIndex <= mEditStr.length() - 1) {
			c = mPreEditStr.charAt(mCurrentIndex);
		}
		if (mCurrentIndex > mEditStr.length() - 1) {
			if (mInputCharChange == false) {
				c = '.';
			} else {
				c = mPreEditStr.charAt(mCurrentIndex);
			}
		}
		mInputCharChange = true;
		if (c != '\0')
			if ('0' <= c && c <= '9') {

				if (c == '9')
					c = 'A';
				else
					c++;

			} else if ('a' <= c && c <= 'z') {
				if (c == 'z')
					c = ' ';
				else
					c++;
			} else if ('A' <= c && c <= 'Z') {
				if (c == 'Z')
					c = 'a';
				else
					c++;
			} else {
				switch (c) {
				case ' ':
					c = '_';
					break;
				case '_':
					c = '-';
					break;
				case '-':
					c = '.';
					break;
				case '.':
					c = '@';
					break;
				case '@':
					c = '0';
					break;
				default:
					c = '-';
					break;
				}
			}
		if (mFirstShow) {
			mFirstShow = false;
			tempChar = mPreEditStr.toCharArray();
			tempChar[mCurrentIndex] = c;
			mPreEditStr = String.valueOf(tempChar);
		}
		tempChar = mPreEditStr.toCharArray();
		tempChar[mCurrentIndex] = c;
		mPreEditStr = String.valueOf(tempChar);
		if (mEnqType == 1) {
			tempChar = mPreEditStr.toCharArray();
			for (int i = 0; i < tempChar.length; i++) {
				tempChar[i] = '*';
			}
			tempChar[mCurrentIndex] = c;
			mCiEnqInput
					.setText(getSpan(String.valueOf(tempChar), mCurrentIndex));

		} else {
			SpannableStringBuilder sp = new SpannableStringBuilder();
			sp.append(mPreEditStr);
			// mEditStr =mPreEditStr;
			sp.setSpan(new ForegroundColorSpan(Color.YELLOW), mCurrentIndex,
					mCurrentIndex + 1, Spanned.SPAN_COMPOSING);
			mCiEnqInput.setText(sp, BufferType.EDITABLE);
		}
		mHandler.removeMessages(1);
		Message msg = new Message();
		if (mCAMManager.getCamEnquiry().isBlindAns()) {
			msg.what = 1;
			mHandler.sendMessageDelayed(msg, 1500);
		}
		MtkLog.d(TAG, "-------mEditStr-----up------" + mEditStr);
	}

	// press down key
	void changeDown() {
		char c = '.';
		if (mFirstShow) {
			c = '.';
		}
		if (mCurrentIndex <= mEditStr.length() - 1) {
			c = mPreEditStr.charAt(mCurrentIndex);
		}
		if (mCurrentIndex > mEditStr.length() - 1) {
			if (mInputCharChange == false) {
				c = '.';
			} else {
				c = mPreEditStr.charAt(mCurrentIndex);
			}
		}
		mInputCharChange = true;
		if ('0' <= c && c <= '9') {

			if (c == '0')
				c = '@';
			else
				c--;

		} else if ('a' <= c && c <= 'z') {
			if (c == 'a')
				c = 'Z';
			else
				c--;
		} else if ('A' <= c && c <= 'Z') {
			if (c == 'A')
				c = '9';
			else
				c--;
		} else {
			switch (c) {
			case '@':
				c = '.';
				break;
			case '.':
				c = '-';
				break;
			case '-':
				c = '_';
				break;
			case '_':
				c = ' ';
				break;
			case ' ':
				c = 'z';
				break;
			default:
				c = '@';
				break;
			}
		}
		if (mFirstShow) {
			mFirstShow = false;
			tempChar = mPreEditStr.toCharArray();
			tempChar[mCurrentIndex] = c;
			mPreEditStr = String.valueOf(tempChar);
		}
		tempChar = mPreEditStr.toCharArray();
		tempChar[mCurrentIndex] = c;
		mPreEditStr = String.valueOf(tempChar);
		if (mEnqType == 1) {
			tempChar = mPreEditStr.toCharArray();
			for (int i = 0; i < tempChar.length; i++) {
				tempChar[i] = '*';
			}
			tempChar[mCurrentIndex] = c;
			mCiEnqInput
					.setText(getSpan(String.valueOf(tempChar), mCurrentIndex));

		} else {
			SpannableStringBuilder sp = new SpannableStringBuilder();
			sp.append(mPreEditStr);
			// mEditStr =mPreEditStr;
			sp.setSpan(new ForegroundColorSpan(Color.YELLOW), mCurrentIndex,
					mCurrentIndex + 1, Spanned.SPAN_COMPOSING);
			mCiEnqInput.setText(sp, BufferType.EDITABLE);
		}
		mHandler.removeMessages(1);
		Message msg = new Message();
		if (mCAMManager.getCamEnquiry().isBlindAns()) {
			msg.what = 1;
			mHandler.sendMessageDelayed(msg, 1500);
		}
	}

	// press left key
	void leftEdit() {
		mHandler.removeMessages(1);
		if (mFirstShow == true)
			mFirstShow = false;

		if (mCurrentIndex > 0) {
			if (mInputCharChange == false
					&& "_".equals(String.valueOf(mPreEditStr.charAt(mPreEditStr
							.length() - 1)))
					&& mEditStr.length() < mPreEditStr.length()) {
				mPreEditStr = mPreEditStr
						.substring(0, mPreEditStr.length() - 1);
			}
			mEditStr = mPreEditStr;
			mInputCharChange = false;
			mCurrentIndex--;

		} else if (mCurrentIndex == 0 && mInputCharChange == true) {
			mInputCharChange = false;
			mEditStr = mPreEditStr;
		}
		if (mEnqType == 1) {
			tempChar = mPreEditStr.toCharArray();
			if ("_".equals(String.valueOf(mPreEditStr.charAt(mPreEditStr
					.length() - 1)))
					&& mEditStr.length() < mPreEditStr.length()) {
				for (int i = 0; i < tempChar.length - 1; i++) {
					tempChar[i] = '*';
				}
				tempChar[tempChar.length - 1] = '_';
			} else {
				for (int i = 0; i < tempChar.length; i++) {
					tempChar[i] = '*';
				}
			}
			mCiEnqInput
					.setText(getSpan(String.valueOf(tempChar), mCurrentIndex));
		} else {
			mCiEnqInput.setText(getSpan(mPreEditStr, mCurrentIndex));
		}

		MtkLog.d(TAG, "------Left------------mEditStr---------" + mEditStr);

	}

	// press right key
	void rightEdit() {
		mHandler.removeMessages(1);
		if (mCurrentIndex >= length) {
			mCurrentIndex = length - 1;
			return;
		}
		if (mFirstShow == true)
			mFirstShow = false;
		if (mCurrentIndex <= mEditStr.length() - 1) {
			mEditStr = mPreEditStr;
			mCurrentIndex++;
			mInputCharChange = false;
			if ((mCurrentIndex > mEditStr.length() - 1 && mCurrentIndex < length)) {
				mPreEditStr = mPreEditStr + "_";
			}
		} else {
			if (mCurrentIndex > mEditStr.length() - 1
					&& mInputCharChange == true) {
				mInputCharChange = false;
				mCurrentIndex++;
				mEditStr = mPreEditStr;
				if (mCurrentIndex <= length - 1) {
					mPreEditStr = mPreEditStr + "_";
				}
			}
		}
		if (mCurrentIndex >= length) {
			mCurrentIndex = length - 1;
		}
		if (mEnqType == 1) {
			tempChar = mPreEditStr.toCharArray();
			if ("_".equals(String.valueOf(mPreEditStr.charAt(mPreEditStr
					.length() - 1)))
					&& mEditStr.length() < mPreEditStr.length()) {
				for (int i = 0; i < tempChar.length - 1; i++) {
					tempChar[i] = '*';
				}
				tempChar[tempChar.length - 1] = '_';
			} else {
				for (int i = 0; i < tempChar.length; i++) {
					tempChar[i] = '*';
				}
			}
			mCiEnqInput
					.setText(getSpan(String.valueOf(tempChar), mCurrentIndex));
		} else {
			mCiEnqInput.setText(getSpan(mPreEditStr, mCurrentIndex));
		}
		MtkLog.d(TAG, "------Right------------mEditStr---------" + mEditStr);
	}

	// numkey Listener
	NumberKeyListener numberKeyListener = new NumberKeyListener() {

		protected char[] getAcceptedChars() {
			return new char[] { 'a' };
		}

		public int getInputType() {
			return 0;
		}
	};

	// get left layout in menu
	public void getMenuLeftLayout(RelativeLayout menuLayout) {
		mLeftLayout = menuLayout;
		mFirstTextView = (TextView) mLeftLayout
				.findViewById(R.id.menumain_left_ci);
		mSecondTextView = (TextView) mLeftLayout
				.findViewById(R.id.menumain_left_enter);
		mThirdTextView = (TextView) mLeftLayout
				.findViewById(R.id.menumain_left_select);
		mLastTextView = (TextView) mLeftLayout
				.findViewById(R.id.menumain_left_exit);
	}

	// show three views in left
	private void showMenuLeftView() {
		mFirstTextView.setVisibility(View.INVISIBLE);
		mSecondTextView.setVisibility(View.VISIBLE);
		mThirdTextView.setVisibility(View.VISIBLE);
		mLastTextView.setVisibility(View.VISIBLE);
		mSecondTextView.setText(context.getString(R.string.menu_select));
		mThirdTextView.setText(context.getString(R.string.menu_cancel));
		mThirdTextView.setCompoundDrawablesWithIntrinsicBounds(getResources()
				.getDrawable(R.drawable.tk_cm_key_exit), null, null, null);
		mLastTextView.setText(context.getString(R.string.menu_ci_close));
	}

	// show four views in left
	private void showMenuLeftFourView() {
		mFirstTextView.setVisibility(View.VISIBLE);
		mSecondTextView.setVisibility(View.VISIBLE);
		mThirdTextView.setVisibility(View.VISIBLE);
		mLastTextView.setVisibility(View.VISIBLE);
		mFirstTextView.setText(context.getString(R.string.menu_select));
		mSecondTextView.setText(context.getString(R.string.menu_ok));
		mThirdTextView.setText(context.getString(R.string.menu_cancel));
		mThirdTextView.setCompoundDrawablesWithIntrinsicBounds(getResources()
				.getDrawable(R.drawable.tk_cm_key_exit), null, null, null);
		mLastTextView.setText(context.getString(R.string.menu_ci_close));
	}

	public void removeMenuEnqUpdateListener() {
		mCAMManager.removeMenuEnqUpdateListener(enqlistener);
	}

	public static String stringToAscii(String value) {
		if(value == null || value.length() == 0){
			return null;
		}
		StringBuffer sbu = new StringBuffer();
		char[] chars = value.toCharArray();
		for (int i = 0; i < chars.length; i++) {
			if (i != chars.length - 1) {
				sbu.append((int) chars[i]).append(",");
			} else {
				sbu.append((int) chars[i]);
			}
		}
		return sbu.toString();
	}
	public static String asciiToString(String value) {
		if(value == null || value.length() == 0){
			return null;
		}
		StringBuffer sbu = new StringBuffer();
		String[] chars = value.split(",");
		for (int i = 0; i < chars.length; i++) {
			sbu.append((char) Integer.parseInt(chars[i]));
		}
		return sbu.toString();
	}

//	@SuppressWarnings("unused")
//	private String removeTangledString(String value) {
//		StringBuffer sbu = new StringBuffer();
//
//		String[] chars = value.split(",");
//
//		for (int i = 0; i < chars.length; i++) {
//
//			if (!chars[i].equals("149")) {
//				sbu.append(chars[i]).append(",");
//			} else {
//				break;
//			}
//		}
//		return sbu.toString();
//	}




//	private String[] getAvailableStrings(String[] value) {
//		for (int i = 0; i < value.length; i++) {
//			value[i] = asciiToString(removeTangledString(stringToAscii(value[i])));			
//		}
//		return value;
//	}
//	private String getAvailableStrings(String value) {
//
//		value = asciiToString(removeTangledString(stringToAscii(value)));
//
//		return value;
//	}
	
	
	
	public synchronized void onChannelSignalChange(boolean hasSignal){
		if(hasSignal==false){
			MenuMain.getInstance().finish();
		}
	}

	public synchronized void onChannelScramble(boolean hasScramble, String state){
		
	}
}
