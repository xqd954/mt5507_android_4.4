/*
 $License:
    Copyright (C) 2011-2012 InvenSense Corporation, All Rights Reserved.
    See included License.txt for License information.
 $
 */


/******************************************************************************
 *
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/hardware/invensense/60xx/libsensors_iio/software/core/mpl/no_gyro_fusion.h#1 $
 *
 *****************************************************************************/

#ifndef MLDMP_NOGYROFUSION_H__
#define MLDMP_NOGYROFUSION_H__
#include "mltypes.h"

#ifdef __cplusplus
extern "C" {
#endif

    inv_error_t inv_enable_no_gyro_fusion(void);
    inv_error_t inv_disable_no_gyro_fusion(void);
    inv_error_t inv_start_no_gyro_fusion(void);
    inv_error_t inv_start_no_gyro_fusion(void);
    inv_error_t inv_init_no_gyro_fusion(void);

#ifdef __cplusplus
}
#endif


#endif // MLDMP_NOGYROFUSION_H__
