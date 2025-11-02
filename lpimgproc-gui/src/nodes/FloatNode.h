#include "nodes/Node.h"

class FloatNode : public Node {
public:
    FloatNode() {
        initOutputs(1);
    }

    std::string name() const override { return "Float"; }

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