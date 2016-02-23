/*
 $License:
    Copyright (C) 2011 InvenSense Corporation, All Rights Reserved.
 $
 */

/*******************************************************************************
 *
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/hardware/invensense/65xx/libsensors_iio/software/core/mllite/linux/ml_stored_data.h#1 $
 *
 ******************************************************************************/

#ifndef INV_MPL_STORED_DATA_H
#define INV_MPL_STORED_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

/*
    Includes.
*/
#include "mltypes.h"

/*
    Defines
*/
#define MLCAL_FILE "/data/inv_cal_data.bin"

/*
    APIs
*/
inv_error_t inv_load_calibration(void);
inv_error_t inv_store_calibration(void);

/*
    Internal APIs
*/
inv_error_t inv_read_cal(unsigned char **, size_t *);
inv_error_t inv_write_cal(unsigned char *cal, size_t len);
inv_error_t inv_load_cal_V0(unsigned char *calData, size_t len);
inv_error_t inv_load_cal_V1(unsigned char *calData, size_t len);

/*
    Other prototypes
*/
inv_error_t inv_load_cal(unsigned char *calData);
inv_error_t inv_store_cal(unsigned char *calData, size_t length);

#ifdef __cplusplus
}
#endif
#endif  /* INV_MPL_STORED_DATA_H */
