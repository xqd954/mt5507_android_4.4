/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/core/java12/com/vladium/util/ClassLoadContext.java#1 $
 */
package com.vladium.util;

// ----------------------------------------------------------------------------
/**
 * Information context for {@link IClassLoadStrategy#getClassLoader(ClassLoadContext)}.
 * 
 * @author Vlad Roubtsov, (C) 2003
 */
public
class ClassLoadContext
{
    // public: ................................................................
    
    
    /**
     * Returns the class representing the caller of {@link ClassLoaderResolver}
     * API. Can be used to retrieve the caller's classloader etc (which may be
     * different from the ClassLoaderResolver's own classloader) ['null' if caller
     * resolver could be instantiated due to security restrictions]. 
     */
    public final Class getCallerClass ()
    {
        return m_caller;
    }
    
    // protected: .............................................................

    // package: ...............................................................
    
    
    /**
     * This constructor is package-private to restrict instantiation to
     * {@link ClassLoaderResolver} only.
     * 
     * @param caller [can be null]
     */
    ClassLoadContext (final Class caller)
    {
        m_caller = caller;
    }
    
    // private: ...............................................................
    

    private final Class m_caller;

} // end of class
// ----------------------------------------------------------------------------