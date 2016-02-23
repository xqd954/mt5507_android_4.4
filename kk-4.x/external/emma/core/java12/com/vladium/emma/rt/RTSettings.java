/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/core/java12/com/vladium/emma/rt/RTSettings.java#1 $
 */
package com.vladium.emma.rt;

// ----------------------------------------------------------------------------
/**
 * Conceptually, this class is an extention of class RT. This is a separate class,
 * however, to help load RT in a lazy manner.
 * 
 * @author Vlad Roubtsov, (C) 2003
 */
public
abstract class RTSettings
{
    // public: ................................................................
    
    
    public static synchronized boolean isStandaloneMode ()
    {
        return ! s_not_standalone;
    }
    
    public static synchronized void setStandaloneMode (final boolean standalone)
    {
        s_not_standalone = ! standalone;
    }
    
    // protected: .............................................................

    // package: ...............................................................
    
    // private: ...............................................................
    
    
    private RTSettings () {} // prevent subclassing
        
    private static boolean s_not_standalone;

} // end of class
// ----------------------------------------------------------------------------