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
  Initialization routines:
  
  	- Read texture images
  	- Initialize planets' data
  	- Create display lists
  	- Other initialization routines
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
extern "C" {
#include "jpeglib.h"
}
#include <setjmp.h>
#include "ou.h"
#include "texture.h"
#include "bmf.h"
#include "glext.h"

GLfloat LightPos[4] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat ambient[4] = { 0.2, 0.2, 0.2, 0.0 };
GLfloat White[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat Black[4] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat Fog[4] = { 1.0, 0.9, 0.8, 1.0 };
float fogdensity = 0.1;
int texture = 1, lighting = 1, drawstars = 1, gravity = 1, logo = 1;
GLenum smodel = GL_SMOOTH;
GLuint StarsDL, MessierDL, MilkyWayDL;
int ImgWidth, ImgHeight, components, red, polaris;
int width = 640, height = 480;	/* default WIDTH and HEIGHT */
planetdata planets[MAXBODIES];
stardata stars[NUMSTARS];
messierdata messierobjs[MAXMESSIER];
double days;
int timefactor = 1;				/* iterarion = 1 second */
int slices = 16, stacks = 16, NUMBODIES, num_messier, border = 0, filter =
	GL_LINEAR;
int mipmap = 1, mipmap_filter = GL_LINEAR_MIPMAP_LINEAR;
char texturepath[100] = TEXTURE_DIR;
char confpath[100] = CONF_DIR;
unsigned char tmpimg[258 * 258 * 4], *splash_image;
int splashwidth = 0, splashheight = 0, splashbitmap = 1, LOD = HIGH, tex_compr;
int color_depth = 24;
GLenum cd_rgb, cd_lum, cd_rgba, cd_luma;
GLuint splashtex[4], logotex;
extern float star_mag;

static void SetUpTrails(void);
static void Spheroid(double, float, float, float, float, int, int,
					 planetdata *, int);
void Print(char *), InitLogo(void);
GLubyte *read_JPEG_file(char *, int *, int *, int *);


void Init(void)
{
	time_t t;


	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	t = time(NULL);
	srand(t);
	days = t / 3600.0 / 24.0 - 10092.0;
	initfontengine();
	hasPointEXT = glutExtensionSupported("GL_EXT_point_parameters");
	tex_compr = glutExtensionSupported("GL_EXT_texture_compression_s3tc");
	tex_compr *= glutExtensionSupported("GL_ARB_texture_compression");
#ifdef WIN32
	InitPointEXT();
#endif
	ReadConfigFile();
	SetUpTrails();
	Print("Setting up Stars");
	InitStars(star_mag, 1);
	Print("Setting up Messier Objects");
	InitMessier();
	Print("Setting up Milky Way");
	InitMilkyWay();
	InitLogo();
	InitFlares();
	glShadeModel(smodel);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	glHint((GLenum) GL_TEXTURE_COMPRESSION_HINT_ARB, GL_NICEST);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}


void InitLogo(void)
{
	static int i, width, height, components;
	static double j;
	static unsigned char *image, *localimage;
	char sbuf[80];

	image = read_JPEG_file("cyclo.jpg", &width, &height, &components);
	if (!image) {
		sprintf(sbuf, "Couldn't read image %s/cyclo.jpg\n", texturepath);
		error(sbuf);
		shutdown(1);
	}

	localimage = (unsigned char *) malloc(width * height * 4);
	if (localimage == NULL) {
		error("Couldn't allocate memory");
		shutdown(1);
	}

	for (i = 0; i < (width * height); i++) {
		localimage[i * 4] = image[i * 3];
		localimage[i * 4 + 1] = image[i * 3 + 1];
		localimage[i * 4 + 2] = image[i * 3 + 2];
		j = (image[i * 3] + image[i * 3 + 1] + image[i * 3 + 2]) / 3.0;
		if (j < (double) 100.0)
			j = 0.0;
		localimage[i * 4 + 3] = j > 255.0 ? 255 : (int) j;
	}

	glGenTextures(1, &logotex);
	glBindTexture(GL_TEXTURE_2D, logotex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 components == 3 ? cd_rgba : cd_luma,
				 width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localimage);
	free(image);
	free(localimage);
}

/*
    A quick and dirty Print function for the opening screen
 
    I know it's not the best way to do this but it will only have impact in the
    loading time ;-)
*/
void Print(char *s)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (splashbitmap) {
		if (!splashwidth) {
			char *files[4]={"splash1.jpg", "splash2.jpg", "splash3.jpg", "splash4.jpg"};
			int i;

			splash_image =
				read_JPEG_file(files[rand() % 4], &splashwidth, &splashheight,
							   &components);

			if ((splashwidth != 512) || (splashheight != 512)
				|| (components != 3)) {
				error("splash.jpg must be 512x512 24bits");
				shutdown(1);
			}
			glGenTextures(4, splashtex);

			for (i = 0; i < 256; i++)
				memcpy(tmpimg + i * 256 * 3,
					   splash_image + i * splashwidth * components,
					   256 * components);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, splashtex[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexImage2D(GL_TEXTURE_2D,
						 0,
						 components == 3 ? cd_rgb : cd_lum,
						 256, 256,
						 0,
						 components == 3 ? GL_RGB : GL_LUMINANCE,
						 GL_UNSIGNED_BYTE, tmpimg);

			for (i = 0; i < 256; i++)
				memcpy(tmpimg + i * 256 * 3,
					   splash_image + i * splashwidth * components +
					   256 * components, 256 * components);

			glBindTexture(GL_TEXTURE_2D, splashtex[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexImage2D(GL_TEXTURE_2D,
						 0,
						 components == 3 ? cd_rgb : cd_lum,
						 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, tmpimg);


			for (i = 0; i < 256; i++)
				memcpy(tmpimg + i * 256 * 3,
					   splash_image + (i + 256) * splashwidth * components,
					   256 * components);

			glBindTexture(GL_TEXTURE_2D, splashtex[2]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexImage2D(GL_TEXTURE_2D,
						 0,
						 components == 3 ? cd_rgb : cd_lum,
						 256, 256,
						 0,
						 components == 3 ? GL_RGB : GL_LUMINANCE,
						 GL_UNSIGNED_BYTE, tmpimg);

			for (i = 0; i < 256; i++)
				memcpy(tmpimg + i * 256 * 3,
					   splash_image + (i +
									   256) * splashwidth * components +
					   256 * components, 256 * components);

			glBindTexture(GL_TEXTURE_2D, splashtex[3]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexImage2D(GL_TEXTURE_2D,
						 0,
						 components == 3 ? cd_rgb : cd_lum,
						 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, tmpimg);
			free(splash_image);
		}

		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		glColor3f(1.0, 1.0, 1.0);

		glBindTexture(GL_TEXTURE_2D, splashtex[0]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex2i(0, 0);
		glTexCoord2f(1.0, 0.0);
		glVertex2i(width / 2, 0);
		glTexCoord2f(1.0, 1.0);
		glVertex2i(width / 2, height / 2);
		glTexCoord2f(0.0, 1.0);
		glVertex2i(0, height / 2);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, splashtex[1]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex2i(width / 2, 0);
		glTexCoord2f(1.0, 0.0);
		glVertex2i(width, 0);
		glTexCoord2f(1.0, 1.0);
		glVertex2i(width, height / 2);
		glTexCoord2f(0.0, 1.0);
		glVertex2i(width / 2, height / 2);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, splashtex[2]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex2i(0, height / 2);
		glTexCoord2f(1.0, 0.0);
		glVertex2i(width / 2, height / 2);
		glTexCoord2f(1.0, 1.0);
		glVertex2i(width / 2, height);
		glTexCoord2f(0.0, 1.0);
		glVertex2i(0, height);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, splashtex[3]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex2i(width / 2, height / 2);
		glTexCoord2f(1.0, 0.0);
		glVertex2i(width, height / 2);
		glTexCoord2f(1.0, 1.0);
		glVertex2i(width, height);
		glTexCoord2f(0.0, 1.0);
		glVertex2i(width / 2, height);
		glEnd();

	} else
		glClear(GL_COLOR_BUFFER_BIT);


	switch (fonttype) {
	case TEXFONT:
		txfBindFontTexture(txf);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case BMAPFONT:
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glColor3f(1.0, 1.0, 1.0);
	}
	printstring(width / 2 - strlen(s) * glyphwidth / 2, height / 2, 0, s);
	glutSwapBuffers();
}

/*  
    Spheroid drawing routine based on gluSphere Mesa function (src-glu/quadric.c)
    patched to allow multiple texture objects and flattening.
*/
static void Spheroid(double radius, float xflat, float yflat, float zflat,
					 float randomness, int slices, int stacks,
					 planetdata * pdata, int textured)
{
	static GLfloat rho, drho, theta, dtheta;
	GLfloat x, y, z;
	GLfloat s, t, ds, dt;
	int i, j, k, imin, imax, tex;
	double *rarray1, *rarray2, *tmp;


	drho = M_PI / (GLfloat) stacks;
	dtheta = 2.0 * M_PI / (GLfloat) slices;

/* Allocate memory for surface randomness arrays */
	rarray1 = (double *) malloc((slices + 1) * 3 * sizeof(double));
	rarray2 = (double *) malloc((slices + 1) * 3 * sizeof(double));

	if (textured) {

		if (stacks % pdata->texheight) {
			error("Incorrect STACKS value, edit configuration file\n");
			shutdown(1);
		}

		if (slices % pdata->texwidth) {
			error("Incorrect SLICES value, edit configuration file\n");
			shutdown(1);
		}

		ds = 1.0 / slices * pdata->texwidth;
		dt = 1.0 / stacks * pdata->texheight;
		t = 1.0;
		imin = 0;
		imax = stacks;


		/* Initialize surface randomness arrays */
		for (i = 0; i < (slices + 1) * 3; i++) {
			rarray1[i] =
				(1.0 - randomness) +
				(randomness * 2.0 * rand()) / RAND_MAX;
			rarray2[i] =
				(1.0 - randomness) +
				(randomness * 2.0 * rand()) / RAND_MAX;
		}

		for (i = imin; i < imax; i++) {
			rho = i * drho;
			if (!(i % (imax / pdata->texheight)))
				t = 1.0;
			tmp = rarray1;
			rarray1 = rarray2;
			rarray2 = tmp;
			for (k = 0; k < (slices + 1) * 3; k++)
				rarray2[k] =
					(1.0 - randomness) +
					(randomness * 2.0 * rand()) / RAND_MAX;

			for (k = 0; k < pdata->texwidth; k++) {
				/* Find the texture map we should apply in this iteration */
				tex = imax / pdata->texheight;
				tex =
					(pdata->texwidth * (pdata->texheight - 1) -
					 i / tex * pdata->texwidth) + k;
				glBindTexture(GL_TEXTURE_2D, pdata->textures[tex]);
				glBegin(GL_QUAD_STRIP);
				s = 0.0;
				/* overwrite last element with first element data */
				rarray1[(k + 1) * slices / pdata->texwidth * 3] =
					rarray1[k * slices / pdata->texwidth * 3];
				rarray1[(k + 1) * slices / pdata->texwidth * 3 + 1] =
					rarray1[k * slices / pdata->texwidth * 3 + 1];
				rarray1[(k + 1) * slices / pdata->texwidth * 3 + 2] =
					rarray1[k * slices / pdata->texwidth * 3 + 2];
				rarray2[(k + 1) * slices / pdata->texwidth * 3] =
					rarray2[k * slices / pdata->texwidth * 3];
				rarray2[(k + 1) * slices / pdata->texwidth * 3 + 1] =
					rarray2[k * slices / pdata->texwidth * 3 + 1];
				rarray2[(k + 1) * slices / pdata->texwidth * 3 + 2] =
					rarray2[k * slices / pdata->texwidth * 3 + 2];
				for (j = (k * slices / pdata->texwidth);
					 j <= ((k + 1) * slices / pdata->texwidth); j++) {
					theta = (j == slices) ? 0.0 : j * dtheta;
					x = -sin(theta) * sin(rho) * rarray1[j * 3];
					y = cos(theta) * sin(rho) * rarray1[j * 3 + 1];
					z = cos(rho) * rarray1[j * 3 + 2];
					glNormal3f(x, y, z);
					glTexCoord2f(s, t);
					glVertex3f(x * radius * (1.0 - xflat),
							   y * radius * (1.0 - yflat),
							   z * radius * (1.0 - zflat));
					x = -sin(theta) * sin(rho + drho) * rarray2[j * 3];
					y = cos(theta) * sin(rho + drho) * rarray2[j * 3 + 1];
					z = cos(rho + drho) * rarray2[j * 3 + 2];
					glNormal3f(x, y, z);
					glTexCoord2f(s, t - dt);
					s += ds;
					glVertex3f(x * radius * (1.0 - xflat),
							   y * radius * (1.0 - yflat),
							   z * radius * (1.0 - zflat));
				}
				glEnd();
			}
			t -= dt;
		}
	} else {
		ds = 1.0 / slices;
		dt = 1.0 / stacks;
		imin = 0;
		imax = stacks;
		for (i = imin; i < imax; i++) {
			rho = i * drho;
			glBegin(GL_QUAD_STRIP);
			for (j = 0; j <= slices; j++) {
				theta = (j == slices) ? 0.0 : j * dtheta;
				x = -sin(theta) * sin(rho);
				y = cos(theta) * sin(rho);
				z = cos(rho);
				glNormal3f(x, y, z);
				glVertex3f(x * radius, y * radius, z * radius);
				x = -sin(theta) * sin(rho + drho);
				y = cos(theta) * sin(rho + drho);
				z = cos(rho + drho);
				glNormal3f(x, y, z);
				glVertex3f(x * radius, y * radius, z * radius);
			}
			glEnd();
		}
	}
	free(rarray1);
	free(rarray2);
}



/* 
    Read texture, split large textures into 256x256 pieces and set up
    display lists
*/
void SetUpBody(int body, char *texfile, int compress_texture)
{
	int width = 256, height =
		256, x1, x2, x3, y, i, j, k, current;
	GLenum gluerr;
	BMFObject *obj;
	char sbuf[80];
	GLenum intformat, format;

	sprintf(sbuf, "Setting up body: %s", planets[body].Name);
	Print(sbuf);

	switch (texfile[strlen(texfile) - 1]) {
	case 'f':					/* BMF file */
		obj = LoadBMF(texfile, RADIUSSCALE(planets[body].Radius));
		planets[body].HighDetail = glGenLists(1);
		glNewList(planets[body].HighDetail, GL_COMPILE);
		glMaterialf(GL_FRONT, GL_SHININESS, 1.0);
		if (obj->sparse) {
			for (i = 0; i < obj->NumMaterials; i++) {
				glInterleavedArrays(GL_T2F_N3F_V3F, 0,
									obj->sparse[i].vertexlist);
				glMaterialfv(GL_FRONT, GL_AMBIENT,
							 (GLfloat *) & obj->sparse[i].ambient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE,
							 (GLfloat *) & obj->sparse[i].diffuse);
				glMaterialfv(GL_FRONT, GL_SPECULAR,
							 (GLfloat *) & obj->sparse[i].specular);

				if (obj->sparse[i].texture_name_length > 1) {
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, obj->sparse[i].texbind);
				} else
					glDisable(GL_TEXTURE_2D);
				glDrawElements(GL_TRIANGLES,
							   obj->sparse[i].number_of_triangles * 3,
							   GL_UNSIGNED_SHORT,
							   obj->sparse[i].indexlist);
			}
		} else {
			for (i = 0; i < obj->NumMaterials; i++) {
				glInterleavedArrays(GL_T2F_N3F_V3F, 0,
									obj->strip[i].vertexlist);
				glMaterialfv(GL_FRONT, GL_AMBIENT,
							 (GLfloat *) & obj->strip[i].ambient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE,
							 (GLfloat *) & obj->strip[i].diffuse);
				glMaterialfv(GL_FRONT, GL_SPECULAR,
							 (GLfloat *) & obj->strip[i].specular);

				if (obj->strip[i].texture_name_length > 1) {
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, obj->strip[i].texbind);
				} else
					glDisable(GL_TEXTURE_2D);

				current = 0;
				for (j = 0; j < obj->strip[i].number_of_strips; j++) {
					glDrawElements(GL_TRIANGLE_STRIP,
								   obj->strip[i].length_of_strip[j],
								   GL_UNSIGNED_SHORT,
								   &obj->strip[i].stripindex[current]);
					current += obj->strip[i].length_of_strip[j];
				}
			}
		}
		glMaterialfv(GL_FRONT, GL_SPECULAR, Black);
		glEndList();
		planets[body].LowDetail = planets[body].MidDetail =
			planets[body].HighDetail;
		planets[body].texsize = obj->texsize;
		break;

	case 'g':					/* Spherical body, JPEG texture */
		planets[body].Image =
			read_JPEG_file(texfile, &ImgWidth, &ImgHeight, &components);

		if (!planets[body].Image) {
			sprintf(sbuf, "Couldn't read image %s/%s\n", texturepath,
					texfile);
			error(sbuf);
			shutdown(1);
		}

		planets[body].Image =
			texture_LOD(planets[body].Image, &ImgWidth, &ImgHeight,
						components);
		if (components == 1)
			j = 1;
		else
			j = color_depth / 8;
		planets[body].texsize = (ImgWidth * ImgHeight * j);

		if (ImgWidth < width) {
			planets[body].texwidth = 1;
			width = ImgWidth;
		} else
			planets[body].texwidth = ImgWidth / width;
		if (ImgHeight < height) {
			planets[body].texheight = 1;
			height = ImgHeight;
		} else
			planets[body].texheight = ImgHeight / height;
		planets[body].texnum =
			planets[body].texwidth * planets[body].texheight;
		planets[body].textures =
			(GLuint *) malloc(planets[body].texnum * sizeof(GLuint));
		if (planets[body].textures == NULL) {
			error("Couldn't allocate memory");
			shutdown(1);
		}

		intformat = components == 3 ? cd_rgb : cd_lum;
		format = components == 3 ? (GLenum) GL_RGB : (GLenum) GL_LUMINANCE;
		if ((format == GL_RGB) && tex_compr && compress_texture)
			intformat = (GLenum) GL_COMPRESSED_RGB_ARB;


		glGenTextures(planets[body].texnum, planets[body].textures);

		for (j = 0; j < planets[body].texheight; j++)
			for (i = 0; i < planets[body].texwidth; i++) {
				glBindTexture(GL_TEXTURE_2D,
							  planets[body].textures[planets[body].
													 texwidth * j + i]);
				if (border) {
					/* Copy pixel data to texture buffer and set up borders */
					x1 = i * width;
					x2 = (i * width + ImgWidth - 1) % ImgWidth;
					x3 = ((i + 1) * width) % ImgWidth;
					for (k = 0; k < (height + 2); k++) {
						y = (j * height + k + ImgHeight - 1) % ImgHeight;
						memcpy(tmpimg + k * (width + 2) * components +
							   components,
							   planets[body].Image +
							   y * ImgWidth * components + x1 * components,
							   width * components);
						memcpy(tmpimg + k * (width + 2) * components,
							   planets[body].Image +
							   y * ImgWidth * components + x2 * components,
							   components);
						memcpy(tmpimg + k * (width + 2) * components +
							   (width + 1) * components,
							   planets[body].Image +
							   y * ImgWidth * components + x3 * components,
							   components);
					}
					glTexImage2D(GL_TEXTURE_2D,
								 0,
								 intformat,
								 width + 2, height + 2,
								 1,
								 format,
								 GL_UNSIGNED_BYTE, tmpimg);
				} else {
					for (k = 0; k < height; k++)
						memcpy(tmpimg + k * width * components,
							   planets[body].Image +
							   ImgWidth * width * components * j +
							   width * components * i +
							   ImgWidth * components * k,
							   width * components);
					if (mipmap) {
						if (
							(gluerr =
							 (GLenum) gluBuild2DMipmaps(GL_TEXTURE_2D,
														intformat,
														width,
														height,
														format,
														GL_UNSIGNED_BYTE,
														tmpimg))) {
							error((char *) gluErrorString(gluerr));
							shutdown(1);
						}
					} else {
						glTexImage2D(GL_TEXTURE_2D,
									 0,
									 intformat,
									 width, height,
									 0,
									 format,
									 GL_UNSIGNED_BYTE, tmpimg);
					}
				}

				if (mipmap) {
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
									mipmap_filter);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
									mipmap_filter);
				} else {
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
									filter);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
									filter);
				}
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
								GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
								GL_CLAMP);
			}
		free(planets[body].Image);
		planets[body].HighDetail = glGenLists(1);
		glNewList(planets[body].HighDetail, GL_COMPILE);
		Spheroid(RADIUSSCALE(planets[body].Radius), planets[body].xflat,
				 planets[body].yflat, planets[body].zflat, 0.0,
				 slices, stacks, &planets[body], 1);
		glEndList();
		planets[body].MidDetail = glGenLists(1);
		glNewList(planets[body].MidDetail, GL_COMPILE);
		Spheroid(RADIUSSCALE(planets[body].Radius), planets[body].xflat,
				 planets[body].yflat, planets[body].zflat, 0.0,
				 slices / 2, stacks / 2, &planets[body], 1);
		glEndList();
		planets[body].LowDetail = glGenLists(1);
		glNewList(planets[body].LowDetail, GL_COMPILE);
		Spheroid(RADIUSSCALE(planets[body].Radius), planets[body].xflat,
				 planets[body].yflat, planets[body].zflat, 0.0,
				 slices / 4, stacks / 4, &planets[body], 1);
		glEndList();
		break;
	}
}


