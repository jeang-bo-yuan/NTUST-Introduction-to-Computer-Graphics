#include "My_GLU.h"
#include <cmath>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

void My::perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	GLdouble f = 1.0 / tan((fovy * 3.14 / 180) / 2.0);
	// store 4*4 matrix in column-major
	GLdouble m[16] = {
		f / aspect, 0, 0, 0,
		0, f, 0, 0,
		0, 0, (zFar + zNear) / (zNear - zFar), -1,
		0, 0, 2 * zFar * zNear / (zNear - zFar), 0
	};
	glMultMatrixd(m);
}

void My::lookAt(GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ,
	GLdouble centerX, GLdouble centerY, GLdouble centerZ,
	GLdouble upX, GLdouble upY, GLdouble upZ)
{
	glm::dvec3 F(centerX - eyeX, centerY - eyeY, centerZ - eyeZ);
	F = glm::normalize(F);

	glm::dvec3 UP(upX, upY, upZ);
	UP = glm::normalize(UP);

	glm::dvec3 s = glm::cross(F, UP);

	glm::dvec3 u = glm::cross(glm::normalize(s), F);

	// column-major
	double M[16] = {
		s[0], u[0], -F[0], 0,
		s[1], u[1], -F[1], 0,
		s[2], u[2], -F[2], 0,
		0   , 0   , 0    , 1
	};

	glMultMatrixd(M);
	glTranslated(-eyeX, -eyeY, -eyeZ);
}
