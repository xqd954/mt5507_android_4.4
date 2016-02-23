/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/core/java12/com/vladium/emma/report/IReportDataView.java#1 $
 */
package com.vladium.emma.report;

// ----------------------------------------------------------------------------
/**
 * @author Vlad Roubtsov, (C) 2003
 */
public interface IReportDataView
{
    // public: ................................................................
    

    int HIER_CLS_VIEW   = 0;
    int HIER_SRC_VIEW   = 1;   
    
    IItem getRoot ();

} // end of interface
// ----------------------------------------------------------------------------