#include "nodes/Node.h"

class NonMaximumSuppressionNode : public Node {
public:
    NonMaximumSuppressionNode() {
        initInputs(2);
        initOutputs(1);

        inputs_[0].name = "Magnitudes";
        inputs_[1].name = "Angles";
    }

    std::string name() const override { return "Non-Maximum Suppression"; }



protected:
    void compute() override {
        Value inMags = getInputValue(0);
        Value inAngs = getInputValue(1);

        auto pMags = std::get_if<ImagePtr>(&inMags);
        auto pAngs = std::get_if<ImagePtr>(&inAngs);


        if (*pMags && *pAngs) {
            lpimgproc::Image out = lpimgproc::operators::nonMaximumSuppression(**pMags, **pAngs);
            outputs_[0].value = std::make_shared<lpimgproc::Image>(std::move(out));
            return;

        }

        outputs_[0].value = std::monostate{};
    }

};
