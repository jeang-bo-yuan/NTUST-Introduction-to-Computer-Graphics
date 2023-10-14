#include "Draw.H"
#include <iostream>
#include <GL/GL.h>

Draw::Param_Equation Draw::make_line(const Pnt3f p1, const Pnt3f p2) {
	return [p1, p2](float t) -> Pnt3f {
		return (1.f - t) * p1 + t * p2;
	};
}

namespace {
	/**
	 * @brief 畫軌道和枕木（sleeper）
	 * @param point_eq - 點的參數式
	 * @param orient_eq - orient的參數式
	 * @param doingShadow - 是否在畫陰影
	 */
	void draw_line_and_sleeper(const Draw::Param_Equation& point_eq,
		                       const Draw::Param_Equation& orient_eq,
		                       const bool doingShadow) {
		// 間隔為Param_Interval
		for (float t = 0.0f; t < 1.0f; t += Param_Interval) {
			// points
			Pnt3f p1 = point_eq(t);
			Pnt3f p2 = point_eq(t + Param_Interval);
			// orient
			Pnt3f orient = orient_eq(t);
			// 方向向量
			Pnt3f u = p2 - p1;
			// u 和 orient 外積，得到軌道水平平移的方向
			Pnt3f horizontal = u * orient;
			horizontal.normalize();
			horizontal = horizontal * 2; // 軌道的寬為 2 + 2

			if (!doingShadow) glColor3ub(0, 0, 0);
			// 畫軌道
			glBegin(GL_LINES);
			glVertex3fv((p1 + horizontal).v());
			glVertex3fv((p2 + horizontal).v());

			glVertex3fv((p1 - horizontal).v());
			glVertex3fv((p2 - horizontal).v());
			glEnd();

			// 畫sleeper
			horizontal = horizontal * 1.5f; // sleeper的寬為3 + 3
			Pnt3f DOWN = u * horizontal;
			DOWN.normalize(); // sleeper 在下方 1 單位
			u = u * 0.2f; // sleeper 占軌道長的 0.6
			p1 = p1 + u + DOWN;
			p2 = p2 - u + DOWN;
			if (!doingShadow) glColor3ub(255, 255, 255);
			glBegin(GL_QUADS);
			glVertex3fv((p1 + horizontal).v());
			glVertex3fv((p2 + horizontal).v());
			glVertex3fv((p2 - horizontal).v());
			glVertex3fv((p1 - horizontal).v());
			glEnd();
		}
	}
}

void Draw::draw_track(const CTrack& track, const SplineType type, const bool doingShadow) {
	// for each control point
	for (size_t i = 0; i < track.points.size(); ++i) {
		const ControlPoint& control1 = track.points[i];
		const ControlPoint& control2 = track.points[(i + 1) % track.points.size()];
		Param_Equation point_eq, orient_eq;
		
		switch (type) {
		case SplineType::Linear:
			point_eq = Draw::make_line(control1.pos, control2.pos);
			orient_eq = Draw::make_line(control1.orient, control2.orient);
			break;
		case SplineType::Cardinal_Cubic:
			std::cout << "Not Implemented!" << std::endl;
			return;
			break;
		case SplineType::Cubic_B_Spline:
			std::cout << "Not Implemented!" << std::endl;
			return;
			break;
		}

		draw_line_and_sleeper(point_eq, orient_eq, doingShadow);
	}
}
