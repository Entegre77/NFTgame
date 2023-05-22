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

#ifdef WIN32
typedef void (APIENTRY * PFNGLPOINTPARAMETERFEXTPROC)
 (GLenum pname, GLfloat param);
typedef void (APIENTRY * PFNGLPOINTPARAMETERFVEXTPROC)
 (GLenum pname, const GLfloat * params);
static PFNGLPOINTPARAMETERFEXTPROC wglPointParameterfEXT;
static PFNGLPOINTPARAMETERFVEXTPROC wglPointParameterfvEXT;

void InitPointEXT(void)
{
	wglPointParameterfEXT = (PFNGLPOINTPARAMETERFEXTPROC)
		wglGetProcAddress("glPointParameterfEXT");
	wglPointParameterfvEXT = (PFNGLPOINTPARAMETERFVEXTPROC)
		wglGetProcAddress("glPointParameterfvEXT");
}
#endif

int hasPointEXT;

void CometTail(int body)
{
	int i;
	float a;
	static GLfloat quad[3] = { 0.25, 0.0, 1 / 60.0 };

	if (!hasPointEXT)
		return;

	glEnable(GL_POINT_SMOOTH);
	glPointSize(5.0);
#ifdef WIN32
	wglPointParameterfvEXT(GL_DISTANCE_ATTENUATION_EXT, quad);
#else
#ifdef GL_EXT_point_parameters
	glPointParameterfvEXT(GL_DISTANCE_ATTENUATION_EXT, quad);
#endif
#endif
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBegin(GL_POINTS);

	for (i = 0; i < planets[body].num_particles; i++) {
		a =
			sqrt(1.0 -
				 planets[body].tail[i].dist / planets[body].tail_lenght);
		glColor4f(0.2 + a / 2.0, 0.2 + a / 2.0, 0.2 + a / 2.0, a);
		glVertex3f(planets[body].tail[i].origin[0] +
				   planets[body].tail[i].dir[0] *
				   planets[body].tail[i].dist,
				   planets[body].tail[i].origin[1] +
				   planets[body].tail[i].dir[1] *
				   planets[body].tail[i].dist,
				   planets[body].tail[i].origin[2] +
				   planets[body].tail[i].dir[2] *
				   planets[body].tail[i].dist);
		planets[body].tail[i].dist += 0.01;
		if (planets[body].tail[i].dist > planets[body].tail_lenght)
			planets[body].tail[i].dist = 0.0;
	}
	glEnd();
	glDisable(GL_BLEND);
	if (lighting)
		glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glPointSize(1.0);
}
