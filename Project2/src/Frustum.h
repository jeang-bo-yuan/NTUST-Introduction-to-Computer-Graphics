#pragma once

#include "LineSeg.h"
#include <vector>
#include <glm/vec2.hpp>

namespace My {
	class Frustum_2D {
		enum BOUND {
			RIGHT_ = 0,
			NEAR_,
			LEFT_,
			FAR_
		};
		//! 四個邊界，其index可用BOUND::...表示
		LineSeg bounds[4];

	public:
		/**
		* @brief constructor
		* @param viewer_posn - where is the viewer
		* @param view_dir - where the viewer look at, in degree
		* @param view_fov - horizontal field of view in degree
		* @param zNear
		* @param zFar
		*/
		Frustum_2D(const float* viewer_posn, float view_dir, float view_fov, float zNear, float zFar);

		/**
		 * @brief clip line
		 * @param start - start point of line
		 * @param end - end point of line
		 * @return false, if whole line is outside frustum. true, otherwise
		 */
		bool clip(glm::vec2& start, glm::vec2& end);
	};
}