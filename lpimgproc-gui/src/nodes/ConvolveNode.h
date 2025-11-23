#include "nodes/Node.h"

class ConvolveNode : public Node {
public:
    ConvolveNode() {
        initInputs(2);
        initOutputs(1);

        inputs_[0].name = "Image";
        inputs_[1].name = "Kernel";
    }

    std::string name() const override { return "Convolve"; }



protected:
    void compute() override {
        Value inImg = getInputValue(0);
        Value inKernal = getInputValue(1);

        auto pimg = std::get_if<ImagePtr>(&inImg); 
        auto pkernel = std::get_if<KernelPtr>(&inKernal);


            if (*pimg && *pkernel) {
                lpimgproc::Image out = lpimgproc::operators::convolve(**pimg, **pkernel);
                outputs_[0].value = std::make_shared<lpimgproc::Image>(std::move(out));
                return;
            
        }

        outputs_[0].value = std::monostate{};
    }

};
