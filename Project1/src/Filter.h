/*****************************************************************//**
 * \file Filter.h
 * \author 蔣博元
 * \date 2023/9/18
 * \brief 用Filter_t物件來儲存一組filter用的mask
 *********************************************************************/
#pragma once
#include <initializer_list>
#include "Palette.h"

namespace Filter {
	//! store image's infomation
	struct ImageInfo_t {
		int height;
		int width;
		const unsigned char* data;
	};

	//! a Row * Col filter
	template <int Row, int Col>
	class Filter_t {
		static_assert(Row > 0 && Col > 0, "Row and Col must be both positive");

		float mask[Row][Col]; //!< Row列 Col欄

	public:
		//! @brief Construct from a list, ex: { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }.
		//! std::out_of_range is thrown, if `_list`'s size is bigger than Row * Col.
		//! @param _list - a list contains lists of float.
		Filter_t(std::initializer_list<std::initializer_list<float>> _list) : mask{0} {
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

		//! @brief Put mask on image's pixel(r1, c1) ~ pixel(r2, c2), and return result.
		//! 如果(r1, c1)和(r2, c2)圍出的空間不是Row列Col欄，則丟出std::invalid_argument
		//! @param r1 - start row
		//! @param c1 - start column
		//! @param r2 - end row
		//! @param c2 - end column
		//! @param image - image's info
		Color::RGB_t calculate(int r1, int c1, int r2, int c2, const ImageInfo_t& image) const {
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
					float rate = mask[r - r1][c - c1];

					result[0] += image.data[id] * rate;
					result[1] += image.data[id + 1] * rate;
					result[2] += image.data[id + 2] * rate;
				}
			}

			return Color::RGB_t((uint8_t)result[0], (uint8_t)result[1], (uint8_t)result[2]);
		}

		/**
		 * @brief 存取mask中的元素，若超出範圍則丟出std::out_of_range
		 * @param r - 列
		 * @param c - 欄
		 */
		inline float& at(int r, int c) {
			if (r >= Row || c >= Col || r < 0 || c < 0) throw std::out_of_range("Filtert_t::at() : Out of range");
			return mask[r][c];
		}
		inline const float& at(int r, int c) const {
			if (r >= Row || c >= Col || r < 0 || c < 0) throw std::out_of_range("Filtert_t::at() : Out of range");
			return mask[r][c];
		}
	}; // class Filter_t
}
