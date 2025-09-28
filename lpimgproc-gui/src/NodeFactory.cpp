#include "NodeFactory.h"
#include "nodes/InvertNode.h"
#include "nodes/ValueNode.h"

NodeFactory::NodeFactory() {
    registry_["Invert"] = []() { return std::make_shared<InvertNode>(); };
    registry_["Value"] = []() { return std::make_shared<ValueNode>(); };
}

std::shared_ptr<Node> NodeFactory::createNode(const QString& name) {
    if (registry_.count(name)) {
        return registry_[name]();
    }
    return nullptr;
}

std::vector<QString> NodeFactory::getCreatableNodeNames() const {
    std::vector<QString> names;
    for (const auto& pair : registry_) {
        names.push_back(pair.first);
    }
    return names;
}