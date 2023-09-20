#include "Filter.h"

Filter::Filter_t::Filter_t(int _Row, int _Col, std::initializer_list<std::initializer_list<float>> _list)
	: Row(_Row), Col(_Col), mask(new float[_Row * _Col]), calculate_buffer(new Color::RGB_t[_Row * _Col])
{
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

	// fill
	if (!outside_image(r2, c2))
		// 右下往左上填
		fill_calculate_buffer(r2, c2, r1, c1, image, Fill_Mode_t::RD_LT);
	else if (!outside_image(r1, c1))
		// 左上向右下填
		fill_calculate_buffer(r1, c1, r2, c2, image, Fill_Mode_t::LT_RD);
	else if (!outside_image(r1, c2))
		// 右上向左下填
		fill_calculate_buffer(r1, c2, r2, c1, image, Fill_Mode_t::RT_LD);
	else
		// 左下向右上填
		fill_calculate_buffer(r2, c1, r1, c2, image, Fill_Mode_t::LD_RT);

	for (int r = r1; r <= r2; ++r) {
		for (int c = c1; c <= c2; ++c) {
			Color::RGB_t color = calculate_buffer[(r - r1) * Col + (c - c1)];
			float rate = this->at(r - r1, c - c1);

			result[0] += color.R * rate;
			result[1] += color.G * rate;
			result[2] += color.B * rate;
		}
	}

#define clip(val) (uint8_t)(val < 0 ? 0 : val > 0xFF ? 0xFF : val)
	return Color::RGB_t(clip(result[0]), clip(result[1]), clip(result[2]));
#undef clip
}

void Filter::Filter_t::fill_calculate_buffer(int rs, int cs, int re, int ce, const ImageInfo_t& image, Fill_Mode_t mode) const {
	memset(calculate_buffer.get(), 0, sizeof(Color::RGB_t) * Row * Col);

	int deltaR = (mode == Fill_Mode_t:: LT_RD || mode == Fill_Mode_t::RT_LD ? 1 : -1); // 上往下則1否則-1
	int deltaC = (mode == Fill_Mode_t::LT_RD || mode == Fill_Mode_t::LD_RT ? 1 : -1); // 左往右則1否則-1
	int r = rs;
	// 最上方那列
	int upR = (mode == Fill_Mode_t::LT_RD || mode == Fill_Mode_t::RT_LD ? rs : re);
	// 最左方那欄
	int leftC = (mode == Fill_Mode_t::LT_RD || mode == Fill_Mode_t::LD_RT ? cs : ce);

	// 從rs填到re
	while (true) {
		for (int c = cs; ; c += deltaC) {
			// 現在要填入calculate_buffer中的bufferR列bufferC欄
			int bufferR = r - upR;
			int bufferC = c - leftC;
			int index_of_buffer = bufferR * Col + bufferC;

			if (0 <= r && r < image.height && 0 <= c && c < image.width) { // 若image有第r列第c欄，則直接從image複製
				calculate_buffer[index_of_buffer] = Color::RGB_t(image.data + (r * image.width + c) * 4);
			}
			else if (r < 0 || r >= image.height) { // r超出邊界，則鉛直對稱去複製calculate_buffer中的內容
				calculate_buffer[index_of_buffer] = calculate_buffer[(Row - bufferR - 1) * Col + bufferC];
			}
			else { // 水平對稱去複製calculate_buffer中的內容
				calculate_buffer[index_of_buffer] = calculate_buffer[bufferR * Col + (Col - bufferC - 1)];
			}

			if (c == ce) break;
		}

		if (r == re) break;
		r += deltaR;
	}
}
