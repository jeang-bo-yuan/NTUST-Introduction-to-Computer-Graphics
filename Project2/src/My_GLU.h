#pragma once
#include "gl/gl.h"

namespace My {
	//! @brief implementation of gluPerspective
	//! @details
	//! Regerence: https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml
	void perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

	//! @brief implementation of gluLookAt
	//! @details
	//! Reference: https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml
	void lookAt(GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ,
		GLdouble centerX, GLdouble centerY, GLdouble centerZ,
		GLdouble upX, GLdouble upY, GLdouble upZ);
}