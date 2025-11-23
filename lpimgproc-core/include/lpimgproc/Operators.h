#include "lpimgproc/Image.h"
#include "lpimgproc/Kernel.h"

namespace lpimgproc::operators {


    enum InvertFlags : uint32_t {
        None = 0,
        InvertAlpha = 1 << 0,
    };

    Image invert(const Image& img, uint32_t flags = InvertFlags::None);

    Image grayscale(const Image& img);

    Image convolve(const Image& img, const Kernel& kernel);

    Image add(const Image& img1, const Image& img2);

    Image pow(const Image& img, const float exp);

    Image atan2(const Image& img1, const Image& img2);

    Image normalise(const Image& img);

    Image nonMaximumSuppression(const Image& magnitude, const Image& angle);

    Image doubleThreshold(const Image& img, float lowThreshold, float highThreshold);

    Image hysteresis(const Image& img);
}