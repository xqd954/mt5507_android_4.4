/*
 * Copyright (c) 2011 David Kocher. All rights reserved.
 * Please refer to the LICENSE.txt for licensing details.
 */

package ch.ethz.ssh2.channel;

import java.io.IOException;

/**
 * @version $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/ganymed-ssh2/src/main/java/ch/ethz/ssh2/channel/ChannelClosedException.java#1 $
 */
public class ChannelClosedException extends IOException
{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public ChannelClosedException(String s)
	{
		super(s);
	}
}
