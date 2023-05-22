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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <setjmp.h>
#include "ou.h"
#include "gui.h"


double campos[3] = { 0.0, 0.0, 1.0 }, cameye[3] = {
0.0, 0.0, -1.0}, camup[3] = {
0.0, 1.0, 0.0};
int camrot[3] = { 0, 0, 0 };
double tocam[3] = { 0.0, 0.0, 1.0 }, toeye[3] = {
0.0, 0.0, -1.0}, toup[3] = {
0.0, 1.0, 0.0};
double camvel[3] = { 0.0, 0.0, 0.0 };
float fov = 60.0, fps, sec, star_mag = 3.0, speed = 0.0, d, zoom =
	10.0, aratio;
float nearclip = 0.001, farclip = UNIVERSERADIUS * 2.0;
int frames = 0, paused = 0, cmode = 1, currsource = 0, currtarget =
	3, nearest;
int demomode = 1, demostep = 15, bench = 0, plabels = 0, slabels =
	0, init = 0;
int fullscreen = 0, info = 1, realtime = 0, jpeg_quality =
	90, atmospheres = 1;
char game_string[20] = "640x480:16";
extern int joyavailable, joyactive;
int follow = 0, smooth_transitions = 1, dosmooth = 0, transition = 0;
void Camera(void), Display(void);
rlist renderlist[MAXBODIES];
int objstorender, main_window;
TexFont *txf = NULL;
int fonttype = TEXFONT, glyphheight, glyphwidth;
FILE *logfile = NULL;
int totaltexsize = 0;

void CheckError(void)
{
	GLenum error = glGetError();
	if (error) {
		char *err = (char *) gluErrorString(error);
		error(err);
		shutdown(1);
	}
}



