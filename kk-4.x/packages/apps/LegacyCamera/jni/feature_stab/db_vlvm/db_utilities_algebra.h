/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/packages/apps/LegacyCamera/jni/feature_stab/db_vlvm/db_utilities_algebra.h#1 $ */

#ifndef DB_UTILITIES_ALGEBRA
#define DB_UTILITIES_ALGEBRA

#include "db_utilities.h"



/*****************************************************************
*    Lean and mean begins here                                   *
*****************************************************************/
/*!
 * \defgroup LMAlgebra (LM) Algebra utilities
 */
/*\{*/

inline void db_HomogenousNormalize3(double *x)
{
    db_MultiplyScalar3(x,db_SafeSqrtReciprocal(db_SquareSum3(x)));
}

/*\}*/

#endif /* DB_UTILITIES_ALGEBRA */
