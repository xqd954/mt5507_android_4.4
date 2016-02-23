/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/core/java12/com/vladium/jcd/cls/constant/CONSTANT_literal_info.java#1 $
 */
package com.vladium.jcd.cls.constant;

// ----------------------------------------------------------------------------
/**
 * Abstract base for all CONSTANT_XXX_info structures representing literal values
 * in the constant pool.
 * 
 * @see CONSTANT_Integer_info
 * @see CONSTANT_Long_info
 * @see CONSTANT_Float_info
 * @see CONSTANT_Double_info
 * @see CONSTANT_String_info
 * 
 * @author (C) 2001, Vlad Roubtsov
 */
public
abstract class CONSTANT_literal_info extends CONSTANT_info
{
    // public: ................................................................
    
    // Cloneable: inherited clone() is Ok
    
    // protected: .............................................................

    
    protected CONSTANT_literal_info ()
    {
    }
    
    // package: ...............................................................

    // private: ...............................................................

} // end of class
// ----------------------------------------------------------------------------
