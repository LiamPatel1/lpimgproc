#include "NodeEditorWidget.h"
#include <QVBoxLayout>
#include "views/PinItem.h"
#include "views/ConnectionItem.h"

#include <QKeyEvent>

void NodeScene::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete) {
        std::set<ConnectionItem*> connections_to_delete;
        QList<NodeItem*> nodes_to_delete;

        for (QGraphicsItem* item : selectedItems()) {
            if (auto node = dynamic_cast<NodeItem*>(item)) {
                nodes_to_delete.append(node);
            }
            else if (auto conn = dynamic_cast<ConnectionItem*>(item)) {
                connections_to_delete.insert(conn);
            }
        }

        for (NodeItem* node : nodes_to_delete) {
            for (int i = 0; i < node->logic()->inputs().size(); ++i) {
                for (auto conn : node->inputPin(i)->connections()) {
                    connections_to_delete.insert(conn);
                }
            }
            for (int i = 0; i < node->logic()->outputs().size(); ++i) {
                for (auto conn : node->outputPin(i)->connections()) {
                    connections_to_delete.insert(conn);
                }
            }
        }

        for (ConnectionItem* conn : connections_to_delete) {
            delete conn;
        }

        for (NodeItem* node : nodes_to_delete) {
            delete node;
        }
    }
    else {
        QGraphicsScene::keyPressEvent(event);
    }
}



NodeEditorWidget::NodeEditorWidget(QWidget* parent) : QWidget(parent) {
    scene_ = new NodeScene(this); 
    scene_->setBackgroundBrush(QColor(40, 40, 40));

    view_ = new QGraphicsView(scene_);
    view_->setRenderHint(QPainter::Antialiasing);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(view_);
    setLayout(layout);
}

void NodeEditorWidget::addNode(std::shared_ptr<Node> node_logic, const QPointF& pos) {
    auto node_item = new NodeItem(node_logic);
    node_item->setPos(pos);
    scene_->addItem(node_item);
}


QGraphicsView* NodeEditorWidget::view() const {
    return view_;
}