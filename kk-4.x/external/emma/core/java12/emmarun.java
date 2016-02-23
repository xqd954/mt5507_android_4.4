/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/core/java12/emmarun.java#1 $
 */
import com.vladium.emma.Command;
import com.vladium.emma.EMMARuntimeException;

// ----------------------------------------------------------------------------
/**
 * @author Vlad Roubtsov, (C) 2003
 */
public
final class emmarun
{
    // public: ................................................................
    
    // TODO: set m_out consistently with LoggerInit    
    
    public static void main (final String [] args)
        throws EMMARuntimeException
    {
        final Command command = Command.create ("run", emmarun.class.getName (), args);
        command.run ();
    }
    
    // protected: .............................................................

    // package: ...............................................................
    
    // private: ...............................................................

} // end of class
// ----------------------------------------------------------------------------