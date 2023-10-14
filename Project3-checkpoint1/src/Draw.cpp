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
	 * @brief 礶瓂笵㎝狤れsleeper
	 * @param point_eq - 翴把计Α
	 * @param orient_eq - orient把计Α
	 * @param doingShadow - 琌礶潮紇
	 */
	void draw_line_and_sleeper(const Draw::Param_Equation& point_eq,
		                       const Draw::Param_Equation& orient_eq,
		                       const bool doingShadow) {
		// 丁筳Param_Interval
		for (float t = 0.0f; t < 1.0f; t += Param_Interval) {
			// points
			Pnt3f p1 = point_eq(t);
			Pnt3f p2 = point_eq(t + Param_Interval);
			// orient
			Pnt3f orient = orient_eq(t);
			// よ秖
			Pnt3f u = p2 - p1;
			// u ㎝ orient 縩眔瓂笵キキ簿よ
			Pnt3f horizontal = u * orient;
			horizontal.normalize();
			horizontal = horizontal * 2; // 瓂笵糴 2 + 2

			if (!doingShadow) glColor3ub(0, 0, 0);
			// 礶瓂笵
			glBegin(GL_LINES);
			glVertex3fv((p1 + horizontal).v());
			glVertex3fv((p2 + horizontal).v());

			glVertex3fv((p1 - horizontal).v());
			glVertex3fv((p2 - horizontal).v());
			glEnd();

			// 礶sleeper
			horizontal = horizontal * 1.5f; // sleeper糴3 + 3
			Pnt3f DOWN = u * horizontal;
			DOWN.normalize(); // sleeper よ 1 虫
			u = u * 0.2f; // sleeper 瓂笵 0.6
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
