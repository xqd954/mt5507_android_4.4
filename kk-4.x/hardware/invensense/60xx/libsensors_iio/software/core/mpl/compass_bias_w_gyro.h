/*
 $License:
    Copyright (C) 2011-2012 InvenSense Corporation, All Rights Reserved.
    See included License.txt for License information.
 $
 */

/******************************************************************************
 *
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/hardware/invensense/60xx/libsensors_iio/software/core/mpl/compass_bias_w_gyro.h#1 $
 *
 *****************************************************************************/

#ifndef MLDMP_COMPASSBIASWGYRO_H__
#define MLDMP_COMPASSBIASWGYRO_H__
#include "mltypes.h"

#ifdef __cplusplus
extern "C" {
#endif

    inv_error_t inv_enable_compass_bias_w_gyro();
    inv_error_t inv_disable_compass_bias_w_gyro();
    void inv_init_compass_bias_w_gyro();

#ifdef __cplusplus
}
#endif


#endif // MLDMP_COMPASSBIASWGYRO_H__
