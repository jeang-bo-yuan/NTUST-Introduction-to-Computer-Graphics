#pragma once

#include "LineSeg.h"

namespace My {
	class Frustum_2D {
		LineSeg right_bound;
		LineSeg left_bound;
		LineSeg near_bound;

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
	};
}