/************************************************************************
     File:        Track.H

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     Container for the "World"

						This provides a container for all of the "stuff" 
						in the world.

						It could have been all global variables, or it could 
						have just been
						contained in the window. The advantage of doing it 
						this way is that
						we might have multiple windows looking at the same 
						world. But, I don't	think we'll actually do that.

						See the readme for commentary on code style

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/
#pragma once

#include <vector>

using std::vector; // avoid having to say std::vector all of the time

// make use of other data structures from this project
#include "ControlPoint.H"
#include "Enum.H"

/**
 * @brief 儲存軌道的控制點，以及火車的位置
 */
class CTrack {
	public:		
		// Constructor
		CTrack();

	public:
		// when we want to clear the control points, we really "reset" them 
		// to have 4 default positions (since we should never have fewer
		// than 4 points)
		void resetPoints();


		// read and write to files
		void readPoints(const char* filename);
		void writePoints(const char* filename);

		/// @brief 取得下一個控制點的index
		/// @param index - 目前的控制點的index
		/// @return 若index是points中的最後一個，回傳`0`；否則，回傳`index + 1`。
		inline size_t next_cp(size_t index) const {
			return (index >= points.size() - 1 ? 0 : index + 1);
		}

		/// @brief 取得前一個控制點的index
		/// @param index - 目前的控制點的index
		/// @return 若index是points中的第一個，回傳`points.size() - 1`；否則，回傳`index - 1`。
		inline size_t prev_cp(size_t index) const {
			return (index == 0 ? points.size() - 1 : index - 1);
		}

		/** @brief 計算參數值為U時，所對應的點的位置
		    @param U - 和 trainU 一樣，代表在第`floor(U)`和下一個控制點間的點
		    @param type - 使用的參數曲線
		    @param[out] FACE - 若不為`nullptr`，則dereference並設成該點面向的方向
		    @param[out] LEFT - 若不為`nullptr`，則dereference並設成該點左邊的方向
		    @param[out] UP - 若不為`nullptr`，則dereference並設成該點向上的方向
		    @return 點的位置
		    @pre 0 <= U < `this->points.size()`
			@post
			1. 若U或type是非法的，則回傳`this->points[0].pos`
			2. FACE, RIGHT, UP 都會設成單位向量
		  */
		Pnt3f calc_pos(float U, SplineType type, Pnt3f* FACE = nullptr, Pnt3f* LEFT = nullptr, Pnt3f* UP = nullptr) const;

		/**
		 * @brief 計算某兩個control point間的曲線長
		 * @param cp_id - 第一個control point的index
		 * @param type - 使用的參數曲線
		 * @return 曲線長
		 * @post 若type是非法的，回傳0
		 */
		float arc_length(size_t cp_id, SplineType type) const;

	public:
		/// @brief 所有控制點
		/// @details
		/// rather than have generic objects, we make a special case for these few
		/// objects that we know that all implementations are going to need and that
		/// we're going to have to handle specially
		vector<ControlPoint> points;

		//###################################################################
		// TODO: you might want to do this differently
		//###################################################################
		/// @name the state of the train
		/// @brief basically, all I need to remember is where it is in parameter space
		/// @{

		/// @brief 火車位於 第`floor(trainU)` 和其下一個控制點間
		float trainU;

		/// @}
};