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

void Draw::set_equation(const CTrack& track,
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

	switch (type.value()) {
	case SplineType::Linear:
		point_eq = Draw::make_line(P1.pos, P2.pos);
		orient_eq = Draw::make_line(P1.orient, P2.orient);
		break;
	case SplineType::Cardinal_Cubic:
	case SplineType::Cubic_B_Spline: {
		const ControlPoint& P0 = track.points[track.prev_cp(cp_id)];
		const ControlPoint& P3 = track.points[track.next_cp(cp2_id)];

		if (type.value() == SplineType::Cardinal_Cubic) {
			point_eq = Draw::make_cardinal(P0.pos, P1.pos, P2.pos, P3.pos);
			orient_eq = Draw::make_cardinal(P0.orient, P1.orient, P2.orient, P3.orient);
		}
		else if (type.value() == SplineType::Cubic_B_Spline) {
			point_eq = Draw::make_cubic_b_spline(P0.pos, P1.pos, P2.pos, P3.pos);
			orient_eq = Draw::make_cubic_b_spline(P0.orient, P1.orient, P2.orient, P3.orient);
		}

		break;
	}
	}

	return;
}

namespace {
	// forward declaration
	void draw_block();

	/**
	 * @brief 畫軌道和枕木（sleeper）
	 * @param p1 - start point
	 * @param p2 - end point
	 * @param orient - the orient of the track
	 * @param doingShadow - 是否在畫陰影
	 */
	void draw_line_and_sleeper(Pnt3f p1, Pnt3f p2, Pnt3f orient, const bool doingShadow)
	{
		// 間隔為Param_Interval
		// points
		Pnt3f middle = (p1 + p2) * 0.5f;
		// 方向向量
		Pnt3f u = p2 - p1;
		float line_len = sqrtf(u.x * u.x + u.y * u.y + u.z * u.z); // 這一段的長度
		u.normalize();
		// u 和 orient 外積，得到軌道水平平移的方向
		Pnt3f horizontal = u * orient; horizontal.normalize();
		// 軌道的寬為 2 + 2
		Pnt3f line_space = horizontal * 2;

		if (!doingShadow) glColor3ub(0, 0, 0);
		// 畫軌道
		glBegin(GL_LINES);
			glVertex3fv((p1 + line_space).v());
			glVertex3fv((p2 + line_space).v());

			glVertex3fv((p1 - line_space).v());
			glVertex3fv((p2 - line_space).v());
		glEnd();

		// 畫sleeper
		Pnt3f DOWN = u * horizontal; DOWN.normalize();
		GLfloat rotate_mat[16] = {
			horizontal.x, horizontal.y, horizontal.z, 0,
			DOWN.x, DOWN.y, DOWN.z, 0,
			u.x, u.y, u.z, 0,
			0, 0, 0, 1
		};
		middle = middle + DOWN * 0.1f; // 住下一點點
		glPushMatrix();
			glTranslatef(middle.x, middle.y, middle.z);
			glMultMatrixf(rotate_mat);
			glScalef(3.f, 0.4f, line_len * 0.3f);
			if (!doingShadow) glColor3ub(255, 255, 255);
			glEnable(GL_NORMALIZE);
			draw_block();
			glDisable(GL_NORMALIZE);
		glPopMatrix();
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

			glVertexPointer(3, GL_FLOAT, 0, nullptr);
		glDisableClientState(GL_VERTEX_ARRAY);
	} // draw_block
} // unnamed namespace

