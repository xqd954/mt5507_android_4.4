package com.mediatek.tvcm;

import java.util.Enumeration;
import java.util.Vector;

import com.mediatek.tv.service.InputService;
import com.mediatek.tvcommon.ITVCommon.TVSelectorListener;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVInputCommon;
import com.mediatek.tvcommon.TVInputCommon.InputSourceEventListener;
import com.mediatek.tvcommon.TVInputCommon.TVInputSourceListener;
import com.mediatek.tvcommon.TVMethods;
import com.mediatek.tvcommon.TVOutputCommon;
import com.mediatek.tvcommon.TVCommonManager;

import android.content.Context;
import android.graphics.Rect;
import android.os.RemoteException;

public class TVInputManager extends TVComponent {
	static private TVInputManager instance;
	private TVMethods mtvm;
	private TVInputCommon mTVInputCommon;
	private TVOutputCommon mTVOutputCommon;

	public final static String INPUT_TYPE_TV = InputService.INPUT_TYPE_TV;
	public final static String INPUT_TYPE_AV = InputService.INPUT_TYPE_AV;
	public final static String INPUT_TYPE_SVIDEO = InputService.INPUT_TYPE_SVIDEO;
	public final static String INPUT_TYPE_COMPONENT = InputService.INPUT_TYPE_COMPONENT;
	public final static String INPUT_TYPE_COMPOSITE = InputService.INPUT_TYPE_COMPOSITE;
	public final static String INPUT_TYPE_HDMI = InputService.INPUT_TYPE_HDMI;
	public final static String INPUT_TYPE_VGA = InputService.INPUT_TYPE_VGA;

	public static final int OUTPUT_MODE_PIP = TVInputCommon.OUTPUT_MODE_PIP;
	public static final int OUTPUT_MODE_POP = TVInputCommon.OUTPUT_MODE_POP;
	public static final int OUTPUT_MODE_NORMAL = TVInputCommon.OUTPUT_MODE_NORMAL;
	
	TVInputManager(Context context) {
		super(context);
		mtvm = TVMethods.getInstance(context);
		mTVInputCommon = TVInputCommon.getInstance(context);
		mTVOutputCommon=TVOutputCommon.getInstance(context);
	}

	static public TVInputManager getInstance(Context context) {
		if (instance == null) {
			instance = new TVInputManager(context);
		}
		return instance;
	}
	
