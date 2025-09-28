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


PinItem::PinItem(NodeItem* parent, PinType type, int index)
    : QGraphicsEllipseItem(-5, -5, 10, 10, parent), parent_node_(parent), pin_type_(type), index_(index) {
    setBrush(QBrush(Qt::cyan));
    setAcceptHoverEvents(true);
}




void PinItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    setBrush(QBrush(Qt::yellow));
    QGraphicsEllipseItem::hoverEnterEvent(event);
}

void PinItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    setBrush(QBrush(Qt::cyan));
    QGraphicsEllipseItem::hoverLeaveEvent(event);
}

void PinItem::addConnection(ConnectionItem* conn) {
    connections_.push_back(conn);
}

void PinItem::removeConnection(ConnectionItem* conn) {
    connections_.erase(std::remove(connections_.begin(), connections_.end(), conn), connections_.end());
}


void PinItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (pin_type_ == Output) {
        temp_connection_line_ = new QGraphicsLineItem(QLineF(scenePos(), scenePos()));
        temp_connection_line_->setPen(QPen(Qt::white, 2));
        scene()->addItem(temp_connection_line_);
    }
    event->accept();
}

void PinItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (temp_connection_line_) {
        temp_connection_line_->setLine(QLineF(scenePos(), event->scenePos()));
    }
}

void PinItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (temp_connection_line_) {
        temp_connection_line_->setVisible(false);

        QGraphicsItem* item_under_mouse = scene()->itemAt(event->scenePos(), QTransform());
        PinItem* end_pin = dynamic_cast<PinItem*>(item_under_mouse);

        if (end_pin && end_pin->pinType() == Input && end_pin->nodeItem() != this->nodeItem()) {

            if (!end_pin->connections().empty()) {
                delete end_pin->connections().front();
            }

            auto* conn = new ConnectionItem(this, end_pin);
            scene()->addItem(conn);

            auto& out_pin_logic = this->nodeItem()->logic()->outputs()[this->index()];
            auto& in_pin_logic = end_pin->nodeItem()->logic()->inputs()[end_pin->index()];

            in_pin_logic.connection = &out_pin_logic;
            in_pin_logic.outputIndex = this->index();
            out_pin_logic.connections.push_back(&in_pin_logic);

            end_pin->nodeItem()->logic()->makeDirty();

        }

        scene()->removeItem(temp_connection_line_);
        delete temp_connection_line_;
        temp_connection_line_ = nullptr;
    }
}