#include "My_GLU.h"
#include <cmath>

namespace {
	void normalize_d(GLdouble* vec, GLint N) {
		// calculate the vector's length
		GLdouble length = 0;
		for (GLint i = 0; i < N; ++i) {
			length += (vec[i] * vec[i]);
		}
		length = sqrt(length);

		for (GLint i = 0; i < N; ++i) {
			vec[i] /= length;
		}
	}

	void cross_product_d(GLdouble* out, const GLdouble* u, const GLdouble* v) {
		out[0] = u[1] * v[2] - v[1] * u[2];
		out[1] = -u[0] * v[2] + v[0] * u[2];
		out[2] = u[0] * v[1] - v[0] * u[1];
	}
}

void My::perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	GLdouble f = 1.0 / tan((fovy * 180 / 3.14) / 2.0);
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
	GLdouble F[3] = { centerX - eyeX, centerY - eyeY, centerZ - eyeZ };
	normalize_d(F, 3);
	GLdouble UP[3] = { upX, upY, upZ };
	normalize_d(UP, 3);

	GLdouble M[4][4] = { {0} };
	M[0][2] = -F[0];
	M[1][2] = -F[1];
	M[2][2] = -F[2];
	M[3][3] = 1;

	GLdouble s[3];
	cross_product_d(s, F, UP);
	M[0][0] = s[0];
	M[1][0] = s[1];
	M[2][0] = s[2];

	GLdouble u[3];
	normalize_d(s, 3);
	cross_product_d(u, s, F);
	M[0][1] = u[0];
	M[1][1] = u[1];
	M[2][1] = u[2];

	glMultMatrixd(&M[0][0]);
	glTranslated(-eyeX, -eyeY, -eyeZ);
}
