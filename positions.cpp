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


/*

	Calculate planets positions in a given date.
	
	The main algorithm uses elements dated Julian 2450680.5 (8/20/1997) so 
	it's quite accurate for dates near this.
	
	To be improved in the future ...
	
	Keith Burnett's planet positioning algorithm used with persmission.
	
*/

#include "ou.h"

static double SolveKepler(double, double);

static double SolveKepler(double M, double ec)
{
	static double E, d, delta;

	E = M;
	d = E - ec * sin(E) - M;
	while (fabs(d) > 1.0E-8) {
		delta = d / (1.0L - ec * cos(E));
		E = E - delta;
		d = E - ec * sin(E) - M;
	}
	return E;
}


/* 
    Based on Keith Burnett's QBASIC code found here:
	http://www.xylem.demon.co.uk/kepler/
*/
void UpdatePositions(double days, int num)
{
	static int j, k;
	static double e, M, E, r, v, o, p, i, x, y, z, l, b;
	static double oldpos[3], axis[3];

	if (!num) {
		j = 1;
		k = NUMBODIES;
	} else {
		j = num;
		k = j + 1;
	}

	for (j = 1; j < k; j++) {
		if (planets[j].Type == SPACESHIP) {
			ADDVECTORS(planets[j].pos, planets[j].pos, planets[j].vel);
			continue;
		}
		if (planets[j].Type >= RING) {
			if (planets[j].Type == ATMOSPHERE) {
				i =
					atan2(planets[planets[j].Sat].pos[Z],
						  planets[planets[j].Sat].pos[X]) * 180.0 / M_PI;
				e = planets[j].Rotation * (days / 360.252502);	/* convert earth days to local */
				planets[j].DeltaRotation =
					(e - floor(e)) * 360.0 - i + 85.0;
			}
			continue;
		}

		COPYVECTOR(oldpos, planets[j].pos);

		if (planets[j].CustomFunction) {
			switch (planets[j].CustomFunction) {
			case 1:
				MercuryPos(days + 2450680.5, &l, &b, &r);
				break;
			case 2:
				VenusPos(days + 2450680.5, &l, &b, &r);
				break;
			case 3:
				EarthPos(days + 2450680.5, &l, &b, &r);
				break;
			case 4:
				MarsPos(days + 2450680.5, &l, &b, &r);
				break;
			case 5:
				JupiterPos(days + 2450680.5, &l, &b, &r);
				break;
			case 6:
				SaturnPos(days + 2450680.5, &l, &b, &r);
				break;
			case 7:
				UranusPos(days + 2450680.5, &l, &b, &r);
				break;
			case 8:
				NeptunePos(days + 2450680.5, &l, &b, &r);
				break;
			case 9:
				PlutoPos(days + 2450680.5, &l, &b, &r);
				break;
			case 10:
				MoonPos(days + 2450680.5, &l, &b, &r);
				break;
			}
			if (planets[j].Type != SATELLITE)
				r = DISTCORRECTION(r);
			INITVECTOR(planets[j].pos, r * cos(b) * sin(l),
					   r * cos(b) * sin(b), r * cos(b) * cos(l));
		} else {
			e = planets[j].Eccentricity;
			M =
				planets[j].DailyMotion * days + planets[j].MeanLongitude -
				planets[j].Perihelion;
			E = SolveKepler(M, e);
			r = planets[j].MeanDistance * (1.0 - e * cos(E));
			v = 2.0L * atan(sqrt((1.0L + e) / (1.0L - e)) * tan(E / 2.0L));
			o = planets[j].AscendingNode;
			p = planets[j].Perihelion;
			i = planets[j].Inclination;
			INITVECTOR(planets[j].pos,
					   r * (cos(o) * cos(v + p - o) -
							sin(o) * sin(v + p - o) * cos(i)),
					   r * (sin(v + p - o) * sin(i)),
					   -r * (sin(o) * cos(v + p - o) +
							 cos(o) * sin(v + p - o) * cos(i)));
		}

		if (planets[j].Sat) {
			/* satellites coords are expressed in host planet radii, convert 
			 * to global coord ... similar to RADIUSSCALE macro */

			MULTVECTOR(planets[j].pos, planets[j].pos,
					   RADIUSSCALE(planets[planets[j].Sat].Radius));

			if (!planets[j].CustomFunction) {
				/* Rotate satellite to match host planet tilt + satellite orbit
				 * tilt. We could do this with glRotate when drawing objects
				 * but then we don't know real satellite coords so it's
				 * impossible to point the camera accurately */
				i =
					-atan2(planets[planets[j].Sat].pos[X],
						   planets[planets[j].Sat].pos[Z]);
				o = cos(i);
				p = sin(i);
				x = planets[j].pos[X] * o + planets[j].pos[Z] * p;
				y = planets[j].pos[Y];
				z = -planets[j].pos[X] * p + planets[j].pos[Z] * o;
				o =
					cos(DEG2RAD(planets[planets[j].Sat].Degrees) +
						planets[j].Inclination);
				p =
					sin(DEG2RAD(planets[planets[j].Sat].Degrees) +
						planets[j].Inclination);
				COPYVECTOR(planets[j].pos, planets[planets[j].Sat].pos);
				planets[j].pos[X] += x;
				planets[j].pos[Y] += y * o + z * p;
				planets[j].pos[Z] += z * o - y * p;
			} else {
				/* Customs functions return always corrected values */
				ADDVECTORS(planets[j].pos, planets[j].pos,
						   planets[planets[j].Sat].pos);
			}
		}

		/* Rotate body. Due to the rendering process (where every body is moved
		 * from origin to its x,y,z position) it's neccesary for us to correct
		 * rotation angle so we undo the implicit rotation in that translation */

		if (planets[j].Type == ASTEROID) {
			INITVECTOR(axis, planets[j].xflat, planets[j].yflat,
					   planets[j].zflat);
			Rotation(DEG2RAD
					 (planets[j].Rotation * timefactor / SECS_A_DAY), axis,
					 planets[j].dir);
			Rotation(DEG2RAD
					 (planets[j].Rotation * timefactor / SECS_A_DAY), axis,
					 planets[j].up);
		} else {
			i = atan2(planets[j].pos[Z], planets[j].pos[X]) * 180.0 / M_PI;
			e = planets[j].Rotation * (days / 360.252502);	/* convert earth days to local */
			planets[j].DeltaRotation = (e - floor(e)) * 360.0 - i + 85.0;
		}


		if (planets[j].Type <= ORBITER) {
			COPYVECTOR(planets[j].prevel, planets[j].vel);
			SUBVECTORS(planets[j].vel, planets[j].pos, oldpos);
		}
	}
}
