/************************************************************************
     File:        TrainView.cpp

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

#include <iostream>
#include <Fl/fl.h>

// we will need OpenGL, and OpenGL needs windows.h
#include <windows.h>
//#include "GL/gl.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GL/glu.h"

#include "TrainView.H"
#include "TrainWindow.H"
#include "Utilities/3DUtils.H"
#include "Draw.H"


#ifdef EXAMPLE_SOLUTION
#	include "TrainExample/TrainExample.H"
#endif


//************************************************************************
//
// * Constructor to set up the GL window
//========================================================================
TrainView::
TrainView(int x, int y, int w, int h, const char* l) 
	: Fl_Gl_Window(x,y,w,h,l)
//========================================================================
{
	mode( FL_RGB|FL_ALPHA|FL_DOUBLE | FL_STENCIL );

	resetArcball();
}

//************************************************************************
//
// * Reset the camera to look at the world
//========================================================================
void TrainView::
resetArcball()
//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}

//************************************************************************
//
// * FlTk Event handler for the window
//########################################################################
// TODO: 
//       if you want to make the train respond to other events 
//       (like key presses), you might want to hack this.
//########################################################################
//========================================================================
int TrainView::handle(int event)
{
	// see if the ArcBall will handle the event - if it does, 
	// then we're done
	// note: the arcball only gets the event if we're in world view
	if (tw->worldCam->value())
		if (arcball.handle(event)) 
			return 1;

	// remember what button was used
	static int last_push;

	switch(event) {
		// Mouse button being pushed event
		case FL_PUSH:
			last_push = Fl::event_button();
			// if the left button be pushed is left mouse button
			if (last_push == FL_LEFT_MOUSE  ) {
				doPick();
				damage(1);
				return 1;
			};
			break;

	   // Mouse button release event
		case FL_RELEASE: // button release
			damage(1);
			last_push = 0;
			return 1;

		// Mouse button drag event
		case FL_DRAG:

			// Compute the new control point position
			if ((last_push == FL_LEFT_MOUSE) && (selectedCube >= 0)) {
				ControlPoint* cp = &m_pTrack->points[selectedCube];

				double r1x, r1y, r1z, r2x, r2y, r2z;
				getMouseLine(r1x, r1y, r1z, r2x, r2y, r2z);

				double rx, ry, rz;
				mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z, 
								static_cast<double>(cp->pos.x), 
								static_cast<double>(cp->pos.y),
								static_cast<double>(cp->pos.z),
								rx, ry, rz,
								(Fl::event_state() & FL_CTRL) != 0);

				cp->pos.x = (float) rx;
				cp->pos.y = (float) ry;
				cp->pos.z = (float) rz;
				/// @note Update track's CTrack::Arc_Len_Accum after drag
				tw->update_arc_len_accum();
				damage(1);
			}
			break;

		// in order to get keyboard events, we need to accept focus
		case FL_FOCUS:
			return 1;

		// every time the mouse enters this window, aggressively take focus
		case FL_ENTER:	
			focus(this);
			break;
		case FL_KEYBOARD:
		 		int k = Fl::event_key();
				int ks = Fl::event_state();
				if (k == 'p') {
					// Print out the selected control point information
					if (selectedCube >= 0) 
						printf("Selected(%d) (%g %g %g) (%g %g %g)\n",
								 selectedCube,
								 m_pTrack->points[selectedCube].pos.x,
								 m_pTrack->points[selectedCube].pos.y,
								 m_pTrack->points[selectedCube].pos.z,
								 m_pTrack->points[selectedCube].orient.x,
								 m_pTrack->points[selectedCube].orient.y,
								 m_pTrack->points[selectedCube].orient.z);
					else
						printf("Nothing Selected\n");

					return 1;
				};
				break;
	}

	return Fl_Gl_Window::handle(event);
}

//************************************************************************
//
// * this is the code that actually draws the window
//   it puts a lot of the work into other routines to simplify things
//========================================================================
void TrainView::draw()
{

	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	//initialized glad
	if (gladLoadGL())
	{
		//initiailize VAO, VBO, Shader...
		if (m_shader == nullptr) this->init_shader();
		if (m_common_matrices == nullptr) this->init_UBO();
		if (m_plane == nullptr) this->init_VAO();
		if (m_texture == nullptr) this->init_texture();
	}
	else
		throw std::runtime_error("Could not initialize GLAD!");

	// Set up the view port
	glViewport(0,0,w(),h());

	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0,0,.3f,0);		// background should be blue

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();		// put the code to set up matrices here

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// top view only needs one light
	if (tw->topCam->value()) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	} else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	GLfloat lightPosition1[]	= {0,1,1,0}; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[]	= {1, 0, 0, 0};
	GLfloat lightPosition3[]	= {0, -1, 0, 0};
	GLfloat yellowLight[]		= {0.5f, 0.5f, .1f, 1.0};
	GLfloat whiteLight[]			= {1.0f, 1.0f, 1.0f, 1.0};
	GLfloat blueLight[]			= {.1f,.1f,.3f,1.0};
	GLfloat grayLight[]			= {.3f, .3f, .3f, 1.0};

	switch (tw->lightBrowser->value()) {
	case (int)LightType::Directional:
		glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
		glLightfv(GL_LIGHT0, GL_SPECULAR, whiteLight);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.f);
		break;
	case (int)LightType::Point:
		lightPosition1[3] = 1;
		lightPosition2[3] = 1;
		lightPosition3[3] = 1;
		glLightfv(GL_LIGHT0, GL_DIFFUSE, yellowLight);
		glLightfv(GL_LIGHT0, GL_SPECULAR, yellowLight);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.f);
		break;
	case (int)LightType::Spotlight:
		glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
		glLightfv(GL_LIGHT0, GL_SPECULAR, whiteLight);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.f);

		// calculate train's pos
		Pnt3f dir;
		Pnt3f pos = this->m_pTrack->calc_pos(this->m_pTrack->trainU, &dir);

		lightPosition1[0] = pos.x; lightPosition1[1] = pos.y + 2 * train_size; lightPosition1[2] = pos.z; lightPosition1[3] = 1;
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir.v());
		break;
	}
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);



	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	// set to opengl fixed pipeline(use opengl 1.x draw function)
	glUseProgram(0);

	setupFloor();
	drawFloor(200,10);

	draw_plane();

	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	setupObjects();

	drawStuff();

	// this time drawing is for shadows (except for top view)
	if (!tw->topCam->value()) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(w()) / static_cast<float>(h());

	// Check whether we use the world camp
	if (tw->worldCam->value())
		arcball.setProjection(false);
	// Or we use the top cam
	else if (tw->topCam->value()) {
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		} 
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90,1,0,0);
	} 
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else {
		Pnt3f FACE, UP, LEFT;
		Pnt3f pos = this->m_pTrack->calc_pos(this->m_pTrack->trainU, &FACE, &LEFT, &UP);

		glMatrixMode(GL_PROJECTION);
		gluPerspective(50.f, aspect, .1f, 1000.f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		GLfloat rotate_matrix[16] = {
			-LEFT.x, UP.x, -FACE.x, 0,
			-LEFT.y, UP.y, -FACE.y, 0,
			-LEFT.z, UP.z, -FACE.z, 0,
			0,       0,    0,       1
		};
		glMultMatrixf(rotate_matrix);
		glTranslatef(-train_size * UP.x, -train_size * UP.y, -train_size * UP.z);
		glTranslatef(-pos.x, -pos.y, -pos.z);
	}
}

//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::drawStuff(bool doingShadows)
{
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	if (!tw->trainCam->value()) {
		for(size_t i=0; i<m_pTrack->points.size(); ++i) {
			if (!doingShadows) {
				if ( ((int) i) != selectedCube)
					glColor3ub(240, 60, 60);
				else
					glColor3ub(240, 240, 30);
			}
			m_pTrack->points[i].draw();
		}
	}
	// draw the track
	//####################################################################
	// TODO: 
	// call your own track drawing code
	//####################################################################
	m_pTrack->set_tension(tw->tension->value());
	Draw::draw_track(*m_pTrack, doingShadows);

	// draw the train
	//####################################################################
	// TODO: 
	//	call your own train drawing code
	//####################################################################
	// don't draw the train if you're looking out the front window
	if (!tw->trainCam->value())
		Draw::draw_train(*m_pTrack, doingShadows);
}

// 
//************************************************************************
//
// * this tries to see which control point is under the mouse
//	  (for when the mouse is clicked)
//		it uses OpenGL picking - which is always a trick
//########################################################################
// TODO: 
//		if you want to pick things other than control points, or you
//		changed how control points are drawn, you might need to change this
//########################################################################
//========================================================================
void TrainView::
doPick()
//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	make_current();		

	// where is the mouse?
	int mx = Fl::event_x(); 
	int my = Fl::event_y();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Set up the pick matrix on the stack - remember, FlTk is
	// upside down!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();
	gluPickMatrix((double)mx, (double)(viewport[3]-my), 
						5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100,buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	// draw the cubes, loading the names as we go
	for(size_t i=0; i<m_pTrack->points.size(); ++i) {
		glLoadName((GLuint) (i+1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3]-1;
	} else // nothing hit, nothing selected
		selectedCube = -1;

	printf("Selected Cube %d\n",selectedCube);
}

void TrainView::init_shader()
{
	m_shader = std::make_unique<Shader>(
		"./shaders/simple.vert",
		nullptr, nullptr, nullptr,
		"./shaders/simple.frag");
}

void TrainView::init_UBO()
{
	m_common_matrices = std::make_unique<UBO>();
	// 存 2 個 4x4 matrix
	m_common_matrices->size = 2 * sizeof(glm::mat4);
	// generate a buffer object
	glGenBuffers(1, &m_common_matrices->ubo);

	glBindBuffer(GL_UNIFORM_BUFFER, m_common_matrices->ubo);
	// allocate storage
	glBufferData(GL_UNIFORM_BUFFER, m_common_matrices->size, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void TrainView::setup_UBO()
{
	glm::mat4 modelview;
	glm::mat4 projection;

	glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(modelview));
	glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(projection));

	glBindBuffer(GL_UNIFORM_BUFFER, m_common_matrices->ubo);
	// 將projection和modelview兩矩陣依序存入OpenGL分配的空間中
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(modelview));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void TrainView::init_VAO()
{
	constexpr GLfloat w = 0.5f;
	constexpr GLfloat vertices[] = {
		// vertex     color
		0, 0, -w,     0, 1, 0,
		w, 0, w,      0, 0, 1,
		-w, 0, w,     1, 0, 0,
	};
	constexpr GLfloat normal[] = {
		0, 1, 0,
		0, 1, 0,
		0, 1, 0
	};
	constexpr GLfloat texture_coordinate[] = {
		0.5, 1,
		0, 0,
		1, 0
	};
	constexpr GLuint element[] = {
		0, 1, 2
	};

	m_plane = std::make_unique<VAO>();
	m_plane->element_amount = sizeof(element) / sizeof(GLuint);
	glGenVertexArrays(1, &m_plane->vao);
	glGenBuffers(3, m_plane->vbo);
	glGenBuffers(1, &m_plane->ebo);

	glBindVertexArray(m_plane->vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_plane->vbo[0]); // bind vbo[0]
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // store vertices into it
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0); // set 0-th attribute data
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // set 3-rd attribute date
	glEnableVertexAttribArray(0); // enable 0-th attribute data
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, m_plane->vbo[1]); // bind vbo[1]
	glBufferData(GL_ARRAY_BUFFER, sizeof(normal), normal, GL_STATIC_DRAW); // store normal into it
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // set vbo[1] as vao's 1-st attribute data
	glEnableVertexAttribArray(1); // enable 1-st attribute data

	glBindBuffer(GL_ARRAY_BUFFER, m_plane->vbo[2]); // bind vbo[2]
	glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coordinate), texture_coordinate, GL_STATIC_DRAW); // store texture_coordinate into it
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // set vbo[2] as vao's 2-nd attribute data
	glEnableVertexAttribArray(2); // enable 2-nd attribute data

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane->ebo); // bind ebo
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(element), element, GL_STATIC_DRAW); // store element into it

	// Attribut 和 EBO 的綁定會記在VBO內
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void TrainView::init_texture()
{
	m_texture = std::make_unique<Texture2D>("./images/church.png");
}

void TrainView::draw_plane()
{
	this->setup_UBO();

	// 將m_common_matrices->ubo這個buffer object綁定到GL_UNIFORM_BUFFER中的index 0
	// GL_UNIFORM_BUFFER包含許多的index，要用glBindBufferRange才能綁定到特定index
	// simple.vert內的layout uniform common_matrices會去使用GL_UNIFORM_BUFFER中的index 0
	glBindBufferRange(GL_UNIFORM_BUFFER, /* bind to index */ 0, m_common_matrices->ubo, 0, m_common_matrices->size);

	// bind shader
	m_shader->Use();

	glm::mat4 model_matrix;
	model_matrix = glm::translate(model_matrix, glm::vec3(0.f, 10.f, 0.f));
	model_matrix = glm::scale(model_matrix, glm::vec3(10.f, 10.f, 10.f));

	glUniformMatrix4fv( // set uniform data
		glGetUniformLocation(m_shader->Program, "u_model"),
		1,
		GL_FALSE, // don't transpose
		glm::value_ptr(model_matrix)
	);

	glUniform3fv( // set uniform data
		glGetUniformLocation(m_shader->Program, "u_color"),
		1,
		glm::value_ptr(glm::vec3(0.f, 1.f, 0.f))
	);

	m_texture->bind(0);
	glUniform1i(
		glGetUniformLocation(m_shader->Program, "u_texture"),
		0
	);

	// bind VAO
	glBindVertexArray(m_plane->vao);

	glDrawElements(GL_TRIANGLES, m_plane->element_amount, GL_UNSIGNED_INT, 0);

	// unbind VAO
	glBindVertexArray(0);

	// unbind uniform buffer
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// unbind shader
	glUseProgram(0);
}
