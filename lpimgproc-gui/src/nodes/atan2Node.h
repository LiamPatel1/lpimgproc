#include "nodes/Node.h"

class Atan2Node : public Node {
public:
    Atan2Node() {
        initInputs(2);
        initOutputs(1);
    }

    std::string name() const override { return "atan2"; }



protected:
    void compute() override {
        Value inImg1 = getInputValue(0);
        Value inImg2 = getInputValue(1);

        auto pimg1 = std::get_if<ImagePtr>(&inImg1);
        auto pimg2 = std::get_if<ImagePtr>(&inImg2);


        if (*pimg1 && *pimg2) {
            lpimgproc::Image out = lpimgproc::operators::atan2(**pimg1, **pimg2);
            outputs_[0].value = std::make_shared<lpimgproc::Image>(std::move(out));
            return;

        }

        outputs_[0].value = std::monostate{};
    }

};
