/*****************************************************************//**
 * \file Draw.H
 * \author ���դ�
 * \date 2023/10/14
 * \brief �e�F�誺���
 *********************************************************************/
#pragma once
#include <functional>
#include "Utilities/Pnt3f.H"
#include "Enum.H"
#include "Track.H"

namespace Draw {

	/// @brief Param_Equation �O�@��function object�A�ΨӪ��ܽu���ѼƦ�
	/// @details �Ѽ� - �ѼƦ���t�A �^�� - �I
	typedef std::function<Pnt3f(float)> Param_Equation;

	/// @brief �إߤ@�Ӫ��u���ѼƦ�
	/// @param p1 - t=0 �ɪ��I
	/// @param p2 - t=1 �ɪ��I
	Param_Equation make_line(const Pnt3f p1, const Pnt3f p2);

	/// @brief �e�X�y�D
	/// @param track - �y�D
	/// @param type - �������I�n�H���ذѼƦ��p��
	/// @param doingShadow - �O�_�b�e���v
	void draw_track(const CTrack& track, const SplineType type, const bool doingShadow);

	/// @brief �e�X����
	/// @param track - �����Ҧb���y�D
	/// @param type - �������I�n�H���ذѼƦ��p��
	/// @param doingShadow - �O�_�b�e���v
	void draw_train(const CTrack& track, const SplineType type, const bool doingShadow);

} // namespace Draw