#pragma once
#include <vector>

namespace lpimgproc {

	class Image {

	public:

		enum ColourSpace : uint8_t {
			RGB,
			RGBA,
			G,
			GA,
		};

		static constexpr uint8_t channels(ColourSpace colour_space) {
			switch (colour_space) {
			case RGB: return 3;
			case RGBA: return 4;
			case G: return 1;
			case GA: return 2;
			default: return 0;

			}
		}

		static constexpr bool hasAlpha(ColourSpace colour_space) {
			switch (colour_space) {
			case GA:
			case RGBA:
				return true;
			default:
				return false;
			}
		}

		Image(uint32_t width, uint32_t height, ColourSpace colour_space);
		Image(uint32_t width, uint32_t height, ColourSpace colour_space, const std::vector<float>& data);

		float& at(uint32_t x, uint32_t y, uint8_t c);
		const float& at(uint32_t x, uint32_t y, uint8_t c) const;
		float& at(uint64_t sub);
		const float& at(uint64_t sub) const;

		const uint32_t width() const;
		const uint32_t height() const;

		const ColourSpace colourSpace() const;

		uint64_t subpixelCount() const;

		const std::vector<float>& data() const;

		
	private:
		const uint32_t width_;
		const uint32_t height_;
		const ColourSpace colour_space_;
		std::vector<float> data_;
	};




}