#pragma once
#include <array>
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
	template <size_t Size>
	class Filter_t {
		static_assert((Size & 1), "Size must be both odd");

		std::array<std::array<float, Size>, Size> mask; //!< Size¦C SizeÄæ

	public:
		//! @brief construct from a list, ex: { {1, 2}, {3, 4} }
		Filter_t(std::initializer_list<std::initializer_list<float>> _list) : mask{0} {
			int r = 0;
			for (const auto& row : _list) {
				int c = 0;
				for (const auto& elem : row) {
					mask[r][c] = elem;
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
			int half_size = Size / 2;
			float result[3] = { 0, 0, 0 };

			// return true if (r, c) is outside image 
			auto outside_image = [&image](int r, int c) -> bool {
				return r < 0 || c < 0 || r >= image.height || c >= image.width;
			};

			for (int deltaR = -half_size; deltaR <= half_size; ++deltaR) {
				for (int deltaC = -half_size; deltaC <= half_size; ++deltaC) {
					if (outside_image(r + deltaR, c + deltaC)) continue;

					int id = ((r + deltaR) * image.width + (c + deltaC)) * 4;
					// ­¼¤Wªº«Y¼Æ
					float multiplier = mask[deltaR + half_size][deltaC + half_size];

					result[0] += image.data[id] * multiplier;
					result[1] += image.data[id + 1] * multiplier;
					result[2] += image.data[id + 2] * multiplier;
				}
			}

			return Color::RGB_t(result[0], result[1], result[2]);
		}
	};
}
