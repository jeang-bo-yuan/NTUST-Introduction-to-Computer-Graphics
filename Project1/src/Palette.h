/*****************************************************************//**
 * \file Palette.h
 * \author 蔣博元
 * \date 2023/9/17
 * \brief 用Palette物件儲存可用的顏色
 *********************************************************************/
#pragma once
#include <vector>
#include <stdint.h>

namespace Color {
    //! 儲存顏色，每個channel一個byte
    struct RGB_t {
        uint8_t R;
        uint8_t G;
        uint8_t B;

        //! default constructor, set all chanel to zero
        constexpr RGB_t() : R(0), G(0), B(0) {}

        //! specify R, G, B
        constexpr RGB_t(uint8_t _R, uint8_t _G, uint8_t _B) : R(_R), G(_G), B(_B) {}

        //! construct from an byte array
        //! @param arr - 取第0, 1, 2項做R, G, B。長度必須>=3（不做檢查）
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

    //! 儲存可用的顏色
    class Palette_t : public std::vector<RGB_t> {
    public:
        //! default constructor
        Palette_t() {}

        /**
         * \brief 指定數量和初值
         * \param count - 數量
         * \param val - 初值（預設為 `RGB_t()`）
         */
        explicit Palette_t(size_t count, RGB_t val = RGB_t()) : std::vector<RGB_t>(count, val) {}

        /**
         * \brief 用initializer_list初始化
         * \param list - 以 {} 包住的一連串 Color::RGB_t
         */
        Palette_t(std::initializer_list<RGB_t> list) : std::vector<RGB_t>(list) {}

        /**
         * \brief 找到 Palette 中「歐氏距離」離 color 最近的顏色。若 Palette 為空則直接回傳 color
         * \param color - 某個顏色
         */
        RGB_t find_closest_to(RGB_t color) const;
    };

    /**
     * \brief 計算兩顏色的歐氏距離的平方
     * \param c1 - color1
     * \param c2 - color2
     */
    inline int euclidean_distance_square(RGB_t c1, RGB_t c2) {
        int deltaR = (int)c1.R - c2.R, deltaG = (int)c1.G - c2.G, deltaB = (int)c1.B - c2.B;
        return deltaR * deltaR + deltaG * deltaG + deltaB * deltaB;
    }

    /**
     * \brief 將color的RGB依序寫入arr的第0、1、2項
     * \param arr - 陣列。長度>=3（不做檢查）
     * \param color - 顏色
     */
    inline void memset(uint8_t* arr, RGB_t color) {
        arr[0] = color.R; arr[1] = color.G; arr[2] = color.B;
    }

    //! 白色
    constexpr RGB_t White(255, 255, 255);

    //! 黑色
    constexpr RGB_t Black;
}
