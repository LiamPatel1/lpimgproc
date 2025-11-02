#include "nodes/Node.h"
#include <lpimgproc/Kernel.h>

class KernelNode : public Node {
public:
    KernelNode() {
        initOutputs(1);
    }

    std::string name() const override { return "Kernel"; }

    void setValue(KernelPtr kernel) {
        if (kernel_ != kernel) {
            kernel_ = kernel;
            makeDirty();
        }
    }



protected:
    void compute() override {
        outputs_[0].value = kernel_;
    }

private:
    KernelPtr kernel_;
};