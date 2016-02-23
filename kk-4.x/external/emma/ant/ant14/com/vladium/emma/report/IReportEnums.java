/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/ant/ant14/com/vladium/emma/report/IReportEnums.java#1 $
 */
package com.vladium.emma.report;

import com.vladium.emma.report.IReportProperties;
import org.apache.tools.ant.types.EnumeratedAttribute;

// ----------------------------------------------------------------------------
/**
 * @author Vlad Roubtsov, (C) 2003
 */
public
interface IReportEnums
{
    // public: ................................................................


    final class TypeAttribute extends EnumeratedAttribute
    {
        public String [] getValues ()
        {
            return VALUES;
        }
        
        // TODO: keep this enum in a centralized location
        private static final String [] VALUES = new String []
        {
            "txt",
            "html",
            "lcov",
            "xml",
        };

    } // end of nested class
    

    final class DepthAttribute extends EnumeratedAttribute
    {
        public String [] getValues ()
        {
            return VALUES;
        }
        
        // TODO: keep this enum in a centralized location
        private static final String [] VALUES = new String []
        {
            IReportProperties.DEPTH_ALL,
            IReportProperties.DEPTH_PACKAGE,
            IReportProperties.DEPTH_SRCFILE,
            IReportProperties.DEPTH_CLASS,
            IReportProperties.DEPTH_METHOD,
        };

    } // end of nested class
    

    final class ViewTypeAttribute extends EnumeratedAttribute
    {
        public String [] getValues ()
        {
            return VALUES;
        }
        
        // TODO: keep this enum in a centralized location
        private static final String [] VALUES = new String []
        {
            IReportProperties.SRC_VIEW,
            IReportProperties.CLS_VIEW,
        };

    } // end of nested class
    
    
    static final class UnitsTypeAttribute extends EnumeratedAttribute
    {
        public String [] getValues ()
        {
            return VALUES;
        }
        
        // TODO: keep this enum in a centralized location
        private static final String [] VALUES = new String []
        {
            IReportProperties.INSTR_UNITS,
            IReportProperties.COUNT_UNITS,
        };

    } // end of nested class

} // end of interface
// ----------------------------------------------------------------------------
