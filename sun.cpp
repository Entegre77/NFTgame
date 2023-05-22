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


void SunBillBoard(double size)
{
	double p1[3], p2[3], p3[3], p4[3];
	double alfa, beta, cosalfa, cosbeta, sinalfa, sinbeta;
	double x, y, z;

/* this macro by Sebastien Metrot */
#define ROTATE(V) \
    do {               \
        y = V[Y]*cosbeta + V[Z]*sinbeta; \
        z = -V[Y]*sinbeta + V[Z]*cosbeta;  \
        x = V[X]*cosalfa - z*sinalfa;  \
        z = V[X]*sinalfa + z*cosalfa;  \
        V[X]=x; V[Y]=y; V[Z]=z;         \
    } while (0);


	p1[X] = p4[X] = p1[Y] = p2[Y] = size;
	p2[X] = p3[X] = p3[Y] = p4[Y] = -size;
	p1[Z] = p2[Z] = p3[Z] = p4[Z] = 0.0;

/* BILLBOARDING effect, rotates sun tile so it shows always the same face and
   angle to camera */
	alfa = atan2(campos[Z], campos[X]) - M_PI / 2.0;
	beta =
		atan2(campos[Y],
			  sqrt(campos[X] * campos[X] + campos[Z] * campos[Z]));

	cosalfa = cos(alfa);
	cosbeta = cos(beta);
	sinalfa = sin(alfa);
	sinbeta = sin(beta);

	ROTATE(p1);
	ROTATE(p2);
	ROTATE(p3);
	ROTATE(p4);

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_ONE, GL_ONE);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(p1[X], p1[Y], p1[Z]);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(p2[X], p2[Y], p2[Z]);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(p3[X], p3[Y], p3[Z]);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(p4[X], p4[Y], p4[Z]);
	glEnd();
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}
