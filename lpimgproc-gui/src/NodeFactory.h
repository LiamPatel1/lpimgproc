#pragma once
#include <functional>
#include <memory>
#include <QString>
#include <vector>
#include <map>

class Node;

class NodeFactory {
public:
    NodeFactory();
    std::shared_ptr<Node> createNode(const QString& name);
    std::vector<QString> getCreatableNodeNames() const;

private:
    std::map<QString, std::function<std::shared_ptr<Node>()>> registry_;
};