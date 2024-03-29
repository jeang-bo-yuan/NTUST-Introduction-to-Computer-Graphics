/************************************************************************
     File:        TrainView.H

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu

     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     
						The TrainView is the window that actually shows the 
						train. Its a
						GL display canvas (Fl_Gl_Window).  It is held within 
						a TrainWindow
						that is the outer window with all the widgets. 
						The TrainView needs 
						to be aware of the window - since it might need to 
						check the widgets to see how to draw

	  Note:        we need to have pointers to this, but maybe not know 
						about it (beware circular references)

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#pragma once

// Preclarify for preventing the compiler error
class TrainWindow;
class CTrack;


//#######################################################################
// TODO
// You might change the TrainView in order to add different objects to
// be drawn, or adjust the lighting, or ...
//#######################################################################


// The TrainView also needs to handle its user interface events.
// since this is an FLTk Window, we need to include FlTk Headers
// the pragmas turn off the warnings from FlTk - our code should have no warnings,
// but their code doesn't meet that standard
#pragma warning(push)
#pragma warning(disable:4312)
#pragma warning(disable:4311)
#include <Fl/Fl_Gl_Window.h>
#pragma warning(pop)

// this uses the old ArcBall Code
#include "Utilities/ArcBallCam.H"

#include "RenderUtilities/BufferObject.h"
#include "RenderUtilities/Shader.h"
#include "RenderUtilities/Texture.h"
#include "Wave_VAO.h"
#include "Box_VAO.h"
#include "TextureCubeMap.h"
#include <memory>

/**
 * @brief 顯示火車的畫面
 */
class TrainView : public Fl_Gl_Window
{
	public:
		// note that we keep the "standard widget" constructor arguments
		TrainView(int x, int y, int w, int h, const char* l = 0);

		// overrides of important window things
		virtual int handle(int);
		virtual void draw();

		// all of the actual drawing happens in this routine
		// it has to be encapsulated, since we draw differently if
		// we're drawing shadows (no colors, for example)
		void drawStuff(bool doingShadows=false);

		// setup the projection - assuming that the projection stack has been
		// cleared for you
		void setProjection();

		// Reset the Arc ball control
		void resetArcball();

		// pick a point (for when the mouse goes down)
		void doPick();

	public:
		ArcBallCam		arcball;			// keep an ArcBall for the UI
		/// @brief 選擇的control point cube
		/// @details simple - just remember which cube is selected
		int				selectedCube;

		/// The parent of this display window
		TrainWindow*	tw;
		/// The track of the entire scene
		CTrack*			m_pTrack;

private:
	std::unique_ptr<Shader> m_shader;
	std::unique_ptr<Texture2D> m_texture;
	std::unique_ptr<VAO> m_plane;
	std::unique_ptr<UBO> m_common_matrices;

	std::unique_ptr<Wave_VAO> m_wave_VAO_p; // wave
	std::unique_ptr<Shader> m_wave_shader_p; // shader for wave

	std::unique_ptr<Box_VAO> m_skybox_VAO_p;
	std::unique_ptr<Shader> m_skybox_shader_p;
	std::unique_ptr<qtTextureCubeMap> m_texture_skybox;

private: // private methods that initialize VAO, UBO, shader ...
	/// load shaders
	void init_shader();

	/// allocate storage for UBO
	void init_UBO();

	/// store modelview and projection matrix into UBO
	void setup_UBO();

	/// allocate VAO's storage and set up values
	void init_VAO();

	/// load texture
	void init_texture();

	/// draw m_plane
	void draw_plane();
};