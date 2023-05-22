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

static void Ring(double, double, int, planetdata *);
static int inrange(double, double, double);

void DrawRing(int body)
{
	glBindTexture(GL_TEXTURE_2D, planets[body].textures[0]);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Ring(RADIUSSCALE(planets[body].InnerRadius),
		 RADIUSSCALE(planets[body].Radius), slices * 2, &planets[body]);
	if (lighting)
		glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}


static void Ring(double innerRadius, double outerRadius, int slices,
				 planetdata * pdata)
{
	GLfloat da, dr;
	int j = pdata->Sat;
	GLfloat sa, ca;
	GLfloat r1 = innerRadius;
	GLfloat r2;
	GLint s;
	GLfloat a;
	double b;


	if (!pdata->initialized) {
		pdata->vertex_list =
			(vertex_data *) malloc(sizeof(vertex_data) * (slices + 1) * 2);
		pdata->indices =
			(GLuint *) malloc(sizeof(GLuint) * (slices + 1) * 2);

		da = 2.0 * M_PI / slices;
		dr = (outerRadius - innerRadius);
		r2 = r1 + dr;

		for (s = 0; s <= slices; s++) {
			if (s == slices)
				a = 0.0;
			else
				a = s * da;
			sa = sin(a);
			ca = cos(a);
			if (s % 2) {
				pdata->vertex_list[s * 2].u = 1.0;
				pdata->vertex_list[s * 2].v = 0.0;
			} else {
				pdata->vertex_list[s * 2].u = 1.0;
				pdata->vertex_list[s * 2].v = 1.0;
			}

			pdata->vertex_list[s * 2].x = r2 * sa;
			pdata->vertex_list[s * 2].y = r2 * ca;
			pdata->vertex_list[s * 2].z = 0.0;
			pdata->vertex_list[s * 2].nx = 0.0;
			pdata->vertex_list[s * 2].ny = 0.0;
			pdata->vertex_list[s * 2].nz = 1.0;
			pdata->indices[s * 2] = s * 2;

			if (a < (M_PI / 3.0)) {
				pdata->vertex_list[s * 2].r =
					pdata->vertex_list[s * 2].g =
					pdata->vertex_list[s * 2].b = 0.2;
				pdata->vertex_list[s * 2].a = 1.0;
			} else {
				pdata->vertex_list[s * 2].r = pdata->Color[0];
				pdata->vertex_list[s * 2].g = pdata->Color[1];
				pdata->vertex_list[s * 2].b = pdata->Color[2];
				pdata->vertex_list[s * 2].a = 1.0;
			}


			if (s % 2) {
				pdata->vertex_list[s * 2 + 1].u = 0.0;
				pdata->vertex_list[s * 2 + 1].v = 0.0;
			} else {
				pdata->vertex_list[s * 2 + 1].u = 0.0;
				pdata->vertex_list[s * 2 + 1].v = 1.0;
			}

			pdata->vertex_list[s * 2 + 1].x = r1 * sa;
			pdata->vertex_list[s * 2 + 1].y = r1 * ca;
			pdata->vertex_list[s * 2 + 1].z = 0.0;
			pdata->vertex_list[s * 2 + 1].nx = 0.0;
			pdata->vertex_list[s * 2 + 1].ny = 0.0;
			pdata->vertex_list[s * 2 + 1].nz = 1.0;
			pdata->indices[s * 2 + 1] = s * 2 + 1;

			if (a < (M_PI / 3.0)) {
				pdata->vertex_list[s * 2 + 1].r =
					pdata->vertex_list[s * 2 + 1].g =
					pdata->vertex_list[s * 2 + 1].b = 0.2;
				pdata->vertex_list[s * 2 + 1].a = 1.0;
			} else {
				pdata->vertex_list[s * 2 + 1].r = pdata->Color[0];
				pdata->vertex_list[s * 2 + 1].g = pdata->Color[1];
				pdata->vertex_list[s * 2 + 1].b = pdata->Color[2];
				pdata->vertex_list[s * 2 + 1].a = 1.0;
			}
		}
		pdata->initialized = 1;
	} else {
		da = 2.0 * M_PI / slices;
		dr = (outerRadius - innerRadius);
		r2 = r1 + dr;
		for (s = 0; s <= slices; s++) {
			if (s == slices)
				a = 0.0;
			else
				a = s * da;
			sa = sin(a);
			ca = cos(a);
			b =
				DEG2RAD(planets[j].DeltaRotation) +
				atan2(planets[j].pos[Z], planets[j].pos[X]) + M_PI / 3.0;
			while (b > (2 * M_PI))
				b -= (2 * M_PI);
			while (b < 0.0)
				b += (2 * M_PI);
			if (inrange(a, b, b + M_PI / 3.0)) {

				pdata->vertex_list[s * 2].r =
					pdata->vertex_list[s * 2].g =
					pdata->vertex_list[s * 2].b = 0.2;
				pdata->vertex_list[s * 2].a = 1.0;
				pdata->vertex_list[s * 2 + 1].r =
					pdata->vertex_list[s * 2 + 1].g =
					pdata->vertex_list[s * 2 + 1].b = 0.2;
				pdata->vertex_list[s * 2 + 1].a = 1.0;
			} else {
				pdata->vertex_list[s * 2].r = pdata->Color[0];
				pdata->vertex_list[s * 2].g = pdata->Color[1];
				pdata->vertex_list[s * 2].b = pdata->Color[2];
				pdata->vertex_list[s * 2].a = 1.0;
				pdata->vertex_list[s * 2 + 1].r = pdata->Color[0];
				pdata->vertex_list[s * 2 + 1].g = pdata->Color[1];
				pdata->vertex_list[s * 2 + 1].b = pdata->Color[2];
				pdata->vertex_list[s * 2 + 1].a = 1.0;
			}
		}
		glInterleavedArrays(GL_T2F_C4F_N3F_V3F, 0, pdata->vertex_list);
		glDrawElements(GL_QUAD_STRIP, (slices + 1) * 2, GL_UNSIGNED_INT,
					   pdata->indices);
	}
}

static int inrange(double a, double b, double c)
{
	int i = 0;
	double tmp;

	if (c > (2 * M_PI))
		tmp = a + 2 * M_PI;
	else
		tmp = a;
	if ((tmp >= b) && (tmp <= c))
		i = 1;
	if ((a >= b) && (a <= c))
		i = 1;
	return i;
}
