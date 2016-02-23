package com.mediatek.ui.nav.util;

public class LockKey {
	private boolean mCurrentSourceIsLocked = false;
	private boolean mCurrentChannelIsLocked = false;

	public synchronized void unLockCurCh() {
		mCurrentChannelIsLocked = false;
	}

	public synchronized void lockCurCh() {
		mCurrentChannelIsLocked = true;
	}

	public synchronized void unLockCurSource() {
		mCurrentSourceIsLocked = false;
	}

	public synchronized void lockCurSource() {
		mCurrentSourceIsLocked = true;
	}

	public boolean isCurSourceLock() {
		return mCurrentSourceIsLocked;
	}

	public boolean isCurChannelLock() {
		return mCurrentChannelIsLocked;
	}

	public void resetLock() {
		mCurrentSourceIsLocked = false;
		mCurrentChannelIsLocked = false;
	}

	@Override
	public String toString() {
		// TODO Auto-generated method stub
		return "" + mCurrentSourceIsLocked + mCurrentChannelIsLocked;
	}

	@Override
	public boolean equals(Object o) {
		// TODO Auto-generated method stub
		boolean flag = false;
		LockKey lk = (LockKey) o;
		if (lk.isCurChannelLock() == this.isCurChannelLock()
				&& lk.isCurSourceLock() == this.isCurSourceLock()) {
			flag = true;
		}
		return flag;
	}


  @Override
	public int hashCode() {
		
		return super.hashCode();
	}

	public void copy(LockKey lk) {

		mCurrentChannelIsLocked = lk.isCurChannelLock();
		mCurrentSourceIsLocked = lk.isCurSourceLock();

	}
}
