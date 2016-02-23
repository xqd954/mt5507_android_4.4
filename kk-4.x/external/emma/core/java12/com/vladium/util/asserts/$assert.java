/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/core/java12/com/vladium/util/asserts/&#36;assert.java#1 $
 */
package com.vladium.util.asserts;

// ----------------------------------------------------------------------------
/**
 * @author Vlad Roubtsov, (C) 2001
 */
public
abstract class $assert
{
    // public: ................................................................
    
    
    // TODO: set to false for release
    
    /**
     * Global compile time assertion flag.
     */
    public static final boolean ENABLED = false;
    
    /**
     * 
     * @param condition
     * @param msg
     */
    public static void ASSERT (final boolean condition, final String msg)
    {
        if (ENABLED)
        {
            if (! condition) throw new RuntimeException (msg);
        }
    }
    
    public static void ASSERT (final boolean condition)
    {
        if (ENABLED)
        {
            if (! condition) throw new RuntimeException ("ASSERTION FAILURE");
        }
    }
    
    
    // protected: .............................................................

    // package: ...............................................................
    
    // private: ...............................................................
    
    
    private $assert () {} // prevent subclassing

} // end of class
// ----------------------------------------------------------------------------
