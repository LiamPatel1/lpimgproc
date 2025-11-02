#include "lpimgproc/Image.h"
#include "lpimgproc/Kernel.h"

namespace lpimgproc::operators {


    enum InvertFlags : uint32_t {
        None = 0,
        InvertAlpha = 1 << 0,
    };

    Image invert(const Image& img, uint32_t flags = InvertFlags::None);

    Image convolve(const Image& img, const Kernel& kernel);

}