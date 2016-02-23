/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/core/java12/com/vladium/emma/report/IReportGenerator.java#1 $
 */
package com.vladium.emma.report;

import com.vladium.util.IProperties;
import com.vladium.emma.EMMARuntimeException;
import com.vladium.emma.data.IMetaData;
import com.vladium.emma.data.ICoverageData;

// ----------------------------------------------------------------------------
/**
 * @author Vlad Roubtsov, (C) 2003
 */
public
interface IReportGenerator
{
    // public: ................................................................
    
    String getType ();
    
    // TODO: make sure reporters are reusable
    
    void process (IMetaData mdata, ICoverageData cdata, SourcePathCache cache, IProperties parameters)
        throws EMMARuntimeException;
    
    void cleanup ();
        
} // end of interface
// ----------------------------------------------------------------------------