#include "Filter.h"
#include <cmath>

Filter::Filter_t::Filter_t(int _Row, int _Col, std::initializer_list<std::initializer_list<float>> _list)
	: Row(_Row), Col(_Col), _filter(new float[_Row * _Col])
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

	auto row_outside = [&image](int r) -> bool {
		return r < 0 || r >= image.height;
	};
	auto col_outside = [&image](int c) -> bool {
		return c < 0 || c >= image.width;
	};

	// return true if (r, c) is outside image 
	auto outside_image = [&image, &row_outside, &col_outside](int r, int c) -> bool {
		return row_outside(r) || col_outside(c);
	};

	// �YFilter��ӧ@�Φb�ϥ~�A�h�^�Ƕ¦�
	if (row_outside(r1) && row_outside(r2) && col_outside(c1) && col_outside(c2))
		return Color::Black;

	// fill
	if (!outside_image(r2, c2))
		// �k�U�����W
		return do_the_calculate(r2, c2, r1, c1, image);
	else if (!outside_image(r1, c1))
		// ���W�V�k�U
		return do_the_calculate(r1, c1, r2, c2, image);
	else if (!outside_image(r1, c2))
		// �k�W�V���U
		return do_the_calculate(r1, c2, r2, c1, image);
	else
		// ���U�V�k�W
		return do_the_calculate(r2, c1, r1, c2, image);
}

Color::RGB_t Filter::Filter_t::do_the_calculate(int rs, int cs, int re, int ce, const ImageInfo_t& image) const {
	float result[3] = { 0.f, 0.f, 0.f };

	const int deltaR = (rs <= re ? 1 : -1); // �W���U�h1�_�h-1
	const int deltaC = (cs <= ce ? 1 : -1); // �����k�h1�_�h-1
	const int upR = std::min(rs, re); // �̤W�診�C
	const int leftC = std::min(cs, ce); // �̥��診��

	// �qrs���re
	for (int r = rs; ; r += deltaR) {
		for (int c = cs; ; c += deltaC) {
			// �{�b�B�z��������RGB
			Color::RGB_t color_of_pixel;

			// �{�b�B�z�������n���W__filter��(filterR�C, filterC��)����
			int filterR = r - upR;
			int filterC = c - leftC;

			if (r < 0 || r >= image.height) { // r�W�X��ɡA�h�]����٥h������
				int new_R = Row - filterR - 1 + upR, new_C = c;
				// �p�Gc�]�W�X��ɡA�hc�]�������
				if (new_C < 0 || new_C >= image.width)
					new_C = Col - filterC - 1 + leftC;
				color_of_pixel = Color::RGB_t(image.data + (new_R * image.width + new_C) * 4);
			}
			else if (0 <= c && c < image.width) { // �Yimage����r�C��c��A�h�����qimage��
				color_of_pixel = Color::RGB_t(image.data + (r * image.width + c) * 4);
			}
			else { // ������٥h��
				int new_C = Col - filterC - 1 + leftC;
				color_of_pixel = Color::RGB_t(image.data + (r * image.width + new_C) * 4);
			}

			// ���Wrate�å[�Jresult
			float rate = this->at(filterR, filterC);
			result[0] += color_of_pixel.R * rate;
			result[1] += color_of_pixel.G * rate;
			result[2] += color_of_pixel.B * rate;

			if (c == ce) break;
		}

		if (r == re) break;
	}

#define clip(val) (uint8_t)(val < 0 ? 0 : val > 0xFF ? 0xFF : val)
	return Color::RGB_t(clip(result[0]), clip(result[1]), clip(result[2]));
#undef clip
}

Filter::Filter_t Filter::Filter_t::bartlett4_4(float r, float c) {
	// domain: [-3, 3]
	const auto kernel = [](float x) -> float {
		return 1 / 3.f - fabs(x) / 9.f;
	};

	// ���G��Filter�|�H(centerR, centerC)������
	int centerR = roundf(r), centerC = roundf(c);

	// �Yr, c���ܱ����ơA�h�ϥ�kernel = 1/2 - x/4���X��3 * 3 filter
	if (fabs(r - centerR) < 0.001 && fabs(c - centerC) < 0.001) {
		return Filter::Filter_t(4, 4, {
			{ 0,        0,        0,        0 },
			{ 0, 1 / 16.f, 2 / 16.f, 1 / 16.f },
			{ 0, 2 / 16.f, 4 / 16.f, 2 / 16.f },
			{ 0, 1 / 16.f, 2 / 16.f, 1 / 16.f },
		});
	}

	Filter::Filter_t result(4, 4);
	float one_dim_horizontal[4] = { 0.f };
	float one_dim_vertical[4] = { 0.f };

	// ���D�@��filter
	for (int id = 0; id < 4; ++id) {
		one_dim_horizontal[id] = kernel(centerC - 2 + id - c);
		one_dim_vertical[id] = kernel(centerR - 2 + id - r);
	}

	// �H�@��filter���ۭ��D�G��filter
	float sum = 0;
	for (int _r = 0; _r < 4; ++_r) {
		for (int _c = 0; _c < 4; ++_c) {
			result.at(_r, _c) = one_dim_horizontal[_c] * one_dim_vertical[_r];
			sum += result.at(_r, _c);
		}
	}

	// ��filter�U�����`�M��1
	for (int _r = 0; _r < 4; ++_r) {
		for (int _c = 0; _c < 4; ++_c) {
			result.at(_r, _c) /= sum;
		}
	}

	return result;
}
