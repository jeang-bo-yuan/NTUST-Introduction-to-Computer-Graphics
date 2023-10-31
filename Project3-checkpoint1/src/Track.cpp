/************************************************************************
     File:        Track.cpp

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

#include "Track.H"
#include "Draw.H"

#include <FL/fl_ask.h>
#include <assert.h>
#include <cmath>

//****************************************************************************
//
// * Constructor
//============================================================================
CTrack::
CTrack() : trainU(0), num_of_cars(0), m_type(SplineType::Cardinal_Cubic)
//============================================================================
{
	resetPoints();
}

//****************************************************************************
//
// * provide a default set of points
//============================================================================
void CTrack::
resetPoints()
//============================================================================
{

	points.clear();
	points.push_back(ControlPoint(Pnt3f(50,5,0)));
	points.push_back(ControlPoint(Pnt3f(0,5,50)));
	points.push_back(ControlPoint(Pnt3f(-50,5,0)));
	points.push_back(ControlPoint(Pnt3f(0,5,-50)));

	// we had better put the train back at the start of the track...
	trainU = 0.0;
}

//****************************************************************************
//
// * Handy utility to break a string into a list of words
//============================================================================
void breakString(char* str, std::vector<const char*>& words) 
//============================================================================
{
	// start with no words
	words.clear();

	// scan through the string, starting at the beginning
	char* p = str;

	// stop when we hit the end of the string
	while(*p) {
		// skip over leading whitespace - stop at the first character or end of string
		while (*p && *p<=' ') p++;

		// now we're pointing at the first thing after the spaces
		// make sure its not a comment, and that we're not at the end of the string
		// (that's actually the same thing)
		if (! (*p) || *p == '#')
		break;

		// so we're pointing at a word! add it to the word list
		words.push_back(p);

		// now find the end of the word
		while(*p > ' ') p++;	// stop at space or end of string

		// if its ethe end of the string, we're done
		if (! *p) break;

		// otherwise, turn this space into and end of string (to end the word)
		// and keep going
		*p = 0;
		p++;
	}
}

//****************************************************************************
//
// * The file format is simple
//   first line: an integer with the number of control points
//	  other lines: one line per control point
//   either 3 (X,Y,Z) numbers on the line, or 6 numbers (X,Y,Z, orientation)
//============================================================================
void CTrack::
readPoints(const char* filename)
//============================================================================
{
	FILE* fp = fopen(filename,"r");
	if (!fp) {
		fl_alert("Can't Open File!\n");
	} 
	else {
		char buf[512];

		// first line = number of points
		fgets(buf,512,fp);
		size_t npts = (size_t) atoi(buf);

		if( (npts<4) || (npts>65535)) {
			fl_alert("Illegal Number of Points Specified in File");
		} else {
			points.clear();
			// get lines until EOF or we have enough points
			while( (points.size() < npts) && fgets(buf,512,fp) ) {
				Pnt3f pos,orient;
				vector<const char*> words;
				breakString(buf,words);
				if (words.size() >= 3) {
					pos.x = (float) strtod(words[0],0);
					pos.y = (float) strtod(words[1],0);
					pos.z = (float) strtod(words[2],0);
				} else {
					pos.x=0;
					pos.y=0;
					pos.z=0;
				}
				if (words.size() >= 6) {
					orient.x = (float) strtod(words[3],0);
					orient.y = (float) strtod(words[4],0);
					orient.z = (float) strtod(words[5],0);
				} else {
					orient.x = 0;
					orient.y = 1;
					orient.z = 0;
				}
				orient.normalize();
				points.push_back(ControlPoint(pos,orient));
			}
		}
		fclose(fp);
	}
	trainU = 0;
}

//****************************************************************************
//
// * write the control points to our simple format
//============================================================================
void CTrack::
writePoints(const char* filename)
//============================================================================
{
	FILE* fp = fopen(filename,"w");
	if (!fp) {
		fl_alert("Can't open file for writing");
	} else {
		fprintf(fp,"%d\n",points.size());
		for(size_t i=0; i<points.size(); ++i)
			fprintf(fp,"%g %g %g %g %g %g\n",
				points[i].pos.x, points[i].pos.y, points[i].pos.z, 
				points[i].orient.x, points[i].orient.y, points[i].orient.z);
		fclose(fp);
	}
}

Pnt3f CTrack::calc_pos(float U, Pnt3f* FACE, Pnt3f* LEFT, Pnt3f* UP) const
{
	// check U's value
	if (U < 0.f && U >= static_cast<float>(this->points.size())) return points[0].pos;

	size_t cp_id = floorf(U);
	float t = U - static_cast<float>(cp_id);

	Draw::Param_Equation point_eq, orient_eq;
	Draw::set_equation(*this, cp_id, m_type, point_eq, orient_eq);

	Pnt3f result = point_eq(t);

	if (FACE != nullptr || LEFT != nullptr || UP != nullptr) {
		// 前
		Pnt3f _FACE = point_eq(t + 0.01f) - result; _FACE.normalize();

		if (FACE != nullptr) *FACE = _FACE;
		if (LEFT == nullptr && UP == nullptr) return result;

		Pnt3f orient = orient_eq(t);
		// 左
		Pnt3f _LEFT = orient * _FACE; _LEFT.normalize();

		if (LEFT != nullptr) *LEFT = _LEFT;

		if (UP != nullptr) {
			// 上
			*UP = _FACE * _LEFT;
			UP->normalize();
		}
	}

	return result;
}

float CTrack::arc_length(size_t cp_id) const
{
	Draw::Param_Equation point_eq, orient_eq;
	Draw::set_equation(*this, cp_id, m_type, point_eq, orient_eq);

	float len = 0.f;
	Pnt3f p1 = point_eq(0.f);
	for (float t = 0.f; t < 1.f; t += GLOBAL::Param_Interval) {
		Pnt3f p2 = point_eq(t + GLOBAL::Param_Interval);
		Pnt3f delta = p2 - p1;

		len += sqrtf(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);

		// 暫存起來
		p1 = p2;
	}

	return len;
}

std::vector<float> CTrack::list_points(float startU, float delta, size_t count) const
{
	if (count == 0) return std::vector<float>();

	float S = this->T_to_S(startU);

	std::vector<float> result;
	result.reserve(count);
	// repeat for `count` times
	// 將S加上delta並轉回參數空間的點
	for (size_t i = 0; i < count; ++i) {
#ifndef NDEBUG
		float oldS = S;
#endif
		// 前進特定長度
		S += delta;
		// prevent overflow
		while (S >= m_Arc_Len_Accum.back().second) S -= m_Arc_Len_Accum.back().second;
		// prevent underflow
		while (S < 0) S += m_Arc_Len_Accum.back().second;

		// （實際空間） 轉回 （參數空間）
		result.push_back(this->S_to_T(S));
	}

	assert(result.size() == count);
	return result;
}

void CTrack::set_spline(SplineType type)
{
	m_type = type;

	// reset
	m_Arc_Len_Accum.clear();
	m_Arc_Len_Accum.reserve(points.size() * 16 + 1);
	m_Arc_Len_Accum.emplace_back(std::pair<float, float>{ 0, 0 });

	// for each control point
	for (size_t cp_id = 0; cp_id < points.size(); ++cp_id) {
		Draw::Param_Equation point_eq, unused;
		Draw::set_equation(*this, cp_id, m_type, point_eq, unused);

		Pnt3f p1 = point_eq(0);
		for (float t = GLOBAL::Param_Interval; t <= 1; t += GLOBAL::Param_Interval) {
			Pnt3f p2 = point_eq(t);
			Pnt3f delta = p2 - p1;
			float len = sqrtf(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);

			m_Arc_Len_Accum.emplace_back(std::pair<float, float>{
				cp_id + t,
				len + m_Arc_Len_Accum.back().second
			});

			// store it for next iteration
			p1 = p2;
		}
	}
}

float CTrack::T_to_S(float T) const
{
	for (size_t i = 0; i < m_Arc_Len_Accum.size() - 1; ++i) {
		const float lowU = m_Arc_Len_Accum[i].first;
		const float highU = m_Arc_Len_Accum[i + 1].first;
		if (lowU <= T && T < highU) {
			const float lowS = m_Arc_Len_Accum[i].second;
			const float highS = m_Arc_Len_Accum[i + 1].second;
			return lowS + (T - lowU) / GLOBAL::Param_Interval * (highS - lowS);
		}
	}

	assert(false); // should not go here
	return NAN;
}

float CTrack::S_to_T(float S) const
{
	/// @note 處理極端特例，當S很接近0或最大長度時，回傳0
	if (fabs(S) < 1.e-4 || fabs(S - m_Arc_Len_Accum.back().second) < 1.e-4) return 0;

	// （實際空間） 轉回 （參數空間）
	for (size_t i = 0; i < m_Arc_Len_Accum.size() - 1; ++i) {
		const float lowS = m_Arc_Len_Accum[i].second;
		const float highS = m_Arc_Len_Accum[i + 1].second;
		if ((lowS <= S && S < highS)) {
			const float lowU = m_Arc_Len_Accum[i].first;
			const float highU = m_Arc_Len_Accum[i + 1].first;
			return lowU + (S - lowS) / (highS - lowS) * GLOBAL::Param_Interval;
		}
	}

	assert(false); // should not go here
	return NAN;
}
