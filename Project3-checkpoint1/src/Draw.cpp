/*****************************************************************//**
 * \file Draw.cpp
 * \author 蔣博元
 * \date 2023/10/14
 * \brief 實作Draw.H中的function
 *********************************************************************/
#include "Draw.H"
#include "Utilities/3DUtils.h"
#include <iostream>
#include <GL/GL.h>
#include <math.h>

Draw::Param_Equation Draw::make_line(const Pnt3f p1, const Pnt3f p2) {
	return [p1, p2](float t) -> Pnt3f {
		return (1.f - t) * p1 + t * p2;
	};
}

namespace {
	/**
	 * @brief 設定兩control point間的參數式
	 * @param track - 軌道
	 * @param cp_id - 第一個control point的index
	 * @param type - 參數式的型式
	 * @param[out] point_eq - 儲存點的參數式
	 * @param[out] orient_eq - 儲存orient的參數式
	 */
	void set_equation(const CTrack& track,
		              const size_t cp_id,
		              const SplineType type,
		              Draw::Param_Equation& point_eq,
		              Draw::Param_Equation& orient_eq) {
		switch (type) {
		case SplineType::Linear: {
			const ControlPoint& control1 = track.points[cp_id];
			const ControlPoint& control2 = track.points[(cp_id + 1) % track.points.size()];

			point_eq = Draw::make_line(control1.pos, control2.pos);
			orient_eq = Draw::make_line(control1.orient, control2.orient);

			break;
		}
		case SplineType::Cardinal_Cubic:
			std::cout << "Not Implemented!" << std::endl;
			return;
			break;
		case SplineType::Cubic_B_Spline:
			std::cout << "Not Implemented!" << std::endl;
			return;
			break;
		}
	}

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
		Param_Equation point_eq, orient_eq;
		
		set_equation(track, i, type, point_eq, orient_eq);
		if (point_eq == nullptr || orient_eq == nullptr) return;

		draw_line_and_sleeper(point_eq, orient_eq, doingShadow);
	}
}

void Draw::draw_train(const CTrack& track, const SplineType type, const bool doingShadow) {
	size_t control_id = floorf(track.trainU);
	float t = track.trainU - (float)control_id;

	control_id %= track.points.size(); // 避免overflow
	size_t next_control = (control_id + 1) % track.points.size();

	Param_Equation point_eq, orient_eq;

	set_equation(track, control_id, type, point_eq, orient_eq);
	if (point_eq == nullptr || orient_eq == nullptr) return;

	// 火車的位置
	Pnt3f train_pos = point_eq(t);
	// 方向向量
	Pnt3f u = track.points[next_control].pos - track.points[control_id].pos;
	// orient
	Pnt3f orient = orient_eq(t);
	// 正上方，垂直軌道
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

	// 從ControlPoint::draw抄來的
	// 先做旋轉使正上方朝向UP，再平移到train_pos
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