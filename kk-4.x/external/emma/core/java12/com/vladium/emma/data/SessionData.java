/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/core/java12/com/vladium/emma/data/SessionData.java#1 $
 */
package com.vladium.emma.data;

import java.io.Serializable;

// ----------------------------------------------------------------------------
/**
 * @author Vlad Roubtsov, (C) 2003
 */
public
final class SessionData implements ISessionData, Serializable 
{
    // public: ................................................................
    
    public IMetaData getMetaData ()
    {
        return m_mdata;
    }
    
    public ICoverageData getCoverageData ()
    {
        return m_cdata;
    }
    
    // protected: .............................................................

    // package: ...............................................................
    
    
    public SessionData (final IMetaData mdata, final ICoverageData cdata)
    {
        if (mdata == null) throw new IllegalArgumentException ("null input: mdata");
        if (cdata == null) throw new IllegalArgumentException ("null input: cdata");
        
        m_mdata = mdata;
        m_cdata = cdata;
    }
    
    // private: ...............................................................
    
    
    private final IMetaData m_mdata;
    private final ICoverageData m_cdata;

} // end of class
// ----------------------------------------------------------------------------