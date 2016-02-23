package com.mediatek.tvcommon;

import com.mediatek.tv.model.ChannelInfo;
import com.mediatek.tv.model.EventActiveWindow;
import com.mediatek.tv.model.EventCommand;

public class TVEventCommand extends EventCommand{

	@Override
	public int describeContents() {
		// TODO Auto-generated method stub
		return super.describeContents();
	}

	@Override
	public EventActiveWindow getActiveWindow() {
		// TODO Auto-generated method stub
		return super.getActiveWindow();
	}

	@Override
	public int getCommandMask() {
		// TODO Auto-generated method stub
		return super.getCommandMask();
	}

	@Override
	public ChannelInfo getCurrentChannelInfo() {
		// TODO Auto-generated method stub
		return super.getCurrentChannelInfo();
	}

	@Override
	public String getEventDetailSeparator() {
		// TODO Auto-generated method stub
		return super.getEventDetailSeparator();
	}

	@Override
	public int getEventMinSeconds() {
		// TODO Auto-generated method stub
		return super.getEventMinSeconds();
	}

	@Override
	public int getFakeEventMinSecond() {
		// TODO Auto-generated method stub
		return super.getFakeEventMinSecond();
	}

	@Override
	public int getMaxDay() {
		// TODO Auto-generated method stub
		return super.getMaxDay();
	}

	@Override
	public String[] getPrefLanuage() {
		// TODO Auto-generated method stub
		return super.getPrefLanuage();
	}

	@Override
	public String getTunerName() {
		// TODO Auto-generated method stub
		return super.getTunerName();
	}

	@Override
	public boolean isActualOnly() {
		// TODO Auto-generated method stub
		return super.isActualOnly();
	}

	@Override
	public boolean isDoClean() {
		// TODO Auto-generated method stub
		return super.isDoClean();
	}

	@Override
	public boolean isDoEnable() {
		// TODO Auto-generated method stub
		return super.isDoEnable();
	}

	@Override
	public boolean isDoRestart() {
		// TODO Auto-generated method stub
		return super.isDoRestart();
	}

	@Override
	public boolean isFakeEventInsertionEnable() {
		// TODO Auto-generated method stub
		return super.isFakeEventInsertionEnable();
	}

	@Override
	public boolean isPartialOverapAllow() {
		// TODO Auto-generated method stub
		return super.isPartialOverapAllow();
	}

	@Override
	public boolean isTimeConfictAllow() {
		// TODO Auto-generated method stub
		return super.isTimeConfictAllow();
	}

	@Override
	public void setActiveWindow(EventActiveWindow activeWindow) {
		// TODO Auto-generated method stub
		super.setActiveWindow(activeWindow);
	}

	@Override
	public void setActualOnly(boolean actualOnly) {
		// TODO Auto-generated method stub
		super.setActualOnly(actualOnly);
	}

	@Override
	public void setCommandMask(int commandMask) {
		// TODO Auto-generated method stub
		super.setCommandMask(commandMask);
	}

	@Override
	public void setCurrentChannelInfo(ChannelInfo currentChannelInfo) {
		// TODO Auto-generated method stub
		super.setCurrentChannelInfo(currentChannelInfo);
	}

	@Override
	public void setDoClean(boolean doClean) {
		// TODO Auto-generated method stub
		super.setDoClean(doClean);
	}

	@Override
	public void setDoEnable(boolean doEnable) {
		// TODO Auto-generated method stub
		super.setDoEnable(doEnable);
	}

	@Override
	public void setDoRestart(boolean doRestart) {
		// TODO Auto-generated method stub
		super.setDoRestart(doRestart);
	}

	@Override
	public void setEventDetailSeparator(String eventDetailSeparator) {
		// TODO Auto-generated method stub
		super.setEventDetailSeparator(eventDetailSeparator);
	}

	@Override
	public void setEventMinSeconds(int eventMinSeconds) {
		// TODO Auto-generated method stub
		super.setEventMinSeconds(eventMinSeconds);
	}

	@Override
	public void setFakeEventInsertionEnable(boolean fakeEventInsertionEnable) {
		// TODO Auto-generated method stub
		super.setFakeEventInsertionEnable(fakeEventInsertionEnable);
	}

	@Override
	public void setFakeEventMinSecond(int fakeEventMinSecond) {
		// TODO Auto-generated method stub
		super.setFakeEventMinSecond(fakeEventMinSecond);
	}

	@Override
	public void setMaxDay(int maxDay) {
		// TODO Auto-generated method stub
		super.setMaxDay(maxDay);
	}

	@Override
	public void setPartialOverapAllow(boolean partialOverapAllow) {
		// TODO Auto-generated method stub
		super.setPartialOverapAllow(partialOverapAllow);
	}

	@Override
	public void setPrefLanuage(String[] prefLanuage) {
		// TODO Auto-generated method stub
		super.setPrefLanuage(prefLanuage);
	}

	@Override
	public void setTimeConfictAllow(boolean timeConfictAllow) {
		// TODO Auto-generated method stub
		super.setTimeConfictAllow(timeConfictAllow);
	}

	@Override
	public void setTunerName(String tunerName) {
		// TODO Auto-generated method stub
		super.setTunerName(tunerName);
	}

	@Override
	public String toString() {
		// TODO Auto-generated method stub
		return super.toString();
	}
	

}
