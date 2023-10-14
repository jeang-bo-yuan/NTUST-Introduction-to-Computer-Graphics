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
	 * @brief �e�y�D�M�E��]sleeper�^
	 * @param point_eq - �I���ѼƦ�
	 * @param orient_eq - orient���ѼƦ�
	 * @param doingShadow - �O�_�b�e���v
	 */
	void draw_line_and_sleeper(const Draw::Param_Equation& point_eq,
		                       const Draw::Param_Equation& orient_eq,
		                       const bool doingShadow) {
		// ���j��Param_Interval
		for (float t = 0.0f; t < 1.0f; t += Param_Interval) {
			// points
			Pnt3f p1 = point_eq(t);
			Pnt3f p2 = point_eq(t + Param_Interval);
			// orient
			Pnt3f orient = orient_eq(t);
			// ��V�V�q
			Pnt3f u = p2 - p1;
			// u �M orient �~�n�A�o��y�D������������V
			Pnt3f horizontal = u * orient;
			horizontal.normalize();
			horizontal = horizontal * 2; // �y�D���e�� 2 + 2

			if (!doingShadow) glColor3ub(0, 0, 0);
			// �e�y�D
			glBegin(GL_LINES);
			glVertex3fv((p1 + horizontal).v());
			glVertex3fv((p2 + horizontal).v());

			glVertex3fv((p1 - horizontal).v());
			glVertex3fv((p2 - horizontal).v());
			glEnd();

			// �esleeper
			horizontal = horizontal * 1.5f; // sleeper���e��3 + 3
			Pnt3f DOWN = u * horizontal;
			DOWN.normalize(); // sleeper �b�U�� 1 ���
			u = u * 0.2f; // sleeper �e�y�D���� 0.6
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
