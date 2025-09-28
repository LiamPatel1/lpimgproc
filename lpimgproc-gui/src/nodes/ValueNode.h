#include "nodes/Node.h"

class ValueNode : public Node {
public:
    ValueNode() {
        initOutputs(1);
    }

    std::string name() const override { return "Value"; }

    void setValue(float val) {
        if (value_ != val) {
            value_ = val;
            makeDirty();
        }
    }

protected:
    void compute() override {
        outputs_[0].value = value_;
    }

private:
    float value_ = 0.0f;
};