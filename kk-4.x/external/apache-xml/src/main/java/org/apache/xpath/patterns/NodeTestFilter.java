/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the  "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/apache-xml/src/main/java/org/apache/xpath/patterns/NodeTestFilter.java#1 $
 */
package org.apache.xpath.patterns;

/**
 * This interface should be implemented by Nodes and/or iterators,
 * when they need to know what the node test is before they do
 * getNextChild, etc.
 */
public interface NodeTestFilter
{

  /**
   * Set the node test for this filter.
   *
   *
   * @param nodeTest Reference to a NodeTest that may be used to predetermine 
   *                 what nodes to return.
   */
  void setNodeTest(NodeTest nodeTest);
}
