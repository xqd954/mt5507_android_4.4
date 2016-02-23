/*
 * Copyright (c) 2006-2011 Christian Plattner.
 * All rights reserved.
 * Please refer to the LICENSE.txt for licensing details.
 */
package ch.ethz.ssh2.util;

import java.io.UnsupportedEncodingException;

/**
 * @author Christian Plattner
 * @version $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/ganymed-ssh2/src/main/java/ch/ethz/ssh2/util/StringEncoder.java#1 $
 */
public class StringEncoder
{
	public static byte[] GetBytes(String data)
	{
        try {
            return data.getBytes("UTF-8");
        }
        catch(UnsupportedEncodingException e) {
            throw new RuntimeException(e);
        }
    }

	public static String GetString(byte[] data)
	{
		return GetString(data, 0, data.length);
	}

	public static String GetString(byte[] data, int off, int len)
	{
        try {
            return new String(data, off, len, "UTF-8");
        }
        catch(UnsupportedEncodingException e) {
            throw new RuntimeException(e);
        }
    }
}
