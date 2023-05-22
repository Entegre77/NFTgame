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
#include "gui.h"
#include <time.h>
#include <sys/timeb.h>

void Help(void), Help2(void), DrawLogo(void);
extern int totaltexsize;
int help = 1, help2 = 0, camseesun;

void OnScreenInfo(void)
{
	time_t t;
	struct tm *tm;
	int i;
	double x, y, z, s, relvel[3], relvelnorm[3], v1[3];
	char sbuf[80];

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	if (slabels) {				/* Star labels on ? */
		glColor3f(0.3, 1.0, 0.3);
		i = 0;
		while (stars[i].magnitude < star_mag) {
			if (i > NUMSTARS)
				break;
			glutprintstring(stars[i].pos[X], stars[i].pos[Y],
							stars[i].pos[Z], stars[i].Name);
			i++;
		}

		glColor3f(1.0, 0.4, 0.4);
		i = 0;
		while (i < num_messier) {
			glutprintstring(messierobjs[i].pos[X], messierobjs[i].pos[Y],
							messierobjs[i].pos[Z], messierobjs[i].Name);
			i++;
		}

	}

	if (ISASPACECRAFT(currtarget))
		SUBVECTORS(relvel, planets[currtarget].vel,
				   planets[currsource].vel);
	else
		SUBVECTORS(relvel, camvel, planets[currsource].vel);

	if (planets[currtarget].Type == SPACESHIP) {
		COPYVECTOR(relvelnorm, relvel);
		NORMALIZE(relvelnorm);
		glColor3f(1.0, 1.0, 0.0);
		if (MODULE(relvelnorm) > 0.0) {
			MULTVECTOR(v1, relvelnorm, UNIVERSERADIUS);
			glutprintstring(v1[X], v1[Y], v1[Z], "X");
		}

		glColor3f(1.0, 0.0, 1.0);
		MULTVECTOR(v1, relvelnorm, -1.0);
		if (MODULE(v1) > 0.0) {
			MULTVECTOR(v1, v1, UNIVERSERADIUS);
			glutprintstring(v1[X], v1[Y], v1[Z], "X");
		}

		glColor3f(0.0, 1.0, 0.0);
		COPYVECTOR(v1, planets[currtarget].dir);
		MULTVECTOR(v1, v1, UNIVERSERADIUS);
		glutprintstring(v1[X], v1[Y], v1[Z], "X");

		glColor3f(1.0, 0.0, 0.0);
		COPYVECTOR(v1, planets[currtarget].grav);
		NORMALIZE(v1);
		MULTVECTOR(v1, v1, UNIVERSERADIUS);
		glutprintstring(v1[X], v1[Y], v1[Z], "X");
	}


	glColor3f(1.0, 1.0, 1.0);
	if (plabels)				/* Planets labels on ? */
		for (i = 0; i < NUMBODIES; i++) {
			if (planets[i].Type >= RING)
				continue;

			/* Distance from planet to camera */
			d =
				DISTANCE(planets[i].pos[X] - campos[X],
						 planets[i].pos[Y] - campos[Y],
						 planets[i].pos[Z] - campos[Z]);

			if (planets[i].Type == SATELLITE)
				if (d >
					(RADIUSSCALE(planets[i].Radius) * UNIVERSERADIUS *
					 0.001 * 60.0 / fov))
					continue;

			x = planets[i].pos[X] - campos[X];
			y =
				planets[i].pos[Y] - campos[Y] -
				RADIUSSCALE(planets[i].Radius);
			z = planets[i].pos[Z] - campos[Z];
			glutprintstring(x, y, z, planets[i].Name);
		}

	if (camseesun)
		LensFlares();


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (camseesun) {
		COPYVECTOR(v1, campos);
		z = MODULE(campos) / 5.0;
		z = CLAMP( AU / z, 0.0 , 0.9);

		NORMALIZE(v1);
		ADDVECTORS(v1, v1, cameye);
		s = MODULE(v1);
		s *= (60.0 / fov);
		if (s < FLAREINTENSITY) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(1.0, 1.0, 0.95, (FLAREINTENSITY - s) / FLAREINTENSITY * z);
			glRecti(0, 0, width, height);
			glDisable(GL_BLEND);
		}
	}


	if (radar)
		Radar();

	if (fonttype == TEXFONT) {
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GEQUAL, 0.5);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		txfBindFontTexture(txf);
	}

	glColor3f(1.0, 1.0, 1.0);

	if (help)
		Help();

	if (help2)
		Help2();

	if (!info) {
		if (lighting)
			glEnable(GL_LIGHTING);
		return;
	}
	t = (int) ((10092.0 + days) * 24.0 * 3600.0);
	tm = localtime(&t);
	strftime(sbuf, 80, "%m / %d / %Y  %X (Local Time)", tm);
	printstring(0, height - glyphheight, 0, sbuf);

	if (!(follow && ISASPACECRAFT(currtarget)))
		switch (cmode) {
		case 0:
			printstring(0, height - 2 * glyphheight, 0, "Free Camera");
			sprintf(sbuf, "%s", planets[currtarget].Name);
			break;
		case 1:
			printstring(0, height - 2 * glyphheight, 0, "Linked Camera");
			sprintf(sbuf, "Tracking %s", planets[currtarget].Name);
			break;
		case 2:
			printstring(0, height - 2 * glyphheight, 0,
						"Body to Body Camera");
			sprintf(sbuf, "%s as seen from %s", planets[currtarget].Name,
					planets[currsource].Name);
			break;
		case 3:
			printstring(0, height - 2 * glyphheight, 0, "Orbiter Camera");
			sprintf(sbuf, "Orbiting around %s", planets[currtarget].Name);
			break;
		default:
			break;
		}

	printstring(0, height - 4 * glyphheight, 0, sbuf);
	if (cmode || (follow && ISASPACECRAFT(currtarget))) {
		SUBVECTORS(v1, planets[currtarget].pos, planets[currsource].pos);
		sprintf(sbuf, "Body distance from %s (Km): %.2f",
				planets[currsource].Name, MODULE(v1) * 1000.0);
		printstring(0, height - 3 * glyphheight, 0, sbuf);
	}

	sprintf(sbuf, "Camera distance from Sun (Km): %.2f",
			DISTANCE(campos[X], campos[Y], campos[Z]) * 1000.0);
	printstring(0, 3 * glyphheight, 0, sbuf);
	sprintf(sbuf, "Time factor: %d sec / iteration", timefactor);
	printstring(0, 2 * glyphheight, 0, sbuf);
	speed = MODULE(relvel);
	s = speed * 1000.0;
	if (s < 10000.0)
		sprintf(sbuf, "Camera speed (Km / sec) rel to %s: %.2f",
				planets[currsource].Name, s);
	else
		sprintf(sbuf, "Camera speed (c) rel to %s: %.2f",
				planets[currsource].Name, s / LIGHTSPEED);
	printstring(0, glyphheight, 0, sbuf);
	if (cmode > 1) {
		sprintf(sbuf, "Zoom factor: %.2f", 10.0 / zoom);
		printstring(0, 0, 0, sbuf);
	}

	sprintf(sbuf, "FOV: %.1f  FPS: %.1f TEX: %.1f", fov, fps,
			totaltexsize / (1024.0 * 1024.0));
	printstring(width - glyphwidth * 32, glyphheight, 0, sbuf);

