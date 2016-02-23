/*
 $License:
    Copyright (C) 2011-2012 InvenSense Corporation, All Rights Reserved.
    See included License.txt for License information.
 $
 */

/******************************************************************************
 *
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/hardware/invensense/60xx/libsensors_iio/software/core/mpl/heading_from_gyro.h#1 $
 *
 *****************************************************************************/

#ifndef _HEADING_FROM_GYRO_H_
#define _HEADING_FROM_GYRO_H_
#include "mltypes.h"

#ifdef __cplusplus
extern "C" {
#endif

    inv_error_t inv_enable_heading_from_gyro(void);
    inv_error_t inv_disable_heading_from_gyro(void);
    void inv_init_heading_from_gyro(void);
    inv_error_t inv_start_heading_from_gyro(void);
    inv_error_t inv_stop_heading_from_gyro(void);

#ifdef __cplusplus
}
#endif


#endif  /* _HEADING_FROM_GYRO_H_ */