void Reshape(int x, int y)
{
	width = x;
	height = y;
	aratio = sqrt(x * x + y * y) / y;
	GUI_Viewport(&width, &height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(CLAMP_SELF(fov, 0.5, 120.0), width / (double) height,
				   nearclip, farclip);
	glMatrixMode(GL_MODELVIEW);
}


/*
   This function finds which bodies need to be renderered during next Display
   call. It also sets up the near and far clipping planes (a hack to reduce
   precision loss in the z-buffer)
*/
void BuildRenderList(void)
{
	double a, b, c, v1[3];
	int i, j, n = 0;

	nearclip = farclip = UNIVERSERADIUS * 4.0;

	for (i = 1; i < NUMBODIES; i++) {

		if (!atmospheres)
			if (planets[i].Type == ATMOSPHERE)
				continue;

		if ((planets[i].Type == RING) || (planets[i].Type == ATMOSPHERE))
			j = planets[i].Sat;
		else
			j = i;

		SUBVECTORS(v1, planets[j].pos, campos);
		d = MODULE(v1);			/* Distance from camera to body */
		c = d / RADIUSSCALE(planets[i].Radius);	/* Apply a distance/radius factor */

		if (c > (UNIVERSERADIUS * 0.001))
			continue;			/* If it's too far to be visible discard it */


		NORMALIZE(v1);
		a = DOTPRODUCT(v1, cameye);
		a = acos(CLAMP(a, -1.0, 1.0));
		/* a stores angle between eye vector and object */
		b = atan(RADIUSSCALE(planets[i].Radius) / d);
		c = fabs(a) - fabs(b);
		if (c > DEG2RAD(aratio * 90.0 / 2.0))
			continue;
		c = fabs(cos(a) * d);
		a = c - RADIUSSCALE(planets[i].Radius) - 0.001;

		if (planets[i].Type == COMET)
			a -= planets[i].tail_lenght;

		if (a < 0.001)
			a = 0.001;

		if (a < nearclip) {
			nearclip = a;
			nearest = i;
		}

		renderlist[n].body = i;
		renderlist[n++].dist = a;
	}
	objstorender = n;
	if (n == 0)
		nearclip = UNIVERSERADIUS * 0.0001 * 60.0 / fov;
	a =
		UNIVERSERADIUS *
		cos(DEG2RAD((aratio > 1.0 ? aratio : 1.0) * fov / 2.0));
	if (nearclip > a)
		nearclip = a;			/* Otherwise stars wouldn't be drawn */

	Reshape(width, height);
}


void Idle(void)
{
	double rad, tmp[3];
#ifdef WIN32
	struct _timeb currtime;
	char sbuf[80];
#else
	struct timeb currtime;
#endif

	if (glutGetWindow() != main_window)
		glutSetWindow(main_window);

	if (!init) {
		init = 1;
		width = glutGet((GLenum) GLUT_WINDOW_WIDTH);
		height = glutGet((GLenum) GLUT_WINDOW_HEIGHT);
		Reshape(width, height);	/* Make sure Reshape is called */
		Init();
		if (bench)
			days = 1000.0;
		UpdatePositions(days, 0);
		while (planets[currtarget].Type >= RING)
			currtarget = (currtarget + 1) % NUMBODIES;
		while (planets[currsource].Type >= RING)
			currsource = (currsource + 1) % NUMBODIES;
		sec = glutGet((GLenum) GLUT_ELAPSED_TIME);
		glutDisplayFunc(Display);
#ifndef HAVE_GLUI
		glutKeyboardFunc(Key);
		glutSpecialFunc(Special);
		glutMouseFunc(Mouse);
#else
		if (fullscreen) {
			glutKeyboardFunc(Key);
			glutSpecialFunc(Special);
			glutMouseFunc(Mouse);
		}
#endif
		glutMotionFunc(Motion);
		glutTimerFunc(demostep * 1000, TimerFunc, 0);
		glutTimerFunc(10000, TimerFunc, 1);
		LOG("Initializing GUI", GUI_Init());
		Key('n', 0, 0);
#ifdef HAVE_XAUDIO
		MP3_InitPlayer();
#endif
//		AutoEvents();
	}
#ifdef HAVE_GLUI
/* GLUI 2.0 doesn't support limits for translation controls */
	CLAMP_SELF(zoom, 2.0, 100.0);
#endif

	if (bench & (frames == 1000)) {
		sec = (glutGet((GLenum) GLUT_ELAPSED_TIME) - sec) / 1000.0;
#ifdef WIN32
		if (fullscreen)
			glutLeaveGameMode();
		ShowCursor(TRUE);
		sprintf(sbuf, "\n%.2f frames/sec\n\n", (float) frames / sec);
		MessageBox(NULL, sbuf, "Overall Performance", MB_OK);
#else
		printf("\n%.2f frames/sec\n\n", (float) frames / sec);
		glutSetCursor(GLUT_CURSOR_INHERIT);
#endif

		shutdown(0);
	}


	if (!(frames % 50))
		fps = 50.0 / gettime();



	if (!paused) {
		/* Update positions to Julian day "days" */
		days += (timefactor / SECS_A_DAY);
		UpdatePositions(days, 0);	/* 0 tells UpdatePositions to process all bodies */
	}


	if (realtime) {
#ifdef WIN32
		_ftime(&currtime);		/* we need milliseconds to avoid jerkiness */
#else
		ftime(&currtime);		/* we need milliseconds to avoid jerkiness */
#endif
		days = (time(NULL) + currtime.millitm / 1000.0) / 3600.0 / 24.0 - 10092.0;	/* days = NOW */
	}

	if (!paused)
		FeelTheGravity();


	if ((!follow) || !ISASPACECRAFT(currtarget)) {
		if (smooth_transitions && transition)
			UpdateCamera(tocam, toeye, toup);
		else
			UpdateCamera(campos, cameye, camup);


		if (transition && dosmooth) {
			rad = RADIUSSCALE(planets[currtarget].Radius) / 10.0;
			CLAMP_SELF(rad, 0.0, 0.001);
			if (!SmoothTrans(campos, cameye, camup, tocam, toeye, toup, rad)) {
				dosmooth = 0;
				transition = 0;
			}
			NORMALIZE(cameye);
			NORMALIZE(camup);
			CROSSPRODUCT(tmp, cameye, camup);
			CROSSPRODUCT(camup, tmp, cameye);
		}

		if (!cmode)
			ADDVECTORS(campos, campos, camvel);
	}

	if (joyactive && joyavailable)
		if (ISASPACECRAFT(currtarget))
			joystick(planets[currtarget].rot, planets[currtarget].dir,
					 planets[currtarget].vel);
		else if (!cmode)
			joystick(camrot, cameye, camvel);



	BuildRenderList();

#ifdef HAVE_GLUI
	if (!fullscreeninGUI)
		glui_rwin->sync_live();
	if (rebuildStarsDL) {
		rebuildStarsDL = 0;
		glDeleteLists(StarsDL, 1);
		InitStars(star_mag, 0);
	}
#endif
	if (init) {
		glutSetWindow(main_window);
		glutPostRedisplay();
	}
//	if (!eventrecord)
//		NewEvents();
#ifdef HAVE_XAUDIO
	MP3_FlushMessages();
#endif
}


/* Stand alone function for future enhancements */
void Camera(void)
{
	gluLookAt(0.0, 0.0, 0.0, cameye[X], cameye[Y], cameye[Z], camup[X],
			  camup[Y], camup[Z]);
}


void Display(void)
{
	double a, c;
	int i, j, n, ondarkside = 0;
	double v[3];
	extern int camseesun;



	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	Camera();

	glDepthMask(GL_FALSE);		/* A workaround for the Mesa 3.1beta2 bug */
	glDisable(GL_DEPTH_TEST);
	glAlphaFunc(GL_GEQUAL, 0.0);

	totaltexsize = 0;

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	if (milkyway) {
		glPushMatrix();
		glCallList(MilkyWayDL);
		glPopMatrix();
		totaltexsize += milkyway_texsize;
	}
	if (messier) {
		glCallList(MessierDL);
		totaltexsize += messier_texsize;
	}
	if (drawstars) {
		glCallList(StarsDL);
	}

	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslated(-campos[X], -campos[Y], -campos[Z]);
	for (i = 0; i < NUMBODIES; i++)
		if (planets[i].TrailEnabled)
			glCallList(planets[i].Trail);
	glPopMatrix();

	if (texture)
		glEnable(GL_TEXTURE_2D);
	if (lighting)
		glEnable(GL_LIGHTING);

	glDepthMask(GL_TRUE);

	glMaterialfv(GL_FRONT, GL_AMBIENT, White);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, White);
	glMaterialfv(GL_FRONT, GL_EMISSION, White);
	glColor3f(1.0, 1.0, 1.0);

	glPushMatrix();
	glTranslated(-campos[X], -campos[Y], -campos[Z]);
	a = RADIUSSCALE(1000.0);

	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
	glBindTexture(GL_TEXTURE_2D, planets[0].textures[0]);


	if (texture) {
		SunBillBoard(a);
		totaltexsize += planets[0].texsize;
	} else
		glCallList(planets[0].HighDetail);

	glPopMatrix();

	glEnable(GL_DEPTH_TEST);

	glMaterialfv(GL_FRONT, GL_EMISSION, Black);
	camseesun = 1;
	for (n = 0; n < objstorender; n++) {

		i = renderlist[n].body;

		if ((planets[i].Type == RING) || (planets[i].Type == ATMOSPHERE))
			j = planets[i].Sat;
		else {
			j = i;
			a = MODULE(campos);
			c = MODULE(planets[i].pos);
			if (!ISASPACECRAFT(currtarget))
				if (a > c) {
					MULTVECTOR(v, campos, c / a);
					SUBVECTORS(v, v, planets[i].pos);
					if (MODULE(v) < RADIUSSCALE(planets[i].Radius))
						camseesun = 0;
				}
		}


		if ((planets[i].Type >= SATELLITE) && (planets[i].Type <= ORBITER)) {
			a = MODULE(planets[j].pos);
			c = MODULE(planets[planets[j].Sat].pos);
			a = c / a;
			if (a < 1.0) {
				MULTVECTOR(v, planets[j].pos, a);
				SUBVECTORS(v, planets[planets[j].Sat].pos, v);
				d = MODULE(v);
				if (d < RADIUSSCALE(planets[planets[j].Sat].Radius)) {
					glDisable(GL_LIGHTING);
					glColor4fv(ambient);
					ondarkside = 1;
				}
			}
		}

		glPushMatrix();

		/* Move body to its position in the orbit */
		glTranslatef(planets[j].pos[X] - campos[X],
					 planets[j].pos[Y] - campos[Y],
					 planets[j].pos[Z] - campos[Z]);
		if (planets[i].Type == COMET)
			CometTail(i);


		if (ISASPACECRAFT(currtarget) || (planets[i].Type == ASTEROID)) {
			if (!paused)
				Rotation(DEG2RAD
						 (planets[i].Rotation * timefactor / SECS_A_DAY),
						 planets[i].up, planets[i].dir);
			MyLookAt(planets[i].dir, planets[i].up);
		} else {

			/* Adjust body tilt */
			glRotatef(-planets[j].Degrees -
					  planets[planets[j].Sat].Degrees -
					  planets[planets[j].Sat].Inclination - 90.0, 1.0, 0.0,
					  0.0);

			/* Rotate body around its axis */
			if (planets[i].Type == RING)
				glRotatef(planets[j].DeltaRotation, 0.0, 0.0, 1.0);
			else
				glRotatef(planets[i].DeltaRotation, 0.0, 0.0, 1.0);
		}

		if (planets[i].Type == ATMOSPHERE)
			glDisable(GL_DEPTH_TEST);

		if (!texture) {
			glMaterialfv(GL_FRONT, GL_AMBIENT, planets[j].Color);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, planets[j].Color);
		} else {
			glMaterialfv(GL_FRONT, GL_AMBIENT, White);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, White);
		}

		switch (planets[i].Type) {
		case PLANET:
		case SATELLITE:
		case ATMOSPHERE:
			if (renderlist[n].dist <
				(UNIVERSERADIUS * 0.000003 *
				 RADIUSSCALE(planets[i].Radius) * 60.0 / fov))
				glCallList(planets[i].HighDetail);
			else {
				if (renderlist[n].dist <
					(UNIVERSERADIUS * 0.00001 *
					 RADIUSSCALE(planets[i].Radius) * 60.0 / fov))
					glCallList(planets[i].MidDetail);
				else
					glCallList(planets[i].LowDetail);
			}
			totaltexsize += planets[i].texsize;
			break;
		case COMET:
			break;
		case ORBITER:
		case SPACESHIP:
			if (renderlist[n].dist <
				(RADIUSSCALE(planets[i].Radius) * 100000.0 / fov))
				glCallList(planets[i].HighDetail);
			totaltexsize += planets[i].texsize;
			break;
		default:
			glCallList(planets[i].HighDetail);
			totaltexsize += planets[i].texsize;
			break;
		}

		if (planets[i].Type == RING) {
			DrawRing(i);
		}
		if (planets[i].Type == ATMOSPHERE)
			glEnable(GL_DEPTH_TEST);
		if (ondarkside) {
			if (lighting)
				glEnable(GL_LIGHTING);
			ondarkside = 0;
		}
		glPopMatrix();
	}

