/*
 $License:
    Copyright (C) 2012 InvenSense Corporation, All Rights Reserved.
 $
 */

/*******************************************************************************
 *
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/hardware/invensense/65xx/libsensors_iio/software/core/mllite/linux/ml_load_dmp.h#1 $
 *
 ******************************************************************************/

#ifndef INV_LOAD_DMP_H
#define INV_LOAD_DMP_H

#ifdef __cplusplus
extern "C" {
#endif

/*
    Includes.
*/
#include "mltypes.h"

/*
    APIs
*/
inv_error_t inv_load_dmp(FILE  *fd);
void read_dmp_img(char *dmp_path, char *out_file);

#ifdef __cplusplus
}
#endif
#endif  /* INV_LOAD_DMP_H */
