#include "lpimgproc/Image.h"
#include "lpimgproc/operators.h"

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


     // TODO: edge handling flags, auto normalisation, just make better
     Image convolve(const Image& img, const Kernel& kernel) {

         Image outputImage(img.width(), img.height(), img.colourSpace());

         const int kernelCenterX = kernel.width() / 2;
         const int kernelCenterY = kernel.height() / 2;

         for (uint32_t y = kernelCenterY; y < img.height() - kernelCenterY; ++y) {
             for (uint32_t x = kernelCenterX; x < img.width() - kernelCenterX; ++x) {

                 for (uint8_t c = 0; c < Image::channels(img.colourSpace()); ++c) {

                     float sum = 0.0f;
                     for (uint32_t ky = 0; ky < kernel.height(); ++ky) {
                         for (uint32_t kx = 0; kx < kernel.width(); ++kx) {
                             int ix = x - kernelCenterX + kx;
                             int iy = y - kernelCenterY + ky;

                             sum += img.at(ix, iy, c) * kernel.at(kx, ky);
                         }
                     }

                     outputImage.at(x, y, c) = sum;
                 }
             }
         }

         return outputImage;
     }




}