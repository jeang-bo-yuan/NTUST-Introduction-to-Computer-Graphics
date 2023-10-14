/*****************************************************************//**
 * \file Enum.H
 * \author 蔣博元
 * \date 2023/10/14
 * \brief 一些常數
 *********************************************************************/
#pragma once

/**
 * @brief 代表選中的Spline Type
 */
enum class SplineType {
	Linear = 1,
	Cardinal_Cubic = 2,
	Cubic_B_Spline = 3
};

/// 各Spline Type顯示的字串
constexpr const char* SplineTypeStr[] = { "Linear", "Cardinal Cubic", "Cubic B-Spline" };