#ifdef DEBUG
	sprintf(sbuf, "N: %.3f F: %.3f %s", nearclip, farclip,
			planets[nearest].Name);
	printstring(width - glyphwidth * 30, 2 * glyphheight, 0, sbuf);
	for (i = 0; i < objstorender; i++) {
		printstring(width - glyphwidth * 22, (3 + i) * glyphheight, 0,
					planets[renderlist[i].body].Name);
	}
	sprintf(sbuf, "up: (%f) %f %f %f", MODULE(camup), camup[X], camup[Y],
			camup[Z]);
	printstring(0, (4 + i) * glyphheight, 0, sbuf);
	i++;
	sprintf(sbuf, "eye: (%f) %f %f %f", MODULE(cameye), cameye[X],
			cameye[Y], cameye[Z]);
	printstring(0, (4 + i) * glyphheight, 0, sbuf);
	i++;
	sprintf(sbuf, "angle: %f", RAD2DEG(acos(DOTPRODUCT(cameye, camup))));
	printstring(0, (4 + i) * glyphheight, 0, sbuf);
#endif

	if (demomode) {
		printstring(0, 4 * glyphheight, 0, "Demo Mode");
	}
	if (paused) {
		printstring(width - glyphwidth * 8, height - glyphheight, 0,
					"Paused");
	}
	if (joyactive && joyavailable) {
		printstring(width - glyphwidth * 16, height - 2 * glyphheight, 0,
					"Joystick Active");
	}
	if (eventrecord) {
		printstring(width - glyphwidth * 20, height - glyphheight, 0,
					"Recording keystrokes");
	}

	if (logo) {
		if (fonttype != TEXFONT) {
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
		}
		DrawLogo();
	}

	if (lighting)
		glEnable(GL_LIGHTING);
}