void SetUpAsteroid(int body, char *model)
{
	int i, j, current;
	BMFObject *obj;
	char sbuf[80];

	sprintf(sbuf, "Setting up Asteroid: %s", planets[body].Name);
	Print(sbuf);
	planets[body].Radius /= RADIUSSCALE(1.0);
	obj = LoadBMF(model, RADIUSSCALE(planets[body].Radius));

	planets[body].HighDetail = glGenLists(1);
	glNewList(planets[body].HighDetail, GL_COMPILE);
	glMaterialf(GL_FRONT, GL_SHININESS, 1.0);
	if (obj->sparse) {
		for (i = 0; i < obj->NumMaterials; i++) {
			glInterleavedArrays(GL_T2F_N3F_V3F, 0,
								obj->sparse[i].vertexlist);
			glMaterialfv(GL_FRONT, GL_AMBIENT,
						 (GLfloat *) & obj->sparse[i].ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE,
						 (GLfloat *) & obj->sparse[i].diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR,
						 (GLfloat *) & obj->sparse[i].specular);

			if (obj->sparse[i].texture_name_length > 1) {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, obj->sparse[i].texbind);
			} else
				glDisable(GL_TEXTURE_2D);
			glDrawElements(GL_TRIANGLES,
						   obj->sparse[i].number_of_triangles * 3,
						   GL_UNSIGNED_SHORT, obj->sparse[i].indexlist);
		}
	} else {
		for (i = 0; i < obj->NumMaterials; i++) {
			glInterleavedArrays(GL_T2F_N3F_V3F, 0,
								obj->strip[i].vertexlist);
			glMaterialfv(GL_FRONT, GL_AMBIENT,
						 (GLfloat *) & obj->strip[i].ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE,
						 (GLfloat *) & obj->strip[i].diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR,
						 (GLfloat *) & obj->strip[i].specular);

			if (obj->strip[i].texture_name_length > 1) {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, obj->strip[i].texbind);
			} else
				glDisable(GL_TEXTURE_2D);

			current = 0;
			for (j = 0; j < obj->strip[i].number_of_strips; j++) {
				glDrawElements(GL_TRIANGLE_STRIP,
							   obj->strip[i].length_of_strip[j],
							   GL_UNSIGNED_SHORT,
							   &obj->strip[i].stripindex[current]);
				current += obj->strip[i].length_of_strip[j];
			}
		}
	}
	glMaterialfv(GL_FRONT, GL_SPECULAR, Black);
	glEndList();
	planets[body].texsize = obj->texsize;
}



