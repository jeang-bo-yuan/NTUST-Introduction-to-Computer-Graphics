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
#include <utility>

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

	/** @brief 計算參數空間中的U，所對應的點的位置
		@param U - 參數空間的位置，和 trainU 一樣，代表在第`floor(U)`和下一個控制點間的點
		@param[out] FACE - 若不為`nullptr`，則dereference並設成該點面向的方向
		@param[out] LEFT - 若不為`nullptr`，則dereference並設成該點左邊的方向
		@param[out] UP - 若不為`nullptr`，則dereference並設成該點向上的方向
		@return 點的位置
		@pre 0 <= U < `this->points.size()`
		@post
		1. 若U是非法的，則回傳`this->points[0].pos`
		2. FACE, RIGHT, UP 都會設成單位向量
		*/
	Pnt3f calc_pos(float U, Pnt3f* FACE = nullptr, Pnt3f* LEFT = nullptr, Pnt3f* UP = nullptr) const;

	/**
		* @brief 計算某兩個control point間的曲線長
		* @param cp_id - 第一個control point的index
		* @return 曲線長
		*/
	float arc_length(size_t cp_id) const;

	/**
		* @brief 列出一長串的點
		* @details 給定一個 參數空間內的起始點（startU） 和 實際空間中的間隔（delta），
		*          list_points會從 起始點（startU） 開始重覆的加上 這個間隔（delta） 並放入回傳的vector中
		* @param startU - start point in param space
		* @param delta - the gap in "real space"
		* @param count - how many points will be listed
		* @return 一個vector，長度為count。vector中的每個元素都是參數空間內的一個點，但不包含startU。
		*/
	std::vector<float> list_points(float startU, float delta, size_t count) const;


	/**
		 * @brief 設定參數曲線的種類，並更新內部的Arc_Len_Accum
		 * @param type - 參數曲線的種類
		 */
	void set_spline(SplineType type);

	/**
	 * @brief 設定Cardinal Curve的Tension
	 * @param tension
	 * @post 若目前的spline type是Cardinal，則更新 Arc_Len_Accum
	 */
	void set_tension(float tension);

	/// return tension of cardinal
	float get_tension() const { return m_tension; }

	/// @brief 取得參數曲線的種類
	SplineType spline_type() const { return m_type; }

public:
	/// @name Arc Length Parameterization
	/// @{
	 
	/// @brief 曲線長累積表的型別
	/// @details Accumulation of arc length. elem.first = t in "param space", elem.second = s in "real space"
	typedef std::vector<std::pair<float, float>> Arc_Len_Accum_T;

	/// @brief 將參數空間中的T換成實際距離S
	/// @details 依據 this->Arc_Len_Accum 做轉換
	float T_to_S(float T) const;

	/// @brief 將實際距離S換成參數空間中的T
	/// @details 依據 this->Arc_Len_Accum 做轉換
	float S_to_T(float S) const;

	/// @brief 取得軌道的曲線長累積表
	/// @details 它是`std::pair<float, float>`組成的vector。
	///          elem.first是參數空間內的一點。
	///          elem.second則是參數空間中 0 ~ elem.first 的曲線長。
	/// @return 累積表的參考
	const Arc_Len_Accum_T& Arc_Len_Accum() const { return m_Arc_Len_Accum; }

	/// @}

public: // attributes
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

	/// 車箱的數量
	size_t num_of_cars;

private:
	SplineType m_type;

	/// @brief Accumulation of arc length. elem.first = t in "param space", elem.second = s in "real space"
	Arc_Len_Accum_T m_Arc_Len_Accum;

	/// Tension of Cardinal Curve
	float m_tension;
};

