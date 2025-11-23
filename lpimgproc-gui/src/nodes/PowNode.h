#include "nodes/Node.h"

class PowNode : public Node {
public:
    PowNode() {
        initInputs(2);
        initOutputs(1);

        inputs_[0].name = "Image";
        inputs_[1].name = "exponent";
    }

    std::string name() const override { return "Power"; }



protected:
    void compute() override {
        Value inImg = getInputValue(0);
        Value inExp = getInputValue(1);

        auto pimg = std::get_if<ImagePtr>(&inImg);
        auto pExp = std::get_if<float>(&inExp);


        if (*pimg && *pExp) {
            lpimgproc::Image out = lpimgproc::operators::pow(**pimg, *pExp);
            outputs_[0].value = std::make_shared<lpimgproc::Image>(std::move(out));
            return;

        }

        outputs_[0].value = std::monostate{};
    }

};