void SetUpComet(int body, char *texfile, float randomness, float lenght,
				int num_part)
{
	int i;
	float a, b, c, xo, yo, zo, xd, yd, zd;
	char sbuf[80];

	sprintf(sbuf, "Setting up body: %s", planets[body].Name);
	Print(sbuf);

	planets[body].tail = (particle *) malloc(sizeof(particle) * num_part);
	planets[body].tail_lenght = lenght;
	planets[body].num_particles = num_part;
	for (i = 0; i < num_part; i++) {
		a = (rand() % 360) * M_PI / 180.0;
		b = (float) rand() / RAND_MAX;
		b *= RADIUSSCALE(planets[body].Radius);
		c = (float) i / (float) num_part *lenght;
		xo = yo = zo = 0.0;
		xd = 0.1 * b / RADIUSSCALE(planets[body].Radius) * sin(a);
		yd = RADIUSSCALE(planets[body].Radius);
		zd = 0.1 * b / RADIUSSCALE(planets[body].Radius) * cos(a);
		planets[body].tail[i].dist = c;
		c = DISTANCE(xd, yd, zd);
		planets[body].tail[i].origin[0] = xo;
		planets[body].tail[i].origin[1] = yo;
		planets[body].tail[i].origin[2] = zo;
		planets[body].tail[i].dir[0] = xd / c;
		planets[body].tail[i].dir[1] = zd / c;
		planets[body].tail[i].dir[2] = yd / c;
	}
}


