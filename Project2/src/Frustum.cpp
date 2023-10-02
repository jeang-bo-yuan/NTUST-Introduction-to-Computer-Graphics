#include "Frustum.h"
#include "Maze.h"
#include <glm/vec2.hpp>

My::Frustum_2D::Frustum_2D(const float* viewer_posn, float view_dir, float view_fov, float zNear, float zFar)
	: right_bound(0, 0, 0, 0), left_bound(0, 0, 0, 0), near_bound(0, 0, 0, 0)
{
	float vx = viewer_posn[Maze::X];
	float vy = viewer_posn[Maze::Y];

	view_dir = Maze::To_Radians(view_dir);
	view_fov = Maze::To_Radians(view_fov);
}
