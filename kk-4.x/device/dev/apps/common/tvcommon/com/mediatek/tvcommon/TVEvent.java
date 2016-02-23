package com.mediatek.tvcommon;

import com.mediatek.tv.model.EventInfo;

/**
 * An instance of TVEvent represent set of informations of a specified digital
 * TV program.
 */
public class TVEvent implements Comparable<TVEvent> {

	protected TVChannel channel;
	private EventInfo rawEvent;
	private Long offset = 0L;

	protected void setOffset(Long offset) {
		this.offset = offset;
	}

	protected Long getOffset() {
		return this.offset;
	}

	TVEvent(EventInfo raw, TVChannel ch) {
		this.rawEvent = raw;
		this.channel = ch;
	}

	EventInfo getRawEvent() {
		return rawEvent;
	}

	/**
	 * @return the channel which this event attached
	 */
	public TVChannel getChannel() {
		return channel;
	}

	/**
	 * @return the start time of this event
	 */
	public long getStartTime() {
		return rawEvent.getStartTime() * 1000;
	}

	/**
	 * @return the duration of this event
	 */
	public long getDuration() {
		return rawEvent.getDuration() * 1000;
	}

	/**
	 * @return detail informations
	 */
	public String getDetail() {
		return rawEvent.getEventDetail();
	}

	/**
	 * @return title of this event
	 */
	public String getTitle() {
		return rawEvent.getEventTitle();
	}

	boolean[][] cateMask = null;

	public boolean[][] getCategaryMask() {
		if (cateMask == null) {
			cateMask = new boolean[0x10][0x10];

			for (int i = 0; i < cateMask.length; i++) {
				for (int j = 0; j < cateMask[i].length; j++) {
					cateMask[i][j] = false;
				}
			}

			int[] rawCategary = rawEvent.getEventCategory();
			// TVManager does not contain length in the array.........
			for (int k = 0; k < rawEvent.getEventCategoryNum(); k++) {
				int c = rawCategary[k];
				int left = c >> 4;
				int right = c & 0xf;
				cateMask[left][right] = true;
			}
		}
		return cateMask;
	}

	/**
	 * @return the the first main category
	 */
	public int findFirstMainCategary() {
		boolean[][] mask = getCategaryMask();
		for (int i = 0; i < mask.length; i++) {
			for (int j = 0; j < mask[i].length; j++) {
				if (mask[i][j]) {
					return i;
				}
			}
		}
		return -1;
	}

	public boolean isCategary(int cont1, int cont2) {
		if (cont1 > 0xf || cont2 > 0xf || cont1 < 0 || cont2 < 0) {
			return false;
		}
		return getCategaryMask()[cont1][cont2];
	}

	@Override
	public int compareTo(TVEvent another) {
		if (this.getStartTime() < another.getStartTime()) {
			return -1;
		} else if (this.getStartTime() > another.getStartTime()) {
			return 1;
		} else {
			return 0;
		}
	}

	@Override
	public boolean equals(Object o) {
		if (o == null) {
			return false;
		}
		if (!(o instanceof TVEvent)) {
			return false;
		}
		TVEvent event = (TVEvent) o;
		if (event == this)
			return true;
		return (event.rawEvent == rawEvent || (event.rawEvent != null && rawEvent
				.equals(event.rawEvent)))
				&& (channel == event.channel || (event.channel != null && channel
						.equals(event.channel))) && offset == event.offset;
	}

	@Override
	public int hashCode() {
		int result = 29;
		result = 17 * result + (channel != null ? channel.hashCode() : 0);
		result = 17 * result + (rawEvent != null ? rawEvent.hashCode() : 0);
		result = 17 * result + (int) (offset ^ (offset >>> 32));
		return result;
	}

}
