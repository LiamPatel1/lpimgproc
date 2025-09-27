#include "lpimgproc/Image.h"
#include <cstdint>

namespace lpimgproc {

	Image::Image(uint32_t width, uint32_t height, ColourSpace colour_space)
		: width_(width), height_(height), colour_space_(colour_space) {

		data_.resize(width_ * height_ * channels(colour_space_), 0.0f);
	}

	Image::Image(uint32_t width, uint32_t height, ColourSpace colour_space, const std::vector<float>& data)
		: width_(width), height_(height), colour_space_(colour_space) {


		if (width * height * channels(colour_space) != data.size()) return;


		data_ = data;
	}



	 float& Image::at(uint32_t x, uint32_t y, uint8_t c) {
		return data_[c * width_ * height_ + y * width_ + x];
	}

	 const float& Image::at(uint32_t x, uint32_t y, uint8_t c) const {
		return data_[c * width_ * height_ + y * width_ + x];
	}

	 float& Image::at(uint64_t sub) {
		return data_[sub];
	}

	 const float& Image::at(uint64_t sub) const {
		return data_[sub];
	}


	uint64_t Image::subpixelCount() const {
		return data_.size();
	}


	const uint32_t Image::width() const {
		return width_;
	}
	const uint32_t Image::height() const {
		return height_;
	}

	const Image::ColourSpace Image::colourSpace() const {
		return colour_space_;
	}


	const std::vector<float>& Image::data() const { 
		return data_;
	}


}