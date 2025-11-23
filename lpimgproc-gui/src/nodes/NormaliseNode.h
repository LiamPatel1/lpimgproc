#include "nodes/Node.h"

class NormaliseNode : public Node {
public:
    NormaliseNode() {
        initInputs(1);
        initOutputs(1);
    }

    std::string name() const override { return "Normalise"; }



protected:
    void compute() override {
        Value inImg = getInputValue(0);

        auto pimg = std::get_if<ImagePtr>(&inImg);


        if (*pimg) {
            lpimgproc::Image out = lpimgproc::operators::normalise(**pimg);
            outputs_[0].value = std::make_shared<lpimgproc::Image>(std::move(out));
            return;

        }

        outputs_[0].value = std::monostate{};
    }

};
