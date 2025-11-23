#include "nodes/Node.h"

class GrayscaleNode : public Node {
public:
    GrayscaleNode() {
        initInputs(1);
        initOutputs(1);
    }

    std::string name() const override { return "Grayscale"; }



protected:
    void compute() override {
        Value inImg = getInputValue(0);

        auto pimg = std::get_if<ImagePtr>(&inImg);


        if (*pimg) {
            lpimgproc::Image out = lpimgproc::operators::grayscale(**pimg);
            outputs_[0].value = std::make_shared<lpimgproc::Image>(std::move(out));
            return;

        }

        outputs_[0].value = std::monostate{};
    }

};
