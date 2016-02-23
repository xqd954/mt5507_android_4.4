/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/core/java12/emma.java#1 $
 */
import com.vladium.emma.IAppConstants;
import com.vladium.emma.Command;
import com.vladium.emma.EMMARuntimeException;

// ----------------------------------------------------------------------------
/**
 * @author Vlad Roubtsov, (C) 2003
 */
public
final class emma
{
    // public: ................................................................
    
    // TODO: set m_out consistently with LoggerInit    
    
    public static void main (final String [] args)
        throws EMMARuntimeException
    {
        // TODO: proper usage, arg validation, etc
        
        if ((args.length == 0) || args [0].startsWith ("-h"))
        {
            System.out.println (USAGE);
            return;
        }
        
        final String commandName = args [0];
        final String [] commandArgs = new String [args.length - 1];
        System.arraycopy (args, 1, commandArgs, 0, commandArgs.length);
        
        final Command command = Command.create (commandName, "emma ".concat (commandName), commandArgs);
        command.run ();
    }
    
    // protected: .............................................................

    // package: ...............................................................
    
    // private: ...............................................................
    
    
    private static final String EOL = System.getProperty ("line.separator", "\n"); 
    
    private static final String USAGE =
    "emma usage: emma <command> [command options]," + EOL +
    "  where <command> is one of:" + EOL +
    EOL +
    "   run     application runner {same as 'emmarun' tool};" + EOL +
    "   instr   offline instrumentation processor;" + EOL +
    "   report  offline report generator;" + EOL +
    "   merge   offline data file merge processor." + EOL +
    EOL +
    "  {use '<command> -h' to see usage help for a given command}" + EOL +
    EOL +
    IAppConstants.APP_USAGE_BUILD_ID;

} // end of class
// ----------------------------------------------------------------------------