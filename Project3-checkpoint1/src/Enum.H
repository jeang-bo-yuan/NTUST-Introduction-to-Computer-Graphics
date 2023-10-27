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
class SplineType {
	int val;

public:
	static const int MIN = 1; //!< Minimum value
	static const int Linear = 1; //!< set to this, if linear is choosen
	static const int Cardinal_Cubic = 2; //!< set to this, if cardinal cubic is choosen
	static const int Cubic_B_Spline = 3; //!< set to this, if cubic b-spline is choosen
	static const int MAX = 3; //!< Maximum value

	/**
	 * @brief constructor
	 * @param v - the value, it will be clamped into [MIN, MAX]
	 */
	SplineType(int v) : val(v < MIN ? MIN : v > MAX ? MAX : v) {}

	/**
	 * @brief get value
	 * @return the value
	 */
	int value() const { return val; }
};

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
