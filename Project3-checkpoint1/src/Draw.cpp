/*****************************************************************//**
 * \file Draw.cpp
 * \author ���դ�
 * \date 2023/10/14
 * \brief ��@Draw.H����function
 *********************************************************************/
#include "Draw.H"
#include "Utilities/3DUtils.h"
#include <iostream>
#include <GL/GL.h>
#include <math.h>
#include <glm/gtc/type_ptr.hpp>

Draw::Param_Equation Draw::make_line(const Pnt3f p1, const Pnt3f p2) {
	return [p1, p2](float t) -> Pnt3f {
		return (1.f - t) * p1 + t * p2;
	};
}

Draw::Param_Equation Draw::make_cubic_b_spline(Pnt3f p0, Pnt3f p1, Pnt3f p2, Pnt3f p3) {
	static const glm::mat4 M (
		-1 / 6.f,  3 / 6.f, -3 / 6.f, 1 / 6.f, // first column
		 3 / 6.f, -6 / 6.f,  3 / 6.f,    0,    // second column
		-3 / 6.f,    0    ,  3 / 6.f,    0,    // third column
		 1 / 6.f,  4 / 6.f,  1 / 6.f,    0     // forth column
	);

	// 4 columns and 3 rows
	glm::mat4x3 G(
		glm::make_vec3(p0.v()), // first column
		glm::make_vec3(p1.v()), // second column
		glm::make_vec3(p2.v()), // third column
		glm::make_vec3(p3.v())  // forth column
	);

	return [G](float t) -> Pnt3f {
		glm::vec4 T(t * t * t, t * t, t, 1);
		glm::vec3 Q = G * M * T;
		return Pnt3f(glm::value_ptr(Q));
	};
}

Draw::Param_Equation Draw::make_cardinal(Pnt3f p0, Pnt3f p1, Pnt3f p2, Pnt3f p3) {
	static const glm::mat4 M(
		-1 / 2.f,  3 / 2.f, -3 / 2.f,  1 / 2.f, // first column
		 2 / 2.f, -5 / 2.f,  4 / 2.f, -1 / 2.f,    // second column
		-1 / 2.f,    0,      1 / 2.f,    0,    // third column
		   0,      2 / 2.f,    0,        0     // forth column
	);

	// 4 columns and 3 rows
	glm::mat4x3 G(
		glm::make_vec3(p0.v()), // first column
		glm::make_vec3(p1.v()), // second column
		glm::make_vec3(p2.v()), // third column
		glm::make_vec3(p3.v())  // forth column
	);

	return [G](float t) -> Pnt3f {
		glm::vec4 T(t * t * t, t * t, t, 1);
		glm::vec3 Q = G * M * T;
		return Pnt3f(glm::value_ptr(Q));
	};
}

namespace {
	/**
	 * @brief �]�w��control point�����ѼƦ�
	 * @param track - �y�D
	 * @param cp_id - �Ĥ@��control point��index
	 * @param type - �ѼƦ�������
	 * @param[out] point_eq - �x�s�u�I���ѼƦ��v
	 * @param[out] orient_eq - �x�s�uorient���ѼƦ��v
	 */
	void set_equation(const CTrack& track,
		              const size_t cp_id,
		              const SplineType type,
		              Draw::Param_Equation& point_eq,
		              Draw::Param_Equation& orient_eq)
	{
		// �}�l
		const ControlPoint& P1 = track.points[cp_id];

		// ����
		size_t cp2_id = track.next_cp(cp_id);
		const ControlPoint& P2 = track.points[cp2_id];

		switch (type) {
		case SplineType::Linear:
			point_eq = Draw::make_line(P1.pos, P2.pos);
			orient_eq = Draw::make_line(P1.orient, P2.orient);
			break;
		case SplineType::Cardinal_Cubic:
		case SplineType::Cubic_B_Spline:
			const ControlPoint& P0 = track.points[track.prev_cp(cp_id)];
			const ControlPoint& P3 = track.points[track.next_cp(cp2_id)];

			if (type == SplineType::Cardinal_Cubic) {
				point_eq = Draw::make_cardinal(P0.pos, P1.pos, P2.pos, P3.pos);
				orient_eq = Draw::make_cardinal(P0.orient, P1.orient, P2.orient, P3.orient);
			}
			else if (type == SplineType::Cubic_B_Spline) {
				point_eq = Draw::make_cubic_b_spline(P0.pos, P1.pos, P2.pos, P3.pos);
				orient_eq = Draw::make_cubic_b_spline(P0.orient, P1.orient, P2.orient, P3.orient);
			}

			break;
		}

		return;
	}

	/**
	 * @brief �e�y�D�M�E��]sleeper�^
	 * @param point_eq - �I���ѼƦ�
	 * @param orient_eq - orient���ѼƦ�
	 * @param doingShadow - �O�_�b�e���v
	 */
	void draw_line_and_sleeper(const Draw::Param_Equation& point_eq,
		                       const Draw::Param_Equation& orient_eq,
		                       const bool doingShadow)
	{
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
			DOWN.normalize();
			DOWN = DOWN * 0.3f; // sleeper �b�U�� 0.3 ���
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

		return;
	}
}

void Draw::draw_track(const CTrack& track, const SplineType type, const bool doingShadow) {
	// for each control point
	for (size_t i = 0; i < track.points.size(); ++i) {
		Param_Equation point_eq, orient_eq;
		
		set_equation(track, i, type, point_eq, orient_eq);
		if (point_eq == nullptr || orient_eq == nullptr) return;

		draw_line_and_sleeper(point_eq, orient_eq, doingShadow);
	}
}

void Draw::draw_train(const CTrack& track, const SplineType type, const bool doingShadow) {
	size_t control_id = floorf(track.trainU);
	float t = track.trainU - (float)control_id;

	control_id %= track.points.size(); // �קKoverflow

	Param_Equation point_eq, orient_eq;

	set_equation(track, control_id, type, point_eq, orient_eq);
	if (point_eq == nullptr || orient_eq == nullptr) return;

	// ��������m
	Pnt3f train_pos = point_eq(t);
	// ��V�V�q
	Pnt3f u = point_eq(t + 0.01f) - point_eq(t);
	// orient
	Pnt3f orient = orient_eq(t);
	// ���W��A�����y�D
	Pnt3f UP = (u * orient) * u;
	UP.normalize();

	constexpr GLfloat vertex_arr[] = {
		train_size, 0, train_size,
		train_size, 0, -train_size,
		-train_size, 0, -train_size,
		-train_size, 0, train_size,
		train_size, train_size * 2, train_size,
		train_size, train_size * 2, -train_size,
		-train_size, train_size * 2, -train_size,
		-train_size, train_size * 2, train_size,
	};
	constexpr GLuint indices[] = {
		0, 1, 2, 3,
		0, 1, 5, 4,
		0, 3, 7, 4,
		4, 5, 6, 7,
		1, 5, 6, 2,
		3, 2, 6, 7
	};

	if (!doingShadow) glColor3ub(0, 255, 255);

	// �qControlPoint::draw�ۨӪ�
	// ��������ϥ��W��¦VUP�A�A������train_pos
	glPushMatrix();
	glTranslatef(train_pos.x, train_pos.y, train_pos.z);
	float theta1 = -radiansToDegrees(atan2(UP.z, UP.x));
	glRotatef(theta1, 0, 1, 0);
	float theta2 = -radiansToDegrees(acos(UP.y));
	glRotatef(theta2, 0, 0, 1);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vertex_arr);
		glDrawElements(GL_QUADS, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, indices);
		glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
}