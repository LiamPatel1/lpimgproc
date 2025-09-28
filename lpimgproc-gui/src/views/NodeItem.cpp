#include "NodeItem.h"
#include <QPen>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QCheckBox>
#include <QGraphicsProxyWidget>
#include <QDoubleSpinBox>
#include <iostream>
#include "PinItem.h"
#include "ConnectionItem.h"

#include "Nodes/ValueNode.h"
#include "Nodes/ImageNodes.h"
#include "Nodes/InvertNode.h"


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
    }

    for (size_t i = 0; i < node_logic_->outputs().size(); ++i) {
        auto pin = new PinItem(this, PinItem::Output, i);
        pin->setPos(150, 30 + i * 20);
        output_pins_.push_back(pin);
    }

    if (auto value_node = std::dynamic_pointer_cast<ValueNode>(node_logic_)) {
        auto* spin_box = new QDoubleSpinBox();
        spin_box->setRange(-1000, 1000);
        spin_box->setSingleStep(0.1);
        spin_box->setValue(0.0);
        spin_box->setFixedWidth(120);

        auto* proxy = new QGraphicsProxyWidget(this);
        proxy->setWidget(spin_box);
        proxy->setPos(15, 50); 

        QObject::connect(spin_box, qOverload<double>(&QDoubleSpinBox::valueChanged),
            [value_node](double val) {
                value_node->setValue(static_cast<float>(val));
            }
        );
    }
    else if (auto invert_node = std::dynamic_pointer_cast<InvertNode>(node_logic_)) {
        auto* checkbox = new QCheckBox("Invert Alpha");
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

