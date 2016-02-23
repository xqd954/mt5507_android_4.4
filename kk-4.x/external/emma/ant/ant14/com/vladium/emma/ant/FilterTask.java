/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/ant/ant14/com/vladium/emma/ant/FilterTask.java#1 $
 */
package com.vladium.emma.ant;

import java.io.File;

import com.vladium.util.Strings;
import com.vladium.emma.instr.FilterCfg;
import com.vladium.emma.instr.FilterCfg.filterElement;

import org.apache.tools.ant.BuildException;

// ----------------------------------------------------------------------------
/**
 * @author Vlad Roubtsov, (C) 2003
 */
public
abstract class FilterTask extends NestedTask
{
    // public: ................................................................


    public void init () throws BuildException
    {
        super.init ();
        
        m_filterCfg = new FilterCfg (this);
    }

    
    // filter attribute/element:
    
    public final void setFilter (final String filter)
    {
        m_filterCfg.setFilter (filter);
    }
    
    public final filterElement createFilter ()
    {
        return m_filterCfg.createFilter ();
    }
    
    // protected: .............................................................
    
    
    protected FilterTask (final SuppressableTask parent)
    {
        super (parent);
    }
    
    
    protected final String [] getFilterSpecs ()
    {
        return m_filterCfg.getFilterSpecs ();
    }
        

    protected static final String COMMA               = ",";
    protected static final String COMMA_DELIMITERS    = COMMA + Strings.WHITE_SPACE;
    protected static final String PATH_DELIMITERS     = COMMA.concat (File.pathSeparator);
    
    // package: ...............................................................
    
    // private: ...............................................................


    private /*final*/ FilterCfg m_filterCfg;    

} // end of class
// ----------------------------------------------------------------------------