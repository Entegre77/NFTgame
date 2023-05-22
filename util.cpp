/*
    OpenUniverse 1.0
    Copyright (C) 2000  Raul Alonso <amil@las.es>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "ou.h"
#include <string.h>


float gettime(void)
{
	static float told = 0.0f;
	float tnew, ris;

	tnew = glutGet((GLenum) GLUT_ELAPSED_TIME);

	ris = tnew - told;

	told = tnew;

	return ris / 1000.0;
}

/* Rotates point around the axis, 'angle' radians,
  borrowed from Mesa */
void Rotation(double angle, double axis[3], double point[3])
{

	double mag, s, c;
	double x, y, z, xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;

	s = sin(angle);
	c = cos(angle);

	mag = MODULE(axis);

	if (mag == 0.0)
		return;

	x = axis[X] / mag;
	y = axis[Y] / mag;
	z = axis[Z] / mag;

	xx = x * x;
	yy = y * y;
	zz = z * z;
	xy = x * y;
	yz = y * z;
	zx = z * x;
	xs = x * s;
	ys = y * s;
	zs = z * s;
	one_c = 1.0 - c;

	x = point[X] * ((one_c * xx) + c);
	y = point[X] * ((one_c * xy) - zs);
	z = point[X] * ((one_c * zx) + ys);

	x += point[Y] * ((one_c * xy) + zs);
	y += point[Y] * ((one_c * yy) + c);
	z += point[Y] * ((one_c * yz) - xs);

	x += point[Z] * ((one_c * zx) - ys);
	y += point[Z] * ((one_c * yz) + xs);
	z += point[Z] * ((one_c * zz) + c);

	point[X] = x;
	point[Y] = y;
	point[Z] = z;
}


void MakeQuat(double *q, double angle, double ax, double ay, double az)
{
	double d;
	double s;

	angle /= 2.0;
	d = DISTANCE(ax, ay, az);
	ax /= d;
	ay /= d;
	az /= d;

	s = -sin(angle);

	q[0] = cos(angle);
	q[1] = s * ax;
	q[2] = s * ay;
	q[3] = s * az;
}


void Quat2Matrix(double *q, float m[16])
{
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = q[1] + q[1];
	y2 = q[2] + q[2];
	z2 = q[3] + q[3];
	xx = q[1] * x2;
	xy = q[1] * y2;
	xz = q[1] * z2;
	yy = q[2] * y2;
	yz = q[2] * z2;
	zz = q[3] * z2;
	wx = q[0] * x2;
	wy = q[0] * y2;
	wz = q[0] * z2;

	m[0] = 1.0 - (yy + zz);
	m[1] = xy - wz;
	m[2] = xz + wy;
	m[3] = 0.0;

	m[4] = xy + wz;
	m[5] = 1.0 - (xx + zz);
	m[6] = yz - wx;
	m[7] = 0.0;

	m[8] = xz - wy;
	m[9] = yz + wx;
	m[10] = 1.0 - (xx + yy);
	m[11] = 0.0;

	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;
}


void Mat2Quat(float m[4][4], double *q)
{
	float tr, s, q2[4];
	int i, j, k;
	int nxt[3] = { 1, 2, 0 };

	tr = m[0][0] + m[1][1] + m[2][2];

	if (tr > 0.0) {
		s = sqrt(tr + 1.0);
		q[0] = s / 2.0;
		s = 0.5 / s;
		q[1] = (m[1][2] - m[2][1]) * s;
		q[2] = (m[2][0] - m[0][2]) * s;
		q[3] = (m[0][1] - m[1][0]) * s;
	} else {
		i = 0;
		if (m[1][1] > m[0][0])
			i = 1;
		if (m[2][2] > m[i][i])
			i = 2;
		j = nxt[i];
		k = nxt[j];

		s = sqrt((m[i][i] - (m[j][j] + m[k][k])) + 1.0);

		q2[i] = s * 0.5;

		if (s != 0.0)
			s = 0.5 / s;

		q2[3] = (m[j][k] - m[k][j]) * s;
		q2[j] = (m[i][j] + m[j][i]) * s;
		q2[k] = (m[i][k] + m[k][i]) * s;

		q[1] = q2[0];
		q[2] = q2[1];
		q[3] = q2[2];
		q[0] = q2[3];
	}
}

