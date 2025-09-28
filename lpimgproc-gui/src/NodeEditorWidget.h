#pragma once

#include <QWidget>
#include <QGraphicsView>
#include "nodes/Node.h"
#include "views/NodeItem.h"

class QKeyEvent;

class NodeScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit NodeScene(QObject* parent = nullptr) : QGraphicsScene(parent) {}

protected:
    void keyPressEvent(QKeyEvent* event) override;
};

class NodeEditorWidget : public QWidget {
    Q_OBJECT

public:
    explicit NodeEditorWidget(QWidget* parent = nullptr);
    void addNode(std::shared_ptr<Node> node_logic, const QPointF& pos);

    QGraphicsView* view() const;

private:
    NodeScene* scene_;
    QGraphicsView* view_;
};