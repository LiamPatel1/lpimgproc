#include "NodeItem.h"
#include <QPen>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QCheckBox>
#include <QComboBox>
#include <QGraphicsProxyWidget>
#include <QDoubleSpinBox>
#include <iostream>
#include "PinItem.h"
#include "ConnectionItem.h"

#include "Nodes/FloatNode.h"
#include "Nodes/ImageNodes.h"
#include "lpimgproc/Kernel.h"
#include "Nodes/InvertNode.h"
#include "nodes/KernelNode.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>

NodeItem::NodeItem(std::shared_ptr<Node> node_logic)
    : QGraphicsRectItem(0, 0, 150, 100), node_logic_(std::move(node_logic)) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setBrush(QBrush(QColor(60, 60, 60)));
    setPen(QPen(Qt::black, 2));

    label_ = new QGraphicsTextItem(QString::fromStdString(node_logic_->name()), this);
    label_->setDefaultTextColor(Qt::white);
    label_->setPos(5, 5);

    for (size_t i = 0; i < node_logic_->inputs().size(); ++i) {
        auto pin = new PinItem(this, PinItem::Input, i);
        pin->setPos(0, 30 + i * 20);
        input_pins_.push_back(pin);
        
        if (!node_logic_->inputs()[i].name.empty()) {
            auto* text = new QGraphicsTextItem(QString::fromStdString(node_logic_->inputs()[i].name), this);
            text->setDefaultTextColor(Qt::lightGray);
            QFont f = text->font();
            f.setPointSize(8);
            text->setFont(f);
            text->setPos(10, 30 + i * 20 - 10);
        }
    }

    for (size_t i = 0; i < node_logic_->outputs().size(); ++i) {
        auto pin = new PinItem(this, PinItem::Output, i);
        pin->setPos(150, 30 + i * 20);
        output_pins_.push_back(pin);
    }

    if (auto float_node = std::dynamic_pointer_cast<FloatNode>(node_logic_)) {
        auto* spin_box = new QDoubleSpinBox();
        spin_box->setRange(-1000, 1000);
        spin_box->setSingleStep(0.1);
        spin_box->setValue(0.0);
        spin_box->setFixedWidth(120);

        auto* proxy = new QGraphicsProxyWidget(this);
        proxy->setWidget(spin_box);
        proxy->setPos(15, 50); 

        QObject::connect(spin_box, qOverload<double>(&QDoubleSpinBox::valueChanged),
            [float_node](double val) {
                float_node->setValue(static_cast<float>(val));
            }
        );
    }
    else if (auto invert_node = std::dynamic_pointer_cast<InvertNode>(node_logic_)) {
        QCheckBox* checkbox = new QCheckBox("Invert Alpha");
        checkbox->setStyleSheet("color: white;"); 
        checkbox->setChecked(invert_node->invertsAlpha()); 

        auto* proxy = new QGraphicsProxyWidget(this);
        proxy->setWidget(checkbox);
        proxy->setPos(15, 50); 

        QObject::connect(checkbox, &QCheckBox::stateChanged,
            [invert_node](int state) {
                invert_node->setInvertAlpha(state == Qt::Checked);
            }
        );
    }

    else if (auto kernel_node = std::dynamic_pointer_cast<KernelNode>(node_logic_)) {
        auto* interfaceBox = new QWidget();
        auto* interfaceLayout = new QVBoxLayout(interfaceBox);

        auto* presetLayout = new QHBoxLayout();
        auto* presetLabel = new QLabel("Preset:");
        auto* presetCombo = new QComboBox();
        presetLayout->addWidget(presetLabel);
        presetLayout->addWidget(presetCombo);
        interfaceLayout->addLayout(presetLayout);

        struct KernelPreset {
            QString name;
            int w;
            int h;
            std::vector<float> data;
        };

        const std::vector<KernelPreset> presets = {
            { "Identity", 3, 3, {
                0, 0, 0,
                0, 1, 0,
                0, 0, 0
            }},
            { "Gaussian Blur", 3, 3, {
                1.0f / 8,  2.0f / 8, 1.0f / 8,
                2.0f / 8,  4.0f / 8, 2.0f / 8,
                1.0f / 8,  2.0f / 8, 1.0f / 8
            }},
            { "Sharpen", 3, 3, {
                 0, -1,  0,
                -1,  5, -1,
                 0, -1,  0
            }},
            { "Edge Detection", 3, 3, {
                -1, -1, -1,
                -1,  8, -1,
                -1, -1, -1
            }},
            { "Emboss", 3, 3, {
                -2, -1,  0,
                -1,  1,  1,
                 0,  1,  2
            }},
            { "Sobel X", 3, 3, {
                -1, 0, 1,
                -2, 0, 2,
                -1, 0, 1
            }},
            { "Sobel Y", 3, 3, {
                -1, -2, -1,
                 0,  0,  0,
                 1,  2,  1
            }},
        };

        presetCombo->addItem("Custom");
        for (const auto& p : presets) {
            presetCombo->addItem(p.name);
        }

        auto* dimensionsLayout = new QHBoxLayout();
        auto* widthbox = new QSpinBox();
        widthbox->setRange(1, 15);
        widthbox->setValue(3);
        auto* heightbox = new QSpinBox();
        heightbox->setRange(1, 15);
        heightbox->setValue(3);

        dimensionsLayout->addWidget(new QLabel("W:"));
        dimensionsLayout->addWidget(widthbox);
        dimensionsLayout->addWidget(new QLabel("H:"));
        dimensionsLayout->addWidget(heightbox);
        interfaceLayout->addLayout(dimensionsLayout);

        auto* inputGridBox = new QGroupBox("Values");
        inputGridBox->setStyleSheet("color: white;");
        auto* inputGridLayout = new QGridLayout(inputGridBox);
        interfaceLayout->addWidget(inputGridBox);

        auto updateKernel = [=]() {
            std::vector<float> data;
            int width = widthbox->value();
            int height = heightbox->value();
            data.reserve(width * height);

            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    if (QLayoutItem* item = inputGridLayout->itemAtPosition(y, x)) {
                        if (auto* spin_box = qobject_cast<QDoubleSpinBox*>(item->widget())) {
                            data.push_back(static_cast<float>(spin_box->value()));
                        }
                    }
                }
            }
            if (data.size() == static_cast<size_t>(width * height)) {
                auto kernel = std::make_shared<lpimgproc::Kernel>(data, width, height);
                kernel_node->setValue(kernel);
            }
            };

        auto onGridValueChanged = [=]() {
            // If the user edits a value manually switch the dropdown to Custom
            // Block signals to prevent the Custom selection from wiping the grid
            const QSignalBlocker blocker(presetCombo);
            presetCombo->setCurrentIndex(0);

            updateKernel();
            };

        auto recreateGrid = [=]() {
            while (QLayoutItem* item = inputGridLayout->takeAt(0)) {
                if (QWidget* widget = item->widget()) {
                    widget->deleteLater();
                }
                delete item;
            }

            for (int y = 0; y < heightbox->value(); ++y) {
                for (int x = 0; x < widthbox->value(); ++x) {
                    auto* value_box = new QDoubleSpinBox();
                    value_box->setRange(-100.0, 100.0);
                    value_box->setSingleStep(0.1);
                    value_box->setDecimals(3); // Increased precision for blur kernels
                    value_box->setValue(0.0);
                    inputGridLayout->addWidget(value_box, y, x);

                    QObject::connect(value_box, &QDoubleSpinBox::valueChanged, onGridValueChanged);
                }
            }
            };


        // Handle Preset Selection
        QObject::connect(presetCombo, &QComboBox::currentIndexChanged, [=](int index) {
            if (index <= 0) return; // Custom selected, do nothing 

            const auto& p = presets[index - 1];

            widthbox->setValue(p.w);
            heightbox->setValue(p.h);

            int i = 0;
            for (int y = 0; y < p.h; ++y) {
                for (int x = 0; x < p.w; ++x) {
                    if (QLayoutItem* item = inputGridLayout->itemAtPosition(y, x)) {
                        if (auto* spin_box = qobject_cast<QDoubleSpinBox*>(item->widget())) {
                            // Block signals so we don't trigger onGridValueChanged
                            const QSignalBlocker blocker(spin_box);
                            if (i < p.data.size()) {
                                spin_box->setValue(p.data[i]);
                            }
                        }
                    }
                    i++;
                }
            }

            updateKernel();
            });

        QObject::connect(widthbox, &QSpinBox::valueChanged, [=]() {
            recreateGrid();
            updateKernel();
            });
        QObject::connect(heightbox, &QSpinBox::valueChanged, [=]() {
            recreateGrid();
            updateKernel();
            });

        recreateGrid();
        presetCombo->setCurrentIndex(1); // Select Identity

        auto* proxy = new QGraphicsProxyWidget(this);
        proxy->setWidget(interfaceBox);
        proxy->setPos(5, 40);

        QSize size = interfaceBox->sizeHint();
        setRect(0, 0, std::max(150, size.width() + 10), size.height() + 50);
        for (auto* pin : output_pins_) {
            pin->setPos(rect().width(), pin->pos().y());
        }
    }

}

NodeItem::~NodeItem() {

    std::set<ConnectionItem*> connections_to_delete;

    for (auto* pin : input_pins_) {
        for (auto* conn : pin->connections()) {
            connections_to_delete.insert(conn);
        }
    }
    for (auto* pin : output_pins_) {
        for (auto* conn : pin->connections()) {
            connections_to_delete.insert(conn);
        }
    }

    for (auto* conn : connections_to_delete) {
        delete conn;
    }
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == ItemPositionHasChanged && scene()) {
        for (auto pin : input_pins_) {
            for (auto conn : pin->connections()) {
                conn->updatePath();
            }
        }
        for (auto pin : output_pins_) {
            for (auto conn : pin->connections()) {
                conn->updatePath();
            }
        }
    }
    return QGraphicsRectItem::itemChange(change, value);
}

