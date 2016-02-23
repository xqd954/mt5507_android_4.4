/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/ant/ant14/com/vladium/emma/ANTMain.java#1 $
 */
package com.vladium.emma;

import com.vladium.emma.IAppConstants;

// ----------------------------------------------------------------------------
/**
 * @author Vlad Roubtsov, (C) 2004
 */
public
final class ANTMain
{
    // public: ................................................................
    
    
    public static void main (final String [] args)
    {
        System.out.println ("this jar contains ANT task definitions for " + IAppConstants.APP_NAME
            + " and is not meant to be executable");
            
        System.out.println ();
        System.out.println (IAppConstants.APP_USAGE_BUILD_ID);
    } 
    
    // protected: .............................................................

    // package: ...............................................................
    
    // private: ...............................................................

} // end of class
// ----------------------------------------------------------------------------