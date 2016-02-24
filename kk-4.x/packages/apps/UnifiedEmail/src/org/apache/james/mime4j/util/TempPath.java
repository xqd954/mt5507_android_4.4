/****************************************************************
 * Licensed to the Apache Software Foundation (ASF) under one   *
 * or more contributor license agreements.  See the NOTICE file *
 * distributed with this work for additional information        *
 * regarding copyright ownership.  The ASF licenses this file   *
 * to you under the Apache License, Version 2.0 (the            *
 * "License"); you may not use this file except in compliance   *
 * with the License.  You may obtain a copy of the License at   *
 *                                                              *
 *   http://www.apache.org/licenses/LICENSE-2.0                 *
 *                                                              *
 * Unless required by applicable law or agreed to in writing,   *
 * software distributed under the License is distributed on an  *
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY       *
 * KIND, either express or implied.  See the License for the    *
 * specific language governing permissions and limitations      *
 * under the License.                                           *
 ****************************************************************/

package org.apache.james.mime4j.util;

import java.io.IOException;

/**
 * 
 * @version $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/packages/apps/UnifiedEmail/src/org/apache/james/mime4j/util/TempPath.java#1 $
 */
public interface TempPath {
    TempPath createTempPath() throws IOException;
    TempPath createTempPath(String prefix) throws IOException;
    
    /**
     * Creates a new temporary file. Wheter it will be be created in memory
     * or on disk is up to to the implementation. 
     * The prefix will be empty and the suffix will be 
     * <code>.tmp</code> if created on disk. 
     * 
     * @return the temporary file.
     */
    TempFile createTempFile() throws IOException;
    
    /**
     * Creates a new temporary file. Wheter it will be be created in memory
     * or on disk is up to to the implementation. 
     * The prefix and suffix can be set by the user. 
     * 
     * @param prefix the prefix to use. <code>null</code> gives no prefix.
     * @param suffix the suffix to use. <code>null</code> gives 
     *        <code>.tmp</code>.
     * @return the temporary file.
     */
    TempFile createTempFile(String prefix, String suffix) throws IOException;
    
    /**
     * Creates a new temporary file. Wheter it will be be created in memory
     * or on disk can be specified using the <code>allowInMemory</code> 
     * parameter. If the implementation doesn't support in-memory files
     * the new file will be created on disk. 
     * The prefix and suffix can be set by the user. 
     * 
     * @param prefix the prefix to use. <code>null</code> gives no prefix.
     * @param suffix the suffix to use. <code>null</code> gives 
     *        <code>.tmp</code>.
     * @param allowInMemory if <code>true</code> the file MIGHT be created in 
     *        memory if supported by the implentation. If <code>false</code> the
     *        file MUST be created on disk.
     * @return the temporary file.
     */
    TempFile createTempFile(String prefix, String suffix, 
                            boolean allowInMemory) throws IOException;
    String getAbsolutePath();
    void delete();
}
