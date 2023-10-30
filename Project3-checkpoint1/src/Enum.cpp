/*****************************************************************//**
 * \file Enum.cpp
 * \author 蔣博元
 * \date 2023/10/30
 * \brief 定義Enum.H中的全域變數和函數
 *********************************************************************/
#include "Enum.H"
#include <cmath>
#include <assert.h>

bool GLOBAL::Arc_Len_Mode = true;

std::vector<std::pair<float, float>> GLOBAL::Arc_Len_Accum;

float GLOBAL::T_to_S(float T)
{
	for (size_t i = 0; i < GLOBAL::Arc_Len_Accum.size() - 1; ++i) {
		const float lowU = GLOBAL::Arc_Len_Accum[i].first;
		const float highU = GLOBAL::Arc_Len_Accum[i + 1].first;
		if (lowU <= T && T < highU) {
			const float lowS = GLOBAL::Arc_Len_Accum[i].second;
			const float highS = GLOBAL::Arc_Len_Accum[i + 1].second;
			return lowS + (T - lowU) / GLOBAL::Param_Interval * (highS - lowS);
		}
	}

	assert(false); // should not go here
	return NAN;
}

float GLOBAL::S_to_T(float S)
{
	/// @note 處理極端特例，當S很接近0或最大長度時，回傳0
	if (fabs(S) < 1.e-4 || fabs(S - GLOBAL::Arc_Len_Accum.back().second) < 1.e-4) return 0;

	// （實際空間） 轉回 （參數空間）
	for (size_t i = 0; i < GLOBAL::Arc_Len_Accum.size() - 1; ++i) {
		const float lowS = GLOBAL::Arc_Len_Accum[i].second;
		const float highS = GLOBAL::Arc_Len_Accum[i + 1].second;
		if ((lowS <= S && S < highS)) {
			const float lowU = GLOBAL::Arc_Len_Accum[i].first;
			const float highU = GLOBAL::Arc_Len_Accum[i + 1].first;
			return lowU + (S - lowS) / (highS - lowS) * GLOBAL::Param_Interval;
		}
	}

	assert(false); // should not go here
	return NAN;
}
