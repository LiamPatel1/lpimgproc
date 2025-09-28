#pragma once
#include <variant>
#include <memory>
#include <vector>
#include <string>
#include <optional>

#include <lpimgproc/Operators.h>

struct OutputPin;
class Node;

namespace lpimgproc {
    class Image;
}

using ImagePtr = std::shared_ptr<lpimgproc::Image>;

using Value = std::variant<std::monostate, ImagePtr, uint32_t, float, std::string>;

struct InputPin {
    OutputPin* connection = nullptr;  
    size_t outputIndex = 0;           
    Node* node = nullptr;             
};

struct OutputPin {
    Value value;
    Node* node = nullptr;             
    std::vector<InputPin*> connections;
};

class Node {
public:
    virtual ~Node() = default;

    virtual std::string name() const = 0;

    Value evaluateOutput(size_t output_index) {
        if (dirty_) {
            compute();
            dirty_ = false;
        }
        return outputs_[output_index].value;
    }

    void makeDirty() {
        if (dirty_) return;
        dirty_ = true;
        for (OutputPin& out : outputs_) {
            for (InputPin* conn : out.connections) {
                if (conn && conn->node) {       
                    conn->node->makeDirty();
                }
            }
        }
    }

    bool dirty() const { return dirty_; }

    const std::vector<InputPin>& inputs() const { return inputs_; }
    const std::vector<OutputPin>& outputs() const { return outputs_; }
    std::vector<InputPin>& inputs() { return inputs_; }
    std::vector<OutputPin>& outputs() { return outputs_; }

protected:
    void initInputs(size_t n) {
        inputs_.clear();
        inputs_.resize(n);
        for (auto& in : inputs_) in.node = this;
    }

    void initOutputs(size_t n) {
        outputs_.clear();
        outputs_.resize(n);
        for (auto& out : outputs_) out.node = this;
    }

    Value getInputValue(size_t input_index) {
        if (input_index >= inputs_.size()) return {};
        InputPin& in = inputs_[input_index];
        if (!in.connection) return {};
        return in.connection->node->evaluateOutput(in.outputIndex);
    }

    virtual void compute() = 0;

    std::vector<InputPin> inputs_;
    std::vector<OutputPin> outputs_;
    bool dirty_ = true;
};


