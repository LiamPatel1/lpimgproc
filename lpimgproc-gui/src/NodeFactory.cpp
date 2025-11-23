#include "NodeFactory.h"
#include "nodes/InvertNode.h"
#include "nodes/FloatNode.h"
#include "nodes/KernelNode.h"
#include "nodes/ConvolveNode.h"
#include <nodes/AddNode.h>
#include <nodes/NormaliseNode.h>
#include <nodes/atan2Node.h>
#include <nodes/PowNode.h>
#include <nodes/NonMaximumSuppressionNode.h>
#include <nodes/DoubleThresholdNode.h>
#include <nodes/Hysteresis.h>
#include <nodes/GrayscaleNode.h>

NodeFactory::NodeFactory() {
    registry_["Invert"] = []() { return std::make_shared<InvertNode>(); };
    registry_["Float"] = []() { return std::make_shared<FloatNode>(); };
    registry_["Kernel"] = []() { return std::make_shared<KernelNode>(); };
    registry_["Convolve"] = []() { return std::make_shared<ConvolveNode>(); };
    registry_["Add"] = []() { return std::make_shared<AddNode>(); };
    registry_["Normalise"] = []() { return std::make_shared<NormaliseNode>(); };
    registry_["atan2"] = []() { return std::make_shared<Atan2Node>(); };
    registry_["Power"] = []() { return std::make_shared<PowNode>(); };
    registry_["Non-Maximum Suppression"] = []() { return std::make_shared<NonMaximumSuppressionNode>(); };
    registry_["Double Threshold"] = []() { return std::make_shared<DoubleThresholdNode>(); };
    registry_["Hysteresis"] = []() { return std::make_shared<HysteresisNode>(); };
    registry_["Grayscale"] = []() { return std::make_shared<GrayscaleNode>(); };
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