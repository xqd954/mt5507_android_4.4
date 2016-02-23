/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/libcore/luni/src/main/java/javax/xml/validation/SchemaFactoryLoader.java#1 $

package javax.xml.validation;

/**
 * <p>This class was removed from JAXP 1.3 before it was finalized but
 * was mistakenly included in Java 5. It only exists now for compatibility
 * reasons. Applications should avoid using it.</p>
 */
public abstract class SchemaFactoryLoader {

    protected SchemaFactoryLoader() {}
    public abstract SchemaFactory newFactory(String schemaLanguage);

} // SchemaFactoryLoader
