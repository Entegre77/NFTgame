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


void Key(unsigned char key, int x, int y)
{
	double v1[3], v2[3], v3[3];
	char sbuf[80];


	if (eventrecord) {
		events[numevents].key = key;
		events[numevents].special = 0;
		events[numevents].when = frames;
		numevents++;
	}

	switch (key) {
	case 27:					/* ESC ... bye bye */
		sec = (glutGet((GLenum) GLUT_ELAPSED_TIME) - sec) / 1000.0;
#ifdef WIN32
		if (fullscreen) {
			glutLeaveGameMode();
			ShowCursor(TRUE);
		}
#else
		printf("\n%.2f frames/sec\n\n", (float) frames / sec);
		glutSetCursor(GLUT_CURSOR_INHERIT);
#endif
//		DumpEvents();
		shutdown(0);
		break;
	case '1':
		cmode = 0;
		cameye[Y] = 0.0;
#ifdef HAVE_GLUI
		if (!fullscreeninGUI)
			update_cmode();
#endif
		break;
	case '2':
		cmode = 1;
		transition = 1;
#ifdef HAVE_GLUI
		if (!fullscreeninGUI)
			update_cmode();
#endif
		break;
	case '3':
		cmode = 2;
		transition = 1;
		INITVECTOR(camup, 0.0,
				   cos(DEG2RAD(planets[currsource].Degrees)),
				   -sin(DEG2RAD(planets[currsource].Degrees)));
#ifdef HAVE_GLUI
		if (!fullscreeninGUI)
			update_cmode();
#endif
		break;
	case '4':
		cmode = 3;
		transition = 1;
#ifdef HAVE_GLUI
		if (!fullscreeninGUI)
			update_cmode();
#endif
		break;
	case 'p':
		paused = !paused;
		break;
	case 'f':
		smodel = (smodel == GL_FLAT) ? GL_SMOOTH : GL_FLAT;
		glShadeModel(smodel);
		break;
	case 'o':
		fov += 0.5;
		break;
	case 'O':
		fov -= 0.5;
		break;
	case 't':
		texture = !texture;
		break;
	case 'e':
		realtime = !realtime;
		break;
	case 'E':
		eventrecord = !eventrecord;
		events[numevents].key = 0;
		events[numevents].special = 0;
		events[numevents].when = frames;
		eventrecordstart = days;
		COPYVECTOR(eventstartcampos, campos);
		COPYVECTOR(eventstartcameye, cameye);
		COPYVECTOR(eventstartcamup, camup);
		numevents++;
		break;
	case 's':
		drawstars = !drawstars;
		break;
	case 'S':
		sprintf(sbuf, "sshot%d.jpg", frames);
		ScreenShot(sbuf);
		break;
	case 'm':
		glDeleteLists(StarsDL, 1);
		star_mag += 0.1;
		InitStars(star_mag, 0);
		break;
	case 'M':
		glDeleteLists(StarsDL, 1);
		star_mag -= 0.1;
		InitStars(star_mag, 0);
		break;
	case 'n':					/* Set camera nearby the current target */
		if (currtarget == 0)
			break;
		if (planets[currtarget].Sat) {	/* If body is a satellite */
			MULTVECTOR(v1, planets[currtarget].vel, fps * 5.0);
			ADDVECTORS(v1, planets[currtarget].pos, v1);
			MULTVECTOR(v2, planets[planets[currtarget].Sat].vel,
					   fps * 5.0);
			ADDVECTORS(v2, planets[planets[currtarget].Sat].pos, v2);
			SUBVECTORS(v3, v1, v2);
			NORMALIZE(v3);
			COPYVECTOR(v2, v3);
			MULTVECTOR(v3, v3,
					   RADIUSSCALE(planets[currtarget].Radius * 8.0) /
					   tan(DEG2RAD(fov)));
			ADDVECTORS(campos, v1, v3);
			SUBVECTORS(cameye, planets[currtarget].pos, campos);
			NORMALIZE(cameye);
			CROSSPRODUCT(camup, cameye, v2);
			NORMALIZE(camup);
		} else {
			MULTVECTOR(v1, planets[currtarget].vel, fps * 5.0);
			ADDVECTORS(v1, planets[currtarget].pos, v1);
			COPYVECTOR(v2, v1);
			NORMALIZE(v2);
			MULTVECTOR(v2, v2,
					   RADIUSSCALE(planets[currtarget].Radius * 8.0) /
					   tan(DEG2RAD(fov)));
			SUBVECTORS(campos, v1, v2);
			SUBVECTORS(cameye, planets[currtarget].pos, campos);
			NORMALIZE(cameye);
			CROSSPRODUCT(camup, cameye, v2);
			NORMALIZE(camup);
		}
		camup[Y] = fabs(camup[Y]);
		dosmooth = 0;
		break;
	case 'h':
		help = !help;
		if (help2)
			help2 = 0;
		break;
	case 'H':
		help2 = !help2;
		if (help)
			help = 0;
		break;
	case 'd':
		demomode = !demomode;
		break;
	case 'l':
		lighting = !lighting;
		break;
	case 'b':
		plabels = !plabels;
		break;
	case 'B':
		slabels = !slabels;
		break;
	case 'i':
		info = !info;
		break;
	case 'j':
		joyactive = !joyactive;
		break;
	case '+':
		if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
			timefactor *= 2;
		else
			timefactor += 1;	/* one second/iteration */
		break;
	case '-':
		if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
			timefactor /= 2;
		else
			timefactor -= 1;	/* one second/iteration */
		break;
	case 'z':
		if (zoom < 100.0)
			zoom += 1.0;
		break;
	case 'Z':
		if (zoom > 2.0)
			zoom -= 1.0;
		break;
	case 'a':
		atmospheres = !atmospheres;
		break;
	case 'r':
		radar = !radar;
		break;
	case 'u':
		radarzoom *= 1.1;
		break;
	case 'U':
		radarzoom /= 1.1;
		break;
	case 'w':
		follow = !follow;
		break;
	case 'y':
		gravity = !gravity;
		break;
	case 'G':
		logo = 1;
		break;
#ifdef HAVE_GLUI
	case 'g':
		if (!fullscreen) {
			glui_rwin->show();
			fullscreeninGUI = 0;
			update_cmode();
		}
		break;
#endif
	default:
		help = 1;
		break;
	}
#ifdef HAVE_GLUI
	if (!fullscreeninGUI) {
		if (glui_optwin)
			glui_optwin->sync_live();
		glui_rwin->sync_live();
	}
#endif
}


