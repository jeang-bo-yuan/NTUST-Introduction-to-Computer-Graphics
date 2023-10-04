#include "Frustum.h"
#include "Frustum.h"
#include "Maze.h"
#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>
#include <cmath>
#include <stdexcept>

#define NULL_SEG LineSeg(0, 0, 0, 0)

My::Frustum_2D::Frustum_2D(const float* viewer_posn, float view_dir, float view_fov)
	: bounds { NULL_SEG, NULL_SEG }
{
	view_dir = Maze::To_Radians(view_dir);
	// �����H2�A��K�᭱�p��
	view_fov = Maze::To_Radians(view_fov) / 2.0;

	const glm::vec2 viewer(viewer_posn[Maze::X], viewer_posn[Maze::Y]);
	const glm::vec2 right_unit(cosf(view_dir - view_fov), sinf(view_dir - view_fov));
	const glm::vec2 left_unit(cosf(view_dir + view_fov), sinf(view_dir + view_fov));

	const glm::vec2 right = viewer + right_unit;
	const glm::vec2 left = viewer + left_unit;

	bounds[RIGHT_] = LineSeg(right.x, right.y, viewer.x, viewer.y);
	bounds[LEFT_] = LineSeg(viewer.x, viewer.y, left.x, left.y);
}

My::Frustum_2D My::Frustum_2D::restrict(const float* viewer_posn, glm::vec2 S, glm::vec2 E) {
	My::Frustum_2D result;

	LineSeg VS(viewer_posn[0], viewer_posn[1], S.x, S.y);

	if (VS.is_on_or_right(E.x, E.y)) {
		result.bounds[LEFT_] = VS;
		result.bounds[RIGHT_] = LineSeg(E.x, E.y, viewer_posn[0], viewer_posn[1]);
	}
	else {
		result.bounds[LEFT_] = LineSeg(viewer_posn[0], viewer_posn[1], E.x, E.y);
		result.bounds[RIGHT_] = LineSeg(S.x, S.y, viewer_posn[0], viewer_posn[1]);
	}

	return result;
}

bool My::Frustum_2D::clip(glm::vec2& start, glm::vec2& end) {
	// for each boundary
	for (LineSeg& boundary : bounds) {
		if (boundary.is_on_or_right(start.x, start.y)) {
			if (boundary.is_on_or_right(end.x, end.y)) {
				// ���I���b��
				continue;
			}
			else {
				// start�b���Aend�b�~ -> ��send��u�q�Mboundary�����I
				end = boundary.intersection(LineSeg(start.x, start.y, end.x, end.y));
			}
		}
		else if (boundary.is_on_or_right(end.x, end.y)) {
			// end�b���Astart�b�~
			start = boundary.intersection(LineSeg(start.x, start.y, end.x, end.y));
		}
		else {
			// ���I���b�~
			return false;
		}
	}
}
