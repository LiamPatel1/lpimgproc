#include <lpimgproc/Image.h>

namespace lpimgproc::operators {

    Image invert(const Image& img) {
        const uint8_t chans = Image::channels(img.colourSpace());

        Image newimg = img; 

        for (uint32_t y = 0; y < img.height(); ++y) {
            for (uint32_t x = 0; x < img.width(); ++x) {
                for (uint8_t c = 0; c < chans; ++c) {
                    if (Image::hasAlpha(img.colourSpace()) && c == chans - 1) {
                        // last channel is alpha , dont invert
                        // TODO: Flag to ignore this behaviour
                        newimg.at(x, y, c) = img.at(x, y, c);
                    }
                    else {
                        newimg.at(x, y, c) = 1.0f - img.at(x, y, c);
                    }
                }
            }
        }

        return newimg;
    }

}