void Special(int k, int x, int y)
{
	double rot, speeddiv, v1[3];
	int m;
	char sbuf[80];

	m = glutGetModifiers();
	if (m & GLUT_ACTIVE_SHIFT) {
		speeddiv = 1.0;
		rot = 0.02;
	} else {
		rot = 0.006;
		speeddiv = 1000.0;
	}

	if (eventrecord) {
		events[numevents].key = 0;
		events[numevents].special = k;
		events[numevents].when = frames;
		numevents++;
	}

	switch (k) {
	case 's':
		if (m & GLUT_ACTIVE_CTRL) {
			sprintf(sbuf, "frame%d.ppm", frames);
			ScreenShot(sbuf);
		}
		break;
	case GLUT_KEY_INSERT:
		if (planets[currtarget].Type <= ASTEROID)
			planets[currtarget].TrailEnabled =
				!planets[currtarget].TrailEnabled;
		break;
	case GLUT_KEY_LEFT:
		if (m & GLUT_ACTIVE_CTRL) {
			if (ISASPACECRAFT(currtarget)) {
				planets[currtarget].rot[Z] += 16;
				CLAMP_SELF(planets[currtarget].rot[Z], -64, 64);
			} else if (!cmode) {
				camrot[Z] += 16;
				CLAMP_SELF(camrot[Z], -64, 64);
			}
		} else {
			if (ISASPACECRAFT(currtarget)) {
				planets[currtarget].rot[X] -= 16;
				CLAMP_SELF(planets[currtarget].rot[X], -64, 64);
			} else if (!cmode) {
				camrot[X] -= 16;
				CLAMP_SELF(camrot[X], -64, 64);
			}
		}
		break;
	case GLUT_KEY_RIGHT:
		if (m & GLUT_ACTIVE_CTRL) {
			if (ISASPACECRAFT(currtarget)) {
				planets[currtarget].rot[Z] -= 16;
				CLAMP_SELF(planets[currtarget].rot[Z], -64, 64);
			} else if (!cmode) {
				camrot[Z] -= 16;
				CLAMP_SELF(camrot[Z], -64, 64);
			}
		} else {
			if (ISASPACECRAFT(currtarget)) {
				planets[currtarget].rot[X] += 16;
				CLAMP_SELF(planets[currtarget].rot[X], -64, 64);
			} else if (!cmode) {
				camrot[X] += 16;
				CLAMP_SELF(camrot[X], -64, 64);
			}
		}
		break;
		break;
	case GLUT_KEY_DOWN:
		if (ISASPACECRAFT(currtarget)) {
			planets[currtarget].rot[Y] += 16;
			CLAMP_SELF(planets[currtarget].rot[Y], -64, 64);
		} else if (!cmode) {
			camrot[Y] += 16;
			CLAMP_SELF(camrot[Y], -64, 64);
		}
		break;
	case GLUT_KEY_UP:
		if (ISASPACECRAFT(currtarget)) {
			planets[currtarget].rot[Y] -= 16;
			CLAMP_SELF(planets[currtarget].rot[Y], -64, 64);
		} else if (!cmode) {
			camrot[Y] -= 16;
			CLAMP_SELF(camrot[Y], -64, 64);
		}
		break;
	case GLUT_KEY_PAGE_UP:
		switch (planets[currtarget].Type) {
		case SPACESHIP:
		case ORBITER:
			if (m & GLUT_ACTIVE_CTRL)
				COPYVECTOR(planets[currtarget].vel,
						   planets[currsource].vel);
			else {
				DIVVECTOR(v1, planets[currtarget].dir, speeddiv);
				ADDVECTORS(planets[currtarget].vel,
						   planets[currtarget].vel, v1);
			}
			break;
		default:
			if (m & GLUT_ACTIVE_CTRL)
				COPYVECTOR(camvel, planets[currsource].vel);
			else {
				DIVVECTOR(v1, cameye, speeddiv);
				ADDVECTORS(camvel, camvel, v1);
			}
			break;
		}
		break;
	case GLUT_KEY_PAGE_DOWN:
		switch (planets[currtarget].Type) {
		case SPACESHIP:
		case ORBITER:
			if (m & GLUT_ACTIVE_CTRL)
				COPYVECTOR(planets[currtarget].vel,
						   planets[currsource].vel);
			else {
				DIVVECTOR(v1, planets[currtarget].dir, speeddiv);
				SUBVECTORS(planets[currtarget].vel,
						   planets[currtarget].vel, v1);
			}
			break;
		default:
			if (m & GLUT_ACTIVE_CTRL)
				COPYVECTOR(camvel, planets[currsource].vel);
			else {
				DIVVECTOR(v1, cameye, speeddiv);
				SUBVECTORS(camvel, camvel, v1);
			}
			break;
		}
		break;
	case GLUT_KEY_HOME:
		if (m & GLUT_ACTIVE_SHIFT) {
			currsource--;
			while (planets[currsource].Type >= RING)
				currsource--;
			if (currsource < 0) {
				currsource = NUMBODIES - 1;
				while (planets[currsource].Type >= RING)
					currsource--;
			}
			if (cmode == 2)
				if (currsource == currtarget) {
					currsource--;
					while (planets[currsource].Type >= RING)
						currsource--;
					if (currsource < 0) {
						currsource = NUMBODIES - 1;
						while (planets[currsource].Type >= RING)
							currsource--;
					}
				}
		} else {
			currtarget--;
			while (planets[currtarget].Type >= RING)
				currtarget--;
			if (currtarget < 0) {
				currtarget = NUMBODIES - 1;
				while (planets[currtarget].Type >= RING)
					currtarget--;
			}
			if (cmode == 2)
				if (currsource == currtarget) {
					currtarget--;
					while (planets[currtarget].Type >= RING)
						currtarget--;
					if (currtarget < 0) {
						currtarget = NUMBODIES - 1;
						while (planets[currtarget].Type >= RING)
							currtarget--;
					}
				}
		}
		transition = 1;
		break;
	case GLUT_KEY_END:
		if (m & GLUT_ACTIVE_SHIFT) {
			currsource = (currsource + 1) % NUMBODIES;
			while (planets[currsource].Type >= RING)
				currsource = (currsource + 1) % NUMBODIES;
			if (cmode == 2)
				if (currsource == currtarget) {
					currsource = (currsource + 1) % NUMBODIES;
					while (planets[currsource].Type >= RING)
						currsource = (currsource + 1) % NUMBODIES;
				}
		} else {
			currtarget = (currtarget + 1) % NUMBODIES;
			while (planets[currtarget].Type >= RING)
				currtarget = (currtarget + 1) % NUMBODIES;
			if (cmode == 2)
				if (currtarget == currsource) {
					currtarget = (currtarget + 1) % NUMBODIES;
					while (planets[currtarget].Type >= RING)
						currtarget = (currtarget + 1) % NUMBODIES;
				}
		}
		transition = 1;
		break;
	}


}
