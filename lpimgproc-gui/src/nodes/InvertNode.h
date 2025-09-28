#include "nodes/Node.h"

class InvertNode : public Node {
public:
    InvertNode() {
        initInputs(1);
        initOutputs(1);
    }

    std::string name() const override { return "Invert"; }

    bool invertsAlpha() const { return invert_alpha_; }

    void setInvertAlpha(bool invert) {
        if (invert_alpha_ != invert) {
            invert_alpha_ = invert;
            makeDirty();
        }
    }


protected:
    void compute() override {
        Value inVal = getInputValue(0);
        if (auto pimg = std::get_if<ImagePtr>(&inVal)) {
            if (*pimg) {
                auto flags = invert_alpha_
                    ? lpimgproc::operators::InvertFlags::InvertAlpha
                    : lpimgproc::operators::InvertFlags::None;

                lpimgproc::Image out = lpimgproc::operators::invert(**pimg, flags);
                outputs_[0].value = std::make_shared<lpimgproc::Image>(std::move(out));
                return;
            }
        }
        outputs_[0].value = std::monostate{};
    }
private:
    bool invert_alpha_ = false;
};
