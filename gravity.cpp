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

#define G 6.67259E-11

/*
   Computes gravity force vector for a body located at pos
*/
void Gravity(double *pos, double *force)
{
	int i;
	double d, a, r[3];

	INITVECTOR(force, 0.0, 0.0, 0.0);
	for (i = 0; i < NUMBODIES; i++) {
		if (planets[i].Type > SATELLITE)	/* Discard spaceships, rings, etc... */
			continue;
		SUBVECTORS(r, pos, planets[i].pos);
		d = MODULE(r) * 1.0E6;
		a = (-G * planets[i].Mass) / (d * d * d);
		if (a == 0.0)
			continue;
		MULTVECTOR(r, r, a);
		ADDVECTORS(force, force, r);
	}
}


/* 
    Updates gravity pull and orientation for spaceships and orbiters
*/
void FeelTheGravity(void)
{
	double v1[3], v2[3], a;
	int i;


	for (i = 1; i < NUMBODIES; i++) {

		if (gravity)
			switch (planets[i].Type) {
			case SPACESHIP:
				COPYVECTOR(planets[i].prevel, planets[i].vel);
				Gravity((double *) planets[i].pos,
						(double *) planets[i].grav);
				COPYVECTOR(v1, planets[i].vel);
				COPYVECTOR(v2, planets[i].grav);
				ADDVECTORS(planets[i].vel, planets[i].vel,
						   planets[i].grav);
				MULTVECTOR(v1, v1, fabs((double) timefactor));
				MULTVECTOR(v2, v2, fabs((double) timefactor));
				ADDVECTORS(v1, v1, v2);

				ADDVECTORS(planets[i].pos, planets[i].pos, v1);

				a = MODULE(planets[i].prevel) * MODULE(planets[i].vel);
				if (a > 0.0) {
					a =
						acos(DOTPRODUCT
							 (planets[i].prevel, planets[i].vel) / a);
					if (a > 0.0) {
						CROSSPRODUCT(v1, planets[i].prevel,
									 planets[i].vel);
						Rotation(-a, v1, planets[i].dir);
						Rotation(-a, v1, planets[i].up);
						NORMALIZE(planets[i].dir);
						NORMALIZE(planets[i].up);
					}
				}
				break;
			case ORBITER:
				a = MODULE(planets[i].prevel) * MODULE(planets[i].vel);
				if (a > 0.0) {
					a = acos(DOTPRODUCT(planets[i].prevel, planets[i].vel)
							 / a);
					if (a > 0.0) {
						CROSSPRODUCT(v1, planets[i].prevel,
									 planets[i].vel);
						Rotation(-a, v1, planets[i].dir);
						Rotation(-a, v1, planets[i].up);
						NORMALIZE(planets[i].dir);
						NORMALIZE(planets[i].up);
					}
				}
				break;
			default:
				continue;
			}

		if (abs(planets[i].rot[Z]) > 8)
			Rotation(planets[i].rot[Z] * 0.0001, planets[i].dir,
					 planets[i].up);
		if (abs(planets[i].rot[X]) > 8)
			Rotation(planets[i].rot[X] * 0.0001, planets[i].up,
					 planets[i].dir);
		if (abs(planets[i].rot[Y]) > 8) {
			CROSSPRODUCT(v1, planets[i].up, planets[i].dir);
			Rotation(planets[i].rot[Y] * 0.0001, v1, planets[i].dir);
			Rotation(planets[i].rot[Y] * 0.0001, v1, planets[i].up);
		}


		if (follow && (i == currtarget)) {
			switch (cmode) {
			case 0:
				CROSSPRODUCT(v2, planets[i].up, planets[i].dir);
				COPYVECTOR(v1, planets[currtarget].dir);
				Rotation(-0.1, v2, v1);
				SUBVECTORS(v1, v1, cameye);
				DIVVECTOR(v1, v1, 50.0);
				ADDVECTORS(v1, v1, cameye);
				NORMALIZE(v1);
				MULTVECTOR(v1, v1,
						   RADIUSSCALE(planets[currtarget].Radius * zoom));
				SUBVECTORS(campos, planets[currtarget].pos, v1);
				SUBVECTORS(cameye, planets[currtarget].pos, campos);
				NORMALIZE(cameye);
				COPYVECTOR(camup, planets[currtarget].up);
				break;
			case 1:
				COPYVECTOR(campos, planets[currtarget].pos);
				MULTVECTOR(v1, planets[currtarget].up,
						   RADIUSSCALE(planets[currtarget].Radius * zoom));
				ADDVECTORS(campos, campos, v1);
				COPYVECTOR(camup, planets[currtarget].dir);
				COPYVECTOR(cameye, planets[currtarget].up);
				MULTVECTOR(cameye, cameye, -1.0);
				break;
			case 2:
				CROSSPRODUCT(v1, planets[currtarget].dir,
							 planets[currtarget].up);
				COPYVECTOR(cameye, v1);
				MULTVECTOR(cameye, cameye, -1.0);
				MULTVECTOR(v1, v1,
						   RADIUSSCALE(planets[currtarget].Radius * zoom));
				ADDVECTORS(campos, planets[currtarget].pos, v1);
				COPYVECTOR(camup, planets[currtarget].up);
				break;
			case 3:
				CROSSPRODUCT(v1, planets[currtarget].up,
							 planets[currtarget].dir);
				COPYVECTOR(cameye, v1);
				MULTVECTOR(cameye, cameye, -1.0);
				MULTVECTOR(v1, v1,
						   RADIUSSCALE(planets[currtarget].Radius * zoom));
				ADDVECTORS(campos, planets[currtarget].pos, v1);
				COPYVECTOR(camup, planets[currtarget].up);
				break;
			default:
				break;
			}
		}
	}

	speed = MODULE(planets[currtarget].vel) * (double) timefactor;
}
