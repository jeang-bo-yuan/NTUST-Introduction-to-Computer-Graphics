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

/// @brief 把计Αの酶籹瓂笵ó
namespace Draw {

	/// @brief Param_Equation 琌function objectノㄓボ絬把计Α
	/// @details 把计 - 把计Αt 肚 - 翴
	typedef std::function<Pnt3f(float)> Param_Equation;

	/// @brief ミ絬把计Α
	/// @param p1 - t=0 翴
	/// @param p2 - t=1 翴
	Param_Equation make_line(const Pnt3f p1, const Pnt3f p2);

	/// @brief ミCubic B-spline把计Α
	/// @param p0 - 北翴
	/// @param p1 - 秨﹍
	/// @param p2 - 挡
	/// @param p3 - 玡北翴
	Param_Equation make_cubic_b_spline(Pnt3f p0, Pnt3f p1, Pnt3f p2, Pnt3f p3);

	/// @brief ミCardinal Spline把计Α
	/// @param p0 - 北翴
	/// @param p1 - 秨﹍
	/// @param p2 - 挡
	/// @param p3 - 玡北翴
	Param_Equation make_cardinal(Pnt3f p0, Pnt3f p1, Pnt3f p2, Pnt3f p3);

	/**
	 * @brief 砞﹚ㄢcontrol point丁把计Α
	 * @param track - 瓂笵
	 * @param cp_id - 材control pointindex
	 * @param type - 把计ΑΑ
	 * @param[out] point_eq - 纗翴把计Α
	 * @param[out] orient_eq - 纗orient把计Α
	 * @post 璝type琌獶猭玥point_eq㎝orient_eq砞Θ`nullptr`
	 */
	void set_equation(const CTrack& track, const size_t cp_id, const SplineType type,
		Draw::Param_Equation& point_eq, Draw::Param_Equation& orient_eq);

	/// @brief 礶瓂笵
	/// @param track - 瓂笵
	/// @param type - い丁翴璶贺把计Α璸衡
	/// @param doingShadow - 琌礶潮紇
	void draw_track(const CTrack& track, const SplineType type, const bool doingShadow);

	/// @brief 礶ó
	/// @param track - ó┮瓂笵
	/// @param type - い丁翴璶贺把计Α璸衡
	/// @param doingShadow - 琌礶潮紇
	void draw_train(const CTrack& track, const SplineType type, const bool doingShadow);

} // namespace Draw
