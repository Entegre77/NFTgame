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
#include <math.h>

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#endif

#include <GL/glut.h>
#ifndef PLANET
#include "cfgparse.tab.h"
#endif
#include "texfont.h"
#include "macros.h"



#define NUMSTARS 3141			/* 3141 for full star catalog, decrease number for better
								   performance, first N brighter stars are used */

#define FLAREINTENSITY 0.5
#define MAXBODIES 50
#define MAXMESSIER 50
#define MAXVERTEX 3000
#define MAXEVENTS 1000
#define AU 149597.870
#define UNIVERSERADIUS 10000000.0	/* Somewhere beyond Pluto */
#define LIGHTSPEED 299792.458	/* Km / sec */
#define SECS_A_DAY (24.0*60.0*60.0)	/* seconds */
enum { X, Y, Z };
enum { TEXFONT, BMAPFONT };
enum { LOW, MEDIUM, HIGH };

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DATEOFELEMENTS 2450680.5	/* In Julian days */


#ifndef WIN32
#define error(s) fprintf(stderr,s)
#define scmp strcasecmp
#define delay(x) usleep(x*1000)
#else
#define error(s) MessageBox(NULL,s,"ERROR", MB_OK);
#define scmp _stricmp
#define delay(x) Sleep(x)
#endif

extern GLfloat LightPos[4];
extern GLfloat ambient[4];
extern GLfloat White[4];
extern GLfloat Black[4];
extern GLfloat Fog[4];
extern GLuint logotex;
extern float fogdensity;
extern int texture, lighting, drawstars, gravity;
extern int messier, milkyway, messier_texsize, milkyway_texsize;
extern GLenum smodel;
extern GLuint StarsDL, MessierDL, MilkyWayDL, EclipticDL;
extern int ImgWidth, ImgHeight, width, height, red, polaris;
extern double days;
extern char texturepath[100], fontpath[100], confpath[100];
extern int slices, stacks, NUMBODIES, num_messier, timefactor;
extern double campos[3], cameye[3], camup[3], camvel[3];
extern int camrot[3];
extern float fov, fps, sec, star_mag, speed, d, zoom, aratio, radarzoom;
extern float nearclip, farclip;
extern int frames, paused, cmode, currsource, currtarget, nearest,
	objstorender;
extern int demomode, bench, help, help2, plabels, slabels, init,
	splashbitmap, LOD;
extern int demostep, color_depth, tex_comp;
extern GLenum cd_rgb, cd_lum, cd_rgba, cd_luma;
extern int fullscreen, info, logo, realtime, trail, jpeg_quality,
	atmospheres;
extern char sbuf[100], game_string[20];
extern int joyavailable, joyactive;
extern int follow, smooth_transitions, dosmooth, transition, radar;
extern int main_window;
extern TexFont *txf;
extern int fonttype, glyphheight, glyphwidth;
extern FILE *logfile;
extern int hasPointEXT, numevents, currevent, eventrecord;
extern double eventrecordstart;
extern double eventstartcampos[3], eventstartcameye[3], eventstartcamup[3];

typedef struct {
	double origin[3];
	double dir[3];
	float color[4];
	double dist;
} particle;

typedef struct {
	char Name[20];
	double pos[3];
	float ascension, declination, magnitude, color[3];
	char type;
} stardata;

typedef struct {
	char Name[20];
	double pos[3];
	float ascension, declination, magnitude, size;
	GLuint textures[1];
} messierdata;


typedef struct {
	float u, v;
	float r, g, b, a;
	float nx, ny, nz;
	float x, y, z;
} vertex_data;


typedef struct {
	char Name[20];
	int Type, TrailEnabled;
	int Sat, CustomFunction;
	GLubyte *Image;
	GLuint LowDetail, MidDetail, HighDetail, Trail, *textures;
	int texnum, texwidth, texheight, texsize;
	float DeltaRotation, InnerRadius, Radius, xflat, yflat, zflat,
		Rotation, Degrees, OrbitalPeriod;
	float Inclination, AscendingNode, Perihelion, MeanDistance,
		DailyMotion;
	float Eccentricity, MeanLongitude, Color[4];
	double pos[3], prevel[3], vel[3], grav[3], dir[3], up[3];
	double Mass;
	float tail_lenght;
	int num_particles, initialized, rot[3];
	particle *tail;
	vertex_data *vertex_list;
	GLuint *indices;
} planetdata;


