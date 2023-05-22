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


/*
    Updates camera position, eye and up vectors acording to current
    camera mode.
    
    Parameters tell the function where to store result, which is useful
    to differentiate normal operation and transitions.
*/
void UpdateCamera(double tcam[3], double teye[3], double tup[3])
{
	int sat;
	double rad, d, o, p, x, y, z, v1[3], v2[3];
	double tocam[3], toeye[3], toup[3];

	switch (cmode) {
	case 0:
		CROSSPRODUCT(v1, camup, cameye);
		NORMALIZE(v1);
		CROSSPRODUCT(camup, cameye, v1);
		NORMALIZE(camup);
		if (abs(camrot[Z]) > 8)
			Rotation(camrot[Z] * 0.0001, cameye, camup);
		if (abs(camrot[X]) > 8)
			Rotation(camrot[X] * 0.0001, camup, cameye);
		if (abs(camrot[Y]) > 8) {
			CROSSPRODUCT(v1, camup, cameye);
			Rotation(camrot[Y] * 0.0001, v1, cameye);
			Rotation(camrot[Y] * 0.0001, v1, camup);
		}
		COPYVECTOR(tcam, campos);
		COPYVECTOR(teye, cameye);
		COPYVECTOR(tup, camup);
		break;
	case 1:
		COPYVECTOR(tocam, campos);
		COPYVECTOR(toup, camup);
		if (smooth_transitions && transition)
			dosmooth = 1;
		SUBVECTORS(toeye, planets[currtarget].pos, campos);
		NORMALIZE(toeye);
		CROSSPRODUCT(v1, toup, toeye);
		CROSSPRODUCT(toup, toeye, v1);
		COPYVECTOR(tcam, tocam);
		COPYVECTOR(teye, toeye);
		COPYVECTOR(tup, toup);
		break;

	case 2:					/* Body to Body Camera, simply places the camera near the current
								   target with current source line view */
		if (planets[currtarget].Type == SATELLITE) {
			/* If current target is a satellite take also into
			   account host planet data */
			sat = currtarget;
			rad = RADIUSSCALE(planets[sat].Radius * zoom);
			d =
				DISTANCE(planets[sat].pos[X] -
						 planets[currsource].pos[X],
						 planets[sat].pos[Y] -
						 planets[currsource].pos[Y],
						 planets[sat].pos[Z] - planets[currsource].pos[Z]);
			if (rad > d)
				rad = d;
			tocam[X] =
				(planets[sat].pos[X] -
				 planets[currsource].pos[X]) / d * (d - rad) +
				planets[currsource].pos[X];
			tocam[Y] =
				(planets[sat].pos[Y] -
				 planets[currsource].pos[Y]) / d * (d - rad) +
				planets[currsource].pos[Y];
			tocam[Z] =
				(planets[sat].pos[Z] -
				 planets[currsource].pos[Z]) / d * (d - rad) +
				planets[currsource].pos[Z];
		} else {
			rad = RADIUSSCALE(planets[currtarget].Radius * zoom);
			d =
				DISTANCE(planets[currtarget].pos[X] -
						 planets[currsource].pos[X],
						 planets[currtarget].pos[Y] -
						 planets[currsource].pos[Y],
						 planets[currtarget].pos[Z] -
						 planets[currsource].pos[Z]);
			if (rad > d)
				rad = d;
			tocam[X] =
				(planets[currtarget].pos[X] -
				 planets[currsource].pos[X]) / d * (d - rad) +
				planets[currsource].pos[X];
			tocam[Y] =
				(planets[currtarget].pos[Y] -
				 planets[currsource].pos[Y]) / d * (d - rad) +
				planets[currsource].pos[Y];
			tocam[Z] =
				(planets[currtarget].pos[Z] -
				 planets[currsource].pos[Z]) / d * (d - rad) +
				planets[currsource].pos[Z];
		}

		SUBVECTORS(toeye, planets[currtarget].pos, tocam);
		NORMALIZE(toeye);
		INITVECTOR(toup, 0.0, 1.0, 0.0);

		CROSSPRODUCT(v1, toup, toeye);
		CROSSPRODUCT(toup, toeye, v1);

		if (smooth_transitions && transition)
			dosmooth = 1;
		COPYVECTOR(tcam, tocam);
		COPYVECTOR(teye, toeye);
		COPYVECTOR(tup, toup);
		break;

	case 3:					/* Orbiter Camera */

		if (currtarget == 0)
			break;

		/* Set Camera in equatorial orbit */
		INITVECTOR(v1, RADIUSSCALE(planets[currtarget].Radius * zoom),
				   0.0, 0.0);
		INITVECTOR(v2, 0.0, 1.0, 0.0);

		/* Rotate camera to simulate orbit */
		Rotation(-days * 10.0, v2, v1);

		/* Rotate orbit plane to match planet tilt */
		o =
			cos(DEG2RAD
				(planets[currtarget].Degrees +
				 planets[planets[currtarget].Sat].Degrees) +
				planets[planets[currtarget].Sat].Inclination);
		p =
			sin(DEG2RAD
				(planets[currtarget].Degrees +
				 planets[planets[currtarget].Sat].Degrees) +
				planets[planets[currtarget].Sat].Inclination);

		tocam[X] = planets[currtarget].pos[X] + v1[X];
		tocam[Y] = planets[currtarget].pos[Y] + v1[Y] * o + v1[Z] * p;
		tocam[Z] = planets[currtarget].pos[Z] + v1[Y] * p + v1[Z] * o;

		/* Do the same with up vector */

		x = 0.0;
		y = 1.0;
		z = 0.0;

		INITVECTOR(toup, 0.0, o, -p);

		SUBVECTORS(toeye, planets[currtarget].pos, tocam);
		NORMALIZE(toeye);

		if (smooth_transitions && transition)
			dosmooth = 1;
		COPYVECTOR(tcam, tocam);
		COPYVECTOR(teye, toeye);
		COPYVECTOR(tup, toup);

		break;

	default:
		break;
	}
}
