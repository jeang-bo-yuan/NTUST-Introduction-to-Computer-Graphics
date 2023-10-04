#pragma once

#include "LineSeg.h"
#include <vector>
#include <glm/vec2.hpp>

namespace My {
	class Frustum_2D {
		enum BOUND {
			RIGHT_ = 0,
			LEFT_,
		};
		//! �����ɡA��index�i��BOUND::...���
		LineSeg bounds[2];

	public:
		/**
		 * @brief default constructor�A��frustum�bclip�ɤ��|�������
		 */
		Frustum_2D() : bounds{ LineSeg(0,0,0,0), LineSeg(0,0,0,0) } {}

		/**
		* @brief constructor
		* @param viewer_posn - �[��̪���m
		* @param view_dir - ���u��V����쨤�]�H�ת�ܡ^
		* @param view_fov - �����������A�]�N�O�����̥���M�̥k��ҧ������ס]�H�ת�ܡ^
		*/
		Frustum_2D(const float* viewer_posn, float view_dir, float view_fov);

		/**
		 * @brief �إߤ@��frustum�Ϩ䥪�B�k����ɤ��O�q�LS�BE�C
		 * @param viewer_posn - �[��̪���m
		 * @param S
		 * @param E
		 */
		static Frustum_2D restrict(const float* viewer_posn, glm::vec2 S, glm::vec2 E);

		/**
		 * @brief clip line
		 * @param start - start point of line
		 * @param end - end point of line
		 * @return false, if whole line is outside frustum. true, otherwise
		 * @post �Y�^��true�Astart�Mend���ȷ|�Q���A�Ϩ�Ҧ��u�q���|�W�Xfrustum���d��C
		 */
		bool clip(glm::vec2& start, glm::vec2& end);
	};
}