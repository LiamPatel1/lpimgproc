#include <QImage>
#include <QString>
#include <memory>
#include <iostream>
#include <algorithm>
#include "Node.h"     
#include "lpimgproc/Image.h"
#include <QDebug>

using namespace lpimgproc;

ImagePtr QImageToLpImage(const QImage& qimg_in) {
    QImage qimg = qimg_in.convertToFormat(QImage::Format_RGBA8888);
    auto lpimg = std::make_shared<Image>(
        static_cast<uint32_t>(qimg.width()),
        static_cast<uint32_t>(qimg.height()),
        Image::RGBA);

    for (int y = 0; y < qimg.height(); ++y) {
        for (int x = 0; x < qimg.width(); ++x) {
            QRgb px = qimg.pixel(x, y);
            lpimg->at(x, y, 0) = qRed(px) / 255.0f;
            lpimg->at(x, y, 1) = qGreen(px) / 255.0f;
            lpimg->at(x, y, 2) = qBlue(px) / 255.0f;
            lpimg->at(x, y, 3) = qAlpha(px) / 255.0f;
        }
    }
    return lpimg;
}

QImage LpImageToQImage(const Image& img) {
    const int w = static_cast<int>(img.width());
    const int h = static_cast<int>(img.height());
    const uint64_t total = img.data().size();
    const int chans = (w > 0 && h > 0) ? static_cast<int>(total / (w * h)) : 4;

    QImage qimg(w, h, QImage::Format_ARGB32);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int r = 0, g = 0, b = 0, a = 255;
            if (chans >= 1) r = static_cast<int>(std::clamp(img.at(x, y, 0) * 255.0f, 0.0f, 255.0f));
            if (chans >= 2) g = static_cast<int>(std::clamp(img.at(x, y, 1) * 255.0f, 0.0f, 255.0f));
            if (chans >= 3) b = static_cast<int>(std::clamp(img.at(x, y, 2) * 255.0f, 0.0f, 255.0f));
            if (chans >= 4) a = static_cast<int>(std::clamp(img.at(x, y, 3) * 255.0f, 0.0f, 255.0f));
            if (chans == 1) g = b = r;
            qimg.setPixel(x, y, qRgba(r, g, b, a));
        }
    }
    return qimg;
}

auto connectPins = [](OutputPin& out, InputPin& in, size_t idx = 0) {
    in.connection = &out;
    in.outputIndex = idx;
    out.connections.push_back(&in);
    };

int main(int argc, char** argv) {

    if (argc < 3) {
        std::cerr << "Usage: ./test_node_graph input.png output.png\n";
        return 1;
    }

    QString inputPath = argv[1];
    QString outputPath = argv[2];

    QImage inputQ(inputPath);
    if (inputQ.isNull()) {
        std::cerr << "Failed to load image: " << inputPath.toStdString() << "\n";
        return 1;
    }

    auto inputNode = std::make_unique<ImageInputNode>(QImageToLpImage(inputQ));
    auto invertNode = std::make_unique<InvertNode>();
    auto displayNode = std::make_unique<ImageDisplayNode>();

    connectPins(inputNode->outputs()[0], invertNode->inputs()[0]);
    connectPins(invertNode->outputs()[0], displayNode->inputs()[0]);

    ImagePtr result = displayNode->getImage();
    if (!result) {
        std::cerr << "Graph produced no output\n";
        return 1;
    }

    const auto& d = result->data();

    QImage outQ = LpImageToQImage(*result);
    if (!outQ.save(outputPath)) {
        std::cerr << "Failed to save output image: " << outputPath.toStdString() << "\n";
        return 1;
    }

    std::cout << "Saved inverted image to " << outputPath.toStdString() << "\n";

    return 0;
}
