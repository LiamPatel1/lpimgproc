#pragma once

#include <QGraphicsPathItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QPainterPath>
#include <memory>
#include "nodes/Node.h"
#include "views/PinItem.h"

class NodeItem;
class ConnectionItem;


class ConnectionItem : public QGraphicsPathItem {
public:
    ConnectionItem(PinItem* start, PinItem* end);
    ~ConnectionItem();

    void updatePath();

private:

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    QVariant itemChange(GraphicsItemChange change, const QVariant& value);


    PinItem* start_pin_;
    PinItem* end_pin_;
};