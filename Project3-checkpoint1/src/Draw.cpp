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
	 * @brief 設定兩control point間的參數式
	 * @param track - 軌道
	 * @param cp_id - 第一個control point的index
	 * @param type - 參數式的型式
	 * @param[out] point_eq - 儲存「點的參數式」
	 * @param[out] orient_eq - 儲存「orient的參數式」
	 */
	void set_equation(const CTrack& track,
		              const size_t cp_id,
		              const SplineType type,
		              Draw::Param_Equation& point_eq,
		              Draw::Param_Equation& orient_eq)
	{
		// 開始
		const ControlPoint& P1 = track.points[cp_id];

		// 結束
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
	 * @brief 畫軌道和枕木（sleeper）
	 * @param point_eq - 點的參數式
	 * @param orient_eq - orient的參數式
	 * @param doingShadow - 是否在畫陰影
	 */
	void draw_line_and_sleeper(const Draw::Param_Equation& point_eq,
		                       const Draw::Param_Equation& orient_eq,
		                       const bool doingShadow)
	{
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
			DOWN.normalize();
			glNormal3f(-DOWN.x, -DOWN.y, -DOWN.z);
			DOWN = DOWN * 0.3f; // sleeper 在下方 0.3 單位
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

		return;
	}

	/// 畫一個2*2*2的正方體，(-1,0,-1) ~ (1, 2, 1)
	void draw_block() {
		constexpr GLfloat vertex_arr[] = {
			1, 0, 1,
			1, 0, -1,
			-1, 0, -1,
			-1, 0, 1,
			1, 1 * 2, 1,
			1, 1 * 2, -1,
			-1, 1 * 2, -1,
			-1, 1 * 2, 1,
		};
		constexpr GLuint indices[] = {
			0, 1, 2, 3, // normal = (0, -1, 0)
			0, 1, 5, 4, // normal = (1, 0, 0)
			0, 3, 7, 4, // normal = (0, 0, 1)
			4, 5, 6, 7, // normal = (0, 1, 0)
			1, 5, 6, 2, // normal = (0, 0, -1)
			3, 2, 6, 7  // normal = (-1, 0, 0)
		};

		glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, vertex_arr);

			glNormal3f(0, -1, 0);
			glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, indices);

			glNormal3f(1, 0, 0);
			glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, indices + 4);

			glNormal3f(0, 0, 1);
			glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, indices + 8);

			glNormal3f(0, 1, 0);
			glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, indices + 12);

			glNormal3f(0, 0, -1);
			glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, indices + 16);

			glNormal3f(-1, 0, 0);
			glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, indices + 20);
		glDisableClientState(GL_VERTEX_ARRAY);
	} // draw_block
}

void Draw::draw_track(const CTrack& track, const SplineType type, const bool doingShadow) {
	// for each control point
	for (size_t i = 0; i < track.points.size(); ++i) {
		Param_Equation point_eq, orient_eq;
		
		set_equation(track, i, type, point_eq, orient_eq);
		if (point_eq == nullptr || orient_eq == nullptr) return;

		draw_line_and_sleeper(point_eq, orient_eq, doingShadow);
	}
} // draw_track

void Draw::draw_train(const CTrack& track, const SplineType type, const bool doingShadow) {
	size_t control_id = floorf(track.trainU);
	float t = track.trainU - (float)control_id;

	control_id %= track.points.size(); // 避免overflow

	Param_Equation point_eq, orient_eq;

	set_equation(track, control_id, type, point_eq, orient_eq);
	if (point_eq == nullptr || orient_eq == nullptr) return;

	// 火車的位置
	Pnt3f train_pos = point_eq(t);
	// 方向向量
	Pnt3f u = point_eq(t + 0.01f) - point_eq(t); u.normalize();
	// orient
	Pnt3f orient = orient_eq(t);
	// 水平方向
	Pnt3f horizontal = orient * u; horizontal.normalize();
	// 正上方，垂直軌道
	Pnt3f UP = u * horizontal; UP.normalize();

	glEnable(GL_NORMALIZE); // 因為用了scale，所以開啟GL_NORMALIZE來強迫normal vector變單位向量

	glPushMatrix();
		// 先做旋轉，再平移到train_pos
		glTranslatef(train_pos.x, train_pos.y, train_pos.z);

		GLfloat rotate_mat[16] = { // 其實是coordinate轉換
			horizontal.x, horizontal.y, horizontal.z, 0, // 對應x軸
			UP.x, UP.y, UP.z, 0, // 對應y軸
			u.x, u.y, u.z, 0, // 對應z軸
			0, 0, 0, 1
		};
		glMultMatrixf(rotate_mat);

		glScalef(train_size, train_size, train_size);

		if (!doingShadow) glColor3ub(0, 0, 255);
		draw_block();
	glPopMatrix();

	glPushMatrix(); // 畫車頭
		Pnt3f pos2 = train_pos + u * train_size * 1.3f;
		glTranslatef(pos2.x, pos2.y, pos2.z);
		glMultMatrixf(rotate_mat);
		glScalef(train_size, train_size / 2.f, train_size * 0.3f);

		if (!doingShadow) glColor3ub(58, 164, 186);
		draw_block();
	glPopMatrix();

	glDisable(GL_NORMALIZE);
} // draw_train