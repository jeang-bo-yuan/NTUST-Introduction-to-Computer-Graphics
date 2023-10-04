#pragma once

#include "LineSeg.h"
#include <vector>
#include <glm/vec2.hpp>

namespace My {
	class Frustum_2D {
		enum BOUND {
			RIGHT_ = 0,
			LEFT_,
		};
		//! 兩個邊界，其index可用BOUND::...表示
		LineSeg bounds[2];

	public:
		/**
		 * @brief default constructor，該frustum在clip時不會做任何事
		 */
		Frustum_2D() : bounds{ LineSeg(0,0,0,0), LineSeg(0,0,0,0) } {}

		/**
		* @brief constructor
		* @param viewer_posn - 觀察者的位置
		* @param view_dir - 視線方向的方位角（以度表示）
		* @param view_fov - 水平的視角，也就是視野最左邊和最右邊所夾的角度（以度表示）
		*/
		Frustum_2D(const float* viewer_posn, float view_dir, float view_fov);

		/**
		 * @brief 建立一個frustum使其左、右兩邊界分別通過S、E。
		 * @param viewer_posn - 觀察者的位置
		 * @param S
		 * @param E
		 */
		static Frustum_2D restrict(const float* viewer_posn, glm::vec2 S, glm::vec2 E);

		/**
		 * @brief clip line
		 * @param start - start point of line
		 * @param end - end point of line
		 * @return false, if whole line is outside frustum. true, otherwise
		 * @post 若回傳true，start和end的值會被更改，使其所成線段不會超出frustum的範圍。
		 */
		bool clip(glm::vec2& start, glm::vec2& end);
	};
}