void Euler2Quat(double roll, double pitch, double yaw, double *q)
{
	float cr, cp, cy, sr, sp, sy, cpcy, spsy;

	cr = cos(roll / 2);
	cp = cos(pitch / 2);
	cy = cos(yaw / 2);

	sr = sin(roll / 2);
	sp = sin(pitch / 2);
	sy = sin(yaw / 2);

	cpcy = cp * cy;
	spsy = sp * sy;

	q[0] = cr * cpcy + sr * spsy;
	q[1] = sr * cpcy - cr * spsy;
	q[2] = cr * sp * cy + sr * cp * sy;
	q[3] = cr * cp * sy - sr * sp * cy;

}



#define SWAP_ROWS(a, b) { double *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) m[(c)*4+(r)]

/*
 * Compute inverse of 4x4 transformation matrix.
 * Code contributed by Jacques Leroy jle@star.be
 * Return TRUE for success, FALSE for failure (singular matrix)
 *
 *      Aug '99: Adapted for openuniverse from the Mesa 3.1 source code
 */
static int invert_matrix_general(double mat[16], double inv[16])
{
	double *m = mat;
	double *out = inv;
	double wtmp[4][8];
	double m0, m1, m2, m3, s;
	double *r0, *r1, *r2, *r3;

	r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

	r0[0] = MAT(m, 0, 0), r0[1] = MAT(m, 0, 1),
		r0[2] = MAT(m, 0, 2), r0[3] = MAT(m, 0, 3),
		r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,
		r1[0] = MAT(m, 1, 0), r1[1] = MAT(m, 1, 1),
		r1[2] = MAT(m, 1, 2), r1[3] = MAT(m, 1, 3),
		r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,
		r2[0] = MAT(m, 2, 0), r2[1] = MAT(m, 2, 1),
		r2[2] = MAT(m, 2, 2), r2[3] = MAT(m, 2, 3),
		r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,
		r3[0] = MAT(m, 3, 0), r3[1] = MAT(m, 3, 1),
		r3[2] = MAT(m, 3, 2), r3[3] = MAT(m, 3, 3),
		r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

	/* choose pivot - or die */
	if (fabs(r3[0]) > fabs(r2[0]))
		SWAP_ROWS(r3, r2);
	if (fabs(r2[0]) > fabs(r1[0]))
		SWAP_ROWS(r2, r1);
	if (fabs(r1[0]) > fabs(r0[0]))
		SWAP_ROWS(r1, r0);
	if (0.0 == r0[0])
		return 0;

	/* eliminate first variable     */
	m1 = r1[0] / r0[0];
	m2 = r2[0] / r0[0];
	m3 = r3[0] / r0[0];
	s = r0[1];
	r1[1] -= m1 * s;
	r2[1] -= m2 * s;
	r3[1] -= m3 * s;
	s = r0[2];
	r1[2] -= m1 * s;
	r2[2] -= m2 * s;
	r3[2] -= m3 * s;
	s = r0[3];
	r1[3] -= m1 * s;
	r2[3] -= m2 * s;
	r3[3] -= m3 * s;
	s = r0[4];
	if (s != 0.0) {
		r1[4] -= m1 * s;
		r2[4] -= m2 * s;
		r3[4] -= m3 * s;
	}
	s = r0[5];
	if (s != 0.0) {
		r1[5] -= m1 * s;
		r2[5] -= m2 * s;
		r3[5] -= m3 * s;
	}
	s = r0[6];
	if (s != 0.0) {
		r1[6] -= m1 * s;
		r2[6] -= m2 * s;
		r3[6] -= m3 * s;
	}
	s = r0[7];
	if (s != 0.0) {
		r1[7] -= m1 * s;
		r2[7] -= m2 * s;
		r3[7] -= m3 * s;
	}

	/* choose pivot - or die */
	if (fabs(r3[1]) > fabs(r2[1]))
		SWAP_ROWS(r3, r2);
	if (fabs(r2[1]) > fabs(r1[1]))
		SWAP_ROWS(r2, r1);
	if (0.0 == r1[1])
		return 0;

	/* eliminate second variable */
	m2 = r2[1] / r1[1];
	m3 = r3[1] / r1[1];
	r2[2] -= m2 * r1[2];
	r3[2] -= m3 * r1[2];
	r2[3] -= m2 * r1[3];
	r3[3] -= m3 * r1[3];
	s = r1[4];
	if (0.0 != s) {
		r2[4] -= m2 * s;
		r3[4] -= m3 * s;
	}
	s = r1[5];
	if (0.0 != s) {
		r2[5] -= m2 * s;
		r3[5] -= m3 * s;
	}
	s = r1[6];
	if (0.0 != s) {
		r2[6] -= m2 * s;
		r3[6] -= m3 * s;
	}
	s = r1[7];
	if (0.0 != s) {
		r2[7] -= m2 * s;
		r3[7] -= m3 * s;
	}

	/* choose pivot - or die */
	if (fabs(r3[2]) > fabs(r2[2]))
		SWAP_ROWS(r3, r2);
	if (0.0 == r2[2])
		return 0;

	/* eliminate third variable */
	m3 = r3[2] / r2[2];
	r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
		r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6], r3[7] -= m3 * r2[7];

	/* last check */
	if (0.0 == r3[3])
		return 0;

	s = 1.0 / r3[3];			/* now back substitute row 3 */
	r3[4] *= s;
	r3[5] *= s;
	r3[6] *= s;
	r3[7] *= s;

	m2 = r2[3];					/* now back substitute row 2 */
	s = 1.0 / r2[2];
	r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
		r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
	m1 = r1[3];
	r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
		r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
	m0 = r0[3];
	r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
		r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

	m1 = r1[2];					/* now back substitute row 1 */
	s = 1.0 / r1[1];
	r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
		r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
	m0 = r0[2];
	r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
		r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

	m0 = r0[1];					/* now back substitute row 0 */
	s = 1.0 / r0[0];
	r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
		r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

	MAT(out, 0, 0) = r0[4];
	MAT(out, 0, 1) = r0[5], MAT(out, 0, 2) = r0[6];
	MAT(out, 0, 3) = r0[7], MAT(out, 1, 0) = r1[4];
	MAT(out, 1, 1) = r1[5], MAT(out, 1, 2) = r1[6];
	MAT(out, 1, 3) = r1[7], MAT(out, 2, 0) = r2[4];
	MAT(out, 2, 1) = r2[5], MAT(out, 2, 2) = r2[6];
	MAT(out, 2, 3) = r2[7], MAT(out, 3, 0) = r3[4];
	MAT(out, 3, 1) = r3[5], MAT(out, 3, 2) = r3[6];
	MAT(out, 3, 3) = r3[7];

	return 1;
}