void DrawLogo(void)
{
	static float intensity = 1.0;

	glBindTexture(GL_TEXTURE_2D, logotex);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0, 1.0, 1.0, intensity);
	intensity -= 0.002;
	if (intensity <= 0.0) {
		logo = 0;
		intensity = 1.0;
	}
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex2i(width - 64, height - 64);
	glTexCoord2f(1.0, 0.0);
	glVertex2i(width, height - 64);
	glTexCoord2f(1.0, 1.0);
	glVertex2i(width, height);
	glTexCoord2f(0.0, 1.0);
	glVertex2i(width - 64, height);
	glEnd();
}


void Help(void)
{
	int i;

	i = height / 2 + 13 * glyphheight;
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "1: Free / Behind camera mode");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "2: Fixed Target / Top camera mode");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "3: Body to Body / Right camera mode");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "4: Orbiter / Left camera mode");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0,
				"w: Spaceship follow mode (if currtarget is spaceship)");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0,
				"Home/End: Select previous/next target body");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0,
				"SHIFT + Home/End: Selects previous/next source body");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "+/- : Increase/Decrease timefactor *");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "Arrow keys : Camera/Ship heading *  **");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "CTRL + arrow keys : Camera/Ship roll **");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0,
				"Page Up/Down : Increase/decrease speed *  **");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0,
				"CTRL + Page Up/Down : Match current source body speed **");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "z/Z : Increase/decrease zoom factor");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0,
				"Insert: Current target trail on/off (planets only)");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight * 2, 0, "* hold SHIFT for faster operation");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight * 2, 0,
				"** commands only available in Free or Spaceship camera modes");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight * 2, 0, "PRESS H FOR MORE HELP");
}

void Help2(void)
{
	int i;

	i = height / 2 + 13 * glyphheight;
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "a : Atmosphere on/off");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "b : Distant bodies labels on/off");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "B : Stars labels on/off");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "d : Demo mode on/off");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "e : Real Time on/off");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "f : Flat/Smooth shading model");
#ifdef HAVE_GLUI
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "g : Show GUI");
#endif
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "G : Show Logo");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "i : Info (HUD) on/off");
	if (joyavailable) {
		printstring(width / 2 - glyphwidth * 30, i -=
					glyphheight, 0, "j : Joystick control on/off");
	}
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "l : Lighting on/off");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "m/M : Brighter/Fainter stars");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0,
				"n : Place camera near current target planet");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "o/O : Wider/Narrower FOV");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "p : Pause");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "r : Top view map on/off");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "s : Stars on/off");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "S : Screenshot");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "t : Texture on/off");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "u/U : Wider/Narrower map range");
	printstring(width / 2 - glyphwidth * 30, i -=
				glyphheight, 0, "y : Gravity on/off");
}
