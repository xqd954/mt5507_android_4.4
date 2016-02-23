/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/ant/ant14/com/vladium/emma/data/mergeTask.java#1 $
 */
package com.vladium.emma.data;

import java.io.File;

import org.apache.tools.ant.BuildException;

import com.vladium.emma.ant.FileTask;
import com.vladium.emma.ant.SuppressableTask;

// ----------------------------------------------------------------------------
/**
 * @author Vlad Roubtsov, (C) 2003
 */
public
final class mergeTask extends FileTask 
{
    // public: ................................................................
    
    
    public mergeTask (final SuppressableTask parent)
    {
        super (parent);
    }
    
    public void execute () throws BuildException
    {
        if (isEnabled ())
        {
            String [] files = getDataPath (true);
            if ((files == null) || (files.length == 0))
                throw (BuildException) newBuildException (getTaskName ()
                    + ": no valid input data files have been specified", location).fillInStackTrace ();
            
            final MergeProcessor processor = MergeProcessor.create ();
            
            processor.setDataPath (files); files = null;
            processor.setSessionOutFile (m_outFile != null ? m_outFile.getAbsolutePath () : null);
            processor.setPropertyOverrides (getTaskSettings ());
            
            processor.run ();
        }
    }
    

    // mergefile|tofile|outfile|file attribute:
    
    public void setMergefile (final File file)
    {
        if (m_outFile != null)
            throw (BuildException) newBuildException (getTaskName ()
                + ": merge data file attribute already set", location).fillInStackTrace ();
            
        m_outFile = file;
    }
    
    public void setOutfile (final File file)
    {
        if (m_outFile != null)
            throw (BuildException) newBuildException (getTaskName ()
                + ": merge data file attribute already set", location).fillInStackTrace ();
            
        m_outFile = file;
    }

    public void setTofile (final File file)
    {
        if (m_outFile != null)
            throw (BuildException) newBuildException (getTaskName ()
                + ": merge data file attribute already set", location).fillInStackTrace ();
            
        m_outFile = file;
    }
    
    public void setFile (final File file)
    {
        if (m_outFile != null)
            throw (BuildException) newBuildException (getTaskName ()
                + ": merge data file attribute already set", location).fillInStackTrace ();
            
        m_outFile = file;
    }
    
        
    // protected: .............................................................

    // package: ...............................................................
    
    // private: ...............................................................
    
    
    private File m_outFile;

} // end of class
// ----------------------------------------------------------------------------