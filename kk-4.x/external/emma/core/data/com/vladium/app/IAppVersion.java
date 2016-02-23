/* Copyright (C) 2003 Vladimir Roubtsov. All rights reserved.
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Common Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/cpl-v10.html
 * 
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/emma/core/data/com/vladium/app/IAppVersion.java#1 $
 */
package com.vladium.app;

// ----------------------------------------------------------------------------
/**
 * @author Vlad Roubtsov, (C) 2003
 */
public
interface IAppVersion
{
    // public: ................................................................


    // filled in by the build:
    
    int APP_MAJOR_VERSION = /* @APP_MAJOR_VERSION@ */ 0;
    int APP_MINOR_VERSION = /* @APP_MINOR_VERSION@ */ 0;

    int APP_BUILD_ID = /* @APP_BUILD_ID@ */ 0;
    String APP_BUILD_RELEASE_TAG = /* @APP_BUILD_RELEASE_TAG@ */ " (unsupported private build)";
    String APP_BUILD_DATE = /* @APP_BUILD_DATE@ */ "unknown";

    String APP_BUG_REPORT_LINK = /* @APP_BUG_REPORT_LINK@ */ "this private build is unsupported";
    String APP_HOME_SITE_LINK = /* @APP_HOME_SITE_LINK@ */ "this private build is unsupported";

    // derived properties [must be compile-time consts]:

    String APP_BUILD_ID_AND_TAG = "" + APP_BUILD_ID + APP_BUILD_RELEASE_TAG;
    
    String APP_VERSION = "" + APP_MAJOR_VERSION + "." + APP_MINOR_VERSION;
    String APP_VERSION_WITH_BUILD_ID_AND_TAG = APP_VERSION + "." + APP_BUILD_ID_AND_TAG;

} // end of interface
// ----------------------------------------------------------------------------
