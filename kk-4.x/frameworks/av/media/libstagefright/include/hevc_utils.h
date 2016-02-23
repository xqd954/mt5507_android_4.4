#ifndef HEVC_UTILS_H_

#define HEVC_UTILS_H_


namespace android {

enum {
    kHEVCProfileMain          = 0x01,
    kHEVCProfileMain10        = 0x02,
    kHEVCProfileMainPicture   = 0x03,
};

const char *HEVCProfileToString(uint8_t profile);

} // namespace android
#endif  // HEVC_UTILS_H_