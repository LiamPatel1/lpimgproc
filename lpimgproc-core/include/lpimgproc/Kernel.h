#pragma once
#include <vector>
#include <stdexcept>
namespace lpimgproc {
	class Kernel {

		std::vector<float> data_;
		uint32_t width_;
		uint32_t height_;

	public:

		explicit Kernel(uint32_t width, uint32_t height, float value = 0.0f)
			: data_(width* height, value), width_(width), height_(height) {
		}

		explicit Kernel(const std::vector<float>& data, uint32_t width, uint32_t height) {

			if (width * height != data.size()) {
				width_ = 0;
				height_ = 0;
				data_ = {};
				throw std::invalid_argument("Kernel data size does not match width * height");
				return;
			}

			width_ = width;
			height_ = height;
			data_ = data;
		}

		float& at(uint32_t x, uint32_t y) {
			return data_[width_ * y + x];
		}
		const float& at(uint32_t x, uint32_t y) const {
			return data_[width_ * y + x];
		}
		float& at(uint64_t idx) {
			return data_[idx];
		}
		const float& at(uint64_t idx) const {
			return data_[idx];
		}

		uint32_t width() const { return width_; }
		uint32_t height() const { return height_; }

		const std::vector<float>& data() const { return data_; }
		std::vector<float>& data() { return data_; }

	};
}