//   CheckError();
	OnScreenInfo();				/* Draw info stuff */
	glutSwapBuffers();
	frames++;
}


int main(int argc, char *argv[])
{

	logfile = fopen("log.txt", "wt");
	LOG("Initializing GLUT", glutInit(&argc, argv));
	LOG("Initializing display mode",
		glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH));
	LOG("Initializing window", glutInitWindowSize(width, height));
	glutInitWindowPosition(0, 0);
	LOG("Parsing command line", ParseCmdLine(argc, argv));
	if (fullscreen) {
#ifdef WIN32
		glutGameModeString(game_string);
		LOG("Entering game mode", glutEnterGameMode());
		main_window = 1;
		ShowCursor(FALSE);
#else
		LOG("Creating window",
			main_window = glutCreateWindow("OpenUniverse"));
		LOG("Entering full screen mode", glutFullScreen());
		glutSetCursor(GLUT_CURSOR_NONE);
#endif
	} else {
		LOG("Creating window",
			main_window = glutCreateWindow("OpenUniverse"));
	}

	if (!main_window) {
		error("Couldn't create window");
		shutdown(1);
	}

	joydetect();
	demomode = !bench;
	speed = 0.0;

	glutIdleFunc(Idle);
#ifdef HAVE_GLUI
	GLUI_Master.set_glutReshapeFunc(Reshape);
#else
	glutReshapeFunc(Reshape);
#endif
	glutMainLoop();
	return 0;
}
