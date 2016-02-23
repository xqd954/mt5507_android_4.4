package com.mediatek.tvcm;

import java.util.Hashtable;

import android.content.Context;
import android.graphics.RectF;

import com.mediatek.tvcommon.TVOutputCommon;
import com.mediatek.tvcommon.TVOutputCommon.AdjustListener;
import com.mediatek.tvcommon.TVOutputCommon.AudioUpdated;
import com.mediatek.tvcommon.TVOutputCommon.MTSOption;
import com.mediatek.tvcommon.TVOutputCommon.VideoResolution;

public class TVOutput {
	public static final int AUTO_ADJUST = TVOutputCommon.AUTO_ADJUST;
	public static final int AUTO_PHASE = TVOutputCommon.AUTO_PHASE;
	public static final int AUTO_COLOR = TVOutputCommon.AUTO_COLOR;
	
	private TVOutputCommon mTVOutputCommon;
	private Context mContext;
	
	private String name = null;
	
	public TVOutput(Context context, String name) {
		mContext = context;
		this.name = name;
		mTVOutputCommon = TVOutputCommon.getInstance(mContext);
	}
	
	public int getSignalLevel() {
		return mTVOutputCommon.getSignalLevel(name);
	}
	
	public int getSignalBer() {
		return mTVOutputCommon.getSignalBer(name);
	}
	
	public void adjust(final AdjustListener listener, final int item) {
		mTVOutputCommon.adjust(name, listener, item);
	}
	
	public void adjust(final AdjustListener listener) {
		mTVOutputCommon.adjust(name, listener, mTVOutputCommon.AUTO_ADJUST);
	}
	
	public MTSOption getMTSOption() {
		return mTVOutputCommon.getMTSOption(name);
	}
	
	public void setFreeze(boolean isFreeze) {
		mTVOutputCommon.setFreeze(name, isFreeze);
	}
	
	public boolean isFreeze(){
		return mTVOutputCommon.isFreeze(name);
	}
	
	public boolean isEnableFreeze(){
		return mTVOutputCommon.isEnableFreeze(name);
	}
	
	public void stop() {
		mTVOutputCommon.stop(name);
	}

	public void stopEx(boolean isForce) {
		mTVOutputCommon.stopEx(name, isForce);
	}
	
	public String getName() {
		return name;
	}
	
	public RectF getScreenRectangle() {
		return mTVOutputCommon.getScreenRectangle(name);
	}
	
	public boolean setSrcRectangle(RectF rect) {
		return mTVOutputCommon.setSrcRectangle(name, rect);
	}
	
	public boolean setScreenRectangle(RectF rect) {
		return mTVOutputCommon.setScreenRectangle(name, rect);
	}
	
	public VideoResolution getInstanceVideoResolution() {
		return mTVOutputCommon.getInstanceVideoResolution(name);
	}
	
	public Hashtable<Integer, String> getCurAudLang() {
		return mTVOutputCommon.getCurAudLang();
	}
	
	public Hashtable<Integer, String> getAudLangTable() {
		return mTVOutputCommon.getAudLangTable();
	}
	
	public void setAudLangByIndex(int audIndex) {
		mTVOutputCommon.setAudLangByIndex(audIndex);
	}
	
	public void registerAudioUpdatedListener(AudioUpdated update) {
		mTVOutputCommon.registerAudioUpdatedListener(update);
	}
	
	public void removeAudioUpdatedListener(AudioUpdated update) {
		mTVOutputCommon.removeAudioUpdatedListener(update);
	}
	
	public void clearAudioUpdatedListener(AudioUpdated update) {
		mTVOutputCommon.clearAudioUpdatedListener(update);
	}
	
	public TVInput getInput() {
		return new TVInput(mContext, mTVOutputCommon.getInput(name));
	}
	
	public void connect(TVInput input) {		
		mTVOutputCommon.connect(name, input.getName());
	}
	
	public RectF getSrcRectangle() {
		return mTVOutputCommon.getSrcRectangle(name);
	}
	
	public boolean aspectRatioCanSet() {
		return mTVOutputCommon.aspectRatioCanSet(name);
	}
	
	public boolean hasSignal() {
		return mTVOutputCommon.hasSignal(name);
	}
}
