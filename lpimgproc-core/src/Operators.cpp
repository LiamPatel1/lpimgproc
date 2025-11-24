#include "lpimgproc/Image.h"
#include "lpimgproc/operators.h"

#include <cmath>
#include <numbers>
#include <algorithm>

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


     Image grayscale(const Image& img) {
         const uint32_t w = img.width();
         const uint32_t h = img.height();
         const uint32_t planeSize = w * h;

         switch (img.colourSpace()) {
         case Image::G:
         case Image::GA:
             return img;

         case Image::RGB: {
             Image newimg(w, h, Image::G);

             for (uint32_t i = 0; i < planeSize; ++i) {
                 float r = img.at(i);
                 float g = img.at(i + planeSize);
                 float b = img.at(i + planeSize * 2);

                 // Standard Luminosity Formula
                 newimg.at(i) = 0.299f * r + 0.587f * g + 0.114f * b;
             }
             return newimg;
         }

         case Image::RGBA: {
             Image newimg(w, h, Image::GA);

             for (uint32_t i = 0; i < planeSize; ++i) {
                 float r = img.at(i);
                 float g = img.at(i + planeSize);
                 float b = img.at(i + planeSize * 2);
                 float a = img.at(i + planeSize * 3);
                 newimg.at(i) = 0.299f * r + 0.587f * g + 0.114f * b;
                 newimg.at(i + planeSize) = a;
             }
             return newimg;
         }

         default:
             throw std::invalid_argument("Unsupported colour space");
         }
     }

     // TODO: edge handling flags, auto normalisation, just make better
     Image convolve(const Image& img, const Kernel& kernel) {

         const uint32_t w = img.width();
         const uint32_t h = img.height();
         const uint8_t channels = Image::channels(img.colourSpace());

         Image output(w, h, img.colourSpace());

         const int kW = (int)kernel.width();
         const int kH = (int)kernel.height();
         const int kCx = kW / 2;  // kernel center
         const int kCy = kH / 2;

         for (uint32_t y = 0; y < h; ++y) {
             for (uint32_t x = 0; x < w; ++x) {

                 for (uint8_t c = 0; c < channels; ++c) {

                     float sum = 0.0f;

                     for (int ky = 0; ky < kH; ++ky) {
                         for (int kx = 0; kx < kW; ++kx) {

                             // image coordinates
                             int ix = (int)x + (kx - kCx);
                             int iy = (int)y + (ky - kCy);

                             // clamp to edge
                             ix = std::clamp(ix, 0, (int)w - 1);
                             iy = std::clamp(iy, 0, (int)h - 1);

                             sum += img.at(ix, iy, c) * kernel.at(kx, ky);
                         }
                     }

                     output.at(x, y, c) = sum;
                 }
             }
         }

         return output;
     }

     Image add(const Image& img1, const Image& img2) {

         if (img1.width() != img2.width() || img1.height() != img2.height() || img1.colourSpace() != img2.colourSpace()) {
             throw std::invalid_argument("images are not compatable");
         }

         Image newimg(img1);

         for (uint64_t i = 0; i < newimg.subpixelCount(); i++) {
             newimg.at(i) = img1.at(i) + img2.at(i);
         }

         return newimg;
     }

     Image pow(const Image& img, const float exp) {

         Image newimg(img);

         for (uint64_t i = 0; i < newimg.subpixelCount(); i++) {
             newimg.at(i) = std::pow(img.at(i), exp);
         }

         return newimg;
     }

     Image atan2(const Image& img1, const Image& img2) {

         if (img1.width() != img2.width() || img1.height() != img2.height() || img1.colourSpace() != img2.colourSpace()) {
             throw std::invalid_argument("images are not compatable");
         }

         Image newimg(img1);

         for (uint64_t i = 0; i < newimg.subpixelCount(); i++) {
             newimg.at(i) = atan2f(img1.at(i), img2.at(i));
         }

         return newimg;
     }

     Image normalise(const Image& img) {

         float minVal = std::numeric_limits<float>::infinity();
         float maxVal = -std::numeric_limits<float>::infinity();

         for (uint64_t i = 0; i < img.subpixelCount(); i++) {
             float v = img.at(i);
             if (v < minVal) minVal = v;
             if (v > maxVal) maxVal = v;
         }

         Image out(img);

         if (minVal == maxVal) {
             for (uint64_t i = 0; i < out.subpixelCount(); i++)
                 out.at(i) = 0.0f;
             return out;
         }

         const float invRange = 1.0f / (maxVal - minVal);

         for (uint64_t i = 0; i < out.subpixelCount(); i++) {
             out.at(i) = (img.at(i) - minVal) * invRange;
         }

         return out;
     }

     Image nonMaximumSuppression(const Image& magnitude, const Image& angle) {
         if (magnitude.width() != angle.width() || magnitude.height() != angle.height()) {
             throw std::invalid_argument("Magnitude and Angle images must be same size");
         }

         uint32_t w = magnitude.width();
         uint32_t h = magnitude.height();
         Image result(w, h, Image::G);

         for (uint32_t y = 1; y < h - 1; ++y) {
             for (uint32_t x = 1; x < w - 1; ++x) {

                 float mag = magnitude.at(x, y, 0);
                 float ang = angle.at(x, y, 0);

                 float deg = ang * 180.0f / std::numbers::pi_v<float>;
                 if (deg < 0) deg += 180.0f;

                 float q = 255.0f;
                 float r = 255.0f;

                 // 0 degrees (Horizontal) - Check Left and Right
                 if ((deg >= 0 && deg < 22.5) || (deg >= 157.5 && deg <= 180)) {
                     q = magnitude.at(x + 1, y, 0);
                     r = magnitude.at(x - 1, y, 0);
                 }
                 // 45 degrees (Diagonal /) - Check Top-Right and Bottom-Left
                 else if (deg >= 22.5 && deg < 67.5) {
                     q = magnitude.at(x + 1, y - 1, 0); // Top Right
                     r = magnitude.at(x - 1, y + 1, 0); // Bottom Left
                 }
                 // 90 degrees (Vertical) - Check Top and Bottom
                 else if (deg >= 67.5 && deg < 112.5) {
                     q = magnitude.at(x, y - 1, 0);
                     r = magnitude.at(x, y + 1, 0);
                 }
                 // 135 degrees (Diagonal \) - Check Top-Left and Bottom-Right
                 else if (deg >= 112.5 && deg < 157.5) {
                     q = magnitude.at(x - 1, y - 1, 0); // Top Left
                     r = magnitude.at(x + 1, y + 1, 0); // Bottom Right
                 }

                 if (mag >= q && mag >= r) {
                     result.at(x, y, 0) = mag;
                 }
                 else {
                     result.at(x, y, 0) = 0.0f;
                 }
             }
         }
         return result;
     }

     Image doubleThreshold(const Image& img, float lowThreshold, float highThreshold) {
         Image res(img.width(), img.height(), Image::G);

         float strong = 1.0f;
         float weak = 0.5f;

         for (uint64_t i = 0; i < img.subpixelCount(); ++i) {
             float val = img.at(i);

             if (val >= highThreshold) {
                 res.at(i) = strong;
             }
             else if (val >= lowThreshold) {
                 res.at(i) = weak;
             }
             else {
                 res.at(i) = 0.0f;
             }
         }
         return res;
     }

     Image hysteresis(const Image& img) {
         Image res = img;
         uint32_t w = res.width();
         uint32_t h = res.height();

         float strong = 1.0f;
         float weak = 0.5f;

         bool changed = true;
         while (changed) {
             changed = false;
             for (uint32_t y = 1; y < h - 1; ++y) {
                 for (uint32_t x = 1; x < w - 1; ++x) {

                     if (res.at(x, y, 0) == weak) {
                         bool connected = false;
                         for (int ky = -1; ky <= 1; ++ky) {
                             for (int kx = -1; kx <= 1; ++kx) {
                                 if (kx == 0 && ky == 0) continue;
                                 if (res.at(x + kx, y + ky, 0) == strong) {
                                     connected = true;
                                 }
                             }
                         }

                         if (connected) {
                             res.at(x, y, 0) = strong;
                             changed = true;
                         }
                     }
                 }
             }
         }

         for (uint64_t i = 0; i < res.subpixelCount(); ++i) {
             if (res.at(i) != strong) {
                 res.at(i) = 0.0f;
             }
         }

         return res;
     }


}