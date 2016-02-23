/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/core/java12/com/vladium/jcd/compiler/IClassFormatOutput.java#1 $
 */
package com.vladium.jcd.compiler;

import java.io.IOException;

import com.vladium.jcd.lib.UDataOutputStream;

// ----------------------------------------------------------------------------
/**
 * Our class tables and nested table structures support this interface so that
 * they could be serialized into binary format as per .class layout format.
 * 
 * @author (C) 2001, Vlad Roubtsov
 */
public
interface IClassFormatOutput
{
    // public: ................................................................

    void writeInClassFormat (UDataOutputStream out) throws IOException;
    
} // end of interface
// ----------------------------------------------------------------------------
