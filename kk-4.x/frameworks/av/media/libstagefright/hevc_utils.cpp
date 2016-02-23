/*
*/

//#define LOG_NDEBUG 0
#define LOG_TAG "hevc_utils"
#include <utils/Log.h>

#include "include/hevc_utils.h"

namespace android {

const char *HEVCProfileToString(uint8_t profile) {
    switch (profile) {
        case kHEVCProfileMain:
            return "Main";
        case kHEVCProfileMain10:
            return "Main 10";
        case kHEVCProfileMainPicture:
            return "Main Still Picture";
        default:   return "Unknown";
    }
}


}  // namespace android
