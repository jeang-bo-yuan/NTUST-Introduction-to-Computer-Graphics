/*****************************************************************//**
 * \file Enum.H
 * \author ���դ�
 * \date 2023/10/14
 * \brief �@�Ǳ`��
 *********************************************************************/
#pragma once

/**
 * @brief �N���襤��Spline Type
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

/// �USpline Type��ܪ��r��
constexpr const char* SplineTypeStr[] = { "Linear", "Cardinal Cubic", "Cubic B-Spline" };

/// Light Type
enum class LightType {
	Directional = 1,
	Point = 2,
	Spotlight = 3
};

/// �ULight Type��ܪ��r��
constexpr const char* LightTypeStr[] = { "Directional Light", "Point Light", "Spot Light"};

/// Contains global variables. They are defined in Enum.cpp
namespace GLOBAL {
	/// �e�y�D�⤺���ɡAt���ܤƶq�A1/16
	constexpr float Param_Interval = 0.0625f;

	/// Arc_Len_Mode�U�A�y�D�����Z��
	constexpr float Track_Interval = 4.f;

	/// if arc length mode is enabled
	extern bool Arc_Len_Mode;
}

/// �����O�����2��train_size���ߤ���
constexpr float train_size = 5;