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
// $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/libcore/luni/src/main/java/javax/xml/xpath/XPathFunction.java#1 $

package javax.xml.xpath;

import java.util.List;

/**
 * <p><code>XPathFunction</code> provides access to XPath functions.</p>
 *
 * <p>Functions are identified by QName and arity in XPath.</p>
 *
 * @author  <a href="mailto:Norman.Walsh@Sun.com">Norman Walsh</a>
 * @author  <a href="mailto:Jeff.Suttor@Sun.com">Jeff Suttor</a>
 * @version $Revision: #1 $, $Date: 2014/10/13 $
 * @since 1.5
 */
public interface XPathFunction {
  /**
   * <p>Evaluate the function with the specified arguments.</p>
   *
   * <p>To the greatest extent possible, side-effects should be avoided in the
   * definition of extension functions. The implementation evaluating an
   * XPath expression is under no obligation to call extension functions in
   * any particular order or any particular number of times.</p>
   *
   * @param args The arguments, <code>null</code> is a valid value.
   *
   * @return The result of evaluating the <code>XPath</code> function as an <code>Object</code>.
   *
   * @throws XPathFunctionException If <code>args</code> cannot be evaluated with this <code>XPath</code> function.
   */
  public Object evaluate(List args)
    throws XPathFunctionException;
}

