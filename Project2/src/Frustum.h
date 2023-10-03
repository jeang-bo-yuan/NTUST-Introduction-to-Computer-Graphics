#pragma once

#include "LineSeg.h"
#include <vector>
#include <glm/vec2.hpp>

namespace My {
	class Frustum_2D {
		enum BOUND {
			RIGHT_ = 0,
			LEFT_,
			FAR_
		};
		//! �|����ɡA��index�i��BOUND::...���
		LineSeg bounds[3];

	public:
		/**
		* @brief constructor
		* @param viewer_posn - �[��̪���m
		* @param view_dir - ���u��V����쨤�]�H�ת�ܡ^
		* @param view_fov - �����������A�]�N�O�����̥���M�̥k��ҧ������ס]�H�ת�ܡ^
		* @param zFar - �̻������Z��
		* @pre 0 < zFar
		*/
		Frustum_2D(const float* viewer_posn, float view_dir, float view_fov, float zFar);

		/**
		 * @brief �إߤ@��frustum�Ϩ䥪�B�k����ɤ��O�q�LS�BE�C
		 * ���y�ܻ��A�^�Ǫ�frustum����ɳQ����b�u�qSE���d��C
		 * @param viewer_posn - �[��̪���m
		 * @param S
		 * @param E
		 * @param zFar - �̻������Z��
		 * @post �Yfov��X�Ӭ�NaN�A�h��Xstd::runtime_error
		 */
		static Frustum_2D restrict(const float* viewer_posn, glm::vec2 S, glm::vec2 E, float zFar);

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