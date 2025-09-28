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

ConnectionItem::ConnectionItem(PinItem* start, PinItem* end)
    : start_pin_(start), end_pin_(end) {
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
    setPen(QPen(Qt::white, 2));
    setZValue(-1);
    start_pin_->addConnection(this);
    end_pin_->addConnection(this);
    updatePath();
}

ConnectionItem::~ConnectionItem() {
    if (start_pin_) {
        start_pin_->removeConnection(this);
    }
    if (end_pin_) {
        end_pin_->removeConnection(this);
    }
}


void ConnectionItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    setPen(QPen(Qt::yellow, 3));
    QGraphicsPathItem::hoverEnterEvent(event);
}

void ConnectionItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    setPen(QPen(isSelected() ? Qt::cyan : Qt::white, isSelected() ? 3 : 2));
    QGraphicsPathItem::hoverLeaveEvent(event);
}

QVariant ConnectionItem::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == QGraphicsItem::ItemSelectedChange) {
        setPen(QPen(value.toBool() ? Qt::cyan : Qt::white, value.toBool() ? 3 : 2));
    }
    return QGraphicsPathItem::itemChange(change, value);
}


void ConnectionItem::updatePath() {
    QPointF startPos = start_pin_->mapToScene(start_pin_->boundingRect().center());
    QPointF endPos = end_pin_->mapToScene(end_pin_->boundingRect().center());

    QPainterPath p;
    p.moveTo(startPos);

    qreal dx = endPos.x() - startPos.x();
    qreal dy = endPos.y() - startPos.y();

    QPointF ctrl1(startPos.x() + dx * 0.5, startPos.y());
    QPointF ctrl2(endPos.x() - dx * 0.5, endPos.y());

    p.cubicTo(ctrl1, ctrl2, endPos);
    setPath(p);
}