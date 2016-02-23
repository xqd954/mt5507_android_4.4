/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/core/java12/com/vladium/jcd/cls/constant/CONSTANT_InterfaceMethodref_info.java#1 $
 */
package com.vladium.jcd.cls.constant;

import java.io.IOException;

import com.vladium.jcd.lib.UDataInputStream;

// ----------------------------------------------------------------------------
/**
 * This structure is used in the constant pool to represent dynamic references
 * to interface methods. The class_index item of a CONSTANT_InterfaceMethodref_info
 * structure must be an interface type that declares the given method. 
 * 
 * @see CONSTANT_ref_info
 * @see CONSTANT_Fieldref_info
 * @see CONSTANT_Methodref_info
 * 
 * @author (C) 2001, Vlad Roubtsov
 */
public
final class CONSTANT_InterfaceMethodref_info extends CONSTANT_ref_info
{
    // public: ................................................................

    public static final byte TAG = 11;
    
    
    public CONSTANT_InterfaceMethodref_info (final int class_index, final int name_and_type_index)
    {
        super (class_index, name_and_type_index);
    }

    public final byte tag ()
    {
        return TAG;
    }
    
    // Visitor:
    
    public Object accept (final ICONSTANTVisitor visitor, final Object ctx)
    {
        return visitor.visit (this, ctx);
    }
        
    public String toString ()
    {
        return "CONSTANT_InterfaceMethodref: [class_index = " + m_class_index + ", name_and_type_index = " + m_name_and_type_index + ']';
    }
    
    // Cloneable: inherited clone() is Ok
    
    // protected: .............................................................

    
    protected CONSTANT_InterfaceMethodref_info (final UDataInputStream bytes) throws IOException
    {
        super (bytes);
    }
    
    // package: ...............................................................

    // private: ...............................................................

} // end of class
// ----------------------------------------------------------------------------
