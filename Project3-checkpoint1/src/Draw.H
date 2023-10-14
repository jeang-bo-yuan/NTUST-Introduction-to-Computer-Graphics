/*****************************************************************//**
 * \file Draw.H
 * \author 奖痴じ
 * \date 2023/10/14
 * \brief 礶狥﹁ㄧ计
 *********************************************************************/
#pragma once
#include <functional>
#include "Utilities/Pnt3f.H"
#include "Enum.H"
#include "Track.H"

namespace Draw {

	/// @brief Param_Equation 琌function objectノㄓボ絬把计Α
	/// @details 把计 - 把计Αt 肚 - 翴
	typedef std::function<Pnt3f(float)> Param_Equation;

	/// @brief ミ絬把计Α
	/// @param p1 - t=0 翴
	/// @param p2 - t=1 翴
	Param_Equation make_line(const Pnt3f p1, const Pnt3f p2);

	/// @brief 礶瓂笵
	/// @param track - 瓂笵
	/// @param type - い丁翴璶贺把计Α璸衡
	/// @param doingShadow - 琌礶潮紇
	void draw_track(const CTrack& track, const SplineType type, const bool doingShadow);

} // namespace Draw
