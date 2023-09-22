/*****************************************************************//**
 * \file Filter.h
 * \author ���դ�
 * \date 2023/9/18
 * \brief ��Filter_t������x�s�@��filter
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
		std::unique_ptr<float[]> _filter; //!< Row�C Col��

	public:
		//! @brief Construct from a list, ex: { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }.
		//! std::out_of_range is thrown, if `_list`'s size is bigger than Row * Col.
		//! @param Row - How many rows the filter have
		//! @param Col - How many columns the filter have
		//! @param _list - A list contains lists of float. If _list isn't specified, filter's content is undefined.
		Filter_t(int Row, int Col, std::initializer_list<std::initializer_list<float>> _list = {});

		//! @brief �M��filter��(r1, c1)�M(r2, c2)�ҳ򪺪Ŷ��C
		//! �p�G(r1, c1)�M(r2, c2)��X���Ŷ����ORow�CCol��A�h��Xstd::invalid_argument�C
		//! @param r1 - ���W�����C
		//! @param c1 - ���W������
		//! @param r2 - �k�U�����C
		//! @param c2 - �k�U������
		//! @param image - image's info
		Color::RGB_t calculate(int r1, int c1, int r2, int c2, const ImageInfo_t& image) const;

		/**
		 * @brief �s��filter���������A�Y�W�X�d��h��Xstd::out_of_range
		 * @param r - �C
		 * @param c - ��
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