void Draw::draw_track(const CTrack& track, const bool doingShadow) {
	std::vector<Draw::Param_Equation> point_eq_vec;
	std::vector<Draw::Param_Equation> orient_eq_vec;
	point_eq_vec.reserve(track.points.size());
	orient_eq_vec.reserve(track.points.size());

	// for each control point
	for (size_t i = 0; i < track.points.size(); ++i) {
		Param_Equation point_eq, orient_eq;
		set_equation(track, i, track.spline_type(), point_eq, orient_eq);
		point_eq_vec.push_back(point_eq);
		orient_eq_vec.push_back(orient_eq);
	}

	if (GLOBAL::Arc_Len_Mode) {
		bool wrap_back = false; // 是否繞回S=0了，則是用來確保軌道頭尾相連
		Pnt3f p1 = point_eq_vec[0](0);
		float S1 = 0.f; // real distance of p1
		for (float S2 = GLOBAL::Track_Interval; !wrap_back; S2 += GLOBAL::Track_Interval) {
			// 繞回S=0（S大於等於最大值）了
			if (S2 >= track.Arc_Len_Accum().back().second) {
				wrap_back = true;
				S2 = track.Arc_Len_Accum().back().second;
			}

			float T2 = track.S_to_T(S2);
			Pnt3f p2, orient; {
				size_t cp_id = static_cast<size_t>(floorf(T2));
				float t = T2 - cp_id;
				p2 = point_eq_vec[cp_id](t);
				orient = orient_eq_vec[cp_id](t - GLOBAL::Param_Interval / 2.f);
			}

			draw_line_and_sleeper(p1, p2, orient, doingShadow);

			// store for next iteration
			p1 = p2;
			S1 = S2;
		}
	}
	else {
		for (size_t cp_id = 0; cp_id < track.points.size(); ++cp_id) {
			Draw::Param_Equation point_eq = point_eq_vec[cp_id];
			Draw::Param_Equation orient_eq = orient_eq_vec[cp_id];

			Pnt3f p1 = point_eq(0);
			for (float t = GLOBAL::Param_Interval; t <= 1; t += GLOBAL::Param_Interval) {
				Pnt3f p2 = point_eq(t);
				Pnt3f orient = orient_eq(t - GLOBAL::Param_Interval / 2.f);
				draw_line_and_sleeper(p1, p2, orient, doingShadow);

				// store for next iteration
				p1 = p2;
			}
		}
	}


} // draw_track

void Draw::draw_train(const CTrack& track, const bool doingShadow) {
	glEnable(GL_NORMALIZE); // 因為用了scale，所以開啟GL_NORMALIZE來強迫normal vector變單位向量

	{ // 畫車頭
		Pnt3f FACE, LEFT, UP;
		Pnt3f train_pos = track.calc_pos(track.trainU, &FACE, &LEFT, &UP);

		glPushMatrix();
			// 先做旋轉，再平移到train_pos
			glTranslatef(train_pos.x, train_pos.y, train_pos.z);

			GLfloat rotate_mat[16] = { // 其實是coordinate轉換
				LEFT.x, LEFT.y, LEFT.z, 0, // 對應x軸
				UP.x, UP.y, UP.z, 0, // 對應y軸
				FACE.x, FACE.y, FACE.z, 0, // 對應z軸
				0, 0, 0, 1
			};
			glMultMatrixf(rotate_mat);

			glScalef(train_size, train_size, train_size);

			if (!doingShadow) glColor3ub(0, 0, 255);
			draw_block();
		glPopMatrix();

		glPushMatrix(); // 車頭前方的凸起
			Pnt3f pos2 = train_pos + FACE * train_size * 1.3f;
			glTranslatef(pos2.x, pos2.y, pos2.z);
			glMultMatrixf(rotate_mat);
			glScalef(train_size, train_size / 2.f, train_size * 0.3f);

			if (!doingShadow) glColor3ub(58, 164, 186);
			draw_block();
		glPopMatrix();
	}

	// 畫更多的車箱
	if (track.num_of_cars > 0) {
		std::vector<float> car_pos_list = track.list_points(track.trainU, -2.3f * train_size, track.num_of_cars);
		constexpr GLubyte color[4][3] = {
			{ 255, 0, 0 },
			{ 0, 255, 0 },
			{ 127, 127, 127 },
			{ 0, 0, 255 },
		};
		for (size_t i = 0; i < track.num_of_cars; ++i) {
			Pnt3f FACE, LEFT, UP;
			Pnt3f car_pos = track.calc_pos(car_pos_list[i], &FACE, &LEFT, &UP);

			glPushMatrix();
			glTranslatef(car_pos.x, car_pos.y, car_pos.z);

			GLfloat rotate_mat[16] = {
				LEFT.x, LEFT.y, LEFT.z, 0,
				UP.x, UP.y, UP.z, 0,
				FACE.x, FACE.y, FACE.z, 0,
				0, 0, 0, 1
			};
			glMultMatrixf(rotate_mat);

			glScalef(train_size, train_size, train_size);

			if (!doingShadow) glColor3ubv(color[i % 4]);
			draw_block();

			glPopMatrix();
		}
	}

	glDisable(GL_NORMALIZE);
} // draw_train