#include <QImage>
#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

#include <QHBoxLayout>
#include <QFileDialog>
#include <QWidget>

#include "NodeEditorWidget.h"

#include <QTimer>
#include <QMenu>
#include <QString>
#include <memory>
#include <iostream>
#include <algorithm>
#include "nodes/Node.h"     
#include "lpimgproc/Image.h"
#include <QDebug>
#include "NodeFactory.h"
#include "nodes/ImageNodes.h"

using namespace lpimgproc;

ImagePtr QImageToLpImage(const QImage& qimg_in) {
    if (qimg_in.hasAlphaChannel()) {
        QImage qimg = qimg_in.convertToFormat(QImage::Format_RGBA8888);
        auto lpimg = std::make_shared<Image>(qimg.width(), qimg.height(), Image::RGBA);
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
    else if (qimg_in.isGrayscale()) {
        QImage qimg = qimg_in.convertToFormat(QImage::Format_Grayscale8);
        auto lpimg = std::make_shared<Image>(qimg.width(), qimg.height(), Image::G);
        for (int y = 0; y < qimg.height(); ++y) {
            for (int x = 0; x < qimg.width(); ++x) {
                lpimg->at(x, y, 0) = qGray(qimg.pixel(x, y)) / 255.0f;
            }
        }
        return lpimg;
    }
    else {
        QImage qimg = qimg_in.convertToFormat(QImage::Format_RGB888);
        auto lpimg = std::make_shared<Image>(qimg.width(), qimg.height(), Image::RGB);
        for (int y = 0; y < qimg.height(); ++y) {
            for (int x = 0; x < qimg.width(); ++x) {
                QRgb px = qimg.pixel(x, y);
                lpimg->at(x, y, 0) = qRed(px) / 255.0f;
                lpimg->at(x, y, 1) = qGreen(px) / 255.0f;
                lpimg->at(x, y, 2) = qBlue(px) / 255.0f;
            }
        }
        return lpimg;
    }
}
QImage LpImageToQImage(const Image& img) {
    const int w = static_cast<int>(img.width());
    const int h = static_cast<int>(img.height());

    switch (img.colourSpace()) {
    case Image::G: {
        QImage qimg(w, h, QImage::Format_Grayscale8);
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                int gray = static_cast<int>(std::clamp(img.at(x, y, 0) * 255.0f, 0.0f, 255.0f));
                qimg.setPixel(x, y, qRgb(gray, gray, gray));
            }
        }
        return qimg;
    }

    case Image::GA: {
        QImage qimg(w, h, QImage::Format_ARGB32);
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                int gray = static_cast<int>(std::clamp(img.at(x, y, 0) * 255.0f, 0.0f, 255.0f));
                int alpha = static_cast<int>(std::clamp(img.at(x, y, 1) * 255.0f, 0.0f, 255.0f));
                qimg.setPixel(x, y, qRgba(gray, gray, gray, alpha));
            }
        }
        return qimg;
    }

    case Image::RGB: {
        QImage qimg(w, h, QImage::Format_ARGB32);
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                int r = static_cast<int>(std::clamp(img.at(x, y, 0) * 255.0f, 0.0f, 255.0f));
                int g = static_cast<int>(std::clamp(img.at(x, y, 1) * 255.0f, 0.0f, 255.0f));
                int b = static_cast<int>(std::clamp(img.at(x, y, 2) * 255.0f, 0.0f, 255.0f));
                qimg.setPixel(x, y, qRgba(r, g, b, 255));
            }
        }
        return qimg;
    }

    case Image::RGBA:
    default: {
        QImage qimg(w, h, QImage::Format_ARGB32);
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                int r = static_cast<int>(std::clamp(img.at(x, y, 0) * 255.0f, 0.0f, 255.0f));
                int g = static_cast<int>(std::clamp(img.at(x, y, 1) * 255.0f, 0.0f, 255.0f));
                int b = static_cast<int>(std::clamp(img.at(x, y, 2) * 255.0f, 0.0f, 255.0f));
                int a = static_cast<int>(std::clamp(img.at(x, y, 3) * 255.0f, 0.0f, 255.0f));
                qimg.setPixel(x, y, qRgba(r, g, b, a));
            }
        }
        return qimg;
    }
    }
}

auto connectPins = [](OutputPin& out, InputPin& in, size_t idx = 0) {
    in.connection = &out;
    in.outputIndex = idx;
    out.connections.push_back(&in);
};
            

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QWidget window;
    auto main_layout = new QHBoxLayout(&window);

    auto left_panel = new QWidget();
    auto left_layout = new QVBoxLayout(left_panel);
    QPushButton loadButton("Load Image");
    QPushButton processButton("Process");
    QPushButton addNodeButton("Add Node");
    QMenu addNodeMenu(&window);
    addNodeButton.setMenu(&addNodeMenu);
    QLabel imageLabel;
    imageLabel.setFixedSize(400, 400);

    left_layout->addWidget(&loadButton);
    left_layout->addWidget(&processButton);
    left_layout->addWidget(&addNodeButton);
    left_layout->addWidget(&imageLabel);
    left_layout->addStretch();


    auto node_editor = new NodeEditorWidget();

    main_layout->addWidget(left_panel);
    main_layout->addWidget(node_editor, 1); 

    NodeFactory factory;
    for (const auto& name : factory.getCreatableNodeNames()) {
        addNodeMenu.addAction(name);
    }

    auto inputNode = std::make_shared<ImageInputNode>();
    auto displayNode = std::make_shared<ImageDisplayNode>();

    node_editor->addNode(inputNode, QPointF(50, 100));
    node_editor->addNode(displayNode, QPointF(550, 100));


    QObject::connect(&loadButton, &QPushButton::clicked, [&]() {
        QString path = QFileDialog::getOpenFileName(&window, "Select image");
        if (path.isEmpty()) return;

        QImage qimg(path);
        if (qimg.isNull()) return;

        ImagePtr lpimg = QImageToLpImage(qimg);
        inputNode->setImage(lpimg);

        imageLabel.setPixmap(QPixmap::fromImage(qimg).scaled(400, 400, Qt::KeepAspectRatio));
        });

    QObject::connect(&processButton, &QPushButton::clicked, [&]() {
        ImagePtr outImg = displayNode->getImage();
        if (!outImg) return;

        QImage outQ = LpImageToQImage(*outImg);
        imageLabel.setPixmap(QPixmap::fromImage(outQ).scaled(400, 400, Qt::KeepAspectRatio));
        });

    QObject::connect(&addNodeMenu, &QMenu::triggered, [&](QAction* action) {
        std::shared_ptr<Node> new_node = factory.createNode(action->text());
        if (new_node) {
            QPoint view_center = node_editor->view()->viewport()->rect().center();
            QPointF scene_center = node_editor->view()->mapToScene(view_center);
            node_editor->addNode(new_node, scene_center);
        }
        });

    window.setWindowTitle("Node Graph Image Processor");
    window.resize(1200, 800);
    window.show();
    return app.exec();
}