#undef SWAP_ROWS



/* Based upon the gluLookAt function from the Mesa 3.1 source tree.
   The very only differences with the original function are:
        -Removed the call to glTranlate
        -Rotation matrix is inverted

   This function is used to rotate spaceships so they orientation
   matches their dir and up vectors.

*/
void MyLookAt(double dir[3], double up[3])
{
	GLdouble m[16], inv[16];
	GLdouble x[3], y[3], z[3];
	GLdouble mag;

	/* Make rotation matrix */

	/* Z vector */
	z[0] = dir[0];
	z[1] = dir[1];
	z[2] = dir[2];
	mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
	if (mag) {					/* mpichler, 19950515 */
		z[0] /= mag;
		z[1] /= mag;
		z[2] /= mag;
	}

	/* Y vector */
	y[0] = up[0];
	y[1] = up[1];
	y[2] = up[2];

	/* X vector = Y cross Z */
	x[0] = y[1] * z[2] - y[2] * z[1];
	x[1] = -y[0] * z[2] + y[2] * z[0];
	x[2] = y[0] * z[1] - y[1] * z[0];

	/* Recompute Y = Z cross X */
	y[0] = z[1] * x[2] - z[2] * x[1];
	y[1] = -z[0] * x[2] + z[2] * x[0];
	y[2] = z[0] * x[1] - z[1] * x[0];

	/* mpichler, 19950515 */
	/* cross product gives area of parallelogram, which is < 1.0 for
	 * non-perpendicular unit-length vectors; so normalize x, y here
	 */

	mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
	if (mag) {
		x[0] /= mag;
		x[1] /= mag;
		x[2] /= mag;
	}

	mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
	if (mag) {
		y[0] /= mag;
		y[1] /= mag;
		y[2] /= mag;
	}
#define M(row,col)  m[col*4+row]
	M(0, 0) = x[0];
	M(0, 1) = x[1];
	M(0, 2) = x[2];
	M(0, 3) = 0.0;
	M(1, 0) = y[0];
	M(1, 1) = y[1];
	M(1, 2) = y[2];
	M(1, 3) = 0.0;
	M(2, 0) = z[0];
	M(2, 1) = z[1];
	M(2, 2) = z[2];
	M(2, 3) = 0.0;
	M(3, 0) = 0.0;
	M(3, 1) = 0.0;
	M(3, 2) = 0.0;
	M(3, 3) = 1.0;
#undef M

	if (invert_matrix_general(m, inv))
		glMultMatrixd(inv);

}


