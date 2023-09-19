/*****************************************************************//**
 * \file Filter.h
 * \author ���դ�
 * \date 2023/9/18
 * \brief ��Filter_t������x�s�@��filter�Ϊ�mask
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

	//! a Size * Size filter
	template <int Size>
	class Filter_t {
		static_assert((Size & 1) && Size > 0, "Size must be odd and positive");

		float mask[Size][Size]; //!< Size�C Size��

	public:
		//! @brief construct from a list, ex: { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }
		//! @param _list - a list contains lists of float. std::out_of_range is thrown, if `_list`'s size is bigger than Size * Size
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

		//! @brief Put mask on image's pixel(r, c), and return result
		//! @param r - row of pixel
		//! @param c - column of pixel
		//! @param image - image's info
		Color::RGB_t calculate(int r, int c, const ImageInfo_t& image) const {
			constexpr int half_size = Size / 2;
			float result[3] = { 0, 0, 0 };

			// return true if (r, c) is outside image 
			auto outside_image = [&image](int r, int c) -> bool {
				return r < 0 || c < 0 || r >= image.height || c >= image.width;
			};

			for (int deltaR = -half_size; deltaR <= half_size; ++deltaR) {
				for (int deltaC = -half_size; deltaC <= half_size; ++deltaC) {
					if (outside_image(r + deltaR, c + deltaC)) continue;

					int id = ((r + deltaR) * image.width + (c + deltaC)) * 4;
					// ���W���Y��
					float rate = mask[deltaR + half_size][deltaC + half_size];

					result[0] += image.data[id] * rate;
					result[1] += image.data[id + 1] * rate;
					result[2] += image.data[id + 2] * rate;
				}
			}

			return Color::RGB_t((uint8_t)result[0], (uint8_t)result[1], (uint8_t)result[2]);
		}

		/**
		 * @brief �s��mask���������A�Y�W�X�d��h��Xstd::out_of_range
		 * @param r - �C
		 * @param c - ��
		 */
		inline float& at(int r, int c) {
			if (r >= Size || c >= Size || r < 0 || c < 0) throw std::out_of_range("Filtert_t::at() : Out of range");
			return mask[r][c];
		}
		inline const float& at(int r, int c) const {
			if (r >= Size || c >= Size || r < 0 || c < 0) throw std::out_of_range("Filtert_t::at() : Out of range");
			return mask[r][c];
		}
	}; // class Filter_t
}
