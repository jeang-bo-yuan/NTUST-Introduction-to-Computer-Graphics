#pragma once

#include "LineSeg.h"
#include <vector>
#include <glm/vec2.hpp>

namespace My {
	class Frustum_2D {
		enum BOUND {
			RIGHT_ = 0,
			LEFT_,
			FAR_
		};
		//! 四個邊界，其index可用BOUND::...表示
		LineSeg bounds[3];

	public:
		/**
		* @brief constructor
		* @param viewer_posn - 觀察者的位置
		* @param view_dir - 視線方向的方位角（以度表示）
		* @param view_fov - 水平的視角，也就是視野最左邊和最右邊所夾的角度（以度表示）
		* @param zFar - 最遠視野距離
		* @pre 0 < zFar
		*/
		Frustum_2D(const float* viewer_posn, float view_dir, float view_fov, float zFar);

		/**
		 * @brief 建立一個frustum使其左、右兩邊界分別通過S、E。
		 * 換句話說，回傳的frustum其邊界被限制在線段SE的範圍。
		 * @param viewer_posn - 觀察者的位置
		 * @param S
		 * @param E
		 * @param zFar - 最遠視野距離
		 * @post 若fov算出來為NaN，則丟出std::runtime_error
		 */
		static Frustum_2D restrict(const float* viewer_posn, glm::vec2 S, glm::vec2 E, float zFar);

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