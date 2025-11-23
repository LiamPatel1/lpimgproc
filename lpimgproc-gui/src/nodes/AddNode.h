#include "nodes/Node.h"

class AddNode : public Node {
public:
    AddNode() {
        initInputs(2);
        initOutputs(1);
    }

    std::string name() const override { return "Add"; }



protected:
    void compute() override {
        Value inImg1 = getInputValue(0);
        Value inImg2 = getInputValue(1);

        auto pimg1 = std::get_if<ImagePtr>(&inImg1);
        auto pimg2 = std::get_if<ImagePtr>(&inImg2);


        if (*pimg1 && *pimg2) {
            lpimgproc::Image out = lpimgproc::operators::add(**pimg1, **pimg2);
            outputs_[0].value = std::make_shared<lpimgproc::Image>(std::move(out));
            return;

        }

        outputs_[0].value = std::monostate{};
    }

};
