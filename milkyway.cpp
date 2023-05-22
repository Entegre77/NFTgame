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

int milkyway = 1, milkyway_texsize = 0;

void InitMilkyWay()
{
#define MW_QUADS 32
	int i, j, height, components, width, tw, th;
	GLuint textures[MW_QUADS];
	char sbuf[100];
	unsigned char *image, *localimage;
	double alfa;


	image = read_JPEG_file("mw.jpg", &width, &height, &components);
	if (!image) {
		sprintf(sbuf, "Couldn't read image mw.jpg\n");
		error(sbuf);
		shutdown(1);
	}

	image = texture_LOD(image, &width, &height, components);
	if (components == 1)
		i = 1;
	else
		i = color_depth / 8;
	milkyway_texsize = (width * height * i);

	th = height;
	tw = width / MW_QUADS;

	localimage = (unsigned char *) malloc(tw * th * components);
	if (localimage == NULL) {
		error("Couldn't allocate memory");
		shutdown(1);
	}

	glGenTextures(MW_QUADS, textures);

	for (j = 0; j < MW_QUADS; j++) {

		for (i = 0; i < th; i++)
			memcpy(localimage + i * tw * components,
				   image + i * width * components +
				   j * tw * components, tw * components);


		glBindTexture(GL_TEXTURE_2D, textures[j]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 components == 3 ? cd_rgb : cd_lum,
					 tw, th,
					 0,
					 components == 3 ? GL_RGB : GL_LUMINANCE,
					 GL_UNSIGNED_BYTE, localimage);
	}
	free(image);
	free(localimage);




	MilkyWayDL = glGenLists(1);

	glNewList(MilkyWayDL, GL_COMPILE);

	glColor3f(1.0, 1.0, 0.9);

	glRotatef(90.0, 0.0, 1.0, 0.0);
	glRotatef(-60.0, 1.0, 0.0, 0.0);

	alfa = atan2(1.0, 5.0);
	for (i = 0; i < MW_QUADS; i++) {

#define X1 (UNIVERSERADIUS*0.9*cos(DEG2RAD(i*(360.0/MW_QUADS))))
#define Z1 (UNIVERSERADIUS*0.9*sin(DEG2RAD(i*(360.0/MW_QUADS))))
#define X2 (UNIVERSERADIUS*0.9*cos(DEG2RAD((i+1)*(360.0/MW_QUADS))))
#define Z2 (UNIVERSERADIUS*0.9*sin(DEG2RAD((i+1)*(360.0/MW_QUADS))))

#define _X1 (UNIVERSERADIUS*0.9*cos(alfa)*cos(DEG2RAD(i*(360.0/MW_QUADS))))
#define _Z1 (UNIVERSERADIUS*0.9*cos(alfa)*sin(DEG2RAD(i*(360.0/MW_QUADS))))
#define _X2 (UNIVERSERADIUS*0.9*cos(alfa)*cos(DEG2RAD((i+1)*(360.0/MW_QUADS))))
#define _Z2 (UNIVERSERADIUS*0.9*cos(alfa)*sin(DEG2RAD((i+1)*(360.0/MW_QUADS))))

		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glBegin(GL_QUADS);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(_X2, UNIVERSERADIUS / 5.0, _Z2);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(_X1, UNIVERSERADIUS / 5.0, _Z1);
		glTexCoord2f(0.0, 0.5);
		glVertex3f(X1, 0.0, Z1);
		glTexCoord2f(1.0, 0.5);
		glVertex3f(X2, 0.0, Z2);
		glTexCoord2f(1.0, 0.5);
		glVertex3f(X2, 0.0, Z2);
		glTexCoord2f(0.0, 0.5);
		glVertex3f(X1, 0.0, Z1);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(_X1, -UNIVERSERADIUS / 5.0, _Z1);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(_X2, -UNIVERSERADIUS / 5.0, _Z2);
		glEnd();
	}
	glEndList();

}
