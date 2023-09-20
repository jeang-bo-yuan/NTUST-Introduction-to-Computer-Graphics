#include "Filter.h"

Filter::Filter_t::Filter_t(int _Row, int _Col, std::initializer_list<std::initializer_list<float>> _list)
	: Row(_Row), Col(_Col), mask(new float[_Row * _Col]) {
	int r = 0;
	for (const auto& row : _list) {
		int c = 0;
		for (const auto& elem : row) {
			this->at(r, c) = elem;
			++c;
		}
		++r;
	}
}

Color::RGB_t Filter::Filter_t::calculate(int r1, int c1, int r2, int c2, const ImageInfo_t& image) const {
	if ((r2 - r1 + 1 != Row) || (c2 - c1 + 1 != Col))
		throw std::invalid_argument("Filter_t::calculate() : the area bounded by (r1, c1) ~ (r2, c2) must be Row * Col");

	float result[3] = { 0, 0, 0 };

	// return true if (r, c) is outside image 
	auto outside_image = [&image](int r, int c) -> bool {
		return r < 0 || c < 0 || r >= image.height || c >= image.width;
	};

	for (int r = r1; r <= r2; ++r) {
		for (int c = c1; c <= c2; ++c) {
			if (outside_image(r, c)) continue;

			int id = (r * image.width + c) * 4;
			float rate = this->at(r - r1, c - c1);

			result[0] += image.data[id] * rate;
			result[1] += image.data[id + 1] * rate;
			result[2] += image.data[id + 2] * rate;
		}
	}

#define clip(val) (uint8_t)(val < 0 ? 0 : val > 0xFF ? 0xFF : val)
	return Color::RGB_t(clip(result[0]), clip(result[1]), clip(result[2]));
#undef clip
}
