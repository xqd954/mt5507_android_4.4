/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.ui;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.DialogInterface;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnFocusChangeListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.GridView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.mediatek.timeshift_pvr.controller.ScheduleItem;
import com.mediatek.timeshift_pvr.controller.StateBase;
import com.mediatek.timeshift_pvr.controller.StateScheduleList;
import com.mediatek.timeshift_pvr.controller.StateScheduleListCallback;
import com.mediatek.timeshift_pvr.manager.Core;
import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;

import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;

/**
 *
 */
public class ScheduleListItemInfoDialog extends CommonDialog implements
		OnItemClickListener, OnItemSelectedListener {

	private final String TAG = "ScheduleListItemInfoDialog";

	private float wScale = 0.85f;
	private float hScale = 0.99f;

	private StateScheduleListCallback<?> mCallback;
	private ListView mDataItemList;
	private GridView mWeekList;

	private TextView mDiskInfo;

	private Button mBtn1;
	private Button mBtn2;
	private Button mBtn3;

	private StateScheduleList mState;
	private LayoutInflater mInflater;

	private List<DataItem> mDataList;
	private ScheduleItem mScheItem;
	private HashMap<String, Boolean> mWeekItem = new HashMap<String, Boolean>();;

	private final String[] WEEK_ARRAY;

	private SpecialConfirmDialog confirm;

	enum BtnTypeEnum {
		ADD_CANCEL, EDIT_DELETE_CANCEL, REPLACE_ADD_CANCEL
	}

	private final int UI_Index_SrcType = 0;
	private final int UI_Index_ChannelNum = 1;
	private final int UI_Index_StartDate = 2;
	private final int UI_Index_StartTime = 3;
	private final int UI_Index_StopTime = 4;
	private final int UI_Index_ScheduleType = 5;
	private final int UI_Index_RepeatType = 6;

	private View.OnClickListener addListener;
	private View.OnClickListener replaceListener;
	private View.OnClickListener cancelListener;
	private View.OnClickListener deleteListener;
	private View.OnClickListener editListener;

	private int temp_count = 20;

	private BtnTypeEnum btnListType = BtnTypeEnum.ADD_CANCEL;

	/**
	 * @param context
	 * @param layoutID
	 */
	public ScheduleListItemInfoDialog(Context context, StateBase state,
			ScheduleItem item) {
		super(context, R.layout.pvr_tshfit_schudulelist_item);

		getWindow().setLayout((int) (state.getManager().getTVWidth() * wScale),
				(int) (state.getManager().getTVHeight() * hScale));

		this.mState = (StateScheduleList) state;
		setScheItem(item);
		this.mWeekItem = prepareWeekItem(getScheItem());

		WEEK_ARRAY = mState.getResource().getStringArray(R.array.week_day);

		mInflater = (LayoutInflater) mContext
				.getSystemService(Context.LAYOUT_INFLATER_SERVICE);

		initView2();
	}

	@Override
	public void dismiss() {
		// TODO Auto-generated method stub
		if (confirm != null&&confirm.isShowing()) {
			confirm.dismiss();
		}

		super.dismiss();
	}

	/**
	 * @param item
	 * @return
	 */
	private HashMap<String, Boolean> prepareWeekItem(ScheduleItem item) {
		HashMap<String, Boolean> mapList = new HashMap<String, Boolean>();
		int repeat = item.getWeeklyRepeat();
		String[] array = mState.getResource().getStringArray(R.array.week_day);

		if (item.getRepeatType() == item.REPEAT_TYPE_WEEK) {
			for (int i = 0; i < 7; i++) {
				if (Math.pow(2, i) == (repeat & ((int) Math.pow(2, i)))) {
					mapList.put(array[i], true);
				} else {
					mapList.put(array[i], false);
				}
			}
		}

		return mapList;
	}

	@Override
	public void initView() {
		// TODO Auto-generated method stub
		super.initView();
		mDataItemList = (ListView) findViewById(R.id.schedulelist_item_list);
		mDataItemList.setDivider(null);

		mDiskInfo = (TextView) findViewById(R.id.schedulelist_item_title_diskinfo);

		mWeekList = (GridView) findViewById(R.id.week_gridview);

		mBtn1 = (Button) findViewById(R.id.schedulelist_item_btn_first);
		mBtn2 = (Button) findViewById(R.id.schedulelist_item_btn_second);
		mBtn3 = (Button) findViewById(R.id.schedulelist_item_btn_third);

		initListener();
	}
	android.os.Handler handler = new android.os.Handler(){
	    public void handleMessage(android.os.Message msg) {
	        
	        switch (msg.what) {
                case 111:
                    mBtn2.setFocusable(true);
                    mBtn2.requestFocus();
                    mBtn2.setSelected(true);
                    break;
                case 222:
                    mBtn1.setFocusable(false);
                    mBtn1.clearFocus();
                    mBtn1.setSelected(false);
                    mBtn3.setFocusable(false);
                    mBtn3.clearFocus();
                    mBtn3.setSelected(false);
                    break;  
                case 333:
                    mBtn3.setFocusable(true);
                    mBtn1.setFocusable(true);
                    mBtn1.requestFocus();
                    mBtn1.setSelected(true);
                    break;
                default:
                    break;
            }
	    };
	};
	@SuppressLint("NewApi")
	private void initView2() {
		SetConfigListViewAdapter infoAdapter = new SetConfigListViewAdapter(
				mState.getActivity());
		mDataList = initItemList();

		infoAdapter.setmGroup(mDataList);
		mDataItemList.setAdapter(infoAdapter);
		mDataItemList.setOnItemClickListener(this);
		mDataItemList.setOnItemSelectedListener(this);
		mWeekList.setChoiceMode(GridView.CHOICE_MODE_MULTIPLE_MODAL);
		mWeekList.setOnItemClickListener(this);
		mWeekList.setAdapter(new WeekListAdapter(mWeekItem));

		if (getScheItem().isNewItem()) {
			setBtnListType(BtnTypeEnum.ADD_CANCEL);
		} else {
			setBtnListType(BtnTypeEnum.EDIT_DELETE_CANCEL);
		}
		mDiskInfo.setText(Util.getGBSizeOfDisk(mState.getManager().getPvrMP()));

		if (getScheItem().getRepeatType() != ScheduleItem.REPEAT_TYPE_WEEK) {
			mWeekList.setVisibility(View.INVISIBLE);
		}

		reSetBtnList();

		handler.sendEmptyMessageDelayed(111, 300);
		updateDissmissTimer();
	}

	/**
	 * 
	 */
	private void reSetBtnList() {

		switch (getBtnListType()) {

		case ADD_CANCEL:
			mBtn1.setVisibility(View.INVISIBLE);
			mBtn2.setText(mState.getResource().getString(
					R.string.pvr_schedule_add));
			mBtn2.setVisibility(View.VISIBLE);
			mBtn3.setText(mState.getResource().getString(
					R.string.pvr_schedule_cancel));

			mBtn2.setOnClickListener(addListener);
			mBtn3.setOnClickListener(cancelListener);
			break;
		case EDIT_DELETE_CANCEL:
			mBtn1.setVisibility(View.VISIBLE);
			mBtn1.setText(mState.getResource().getString(
					R.string.pvr_schedule_edit));
			mBtn2.setText(mState.getResource().getString(
					R.string.pvr_schedule_delete));
			mBtn2.setVisibility(View.VISIBLE);
			mBtn3.setText(mState.getResource().getString(
					R.string.pvr_schedule_cancel));

			mBtn1.setOnClickListener(editListener);
			mBtn2.setOnClickListener(deleteListener);
			mBtn3.setOnClickListener(cancelListener);
			break;

		case REPLACE_ADD_CANCEL:
			mBtn1.setVisibility(View.VISIBLE);
			mBtn1.setText(mState.getResource().getString(
					R.string.pvr_schedule_replace));
			mBtn2.setText(mState.getResource().getString(
					R.string.pvr_schedule_add));
			mBtn2.setVisibility(View.GONE);
			mBtn3.setText(mState.getResource().getString(
					R.string.pvr_schedule_cancel));

			mBtn1.setOnClickListener(replaceListener);
			mBtn2.setOnClickListener(addListener);
			mBtn3.setOnClickListener(cancelListener);
			break;
		default:
			break;
		}
		reSetFocusPath();
	}

	void reSetFocusPath() {
		mBtn1.setNextFocusUpId(R.id.week_gridview);
		mBtn1.setNextFocusLeftId(R.id.week_gridview);

		mBtn2.setNextFocusUpId(R.id.week_gridview);
		mBtn2.setNextFocusLeftId(R.id.week_gridview);

		int id = R.id.week_gridview;
		if (mWeekList.getVisibility() != View.VISIBLE) {
			id = R.id.schedulelist_item_list;
		}

		if (mBtn1.getVisibility() == View.VISIBLE) {
			mBtn1.setNextFocusUpId(id);
			mBtn1.setNextFocusLeftId(id);
			if(mBtn2.getVisibility()==View.VISIBLE){
				mBtn1.setNextFocusRightId(mBtn2.getId());
			}else{
				mBtn1.setNextFocusRightId(mBtn3.getId());
			}
		}
		if (mBtn2.getVisibility() == View.VISIBLE) {
			if(mWeekList.getVisibility()== View.VISIBLE){
				mBtn2.setNextFocusUpId(mWeekList.getId());
			}
			mBtn2.setNextFocusUpId(id);
			if(mBtn1.getVisibility()==View.VISIBLE){
				mBtn2.setNextFocusLeftId(mBtn1.getId());
			}else{
				mBtn2.setNextFocusLeftId(mBtn3.getId());
			}
			mBtn2.setNextFocusRightId(mBtn3.getId());
		}
		if (mBtn3.getVisibility() == View.VISIBLE) {
			if(mWeekList.getVisibility()== View.VISIBLE){
				mBtn3.setNextFocusUpId(mWeekList.getId());
			}
			mBtn3.setNextFocusUpId(id);
			if(mBtn2.getVisibility()==View.VISIBLE){
				mBtn3.setNextFocusLeftId(mBtn2.getId());
			}else{
				mBtn3.setNextFocusLeftId(mBtn1.getId());
			}
			if(mBtn1.getVisibility()== View.VISIBLE){
				mBtn3.setNextFocusRightId(mBtn1.getId());
			}else{
				mBtn3.setNextFocusRightId(mBtn2.getId());
			}
			
		}

	}

	/**
	 * @return
	 */
	private List<DataItem> initItemList() {
		ScheduleItem item = getScheItem();

		List<DataItem> items = new ArrayList<DataItem>();

		// src type;
		String[] srcArray = mContext.getResources().getStringArray(
				R.array.pvr_tshift_srctype);

		List<TVChannel> list = mState.getTvLogicManager()
				.getChannels();

		if (list==null||list.size() == 0) {
			dismiss();
		}

		String srcTypeTitle = TimeShiftManager.SCHEDULE_PVR_SRCTYPE;
		DataItem srcType = new DataItem(srcTypeTitle,
				mContext.getString(R.string.schedule_pvr_srctype),
				TimeShiftManager.INVALID_VALUE, TimeShiftManager.INVALID_VALUE,
				0, srcArray, TimeShiftManager.STEP_VALUE,
				DataItem.DataType.OPTIONVIEW);
		items.add(srcType);

		// Channel Num;
		// String[] channelArray = mContext.getResources().getStringArray(
		// R.array.pvr_tshift_channel_temp_list);

		String[] channelArray = new String[list.size()];
		List<Integer> chList = new ArrayList<Integer>();
		for (int i = 0; i < list.size(); i++) {
			String channelName = list.get(i).getChannelName();
			String channelNumber = list.get(i).getChannelNum()+"";
			if (channelName == null) {
				// channelArray[i]=+"";
				channelName = channelNumber;
			}
			
			if(channelName.equalsIgnoreCase(channelNumber)){
				channelNumber="";	//avoid the situation like this: "14  14".
			}
			
			channelArray[i] = String.format("%s%3s%s", channelNumber, "",
					channelName);
			chList.add(list.get(i).getChannelNum());
			Util.showDLog(TAG, "channelArray[i]:" + i + "_:" + channelArray[i]);
			Util.showDLog(TAG, list.get(i).toString());

		}

		String channelListTitle = TimeShiftManager.SCHEDULE_PVR_CHANNELLIST;
		int channelIndex = 0;

		/*channelIndex = mState.getManager().getTvLogicManager()
				.getChannelNumInt();
		int index = chList.indexOf(channelIndex);*/

		channelIndex = list.indexOf(mState.getManager().getTvLogicManager()
				.getCurrentChannel());
		if (channelIndex == -1) {
			channelIndex = 0;
		}
		
		Util.showDLog(TAG, "schedulelist channelIndex:" + channelIndex + "==>:"
				+ channelIndex);

		DataItem channelList = new DataItem(channelListTitle,
				mContext.getString(R.string.schedule_pvr_channel_num),
				TimeShiftManager.INVALID_VALUE, TimeShiftManager.INVALID_VALUE,
				channelIndex, channelArray, TimeShiftManager.STEP_VALUE,
				DataItem.DataType.OPTIONVIEW);
		items.add(channelList);

		// start date
		Date startDate = item.getStartTime();
		Util.showDLog(TAG, "sItem:start:" + item.getDate());
		if (startDate == null) {
			startDate = new Date();
		}

		DataItem timeSet_date = new DataItem(TimeShiftManager.TIME_DATE,
				mContext.getString(R.string.schedule_pvr_start_date),
				TimeShiftManager.INVALID_VALUE, TimeShiftManager.INVALID_VALUE,
				0, null, TimeShiftManager.STEP_VALUE,
				DataItem.DataType.DATETIMEVIEW);
		timeSet_date.setmDateTimeType(DateTimeView.DATETYPE);
		timeSet_date.setmDateTimeStr(Util.longStrToDateStr(String
				.valueOf(startDate.getTime())));
		timeSet_date.setAutoUpdate(false);

		DataItem timeSet_time = new DataItem(TimeShiftManager.TIME_TIME,
				mContext.getString(R.string.schedule_pvr_start_time),
				TimeShiftManager.INVALID_VALUE, TimeShiftManager.INVALID_VALUE,
				0, null, TimeShiftManager.STEP_VALUE,
				DataItem.DataType.DATETIMEVIEW);
		timeSet_time.setmDateTimeType(DateTimeView.TIMETYPE);
		timeSet_time.setmDateTimeStr(Util.longStrToTimeStr(String
				.valueOf(startDate.getTime())));
		timeSet_time.setAutoUpdate(false);

		DataItem timeSet_time2 = new DataItem(TimeShiftManager.TIME_TIME,
				mContext.getString(R.string.schedule_pvr_stop_time),
				TimeShiftManager.INVALID_VALUE, TimeShiftManager.INVALID_VALUE,
				0, null, TimeShiftManager.STEP_VALUE,
				DataItem.DataType.DATETIMEVIEW);
		timeSet_time2.setmDateTimeType(DateTimeView.TIMETYPE);
		timeSet_time2.setmDateTimeStr(Util.longStrToTimeStr(String.valueOf(item
				.getEndTime().getTime())));
		timeSet_time2.setAutoUpdate(false);

		items.add(timeSet_date);
		items.add(timeSet_time);
		items.add(timeSet_time2);

		// schedule type;
		String[] reminderArray = mContext.getResources().getStringArray(
				R.array.pvr_tshift_schedule_type);

		String reminderTitle = TimeShiftManager.SCHEDULE_PVR_REMINDER_TYPE;
		DataItem reminderType = new DataItem(reminderTitle,
				mContext.getString(R.string.schedule_pvr_reminder_type),
				TimeShiftManager.INVALID_VALUE, TimeShiftManager.INVALID_VALUE,
				item.getRemindType(), reminderArray,
				TimeShiftManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
		items.add(reminderType);

		// Repeat type;
		String[] repeatArray = mContext.getResources().getStringArray(
				R.array.pvr_tshift_repeat_type);

		String repeatTitle = TimeShiftManager.SCHEDULE_PVR_REPEAT_TYPE;
		DataItem repeatType = new DataItem(repeatTitle,
				mContext.getString(R.string.schedule_pvr_repeat_type),
				TimeShiftManager.INVALID_VALUE, TimeShiftManager.INVALID_VALUE,
				item.getRepeatType(), repeatArray, TimeShiftManager.STEP_VALUE,
				DataItem.DataType.OPTIONVIEW);
		items.add(repeatType);

		return items;
	}

	/**
	 * 
	 */
	private void initListener() {
		// TODO Auto-generated method stub

		// btns,listener13
		// btns,listener2
		mDataItemList.setOnFocusChangeListener(new OnFocusChangeListener(){

			@Override
			public void onFocusChange(View v, boolean hasFocus) {
				// TODO Auto-generated method stub
				Util.showDLog("mDataItemList.onFocusChange,"+v.getClass().getSimpleName());
				if(v instanceof OptionView){
					if(hasFocus){
						((OptionView)v).setRightImageSource(true);
					}else
					{
						((OptionView)v).setRightImageSource(false);
					}
				}
			}
		});
		mDataItemList.setOnKeyListener(new android.view.View.OnKeyListener() {

			@Override
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				updateDissmissTimer();
				if (event.getAction() != KeyEvent.ACTION_DOWN) {
					return false;
				}

				switch (keyCode) {
				    case KeyMap.KEYCODE_DPAD_UP:
				        int positonU = mDataItemList.getSelectedItemPosition();
                        /*if (positonU ==3) {
                            View viewL = mDataItemList.getChildAt(3);
                            if (viewL instanceof DateTimeInputView) {
                                ((DateTimeInputView) viewL).setWhiteColor();
                            }
                        }
                        if (positonU ==4) {
                            View viewL = mDataItemList.getChildAt(4);
                            if (viewL instanceof DateTimeInputView) {
                                ((DateTimeInputView) viewL).setWhiteColor();
                            }
                        }
                        
                        if (positonU ==2) {
                            View viewL2 = mDataItemList.getChildAt(2);
                            if (viewL2 instanceof DateTimeInputView) {
                                ((DateTimeInputView) viewL2).setWhiteColor();
                            }
                        }	*/
                       
                    /*    try {
                            DataItem mCurrentDataItem = (DataItem) mDataItemList.getItemAtPosition(mDataItemList.getSelectedItemPosition());
                            if (mCurrentDataItem.getDataType() == DataItem.DataType.DATETIMEVIEW) {
                                DateTimeInputView childView = (DateTimeInputView) mDataItemList.getSelectedView();
                                childView.setCurrentSelectedPosition(-1);
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                        }*/
                        
                      
				        return false;
				    case KeyMap.KEYCODE_DPAD_DOWN:
				        int positonD = mDataItemList.getSelectedItemPosition();
				        MtkLog.d("chengcl","positonD=="+positonD);
				       /* if (positonD ==4) {
				            View viewL2 = mDataItemList.getChildAt(4);
                            if (viewL2 instanceof DateTimeInputView) {
                                ((DateTimeInputView) viewL2).setWhiteColor();
                            }
                        }
				        if (positonD ==3) {
                            View viewL2 = mDataItemList.getChildAt(3);
                            if (viewL2 instanceof DateTimeInputView) {
                                ((DateTimeInputView) viewL2).setWhiteColor();
                            }
                        }	
				        if (positonD ==2) {
                            View viewL2 = mDataItemList.getChildAt(2);
                            if (viewL2 instanceof DateTimeInputView) {
                                ((DateTimeInputView) viewL2).setWhiteColor();
                            }
                        }	*/
				       
	                    /*try {
	                        DataItem mCurrentDataItem = (DataItem) mDataItemList.getItemAtPosition(mDataItemList.getSelectedItemPosition());
	                        if (mCurrentDataItem.getDataType() == DataItem.DataType.DATETIMEVIEW) {
	                            DateTimeInputView childView = (DateTimeInputView) mDataItemList.getSelectedView();
	                            childView.setCurrentSelectedPosition(-1);
	                        }
	                    } catch (Exception e) {
	                        e.printStackTrace();
	                    }
	                 */
	                    if(positonD==mDataItemList.getCount()-1){
	                    	if(mWeekList.getVisibility()==View.VISIBLE){
	                    		//MtkLog.d("chengcl","18272772");
	                    		//(CheckBox)mWeekList.getChildAt(0)event.;
	                    	}else{
	                    		mBtn3.requestFocus();
	                    	}
	                    	reSetFocusPath();
	                    }
				        return false;
				case KeyEvent.KEYCODE_DPAD_LEFT:
					View optionViewL = mDataItemList.getChildAt(mDataItemList
							.getSelectedItemPosition());

					if (optionViewL instanceof RespondedKeyEvent) {
						((RespondedKeyEvent) optionViewL).onKeyLeft();

						if (optionViewL instanceof OptionView) {
							hiddenWeekList(optionViewL);
						}

					} else {
						Util.showDLog("optionView instanceof RespondedKeyEvent: "
								+ "false");
					}
					return true;
				case KeyEvent.KEYCODE_DPAD_RIGHT:

					View optionViewR = mDataItemList.getSelectedView();

					if (optionViewR instanceof RespondedKeyEvent) {
						((RespondedKeyEvent) optionViewR).onKeyRight();

						if (optionViewR instanceof OptionView) {
							hiddenWeekList(optionViewR);
						}
					} else {
						Util.showDLog("optionView instanceof RespondedKeyEvent: "
								+ "false");
					}

					return true;
				case KeyEvent.KEYCODE_DPAD_CENTER:

					break;

				case KeyEvent.KEYCODE_0:
				case KeyEvent.KEYCODE_1:
				case KeyEvent.KEYCODE_2:
				case KeyEvent.KEYCODE_3:
				case KeyEvent.KEYCODE_4:
				case KeyEvent.KEYCODE_5:
				case KeyEvent.KEYCODE_6:
				case KeyEvent.KEYCODE_7:
				case KeyEvent.KEYCODE_8:
				case KeyEvent.KEYCODE_9:
					View view = mDataItemList.getSelectedView();

					handleKeyboardInput(view, keyCode);
				case KeyEvent.KEYCODE_MTKIR_INFO:

					dumpValuesInfo();
					break;

				default:

					break;
				}

				if (keyCode == KeyEvent.KEYCODE_DPAD_CENTER) {
					if (mDataItemList.getFocusedChild() != null) {
						mDataItemList.getFocusedChild().performClick();
					}

				}

				return false;
			}
		});

		mWeekList.setOnKeyListener(new android.view.View.OnKeyListener() {

			@Override
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				// TODO Auto-generated method stub
				updateDissmissTimer();

				Util.showDLog("mWeekList.setOnKeyListener: keycode:" + keyCode);
				Util.showDLog("mWeekList.setOnKeyListener Action:"
						+ event.getAction());
				if (keyCode == KeyEvent.KEYCODE_DPAD_CENTER
						&& event.getAction() == KeyEvent.ACTION_DOWN) {
					if (mWeekList.getSelectedView() != null) {
						Util.showDLog("mWeekList.getSelectedView():::"
								+ mWeekList.getSelectedView().getClass()
										.toString());

						View view = mWeekList.getSelectedView().findViewById(
								R.id.schedule_list_item_week_item_cb);
						if (view != null) {
							view.performClick();
						} else {
							Util.showDLog("findViewById R.id.schedule_list_item_week_item_cb Null");

						}

					} else {
						Util.showDLog("mWeekList.getSelectedView() Null");
					}

				}

				return false;
			}

		});

		addListener = new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				// 0. check item's value.
				// 1.prepare scheduleItem data
				// 2.write data to DB.
				ScheduleItem item = prepareItem();
				int code = checkItem(item);
				if (code != 0) {
					Util.showELog("checkItem fail,Error Code: " + code);
					Toast.makeText(mContext, "add fail", Toast.LENGTH_SHORT)
							.show();
					return;
				}
				List<ScheduleItem> items = getReplaceItems(item);
				if(items!=null && items.size()>0){
					MtkLog.d(TAG,"items==="+items.size());
					showAddConfirmDialog(items,item);
				}else{
					MtkLog.d(TAG,"items ==null");					
					if (mState instanceof StateScheduleList) {
						((StateScheduleList) mState).insertItem(item);
					}
				}
			}

		};

		replaceListener = new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				// 0. check item's value.
				// 1.prepare scheduleItem data
				// 2.write data to DB.
				ScheduleItem item = prepareItem();
				int code = checkItem(item);
				if (code == 0) {
				handler.sendEmptyMessageDelayed(222, 300);
					showReplaceConfirmDialog();
				} else {
					Toast.makeText(mContext, "replace fail", Toast.LENGTH_SHORT)
							.show();
				}

			}
		};

		cancelListener = new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				getmCallback().switchToScheduleList(null);
			}
		};

		deleteListener = new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				showDeleteConfirmDialog();
			}
		};

		editListener = new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				setBtnListType(BtnTypeEnum.REPLACE_ADD_CANCEL);
				reSetBtnList();
			}
		};

	}

	private void hiddenWeekList(View optionViewL) {
		DataItem item = mDataList.get(mDataItemList.getSelectedItemPosition());

		if (item.getmItemID().equalsIgnoreCase(
				TimeShiftManager.SCHEDULE_PVR_REPEAT_TYPE)) {
			int value = ((OptionView) optionViewL).getValue();
			if (value != ScheduleItem.REPEAT_TYPE_WEEK) {
				mWeekList.setVisibility(View.INVISIBLE);
			} else {
				mWeekList.setVisibility(View.VISIBLE);
			}
		}

	}
	/**
	 * add by chuanlincheng
	 * 
	 */
	private List<ScheduleItem> getReplaceItems(ScheduleItem item){			
		Date start = Util.addDateAndTime(item.getDate(), item.getStartTime());
		Long startTime = start.getTime();
		Date end = Util.addDateAndTime(item.getDate(), item.getEndTime());
		Long endTime = end.getTime();
		List<ScheduleItem> replacetItems = new ArrayList<ScheduleItem>();
		List<ScheduleItem> items =null;
		if(mState instanceof StateScheduleList){
			items = ((StateScheduleList)mState).queryItem();
		}
		if(items!=null && items.size()>0){
			for(ScheduleItem sItem:items){
				Date sstart = Util.addDateAndTime(sItem.getDate(), sItem.getStartTime());
				Long sstartTime = sstart.getTime();
				Date send = Util.addDateAndTime(sItem.getDate(), sItem.getEndTime());
				Long sendTime = send.getTime();
				if(item.getChannelName().equalsIgnoreCase(sItem.getChannelName())){
					if(startTime <= sstartTime && endTime>sstartTime){
						replacetItems.add(sItem);
					}else if(startTime < sendTime){
						replacetItems.add(sItem);
					}
				}
			}
		}
		return replacetItems;
	}

	// p
	// {
	// if (optionViewR instanceof RespondedKeyEvent) {
	// ((RespondedKeyEvent) optionViewR).onKeyRight();
	//
	// if (optionViewR instanceof OptionView) {
	// hiddenWeekList(optionViewR);
	// }
	// }
	// }

	private void dumpValuesInfo() {
		// String str = "";
		// for (int i = 0; i < mDataItemList.getCount(); i++) {
		// View view = mDataItemList.getChildAt(i);
		//
		// if (view instanceof DateTimeInputView) {
		// str = ((DateTimeInputView) view).getmDateTimeView().getmDate();
		// } else if (view instanceof OptionView) {
		// str = ((OptionView) view).getValueView().getText().toString();
		// }
		//
		// Util.showDLog("dumpValuesInfo,value: " + i + " :" + str);
		// }
		// boolean isChecked = false;
		//
		// for (int i = 0; i < 7; i++) {
		// isChecked = ((CheckBox) mWeekList.getChildAt(i).findViewById(
		// R.id.schedule_list_item_week_item_cb)).isChecked();
		// Util.showDLog("dumpValuesInfo,isChecked? " + i + " :" + isChecked);
		// }

		String focusName = getCurrentFocus().getClass().getSimpleName();
		Util.showDLog("focusName:" + focusName);
	}

	/*
	 * 
	 */
	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		Util.showDLog("keyCode UP:" + keyCode);
		if (mDataItemList.hasFocus()) {
			((SetConfigListViewAdapter)mDataItemList.getAdapter()).setSelectPos(mDataItemList.getSelectedItemPosition());
			
				showHightLight(mDataItemList.getSelectedItemPosition());
			
		} else {
			hiddenHightLight();
			((SetConfigListViewAdapter)mDataItemList.getAdapter()).setSelectPos(-1);
		}
		this.updateDissmissTimer();
		return super.onKeyUp(keyCode, event);
	}

	/*
	 * 
	 */
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		Util.showDLog("keyCode Down:" + keyCode);
		updateDissmissTimer();
		switch (keyCode) {

		// case KeyMap.KEYCODE_0:
		// Util.showDLog(this.getCurrentFocus().toString());
		//
		// break;
		// case KeyMap.KEYCODE_1:
		// ScheduleItem item = new ScheduleItem();
		// item.setTaskID(temp_count);
		// temp_count--;
		//
		// if (mState instanceof StateScheduleList) {
		// Util.showDLog("replaceItem(item):" + temp_count);
		// ((StateScheduleList) mState).replaceItem(item);
		// }
		//
		// break;
		// case KeyMap.KEYCODE_2:
		//
		// break;
		case KeyEvent.KEYCODE_MTKIR_INFO:

			dumpValuesInfo();
			break;
		case KeyEvent.KEYCODE_DPAD_CENTER:
			if (mWeekList.hasFocus()) {
				Util.showDLog("mWeekList.hasFocus()");
				((CheckBox) mWeekList.getFocusedChild()).performClick();
			}

			if (mDataItemList.hasFocus()) {
				Util.showDLog("mDataItemList.hasFocus()");
				(mDataItemList.getFocusedChild()).performClick();
			}

			break;
		case KeyMap.KEYCODE_BACK:
			this.dismiss();
			Util.showDLog(TAG, "KEYCODE_BACK");
			break;
		}

		return super.onKeyDown(keyCode, event);
	}

	/*
	 * 
	 */
	@Override
	public void onItemClick(AdapterView<?> parent, View view, int position,
			long id) {
		// CheckBox tmp = (CheckBox) view
		// .findViewById(R.id.schedule_list_item_week_item_cb);
		// tmp.setChecked(!tmp.isChecked());
		Util.showDLog("onItemClick.................." + mDataItemList);
	}

	public void setCallback(StateScheduleListCallback callback) {
		mCallback = callback;
	}

	public StateScheduleListCallback<?> getmCallback() {
		return mCallback;
	}

	private void initBtnListener(BtnTypeEnum type) {

		switch (type) {
		case ADD_CANCEL:

			break;
		case EDIT_DELETE_CANCEL:

			break;
		case REPLACE_ADD_CANCEL:

			break;
		default:
			break;

		}
	}

	// Deal with key figures input
	private boolean handleKeyboardInput(View view, int keyCode) {
		char _ch = 0;
		String _mStr = "0";
		switch (keyCode) {
		case KeyMap.KEYCODE_0:
			_ch = '0';
			_mStr = "0";
			break;
		case KeyMap.KEYCODE_1:
			_ch = '1';
			_mStr = "1";
			break;
		case KeyMap.KEYCODE_2:
			_ch = '2';
			_mStr = "2";
			break;
		case KeyMap.KEYCODE_3:
			_ch = '3';
			_mStr = "3";
			break;
		case KeyMap.KEYCODE_4:
			_ch = '4';
			_mStr = "4";
			break;
		case KeyMap.KEYCODE_5:
			_ch = '5';
			_mStr = "5";
			break;
		case KeyMap.KEYCODE_6:
			_ch = '6';
			_mStr = "6";
			break;
		case KeyMap.KEYCODE_7:
			_ch = '7';
			_mStr = "7";
			break;
		case KeyMap.KEYCODE_8:
			_ch = '8';
			_mStr = "8";
			break;
		case KeyMap.KEYCODE_9:
			_ch = '9';
			_mStr = "9";
			break;
		default:
			break;
		}

		if (view != null && view instanceof DateTimeInputView) {

			DateTimeInputView dateTimeInputView = (DateTimeInputView) view;
			DateTimeView _dateTimeView = dateTimeInputView.getmDateTimeView();
			if (_dateTimeView != null) {
				_dateTimeView.input(_ch, dateTimeInputView.getmDataItem());
				return true;
			} else {
				return false;
			}
		}
		return false;
	}

	public BtnTypeEnum getBtnListType() {
		return btnListType;
	}

	public void setBtnListType(BtnTypeEnum mType) {
		this.btnListType = mType;
	}

	/**
	 * @param item
	 * @return error code.like: out of date,...
	 */
	private int checkItem(ScheduleItem item) {
		// Error 1,2,3,4,5,6,7...
		final int TIME_ERROR_1 = 1;
		final int TIME_ERROR_2 = 2;
		final int TIME_ERROR_3 = 3;
		final int TIME_ERROR_4 = 4;
		final int TIME_ERROR_5 = 5;

		Date startDate = item.getDate();

		Date start = Util.addDateAndTime(startDate, item.getStartTime());
		Long startTime = start.getTime();

		Date end = Util.addDateAndTime(startDate, item.getEndTime());
		Long endTime = end.getTime();

		Util.longToDate(startTime); // debug
		Util.longToDate(endTime);
		Util.longToDate(System.currentTimeMillis());
		if (startTime <= System.currentTimeMillis()) {
			Util.showDLog("startTime<=System.currentTimeMillis()");
			Util.longToDate(System.currentTimeMillis());
			return TIME_ERROR_1;
		}

		if (endTime <= startTime) {
			return TIME_ERROR_2;
		}

		Long year = item.getDate().getTime();
		Long afterAYear = System.currentTimeMillis() + 1L * 365 * 24 * 3600
				* 1000;
		if (year >= afterAYear) {
			return TIME_ERROR_3; // more than one year--debug
		}
		if (endTime >= startTime + 1L * 60 * 60 * 1000 + 1000) {
			return TIME_ERROR_4; // only record one hour+1 second.--debug
		}

		if (endTime <= startTime + 1L * 60 * 1000) {
			return TIME_ERROR_5; // the time less than 1 minute.--debug
		}

		return 0;
	}

	private ScheduleItem prepareItem() {
		ScheduleItem item = new ScheduleItem();

		for (int i = 0; i < mDataItemList.getCount(); i++) {
			View optionViewL = mDataItemList.getChildAt(i);

			switch (i) {
			case UI_Index_SrcType: // src type,default:DTV

				break;

			case UI_Index_ChannelNum: // channel number
				int channelIndex = ((OptionView) optionViewL).getValue();
				String channelName = String.valueOf(mState.getManager()
						.getTvLogicManager().getChannels().get(channelIndex)
						.getChannelNum());
				int channelNum = mState.getManager().getTvLogicManager()
						.getChannels().get(channelIndex).getChannelNum();
				item.setChannelName(channelName);
				break;

			case UI_Index_StartDate: // start date
				String startDate = ((DateTimeInputView) optionViewL)
						.getmDateTimeView().getmDate();
				item.setDate(Util.strToDate(startDate));
				break;

			case UI_Index_StartTime: // start time
				String startTime = ((DateTimeInputView) optionViewL)
						.getmDateTimeView().getmDate();
				item.setStartTime(Util.strToTime(startTime));
				break;

			case UI_Index_StopTime: // stop time
				String endTime = ((DateTimeInputView) optionViewL)
						.getmDateTimeView().getmDate();
				item.setEndTime(Util.strToTime(endTime));
				break;

			case UI_Index_ScheduleType: // schedule type
				int scheduleType = ((OptionView) optionViewL).getValue();
				item.setRemindType(scheduleType);
				break;

			case UI_Index_RepeatType: // repeat type
				int repeatType = ((OptionView) optionViewL).getValue();
				item.setRepeatType(repeatType);
				break;
			default:
				break;
			}
		}
		int repeatType = ((OptionView) mDataItemList
				.getChildAt(UI_Index_RepeatType)).getValue();
		StringBuilder weekValue = new StringBuilder();
		if (repeatType == ScheduleItem.REPEAT_TYPE_WEEK) { // weekly
			for (int i = 6; i >= 0; i--) {
				boolean selected = ((WeekListAdapter) mWeekList.getAdapter())
						.isChecked(i);

				weekValue.append(selected ? 1 : 0);
			}

			item.setWeeklyRepeat(Integer.valueOf(weekValue.toString(), 2));
			Util.showDLog(weekValue.toString());
		} else {
			item.setWeeklyRepeat(0);
		}

		item.setTaskID(getScheItem().getTaskID());
		return item;
	}

	/**
	 * @return the mScheItem
	 */
	public ScheduleItem getScheItem() {
		return mScheItem;
	}

	/**
	 * @param mScheItem
	 *            the mScheItem to set
	 */
	public void setScheItem(ScheduleItem mScheItem) {
		this.mScheItem = mScheItem;
	}

	private void showDeleteConfirmDialog() {
		List<ScheduleItem> items = new ArrayList<ScheduleItem>();
		items.add(getScheItem());
		confirm = new SpecialConfirmDialog(mState.getActivity(), mState,
				items);

		confirm.setPositiveButton(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				Util.showDLog("showDeleteConfirmDialog().PositiveButton");
				mState.deleteItem(getScheItem());
				mState.switchToScheduleList(null);
				dismiss();
			}
		});

		confirm.setNegativeButton(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				Util.showDLog("showDeleteConfirmDialog().NegativeButton");
				dismiss();
			}
		});

		String title1 = mState.getResource().getString(
				R.string.pvr_schedulelist_delete_line1);
		String title2 = mState.getResource().getString(
				R.string.pvr_schedulelist_delete_line2);
		confirm.setTitle(title1, title2);
		confirm.show();
	}
	/**
	 * add by chuanlincheng
	 * @param items
	 * @param currenItem
	 */
	private void showAddConfirmDialog(final List<ScheduleItem> items,final ScheduleItem currenItem) {
		confirm = new SpecialConfirmDialog(mState.getActivity(), mState,
				items);
		confirm.setPositiveButton(null);
		confirm.setNegativeButton(null);

		confirm.setPositiveButton(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				Util.showDLog("showDeleteConfirmDialog().PositiveButton");				
				for(ScheduleItem item :items){
					mState.deleteItem(item);					
				}
				mState.insertItem(currenItem);
				dismiss();

			}
		});
		confirm.setNegativeButton(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				Util.showDLog("showDeleteConfirmDialog().NegativeButton");
				dismiss();
			}
		});
		confirm.setOnCancelListener(new OnCancelListener() {
            
            @Override
            public void onCancel(DialogInterface arg0) {
                // TODO Auto-generated method stub
                handler.sendEmptyMessageDelayed(333, 300);
            }
        });
		String title1 = mState.getResource().getString(
				R.string.pvr_schedulelist_replace_line1);
		String title2 = mState.getResource().getString(
				R.string.pvr_schedulelist_replace_line2);

		confirm.setTitle(title1, title2);
		confirm.show();
	}
	private void showReplaceConfirmDialog() {
		List<ScheduleItem> items = new ArrayList<ScheduleItem>();
		items.add(getScheItem());
		confirm = new SpecialConfirmDialog(mState.getActivity(), mState,
				items);
		confirm.setPositiveButton(null);
		confirm.setNegativeButton(null);

		confirm.setPositiveButton(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				Util.showDLog("showDeleteConfirmDialog().PositiveButton");
				ScheduleItem item = prepareItem();
				if (item != null) {
					mState.replaceItem(item);
					mState.switchToScheduleList(null);
					dismiss();
				} else {
					Util.showELog("replace fail... ...");
				}

			}
		});
		confirm.setNegativeButton(new Button.OnClickListener() {
			@Override
			public void onClick(View v) {
				Util.showDLog("showDeleteConfirmDialog().NegativeButton");
				dismiss();
			}
		});
		confirm.setOnCancelListener(new OnCancelListener() {
            
            @Override
            public void onCancel(DialogInterface arg0) {
                // TODO Auto-generated method stub
                handler.sendEmptyMessageDelayed(333, 300);
            }
        });
		String title1 = mState.getResource().getString(
				R.string.pvr_schedulelist_replace_line1);
		String title2 = mState.getResource().getString(
				R.string.pvr_schedulelist_replace_line2);

		confirm.setTitle(title1, title2);
		confirm.show();
	}

	// Adapter:
	public class WeekListAdapter extends BaseAdapter {

		// HashMap<String, Boolean>
		private List<Boolean> weekList = new ArrayList<Boolean>();
		private ViewGroup mGroup;

		public WeekListAdapter(HashMap<String, Boolean> list) {
			for (int i = 0; i < WEEK_ARRAY.length; i++) {
				weekList.add(list.get(WEEK_ARRAY[i]) == null ? false : list
						.get(WEEK_ARRAY[i]));
			}
		}

		public View getView(int position, View convertView, ViewGroup parent) {
			View view;

			if (convertView == null) {
				view = mInflater.inflate(
						R.layout.pvr_tshift_schedule_list_item_week, null);
				mGroup = parent;
			} else {
				view = convertView;
			}
			if(view.isFocused()){
				view.setBackgroundResource(R.drawable.top_focus_img);
			}else{
			}
			setItemValue(view, WEEK_ARRAY[position], getItem(position));
			return view;
		}

		/**
		 * @param view
		 * @param item
		 */
		private void setItemValue(View view, String name, Boolean item) {
			CheckBox tmp = (CheckBox) view
					.findViewById(R.id.schedule_list_item_week_item_cb);
			tmp.setChecked(item);
			tmp.setText(name);

		}

		public final int getCount() {
			return weekList.size();
		}

		public final Boolean getItem(int position) {
			return weekList.get(position);
		}

		public final Boolean isChecked(int position) {
			View view = mGroup.getChildAt(position).findViewById(
					R.id.schedule_list_item_week_item_cb);
			if (view instanceof CheckBox) {
				return ((CheckBox) view).isChecked();
			} else {
				return false;
			}
		}

		public final long getItemId(int position) {
			return position;
		}
	}
	@Override
	public void onItemSelected(AdapterView<?> parent, View view, int position,
			long id) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onNothingSelected(AdapterView<?> parent) {
		// TODO Auto-generated method stub

	}
	
	public void updateDissmissTimer(){
		((StateScheduleList)mState).mHandler.removeMessages(StateScheduleList.Auto_Dismiss_Info_Dialog_Timer);
		((StateScheduleList)mState).mHandler.sendEmptyMessageDelayed(StateScheduleList.Auto_Dismiss_Info_Dialog_Timer, Core.AUTO_DISSMISS_TIMER);
	}
	
	
	//fix DTV00586312
	private void showHightLight(int id){		
			if(id==1){
				((OptionView)mDataItemList.getChildAt(id)).setRightImageSource(true);
				((OptionView)mDataItemList.getChildAt(0)).setRightImageSource(false);
				((OptionView)mDataItemList.getChildAt(5)).setRightImageSource(false);
				((OptionView)mDataItemList.getChildAt(6)).setRightImageSource(false);
				hiddenYellow();
			}else if(id==5){
				((OptionView)mDataItemList.getChildAt(id)).setRightImageSource(true);
				((OptionView)mDataItemList.getChildAt(0)).setRightImageSource(false);
				((OptionView)mDataItemList.getChildAt(1)).setRightImageSource(false);
				((OptionView)mDataItemList.getChildAt(6)).setRightImageSource(false);
				hiddenYellow();
			}else if(id==6){
				((OptionView)mDataItemList.getChildAt(id)).setRightImageSource(true);
				((OptionView)mDataItemList.getChildAt(0)).setRightImageSource(false);
				((OptionView)mDataItemList.getChildAt(1)).setRightImageSource(false);
				((OptionView)mDataItemList.getChildAt(5)).setRightImageSource(false);
				hiddenYellow();
			}else if(id==0){
				((OptionView)mDataItemList.getChildAt(id)).setRightImageSource(true);
				((OptionView)mDataItemList.getChildAt(1)).setRightImageSource(false);
				((OptionView)mDataItemList.getChildAt(5)).setRightImageSource(false);
				((OptionView)mDataItemList.getChildAt(6)).setRightImageSource(false);
				hiddenYellow();
			}else{
				 hiddenHightLight();
				((DateTimeInputView)mDataItemList.getChildAt(id)).setFlag();  
				if(id==2){
					((DateTimeInputView)mDataItemList.getChildAt(3)).setWhiteColor(); 
					((DateTimeInputView)mDataItemList.getChildAt(4)).setWhiteColor(); 
				}else if(id==3){
					((DateTimeInputView)mDataItemList.getChildAt(2)).setWhiteColor(); 
					((DateTimeInputView)mDataItemList.getChildAt(4)).setWhiteColor(); 
				}else if(id==4){
					((DateTimeInputView)mDataItemList.getChildAt(2)).setWhiteColor(); 
					((DateTimeInputView)mDataItemList.getChildAt(3)).setWhiteColor(); 
				}
               
			}		
	}
	
	//fix DTV00586312
	private void hiddenHightLight(){
		if(mDataItemList!=null){
			((OptionView)mDataItemList.getChildAt(5)).setRightImageSource(false);
			((OptionView)mDataItemList.getChildAt(0)).setRightImageSource(false);
			((OptionView)mDataItemList.getChildAt(1)).setRightImageSource(false);
			((OptionView)mDataItemList.getChildAt(6)).setRightImageSource(false);
		}
    }
	//fix DTV00586900
	private void hiddenYellow(){
		((DateTimeInputView)mDataItemList.getChildAt(2)).setWhiteColor(); 
		((DateTimeInputView)mDataItemList.getChildAt(3)).setWhiteColor(); 
		((DateTimeInputView)mDataItemList.getChildAt(4)).setWhiteColor(); 
	}
	
}
