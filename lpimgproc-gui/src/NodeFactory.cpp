#include "NodeFactory.h"
#include "nodes/InvertNode.h"
#include "nodes/FloatNode.h"
#include "nodes/KernelNode.h"
#include "nodes/ConvolveNode.h"

NodeFactory::NodeFactory() {
    registry_["Invert"] = []() { return std::make_shared<InvertNode>(); };
    registry_["Float"] = []() { return std::make_shared<FloatNode>(); };
    registry_["Kernel"] = []() { return std::make_shared<KernelNode>(); };
    registry_["Convolve"] = []() { return std::make_shared<ConvolveNode>(); };

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