void SetUpOrbiter(int body, char *model)
{
	int i, j, current;
	BMFObject *obj;
	char sbuf[80];

	sprintf(sbuf, "Setting up orbiter: %s", planets[body].Name);
	Print(sbuf);
	planets[body].Radius /= RADIUSSCALE(1.0);
	obj = LoadBMF(model, RADIUSSCALE(planets[body].Radius));
	planets[body].HighDetail = glGenLists(1);
	glNewList(planets[body].HighDetail, GL_COMPILE);
	glMaterialf(GL_FRONT, GL_SHININESS, 1.0);
	if (obj->sparse) {
		for (i = 0; i < obj->NumMaterials; i++) {
			glInterleavedArrays(GL_T2F_N3F_V3F, 0,
								obj->sparse[i].vertexlist);
			glMaterialfv(GL_FRONT, GL_AMBIENT,
						 (GLfloat *) & obj->sparse[i].ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE,
						 (GLfloat *) & obj->sparse[i].diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR,
						 (GLfloat *) & obj->sparse[i].specular);

			if (obj->sparse[i].texture_name_length > 1) {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, obj->sparse[i].texbind);
			} else
				glDisable(GL_TEXTURE_2D);
			glDrawElements(GL_TRIANGLES,
						   obj->sparse[i].number_of_triangles * 3,
						   GL_UNSIGNED_SHORT, obj->sparse[i].indexlist);
		}
	} else {
		for (i = 0; i < obj->NumMaterials; i++) {
			glInterleavedArrays(GL_T2F_N3F_V3F, 0,
								obj->strip[i].vertexlist);
			glMaterialfv(GL_FRONT, GL_AMBIENT,
						 (GLfloat *) & obj->strip[i].ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE,
						 (GLfloat *) & obj->strip[i].diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR,
						 (GLfloat *) & obj->strip[i].specular);

			if (obj->strip[i].texture_name_length > 1) {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, obj->strip[i].texbind);
			} else
				glDisable(GL_TEXTURE_2D);

			current = 0;
			for (j = 0; j < obj->strip[i].number_of_strips; j++) {
				glDrawElements(GL_TRIANGLE_STRIP,
							   obj->strip[i].length_of_strip[j],
							   GL_UNSIGNED_SHORT,
							   &obj->strip[i].stripindex[current]);
				current += obj->strip[i].length_of_strip[j];
			}
		}
	}
	glMaterialfv(GL_FRONT, GL_SPECULAR, Black);
	glEndList();
	planets[body].texsize = obj->texsize;
}