typedef struct {
	int body;
	double dist;
	GLuint dlist;
} rlist;

typedef struct {
	unsigned char key;
	int special, when;
} event;

extern rlist renderlist[MAXBODIES];
extern planetdata planets[MAXBODIES];
extern stardata stars[NUMSTARS];
extern messierdata messierobjs[MAXMESSIER];
extern event events[MAXEVENTS];

				/* cmdline.cpp */
void ParseCmdLine(int, char **);

				/* ou.cpp */
void Idle(void);
void Reshape(int, int);
				/* init.cpp */
void Init(void);
				/* stars.cpp */
void InitStars(float, int);
				/* milkyway.cpp */
void InitMilkyWay();
				/* messier.cpp */
void InitMessier();
				/* positions.cpp */
void UpdatePositions(double, int);
				/* joystick.cpp */
void joystick(int *, double *, double *);
void joydetect(void);
				/* cfgparse */
void ReadConfigFile(void);
				/* vsop87.cpp */
void EarthPos(double, double *, double *, double *);
void JupiterPos(double, double *, double *, double *);
void MarsPos(double, double *, double *, double *);
void MercuryPos(double, double *, double *, double *);
void NeptunePos(double, double *, double *, double *);
void SaturnPos(double, double *, double *, double *);
void UranusPos(double, double *, double *, double *);
void VenusPos(double, double *, double *, double *);
void PlutoPos(double, double *, double *, double *);
void MoonPos(double, double *, double *, double *);
				/* keyboard.cpp */
void Key(unsigned char, int, int);
void Special(int, int, int);
				/* mouse.cpp */
void Mouse(int, int, int, int);
void Motion(int, int);
				/* sshot.cpp */
void ScreenShot(char *);
				/* sun.cpp */
void SunBillBoard(double);
				/* timer.cpp */
void TimerFunc(int);
				/* font.cpp */
void initfontengine(void);
void texprintstring(float, float, float, char *);
void glutprintstring(float, float, float, char *string);
void printstring(float, float, float, char *string);
				/* util.cpp */
float gettime(void);
void Rotation(double, double[3], double[3]);
void MakeQuat(double *, double, double, double, double);
void Quat2Matrix(double *, float[16]);
void Mat2Quat(float[4][4], double *);
void Euler2Quat(double, double, double, double *);
void MyLookAt(double[3], double[3]);
void RotMat(double[3], double[3], float[16]);
int SmoothTrans(double[3], double[3], double[3],
				double[3], double[3], double[3], double);
GLubyte *texture_LOD(GLubyte *, int *, int *, int);
void log(char *);
void shutdown(int);
				/* jpeg.cpp */
GLubyte *read_JPEG_file(char *, int *, int *, int *);
				/* dds.cpp */
GLubyte *read_DDS_file(char *, int *, int *, int *);
				/* stars.cpp */
void InitStars(float, int);
extern float star_size;
				/* comet.cpp */
void InitPointEXT(void);
void CometTail(int);
				/* rings.cpp */
void DrawRing(int);
				/* gravity.cpp */
void Gravity(double *, double *);
void FeelTheGravity(void);
				/* radar.cpp */
void Radar(void);
								/* camera.cpp */
void UpdateCamera(double[3], double[3], double[3]);
				/* info.cpp */
void OnScreenInfo(void);
				/* events.cpp */
void AutoEvents(void);
void DumpEvents(void);
void NewEvents(void);
								/* mp3.cpp */
void MP3_InitPlayer(void);
void MP3_FlushMessages(void);
				/* flares.cpp */
void InitFlares(void);
void LensFlares(void);
