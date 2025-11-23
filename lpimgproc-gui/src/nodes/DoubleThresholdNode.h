#include "nodes/Node.h"

class DoubleThresholdNode : public Node {
public:
    DoubleThresholdNode() {
        initInputs(3);
        initOutputs(1);

        inputs_[0].name = "Image";
        inputs_[1].name = "Low Threshold";
        inputs_[2].name = "High Threshold";

    }

    std::string name() const override { return "Double Threshold"; }



protected:
    void compute() override {
        Value inImg = getInputValue(0);
        Value inLow = getInputValue(1);
        Value inHigh = getInputValue(2);

        auto pimg = std::get_if<ImagePtr>(&inImg);
        auto pLow = std::get_if<float>(&inLow);
        auto pHigh = std::get_if<float>(&inHigh);


        if (*pimg && *pLow && pHigh) {
            lpimgproc::Image out = lpimgproc::operators::doubleThreshold(**pimg, *pLow, *pHigh);
            outputs_[0].value = std::make_shared<lpimgproc::Image>(std::move(out));
            return;

        }

        outputs_[0].value = std::monostate{};
    }

};