	public boolean isBlock(String source)
	{
		try {
			return getITVCommon().isInputSourceBlocked(source);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		
		return false;
	}

	public String[] getInputSourceArray() {
		try {
			String[] sources = null;
			if(null != itv){
				
			    sources= itv.getInputSourceArray();
			}
			
			if(null==sources)
			{
				return new String[]{};
			}
			return sources;
		} catch (RemoteException e) {
			e.printStackTrace();
		}

		return null;
	}
	
	public void changeNextInputSource() {
		try {
			String sourceArray []=itv.getInputSourceArray();
			
			if(null==sourceArray||sourceArray.length==0)
			{
				return;
			}
			String cur=itv.getCurrentInputSource();
			int index=0;
			for(int i=0,len=sourceArray.length;i<len;i++)
			{
				if(sourceArray[i].equals(cur))
				{
					index =i;
					break;
				}
			}
			index+=1;
			if(index>=sourceArray.length)
			{
				index=0;
			}
			itv.changeInputSource(getDefaultOutput().getName(), sourceArray[index]);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	/**
	 * get current input source name string.
	 * 
	 * @param outputName
	 *            not used in this method, just a wrapper.
	 * @return current input source name.
	 */
	public String getCurrInputSource(String outputName) {
		try {
			if(null != itv){
			return itv.getCurrentInputSource();
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}

		return null;
	}

	public String getCurrInputSource() {
		try {
			if(null != itv){
			return itv.getCurrentInputSource();
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}

		return null;
	}

	public String[] getOutputArray() {
		// TODO get output array.

		String[] outputArray = { "main", "sub" };

		return outputArray;
	}

	public String getTypeFromInputSource(String inputSourceName) {
		try {
			if(null != itv){
			return itv.getInputSourceType(inputSourceName);
			}
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		return INPUT_TYPE_TV;
	}

	public void block(String inputName, boolean block) {
		try {
			itv.blockInputSource(inputName, block);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	public void setFreeze(String outputName, boolean isFreeze) {
		mtvm.setFreeze(outputName, isFreeze);
	}

	public void setFreeze(boolean isFreeze) {
		mtvm.setFreeze(isFreeze);
	}

	public boolean isFreeze() {
		return mtvm.isFreeze();
	}

	public void setMjcBypassWindow(int windowId, Rect r) {
		mTVOutputCommon.setMjcBypassWindow(windowId, r);
	}

	public void stopDesignateOutput(String outPutName, boolean needSync) {
		mTVInputCommon.stopDesignateOutput(outPutName, needSync);
	}

	public int getOuputMode() {
		return mTVInputCommon.getOuputMode();
	}

	public void changeInputSource(String outputName, String inputName) {
		try {
			if(null != itv){
			itv.changeInputSource(outputName, inputName);
			}	
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void changeInputSource(String inputName) {
		changeInputSource("main", inputName);
	}

	public TVOutput getOutput(String name) {
		return new TVOutput(context,name);
	}

	public static interface InputSourceListener {

		public void onSelected(String output, String input);

		public void onOutputSignalChange(String output, boolean hasSignal);

		public void onInputSignalChange(String input, boolean hasSignal);

		public void onInputGotSignal(String input);

		public void onBlocked(String input);
	}

	private class InputSourceListenerDelegate {
		private InputSourceListener listener = null;
		TVInputSourceListener sourceListener = null;
		TVSelectorListener selectorListener = null;

		public InputSourceListenerDelegate(final InputSourceListener listener) {
			// TODO Auto-generated constructor stub
			this.listener = listener;

			sourceListener = new TVInputSourceListener() {

				@Override
				public void onOutputSignalChange(String output,
						boolean hasSignal) {
					// TODO Auto-generated method stub
					listener.onOutputSignalChange(output, hasSignal);
				}

				@Override
				public void onInputSignalChange(String input, boolean hasSignal) {
					// TODO Auto-generated method stub
					listener.onInputSignalChange(input, hasSignal);
				}

				@Override
				public void onInputGotSignal(String input) {
					// TODO Auto-generated method stub
					listener.onInputGotSignal(input);
				}
			};

			selectorListener = new TVSelectorListener() {

				@Override
				public void onInputSelected(String outputName, String inputName) {
					// TODO Auto-generated method stub
					listener.onSelected(outputName, inputName);
				}

				@Override
				public void onInputBlocked(String inputName) {
					// TODO Auto-generated method stub
					listener.onBlocked(inputName);
				}

				@Override
				public void onChannelSelected(TVChannel ch) {
					// TODO Auto-generated method stub
					return;
				}

				@Override
				public void onChannelBlocked(TVChannel ch) {
					// TODO Auto-generated method stub
					return;
				}
			};
		}
	}

	protected Vector<InputSourceListenerDelegate> delegateListeners = new Vector<InputSourceListenerDelegate>();

	public void registerSourceListener(final InputSourceListener listener) {
		InputSourceListenerDelegate delegate = new InputSourceListenerDelegate(
				listener);

		mTVInputCommon.registerSourceListener(delegate.sourceListener);
		try {
			if(null != itv){
			itv.addSelectorListener(delegate.selectorListener);
			}	
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		delegateListeners.add(delegate);
	}

	public void removeSourceListener(InputSourceListener listener) {
		Enumeration<InputSourceListenerDelegate> e = delegateListeners
				.elements();
		InputSourceListenerDelegate item = null;

		while (e.hasMoreElements()) {
			item = e.nextElement();
			if (item.listener == listener) {
				break;
			}
		}

		if (item != null) {
			delegateListeners.remove(item);
			mTVInputCommon.removeSourceListener(item.sourceListener);
			try {
				if(null != itv){
				itv.removeSelectorListener(item.selectorListener);
				}
			} catch (RemoteException e1) {
				e1.printStackTrace();
			}
		}
	}

	public void registerSourceEventListener(
			final InputSourceEventListener listener) {
		mTVInputCommon.registerSourceEventListener(listener);
	}

	public void removeSourceEventListener(InputSourceEventListener listener) {
		mTVInputCommon.removeSourceEventListener(listener);

	}

	public TVOutput getDefaultOutput() {
		TVCommonManager cm = TVCommonManager.getInstance(context);
		String outputName = cm.getCurrentOutput();
		return new TVOutput(context, outputName);
	}
	
	public void setDefaultOutput(String name) {
		mTVOutputCommon.setDefaultOutput(name);
	}
	
	public TVOutput enterOutputMode(int outputMode) {
		String outputName = mTVOutputCommon.enterOutputMode(outputMode);
		return new TVOutput(context, outputName);
	}
	
	public TVInput getInput(String name) {
		return new TVInput(context, name);
	}
}
