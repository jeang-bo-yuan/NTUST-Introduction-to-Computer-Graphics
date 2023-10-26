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

inline constexpr bool is_valid_SplineType(SplineType type) {
	switch (type) {
	case SplineType::Linear: case SplineType::Cardinal_Cubic: case SplineType::Cubic_B_Spline:
		return true;
	default:
		return false;
	}
}

/// 各Spline Type顯示的字串
constexpr const char* SplineTypeStr[] = { "Linear", "Cardinal Cubic", "Cubic B-Spline" };

/// Light Type
enum class LightType {
	Directional = 1,
	Point = 2,
	Spotlight = 3
};

/// 各Light Type顯示的字串
constexpr const char* LightTypeStr[] = { "Directional Light", "Point Light", "Spot Light"};

/// 畫軌道算內插時，t的變化量
constexpr float Param_Interval = 0.05f;

/// 火車是邊長為2倍train_size的立方體
constexpr float train_size = 5;
