/*****************************************************************//**
 * \file Filter.h
 * \author 蔣博元
 * \date 2023/9/18
 * \brief 用Filter_t物件來儲存一組filter
 *********************************************************************/
#pragma once
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include "Palette.h"

namespace Filter {
	//! store image's infomation
	struct ImageInfo_t {
		int height;
		int width;
		const unsigned char* data;
	};

	//! a Row * Col filter
	class Filter_t {
		int Row;
		int Col;
		std::unique_ptr<float[]> _filter; //!< Row列 Col欄

	public:
		//! @brief Construct from a list, ex: { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }.
		//! std::out_of_range is thrown, if `_list`'s size is bigger than Row * Col.
		//! @param Row - How many rows the filter have
		//! @param Col - How many columns the filter have
		//! @param _list - A list contains lists of float. If _list isn't specified, filter's content is undefined.
		Filter_t(int Row, int Col, std::initializer_list<std::initializer_list<float>> _list = {});

		//! @brief 套用filter到(r1, c1)和(r2, c2)所圍的空間。
		//! 如果(r1, c1)和(r2, c2)圍出的空間不是Row列Col欄，則丟出std::invalid_argument。
		//! @param r1 - 左上角的列
		//! @param c1 - 左上角的欄
		//! @param r2 - 右下角的列
		//! @param c2 - 右下角的欄
		//! @param image - image's info
		Color::RGB_t calculate(int r1, int c1, int r2, int c2, const ImageInfo_t& image) const;

		/**
		 * @brief 存取filter中的元素，若超出範圍則丟出std::out_of_range
		 * @param r - 列
		 * @param c - 欄
		 */
		inline float& at(int r, int c) {
			if (r >= Row || c >= Col || r < 0 || c < 0) throw std::out_of_range("Filtert_t::at() : Out of range");
			return _filter[(size_t)r * Col + c];
		}
		inline float at(int r, int c) const {
			if (r >= Row || c >= Col || r < 0 || c < 0) throw std::out_of_range("Filtert_t::at() : Out of range");
			return _filter[(size_t)r * Col + c];
		}

		//! Get Row
		int getRow(void) const { return Row; }
		//! Get Col
		int getCol(void) const { return Col; }

	private:
		/**
		 * @brief This is the helper function for Filter_t::calculate
		 * @param rs - start row
		 * @param cs - start column
		 * @param re - end row
		 * @param ce - end column
		 * @param image - information of the image
		 */
		Color::RGB_t do_the_calculate(int rs, int cs, int re, int ce, const ImageInfo_t& image) const;
	}; // class Filter_t
}
