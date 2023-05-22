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

char fontpath[100] = FONT_DIR;

void initfontengine(void)
{
	switch (fonttype) {
	case TEXFONT:
		txf = txfLoadFont("default.txf");
		if (txf == NULL) {
			error("Error loading font file");
			shutdown(1);
		}
		txfEstablishTexture(txf, 0, GL_FALSE);
		glyphwidth = txf->tgi->width + txf->tgi->advance;
		glyphheight = txf->tgi->height;
		break;
	case BMAPFONT:
		glyphwidth = 10;
		glyphheight = 10;
		break;
	default:
		error("Invalid font type in ou.conf");
		shutdown(1);
		break;
	}
}

/* A couple of functions borrowed from David Bucciarelli's demos */
void texprintstring(float x, float y, float z, char *string)
{
	extern int filter;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glPushMatrix();
	glTranslatef(x, y, z);
	txfRenderString(txf, string, (int) strlen(string));
	glPopMatrix();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

void glutprintstring(float x, float y, float z, char *string)
{
	int i = 0;
	unsigned char v;

	glRasterPos3f(x, y, z);
	glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &v);
	if (v)
		while (string[i])
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, string[i++]);
}

void printstring(float x, float y, float z, char *string)
{
	switch (fonttype) {
	case TEXFONT:
		texprintstring(x, y, z, string);
		break;
	case BMAPFONT:
		glutprintstring(x, y, z, string);
		break;
	}
}
