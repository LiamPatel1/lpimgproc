#pragma once

#include <QGraphicsPathItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QPainterPath>
#include <memory>
#include "nodes/Node.h"

class PinItem;
class ConnectionItem;
class QGraphicsTextItem;


class NodeItem : public QGraphicsRectItem {
public:
    explicit NodeItem(std::shared_ptr<Node> node_logic);

    ~NodeItem();

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    std::shared_ptr<Node> logic() const { return node_logic_; }
    PinItem* inputPin(int i) { return input_pins_[i]; }
    PinItem* outputPin(int i) { return output_pins_[i]; }

private:
    std::shared_ptr<Node> node_logic_;
    QGraphicsTextItem* label_;
    std::vector<PinItem*> input_pins_;
    std::vector<PinItem*> output_pins_;
};

    