void SetUpSpaceShip(int body, char *model)
{
	int i, j, current;
	BMFObject *obj;
	char sbuf[80];

	sprintf(sbuf, "Setting up SpaceShip: %s", planets[body].Name);
	Print(sbuf);
	planets[body].Radius /= RADIUSSCALE(1.0);
	obj = LoadBMF(model, RADIUSSCALE(planets[body].Radius));
	planets[body].HighDetail = glGenLists(1);
	glNewList(planets[body].HighDetail, GL_COMPILE);
	glMaterialf(GL_FRONT, GL_SHININESS, 1.0);
	if (obj->sparse) {
		for (i = 0; i < obj->NumMaterials; i++) {
			glInterleavedArrays(GL_T2F_N3F_V3F, 0,
								obj->sparse[i].vertexlist);
			glMaterialfv(GL_FRONT, GL_AMBIENT,
						 (GLfloat *) & obj->sparse[i].ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE,
						 (GLfloat *) & obj->sparse[i].diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR,
						 (GLfloat *) & obj->sparse[i].specular);

			if (obj->sparse[i].texture_name_length > 1) {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, obj->sparse[i].texbind);
			} else
				glDisable(GL_TEXTURE_2D);

			glDrawElements(GL_TRIANGLES,
						   obj->sparse[i].number_of_triangles * 3,
						   GL_UNSIGNED_SHORT, obj->sparse[i].indexlist);

			if (obj->sparse[i].texture_name_length <= 1)
				glEnable(GL_TEXTURE_2D);

		}
	} else {
		for (i = 0; i < obj->NumMaterials; i++) {
			glInterleavedArrays(GL_T2F_N3F_V3F, 0,
								obj->strip[i].vertexlist);
			glMaterialfv(GL_FRONT, GL_AMBIENT,
						 (GLfloat *) & obj->strip[i].ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE,
						 (GLfloat *) & obj->strip[i].diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR,
						 (GLfloat *) & obj->strip[i].specular);

			if (obj->strip[i].texture_name_length > 1) {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, obj->strip[i].texbind);
			} else
				glDisable(GL_TEXTURE_2D);

			current = 0;
			for (j = 0; j < obj->strip[i].number_of_strips; j++) {
				glDrawElements(GL_TRIANGLE_STRIP,
							   obj->strip[i].length_of_strip[j],
							   GL_UNSIGNED_SHORT,
							   &obj->strip[i].stripindex[current]);
				current += obj->strip[i].length_of_strip[j];
			}

			if (obj->strip[i].texture_name_length <= 1)
				glEnable(GL_TEXTURE_2D);

		}
	}
	glMaterialfv(GL_FRONT, GL_SPECULAR, Black);
	glEndList();
	planets[body].texsize = obj->texsize;
}




