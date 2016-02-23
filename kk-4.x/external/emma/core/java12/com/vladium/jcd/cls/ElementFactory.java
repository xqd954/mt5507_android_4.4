/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/core/java12/com/vladium/jcd/cls/ElementFactory.java#1 $
 */
package com.vladium.jcd.cls;

// ----------------------------------------------------------------------------
/**
 * @author Vlad Roubtsov, (C) 2003
 */
public
abstract class ElementFactory
{
    // public: ................................................................
    
    
    public static IAttributeCollection newAttributeCollection (final int capacity)
    {
        return new AttributeCollection (capacity);
    }

    public static IConstantCollection newConstantCollection (final int capacity)
    {
        return new ConstantCollection (capacity);
    }

    public static IFieldCollection newFieldCollection (final int capacity)
    {
        return new FieldCollection (capacity);
    }

    public static IInterfaceCollection newInterfaceCollection (final int capacity)
    {
        return new InterfaceCollection (capacity);
    }

    public static IMethodCollection newMethodCollection (final int capacity)
    {
        return new MethodCollection (capacity);
    }
    
    // protected: .............................................................

    // package: ...............................................................
    
    // private: ...............................................................

} // end of class
// ----------------------------------------------------------------------------
