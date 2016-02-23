#include <media/AudioSystem.h>
#include <utils/String8.h>
#include "bt_types.h"

using namespace android;

/* Notify audio/modem module whether Wide Band speech should be used or ont */
extern "C" void SetAudioParameter(kal_bool bWideband)
{
    if (bWideband)
    {
        AudioSystem::setParameters(0, String8("SET_DAIBT_MODE=1"));        
    }
    else
    {
        AudioSystem::setParameters(0, String8("SET_DAIBT_MODE=0"));        
    }
}