int SmoothTrans(double cam[3], double eye[3], double up[3],
				double tocam[3], double toeye[3], double toup[3],
				double mark)
{
	double v[3], d, trans_speed;



	d = 0.0;
	trans_speed = 20.0;

	if (!paused)
		switch (cmode) {
		default:
		case 0:
		case 1:
			break;
		case 2:
		case 3:
			ADDVECTORS(cam, cam, planets[currtarget].vel);
			break;
		}

	SUBVECTORS(v, tocam, cam);
	d += MODULE(v);
	DIVVECTOR(v, v, trans_speed);
	ADDVECTORS(cam, cam, v);

	SUBVECTORS(v, toeye, eye);
	d += MODULE(v);
	DIVVECTOR(v, v, trans_speed);
	ADDVECTORS(eye, eye, v);

	SUBVECTORS(v, toup, up);
	d += MODULE(v);
	DIVVECTOR(v, v, trans_speed);
	ADDVECTORS(up, up, v);

	if (d <= mark)
		return 0;
	else
		return 1;
}

GLubyte *texture_LOD(GLubyte * image, int *width, int *height,
					 int components)
{
	int i, j, k, nw, nh;

	switch (LOD) {
	case LOW:
		nh = *height / 4;
		nw = *width / 4;
		for (j = 0; j < nh; j++)
			for (i = 0; i < nw; i++)
				for (k = 0; k < components; k++)
					image[j * nw * components + i * components + k] =
						image[j * 4 * (*width) * components +
							  i * 4 * components + k];
		*height /= 4;
		*width /= 4;
		break;
	case MEDIUM:
		nh = *height / 2;
		nw = *width / 2;
		for (j = 0; j < nh; j++)
			for (i = 0; i < nw; i++)
				for (k = 0; k < components; k++)
					image[j * nw * components + i * components + k] =
						image[j * 2 * (*width) * components +
							  i * 2 * components + k];
		*height /= 2;
		*width /= 2;
		break;
	case HIGH:
	default:
		break;
	}
	return image;
}

void log(char *message)
{
	if (logfile) {
		fprintf(logfile, "%s", message);
		fflush(logfile);
	}
}

void shutdown(int status)
{
	fclose(logfile);
	exit(status);
}

