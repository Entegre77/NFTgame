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

float radarzoom = 1000.0;
int radar = 0;

void Radar()
{
	int i, inrange = 0;
	char sbuf[80];
	double pos[3], v1[3], angle;
	double t, range;

	glEnable(GL_BLEND);
	glColor4f(0.4, 0.4, 0.4, 0.5);
	glRecti(50, 50, width - 50, height - 50);
	glDisable(GL_BLEND);
	if (height > width)
		range = (double) (width - 100) * radarzoom / 2.0;
	else
		range = (double) (height - 100) * radarzoom / 2.0;

	glPushMatrix();
	glTranslatef(width / 2, height / 2, 0);
	angle = RAD2DEG(atan2(cameye[Z], cameye[X])) - 90.0;
	glRotatef(angle, 0.0, 0.0, 1.0);
	for (i = 0; i < NUMBODIES; i++)
		switch (planets[i].Type) {
		case SATELLITE:
		case ORBITER:
			SUBVECTORS(v1, planets[i].pos, campos);
			t = MODULE(v1);
			if (t > RADIUSSCALE(planets[planets[i].Sat].Radius * 10.0))
				continue;
		case SPACESHIP:
		case ASTEROID:
		case COMET:
		case PLANET:
			pos[X] = (planets[i].pos[X] - campos[X]) / radarzoom;
			pos[Y] = (planets[i].pos[Z] - campos[Z]) / radarzoom;
			pos[Z] = 0.0;
			t = MODULE(pos);
			if (t > (height / 2 - 50))
				continue;
			if (t < 1)
				continue;
			pos[X] = -pos[X];
			pos[Y] = pos[Y];
			pos[Z] = 0.0;
			glColor4fv(planets[i].Color);
			glutprintstring(pos[X], pos[Y], pos[Z], planets[i].Name);
			glBegin(GL_LINE_LOOP);
			glVertex2i(0, 0);
			glVertex2i((int) pos[X], (int) pos[Y]);
			glEnd();
			inrange++;
			break;
		}
	glPopMatrix();
	glColor4f(0.7, 0.7, 0.7, 1.0);
	sprintf(sbuf, "Objects in range: %d", inrange);
	glutprintstring(50, height - 50 - glyphheight, 0, sbuf);
	sprintf(sbuf, "Range (million Km): %.1f", range / 1000.0);
	glutprintstring(50, height - 50 - glyphheight * 2, 0, sbuf);
}
