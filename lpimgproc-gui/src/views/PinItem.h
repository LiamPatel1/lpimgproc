#pragma once

#include <QGraphicsPathItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QPainterPath>
#include <memory>
#include "nodes/Node.h"

class NodeItem;
class ConnectionItem;

class PinItem : public QGraphicsEllipseItem {
public:
    enum PinType { Input, Output };

    PinItem(NodeItem* parent, PinType type, int index);


    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;


    NodeItem* nodeItem() const { return parent_node_; }
    int index() const { return index_; }
    PinType pinType() const { return pin_type_; }
    void addConnection(ConnectionItem* conn);
    void removeConnection(ConnectionItem* conn);

    const std::vector<ConnectionItem*>& connections() const { return connections_; }

private:
    NodeItem* parent_node_;
    PinType pin_type_;
    int index_;
    std::vector<ConnectionItem*> connections_;
    QGraphicsLineItem* temp_connection_line_ = nullptr;
};