void SetUpRings(int body, char *texfile, float inner_radius,
				float outter_radius, float tlevel)
{
	unsigned char *localimage;
	int i;
	char sbuf[80];

	sprintf(sbuf, "Setting up rings: %s", planets[planets[body].Sat].Name);
	Print(sbuf);

	planets[body].Image =
		read_JPEG_file(texfile, &ImgWidth, &ImgHeight, &components);
	if (!planets[body].Image) {
		sprintf(sbuf, "Couldn't read image %s/%s\n", texturepath, texfile);
		error(sbuf);
		shutdown(1);
	}

	localimage =
		(unsigned char *) malloc(ImgHeight * ImgWidth * (components + 1));
	if (localimage == NULL) {
		error("Couldn't allocate memory");
		shutdown(1);
	}


	switch (components) {
	case 1:
		for (i = 0; i < (ImgHeight * ImgWidth); i++) {
			localimage[i * 2] = planets[body].Image[i];
			localimage[i * 2 + 1] =
				(unsigned char) (planets[body].Image[i] / tlevel);
		}
		break;
	case 3:
		for (i = 0; i < (ImgHeight * ImgWidth); i++) {
			localimage[i * 4] = planets[body].Image[i * 3];
			localimage[i * 4 + 1] = planets[body].Image[i * 3 + 1];
			localimage[i * 4 + 2] = planets[body].Image[i * 3 + 2];
			localimage[i * 4 + 3] =
				(unsigned
				 char) ((planets[body].Image[i * 3] +
						 planets[body].Image[i * 3 + 1] +
						 planets[body].Image[i * 3 + 2]) / 3 / tlevel);
		}
		break;
	}

	planets[body].texnum = 1;
	planets[body].textures =
		(GLuint *) malloc(planets[body].texnum * sizeof(GLuint));
	if (planets[body].textures == NULL) {
		error("Couldn't allocate memory");
		shutdown(1);
	}
	glGenTextures(planets[body].texnum, planets[body].textures);
	glBindTexture(GL_TEXTURE_2D, planets[body].textures[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 components == 3 ? cd_rgba : cd_luma,
				 ImgWidth, ImgHeight,
				 0,
				 components == 3 ? GL_RGBA : GL_LUMINANCE_ALPHA,
				 GL_UNSIGNED_BYTE, localimage);

	free(planets[body].Image);
	free(localimage);


}


void SetUpAtmosphere(int body, char *texfile, float level, int cut)
{
	int width = 256, height = 256, x1, x2, x3, y, i, j, k;
	unsigned char localimage[258 * 258 * 4];
	GLenum gluerr;
	char sbuf[80];

	sprintf(sbuf, "Setting up atmosphere: %s",
			planets[planets[body].Sat].Name);
	Print(sbuf);

	planets[body].Image =
		read_JPEG_file(texfile, &ImgWidth, &ImgHeight, &components);
	if (!planets[body].Image) {
		sprintf(sbuf, "Couldn't read image %s/%s\n", texturepath, texfile);
		error(sbuf);
		shutdown(1);
	}

	planets[body].Image =
		texture_LOD(planets[body].Image, &ImgWidth, &ImgHeight,
					components);
	if (components == 1)
		j = 1;
	else
		j = color_depth / 8;
	planets[body].texsize = (ImgWidth * ImgHeight * j);

	if (ImgWidth < width) {
		planets[body].texwidth = 1;
		width = ImgWidth;
	} else
		planets[body].texwidth = ImgWidth / width;
	if (ImgHeight < height) {
		planets[body].texheight = 1;
		height = ImgHeight;
	} else
		planets[body].texheight = ImgHeight / height;
	planets[body].texnum =
		planets[body].texwidth * planets[body].texheight;
	planets[body].textures =
		(GLuint *) malloc(planets[body].texnum * sizeof(GLuint));
	if (planets[body].textures == NULL) {
		error("Couldn't allocate memory");
		shutdown(1);
	}
	glGenTextures(planets[body].texnum, planets[body].textures);

	for (j = 0; j < planets[body].texheight; j++)
		for (i = 0; i < planets[body].texwidth; i++) {

			glBindTexture(GL_TEXTURE_2D,
						  planets[body].textures[planets[body].texwidth *
												 j + i]);

			if (border) {
				/* Copy pixel data to texture buffer and set up borders */
				x1 = i * width;
				x2 = (i * width + ImgWidth - 1) % ImgWidth;
				x3 = ((i + 1) * width) % ImgWidth;
				for (k = 0; k < (height + 2); k++) {
					y = (j * height + k + ImgHeight - 1) % ImgHeight;
					memcpy(tmpimg + k * (width + 2) * components +
						   components,
						   planets[body].Image +
						   y * ImgWidth * components + x1 * components,
						   width * components);
					memcpy(tmpimg + k * (width + 2) * components,
						   planets[body].Image +
						   y * ImgWidth * components + x2 * components,
						   components);
					memcpy(tmpimg + k * (width + 2) * components +
						   (width + 1) * components,
						   planets[body].Image +
						   y * ImgWidth * components + x3 * components,
						   components);
				}

				for (k = 0; k < (258 * 258); k++) {
					localimage[k * (components + 1)] =
						tmpimg[k * components];
					localimage[k * (components + 1) + 1] =
						tmpimg[k * components + 1];
					localimage[k * (components + 1) + 2] =
						tmpimg[k * components + 2];
					if (components == 3)
						y =
							(tmpimg[k * 3] + tmpimg[k * 3 + 1] +
							 tmpimg[k * 3 + 2]) / 3;
					else
						y = tmpimg[k];
					if (y < cut)
						y = 0;
					localimage[k * (components + 1) + components] =
						(unsigned char) ((float) y / level);
				}

				glTexImage2D(GL_TEXTURE_2D,
							 0,
							 components == 3 ? cd_rgba : cd_luma,
							 width + 2, height + 2,
							 1,
							 components ==
							 3 ? GL_RGBA : GL_LUMINANCE_ALPHA,
							 GL_UNSIGNED_BYTE, localimage);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
								filter);
			} else {
				for (k = 0; k < height; k++)
					memcpy(tmpimg + k * width * components,
						   planets[body].Image +
						   ImgWidth * width * components * j +
						   width * components * i +
						   ImgWidth * components * k, width * components);

				for (k = 0; k < (256 * 256); k++) {
					localimage[k * (components + 1)] =
						tmpimg[k * components];
					localimage[k * (components + 1) + 1] =
						tmpimg[k * components + 1];
					localimage[k * (components + 1) + 2] =
						tmpimg[k * components + 2];
					if (components == 3)
						y =
							(tmpimg[k * 3] + tmpimg[k * 3 + 1] +
							 tmpimg[k * 3 + 2]) / 3;
					else
						y = tmpimg[k];
					if (y < cut)
						y = 0;
					localimage[k * (components + 1) + components] =
						(unsigned char) ((float) y / level);
				}

				if (mipmap) {
					if (
						(gluerr =
						 (GLenum) gluBuild2DMipmaps(GL_TEXTURE_2D,
													components ==
													3 ? cd_rgba : cd_luma,
													width, height,
													components ==
													3 ? GL_RGBA :
													GL_LUMINANCE_ALPHA,
													GL_UNSIGNED_BYTE,
													localimage))) {
						error((char *) gluErrorString(gluerr));
						shutdown(1);
					}
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
									mipmap_filter);
				} else {
					glTexImage2D(GL_TEXTURE_2D,
								 0,
								 components == 3 ? cd_rgba : cd_luma,
								 width, height,
								 0,
								 components ==
								 3 ? GL_RGBA : GL_LUMINANCE_ALPHA,
								 GL_UNSIGNED_BYTE, localimage);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
									filter);
				}

			}
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		}
	free(planets[body].Image);
	planets[body].HighDetail = glGenLists(1);
	glNewList(planets[body].HighDetail, GL_COMPILE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	Spheroid(RADIUSSCALE(planets[body].Radius),
			 planets[planets[body].Sat].xflat,
			 planets[planets[body].Sat].yflat,
			 planets[planets[body].Sat].zflat, 0.0, slices, stacks,
			 &planets[body], 1);
	glDisable(GL_BLEND);
	glEndList();
	planets[body].MidDetail = glGenLists(1);
	glNewList(planets[body].MidDetail, GL_COMPILE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	Spheroid(RADIUSSCALE(planets[body].Radius),
			 planets[planets[body].Sat].xflat,
			 planets[planets[body].Sat].yflat,
			 planets[planets[body].Sat].zflat, 0.0, slices / 2, stacks / 2,
			 &planets[body], 1);
	glDisable(GL_BLEND);
	glEndList();
	planets[body].LowDetail = glGenLists(1);
	glNewList(planets[body].LowDetail, GL_COMPILE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	Spheroid(RADIUSSCALE(planets[body].Radius),
			 planets[planets[body].Sat].xflat,
			 planets[planets[body].Sat].yflat,
			 planets[planets[body].Sat].zflat, 0.0, slices / 4, stacks / 4,
			 &planets[body], 1);
	glDisable(GL_BLEND);
	glEndList();
}



static void SetUpTrails(void)
{
	int i, j;
	double d;

	Print("Setting up Trails");
	for (i = 1; i < NUMBODIES; i++) {
		if ((planets[i].Type != PLANET) &&
			(planets[i].Type != ASTEROID) && (planets[i].Type != COMET))
			continue;
		planets[i].Trail = glGenLists(1);
		glNewList(planets[i].Trail, GL_COMPILE);
		glColor3f(planets[i].Color[0], planets[i].Color[1],
				  planets[i].Color[2]);
		glBegin(GL_LINE_LOOP);
		d = 0.0;
		for (j = 0; j < 360; j++) {
			UpdatePositions(d, i);
			glVertex3dv(planets[i].pos);
			d += planets[i].OrbitalPeriod / 360.0;
		}
		glEnd();
		glEndList();
	}
}



void InitSun(char *filename, double blend, int cutlevel)
{
	static int i;
	char sbuf[80];

	Print("Setting up The Sun");

	planets[0].Radius = 2.0;	/*Real Radius is 109.6 */
	planets[0].Degrees = 0.0;
	planets[0].DeltaRotation = 0.0;
	planets[0].Rotation = 360.0 / 28.0;
	planets[0].Inclination = 0.0;
	planets[0].Sat = 0;
	INITVECTOR(planets[0].pos, 0.0, 0.0, 0.0);
	INITVECTOR(planets[0].vel, 0.0, 0.0, 0.0);
	sprintf(planets[0].Name, "Sun");

	planets[0].Image =
		read_JPEG_file(filename, &ImgWidth, &ImgHeight, &components);
	if (!planets[0].Image) {
		sprintf(sbuf, "Couldn't read image %s/sun.jpg\n", texturepath);
		error(sbuf);
		shutdown(1);
	}
	if (components == 1)
		i = 1;
	else
		i = color_depth / 8;
	planets[0].texsize = (ImgWidth * ImgHeight * i);

	planets[0].texnum = 7;
	planets[0].textures = (GLuint *) malloc(sizeof(GLuint) * 7);
	if (planets[0].textures == NULL) {
		error("Couldn't allocate memory");
		shutdown(1);
	}

	glGenTextures(7, planets[0].textures);
	glBindTexture(GL_TEXTURE_2D, planets[0].textures[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 components == 3 ? cd_rgb : cd_lum,
				 ImgWidth, ImgHeight,
				 0, GL_RGB, GL_UNSIGNED_BYTE, planets[0].Image);

	free(planets[0].Image);
}
