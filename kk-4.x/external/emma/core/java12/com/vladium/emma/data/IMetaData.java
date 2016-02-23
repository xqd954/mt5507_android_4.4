/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/core/java12/com/vladium/emma/data/IMetaData.java#1 $
 */
package com.vladium.emma.data;

import java.util.Iterator;

// ----------------------------------------------------------------------------
/**
 * @author Vlad Roubtsov, (C) 2003
 */
public
interface IMetaData extends IMergeable
{
    // public: ................................................................
    
    Object lock ();
    
    IMetaData shallowCopy ();
    
    CoverageOptions getOptions ();
    
    int size ();
    boolean hasSrcFileData ();
    boolean hasLineNumberData ();
    
//    boolean hasDescriptor (ClassDescriptor cls);
    boolean hasDescriptor (String classVMName);
    
    Iterator /* ClassDescripor */ iterator ();
    
    //void setImmutable (); // TODO: this only disables add(); rename
    boolean add (ClassDescriptor cls, boolean overwrite);    

} // end of interface
// ----------------------------------------------------------------------------
