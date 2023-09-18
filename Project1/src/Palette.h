/*****************************************************************//**
 * \file Palette.h
 * \author ���դ�
 * \date 2023/9/17
 * \brief ��Palette�����x�s�i�Ϊ��C��
 *********************************************************************/
#pragma once
#include <vector>
#include <stdint.h>

namespace Color {
    //! �x�s�C��A�C��channel�@��byte
    struct RGB_t {
        uint8_t R;
        uint8_t G;
        uint8_t B;

        //! default constructor, set all chanel to zero
        constexpr RGB_t() : R(0), G(0), B(0) {}

        //! specify R, G, B
        constexpr RGB_t(uint8_t _R, uint8_t _G, uint8_t _B) : R(_R), G(_G), B(_B) {}

        //! construct from an byte array
        //! @param arr - ����0, 1, 2����R, G, B�C���ץ���>=3�]�����ˬd�^
        constexpr explicit RGB_t(uint8_t* arr) : R(arr[0]), G(arr[1]), B(arr[2]) {}

        //! copy constructor
        constexpr RGB_t(const RGB_t&) = default;

        //! move constructor
        constexpr RGB_t(RGB_t&&) = default;

        //! assignment
        RGB_t& operator=(const RGB_t&) = default;

        //! move assignment
        RGB_t& operator=(RGB_t&&) = default;
    };

    //! �x�s�i�Ϊ��C��
    class Palette_t : public std::vector<RGB_t> {
    public:
        //! default constructor
        Palette_t() {}

        /**
         * \brief ���w�ƶq�M���
         * \param count - �ƶq
         * \param val - ��ȡ]�w�]�� `RGB_t()`�^
         */
        explicit Palette_t(size_t count, RGB_t val = RGB_t()) : std::vector<RGB_t>(count, val) {}

        /**
         * \brief ��initializer_list��l��
         * \param list - �H {} �]���@�s�� Color::RGB_t
         */
        Palette_t(std::initializer_list<RGB_t> list) : std::vector<RGB_t>(list) {}

        /**
         * \brief ��� Palette ���u�ڤ�Z���v�� color �̪��C��C�Y Palette ���ūh�����^�� color
         * \param color - �Y���C��
         */
        RGB_t find_closest_to(RGB_t color) const;
    };

    /**
     * \brief �p����C�⪺�ڤ�Z��������
     * \param c1 - color1
     * \param c2 - color2
     */
    inline int euclidean_distance_square(RGB_t c1, RGB_t c2) {
        int deltaR = (int)c1.R - c2.R, deltaG = (int)c1.G - c2.G, deltaB = (int)c1.B - c2.B;
        return deltaR * deltaR + deltaG * deltaG + deltaB * deltaB;
    }

    /**
     * \brief �Ncolor��RGB�̧Ǽg�Jarr����0�B1�B2��
     * \param arr - �}�C�C����>=3�]�����ˬd�^
     * \param color - �C��
     */
    inline void memset(uint8_t* arr, RGB_t color) {
        arr[0] = color.R; arr[1] = color.G; arr[2] = color.B;
    }

    //! �զ�
    constexpr RGB_t White(255, 255, 255);

    //! �¦�
    constexpr RGB_t Black;
}
