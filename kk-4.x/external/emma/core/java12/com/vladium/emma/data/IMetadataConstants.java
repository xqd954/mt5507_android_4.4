/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/core/java12/com/vladium/emma/data/IMetadataConstants.java#1 $
 */
package com.vladium.emma.data;

// ----------------------------------------------------------------------------
/**
 * @author Vlad Roubtsov, 2003
 */
public
interface IMetadataConstants
{
    // public: ................................................................
    
    int METHOD_NO_LINE_NUMBER_TABLE = 0x01;
    int METHOD_ABSTRACT_OR_NATIVE   = 0x02;
    int METHOD_EXCLUDED             = 0x04;
    int METHOD_ADDED                = 0x08;
    
    int METHOD_NO_BLOCK_DATA = (METHOD_ABSTRACT_OR_NATIVE | METHOD_EXCLUDED | METHOD_ADDED);
    int METHOD_NO_LINE_DATA = (METHOD_NO_LINE_NUMBER_TABLE | METHOD_NO_BLOCK_DATA);
    

} // end of interface
// ----------------------------------------------------------------------------