/*
 $License:
    Copyright (C) 2011-2012 InvenSense Corporation, All Rights Reserved.
    See included License.txt for License information.
 $
 */

/******************************************************************************
 *
 * $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/hardware/invensense/65xx/libsensors_iio/software/core/mpl/fusion_9axis.h#1 $
 *
 *****************************************************************************/

#ifndef MLDMP_FUSION9AXIS_H__
#define MLDMP_FUSION9AXIS_H__

#include "mltypes.h"

#ifdef __cplusplus
extern "C" {
#endif

    void inv_init_9x_fusion(void);
    inv_error_t inv_9x_fusion_state_change(unsigned char newState);
    inv_error_t inv_enable_9x_sensor_fusion(void);
    inv_error_t inv_disable_9x_sensor_fusion(void);
    inv_error_t inv_start_9x_sensor_fusion(void);
    inv_error_t inv_stop_9x_sensor_fusion(void);
    inv_error_t inv_9x_fusion_set_mag_fb(double fb);
    inv_error_t inv_9x_fusion_enable_jitter_reduction(int en);

    float inv_9x_sensor_fusion_get_correction_angle(void);
#ifdef __cplusplus
}
#endif


#endif // MLDMP_FUSION9AXIS_H__
