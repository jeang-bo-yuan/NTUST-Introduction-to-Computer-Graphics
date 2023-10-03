#include "Frustum.h"
#include "Frustum.h"
#include "Maze.h"
#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>
#include <cmath>

#define NULL_SEG LineSeg(0, 0, 0, 0)

My::Frustum_2D::Frustum_2D(const float* viewer_posn, float view_dir, float view_fov, float zNear, float zFar)
	: bounds { NULL_SEG, NULL_SEG, NULL_SEG, NULL_SEG }
{
	view_dir = Maze::To_Radians(view_dir);
	// 先除以2，方便後面計算
	view_fov = Maze::To_Radians(view_fov) / 2.0;

	const glm::vec2 viewer(viewer_posn[Maze::X], viewer_posn[Maze::Y]);
	const glm::vec2 right_unit(cosf(view_dir - view_fov), sinf(view_dir - view_fov));
	const glm::vec2 left_unit(cosf(view_dir + view_fov), sinf(view_dir + view_fov));

	const float near_len = zNear / cosf(view_fov);
	const float far_len = zFar / cosf(view_fov);

	const glm::vec2 far_right = viewer + far_len * right_unit;
	const glm::vec2 near_right = viewer + near_len * right_unit;
	const glm::vec2 near_left = viewer + near_len * left_unit;
	const glm::vec2 far_left = viewer + far_len * left_unit;

	bounds[RIGHT_] = LineSeg(far_right.x, far_right.y, near_right.x, near_right.y);
	bounds[NEAR_] = LineSeg(near_right.x, near_right.y, near_left.x, near_left.y);
	bounds[LEFT_] = LineSeg(near_left.x, near_left.y, far_left.x, far_left.y);
	bounds[FAR_] = LineSeg(far_left.x, far_left.y, far_right.x, far_right.y);
}

My::Frustum_2D My::Frustum_2D::restrict(const float* viewer_posn, glm::vec2 S, glm::vec2 E, float zNear, float zFar) {
	glm::vec2 V = glm::make_vec2(viewer_posn);

	// calculate fov
	float a = glm::length(S - V), b = glm::length(E - V), c = glm::length(E - S);
	float fov = Maze::To_Degrees(acos((a * a + b * b - c * c) / (2 * a * b)));

	// calculate dir
	glm::vec2 X = (a * E + b * S) / (a + b);
	glm::vec2 VX = X - V;
	float dir = Maze::To_Degrees(atan2f(VX.y, VX.x));

	printf("\tdir = %f, fov = %f\n", dir, fov);
	return Frustum_2D(viewer_posn, dir, fov, zNear, zFar);
}

bool My::Frustum_2D::clip(glm::vec2& start, glm::vec2& end) {
	// for each boundary
	for (LineSeg& boundary : bounds) {
		if (boundary.is_on_or_right(start.x, start.y)) {
			if (boundary.is_on_or_right(end.x, end.y)) {
				// 兩點都在內
				continue;
			}
			else {
				// start在內，end在外 -> 更新end到線段和boundary的交點
				end = boundary.intersection(LineSeg(start.x, start.y, end.x, end.y));
			}
		}
		else if (boundary.is_on_or_right(end.x, end.y)) {
			// end在內，start在外
			start = boundary.intersection(LineSeg(start.x, start.y, end.x, end.y));
		}
		else {
			// 兩點都在外
			return false;
		}
	}
}
