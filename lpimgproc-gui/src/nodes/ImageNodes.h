#include "nodes/Node.h"


class ImageInputNode : public Node {
public:
    explicit ImageInputNode(ImagePtr img = nullptr) {
        initOutputs(1);
        image_ = std::move(img);
    }

    std::string name() const override { return "Image Input"; }

    void setImage(ImagePtr img) { image_ = std::move(img); makeDirty(); }
protected:
    void compute() override {
        outputs_[0].value = image_ ? Value(image_) : Value(std::monostate{});
    }
private:
    ImagePtr image_;
};

class ImageDisplayNode : public Node {
public:
    ImageDisplayNode() { initInputs(1); }

    std::string name() const override { return "Display"; }

    ImagePtr getImage() {
        Value v = getInputValue(0);
        if (auto pimg = std::get_if<ImagePtr>(&v)) return *pimg;
        return nullptr;
    }
protected:
    void compute() override {}
};


