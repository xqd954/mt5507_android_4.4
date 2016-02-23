package com.mediatek.tvcommon;

import java.util.List;
import java.util.ListIterator;

/**
 * ChannelQueue be used to maintain channels list, both atv and dtv own a unique
 * ChannelQueue object. {@code pos} is the index of current selected channel,
 * {@code lastPosition} is the last index of current channel. Make sure the
 * value of {@code pos} be not equal to {@code lastPosition} except -1.
 */
final class ChannelQueue {

	private final List<TVChannel> channels;

	private int pos = -1;
	private int lastPosition = -1;

	ChannelQueue(List<TVChannel> channels) {
		this.channels = channels;
	}

	public synchronized TVChannel head() {
		if (pos == 0) {
			return channels.get(0);
		}
		if (channels.size() > 0) {
			lastPosition = pos;
			pos = 0;
			return channels.get(pos);
		}
		return null;
	}

	public synchronized TVChannel tail() {
		if (pos == channels.size() - 1) {
			return channels.get(pos);
		}
		if (channels.size() > 0) {
			lastPosition = pos;
			pos = channels.size() - 1;
			return channels.get(pos);
		}
		return null;
	}

	public synchronized boolean hasNext() {
		return channels.size() > 0;
	}

	public synchronized boolean hasPrevious() {
		return channels.size() > 0;
	}

	public synchronized TVChannel next() {
		if (channels.size() > 0) {
			if (pos == -1) {
				pos = 0;
			} else {
				if (channels.size() != 1) {
					lastPosition = pos;
					pos = (pos + 1) % channels.size();
				}
			}
			return channels.get(pos);
		}
		return null;
	}

	public synchronized TVChannel previous() {
		if (channels.size() > 0) {
			if (pos == -1) {
				pos = channels.size() - 1;
			} else {
				if (channels.size() != 1) {
					lastPosition = pos;
					if (--pos == -1) {
						pos = channels.size() - 1;
					}
				}
			}
			return channels.get(pos);
		}
		return null;
	}

	public synchronized TVChannel getPreSelected() {
		TVChannel ch = null;
		if (lastPosition != -1) {
			int temp = pos;
			pos = lastPosition;
			lastPosition = temp;
			ch = channels.get(pos);
		}
		return ch;
	}

	public synchronized TVChannel getChannel() {
		TVChannel ch = null;
		if (pos != -1) {
			ch = channels.get(pos);
		}
		return ch;
	}

	public synchronized TVChannel findChannel(TVChannel ch) {
		int index = channels.indexOf(ch);
		if (index != -1) {
			if (index != pos) {
				lastPosition = pos;
				pos = index;
			}
			return ch;
		}
		return null;
	}

	public synchronized TVChannel findChannelByNumber(int channelNum) {

		if (channels.size() <= 0) {
			return null;
		}

		int low = 0;
		int high = channels.size() - 1;
		// Binary search
		while (low <= high) {
			int mid = (low + high) / 2;
			TVChannel ch = channels.get(mid);
			int num = ch.getChannelNum();
			if (channelNum == num) {
				if (mid != pos) {
					lastPosition = pos;
					pos = mid;
				}
				return ch;
			} else if (channelNum < num) {
				high = mid - 1;
			} else {
				low = mid + 1;
			}
		}

		return null;
	}

	/**
	 * Remove the current Channel.
	 * 
	 */
	public synchronized void remove() {
		if (pos == -1) {
			return;
		}
		channels.remove(pos);
		// make sure pos and lastPos be not equal except -1.
		pos = -1;
	}

	/**
	 * Replaces the current channel with specified object
	 */
	public synchronized void set(TVChannel object) {
		if (pos == -1) {
			return;
		}
		channels.set(pos, object);
	}

	public synchronized List<TVChannel> getChannels() {
		return channels;
	}

	public synchronized void add(TVChannel ch) {
		int index = -1;
		ListIterator<TVChannel> itr = channels.listIterator();
		while (itr.hasNext()) {
			TVChannel each = itr.next();
			if (each.getChannelNum() > ch.getChannelNum()) {
				if (itr.hasPrevious()) {
					itr.previous();
				}
				itr.add(ch);
				index = channels.indexOf(ch);
				break;
			}
		}
		if (index == -1) {
			channels.add(ch);
		} else {
			if (pos >= index) {
				pos++;
			}

			if (lastPosition >= index) {
				lastPosition++;
			}
		}

		/*
		 * int low = 0; int high = channels.size() - 1; int mid = 0; // Binary
		 * search while (low < high) { mid = (low + high) / 2; if
		 * (channels.get(mid).getChannelNum() >= ch.getChannelNum()) { high =
		 * mid - 1; } else { low = mid + 1; } }
		 * 
		 * if (pos >= mid) { pos++; }
		 * 
		 * if (lastPosition >= mid) { lastPosition++; } channels.add(mid, ch);
		 */
	}

	public synchronized boolean remove(TVChannel ch) {
		int index = channels.indexOf(ch);
		if (index != -1) {
			if (pos == index) {
				pos = -1;
			} else if (pos > index) {
				pos--;
			}

			if (lastPosition == index) {
				lastPosition = -1;
			} else if (lastPosition > index) {
				lastPosition--;
			}
			channels.remove(ch);
			return true;
		}
		return false;
	}

}
