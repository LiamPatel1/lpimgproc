#include <lpimgproc/Image.h>
#include <lpimgproc/Operators.h>

namespace lpimgproc::operators {

 Image invert(const Image& img, uint32_t flags) {
     const uint8_t chans = Image::channels(img.colourSpace());
     const bool hasAlpha = Image::hasAlpha(img.colourSpace());
     const bool invertAlpha = (flags & InvertFlags::InvertAlpha) != 0;

     Image newimg = img;

     for (uint32_t y = 0; y < img.height(); ++y) {
         for (uint32_t x = 0; x < img.width(); ++x) {
             for (uint8_t c = 0; c < chans; ++c) {
                 if (hasAlpha && !invertAlpha && c == chans - 1) {
                     // preserve alpha
                     newimg.at(x, y, c) = img.at(x, y, c);
                 }
                 else {
                     // invert
                     newimg.at(x, y, c) = 1.0f - img.at(x, y, c);
                 }
             }
         }
     }

     return newimg;
 }

    

}