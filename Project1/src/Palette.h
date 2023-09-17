/*****************************************************************//**
 * \file Palette.h
 * \author ���դ�
 * \date 2023/9/17
 * \brief ��Palette�����x�s�i�Ϊ��C��
 *********************************************************************/
#include <vector>
#include <stdint.h>

namespace Color {
    //! �x�s�C��A�C��channel�@��byte
    struct RGB_t {
        uint8_t R;
        uint8_t G;
        uint8_t B;

        //! default constructor, set all chanel to zero
        RGB_t() : R(0), G(0), B(0) {}
        //! specify R, G, B
        RGB_t(uint8_t _R, uint8_t _G, uint8_t _B) : R(_R), G(_G), B(_B) {}
        //! construct from an byte array
        //! @param arr - ����0, 1, 2����R, G, B�C���ץ���>=3�]�����ˬd�^
        explicit RGB_t(uint8_t* arr) : R(arr[0]), G(arr[1]), B(arr[2]) {}
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
        explicit Palette_t(size_t count, const RGB_t& val = RGB_t()) : std::vector<RGB_t>(count, val) {}

        /**
         * \brief ��� Palette ���u�ڤ�Z���v�� color �̪��C��C�Y Palette ���ūh�����^�� color
         * \param color - �Y���C��
         */
        const RGB_t& find_closest_to(const RGB_t& color) const;
    };
}
