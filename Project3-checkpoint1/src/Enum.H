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
enum class SplineType {
	Linear = 1,
	Cardinal_Cubic = 2,
	Cubic_B_Spline = 3
};

/// �USpline Type��ܪ��r��
constexpr const char* SplineTypeStr[] = { "Linear", "Cardinal Cubic", "Cubic B-Spline" };

/// �e�y�D�⤺���ɡAt���ܤƶq
constexpr float Param_Interval = 0.05f;

/// �����O�����2��train_size���ߤ���
constexpr float train_size = 5;