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

#define NUM_FLARES 9
#define TYPE_FLARES 6

GLuint flares[TYPE_FLARES];

struct {
	int type;
	double loc, scale, intensity;
} flaresdata[NUM_FLARES];

void InitFlares(void)
{
	unsigned char *image;
	char filename[20], sbuf[100];
	int j, width, height, components;



	glGenTextures(TYPE_FLARES, flares);

	for (j = 0; j < TYPE_FLARES; j++) {

		sprintf(filename,"flare%d.jpg",j+1);
		image = read_JPEG_file(filename, &width, &height, &components);
		if (!image) {
			sprintf(sbuf, "Couldn't read image %s\n", filename);
			error(sbuf);
			shutdown(1);
		}

		if (components > 1) {
			sprintf(sbuf, "Invalid flare image: %s\n", filename);
			error(sbuf);
			shutdown(1);
		}

		glBindTexture(GL_TEXTURE_2D, flares[j]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 cd_lum,
					 width, height,
					 0,
					 GL_LUMINANCE,
					 GL_UNSIGNED_BYTE, image);
		free(image);
	}

	flaresdata[0].type=2;
	flaresdata[0].loc=1.3;
	flaresdata[0].scale=0.1;
	flaresdata[0].intensity=0.6;
	flaresdata[1].type=3;
	flaresdata[1].loc=1.0;
	flaresdata[1].scale=0.25;
	flaresdata[1].intensity=0.4;
	flaresdata[2].type=1;
	flaresdata[2].loc=0.5;
	flaresdata[2].scale=0.5;
	flaresdata[2].intensity=0.3;
	flaresdata[3].type=3;
	flaresdata[3].loc=0.2;
	flaresdata[3].scale=0.1;
	flaresdata[3].intensity=0.3;
	flaresdata[4].type=0;
	flaresdata[4].loc=0.0;
	flaresdata[4].scale=0.1;
	flaresdata[4].intensity=0.3;
	flaresdata[5].type=5;
	flaresdata[5].loc=-0.25;
	flaresdata[5].scale=0.2;
	flaresdata[5].intensity=0.5;
	flaresdata[6].type=5;
	flaresdata[6].loc=-0.4;
	flaresdata[6].scale=0.05;
	flaresdata[6].intensity=0.6;
	flaresdata[7].type=5;
	flaresdata[7].loc=-0.6;
	flaresdata[7].scale=0.1;
	flaresdata[7].intensity=0.4;
	flaresdata[8].type=5;
	flaresdata[8].loc=-1.0;
	flaresdata[8].scale=0.1;
	flaresdata[8].intensity=0.2;	
}

void LensFlares(void)
{
	int i;
	double tmp[3], center[3],lightdir[3], dx[3], dy[3], sx[3], sy[3];
	double lookatsun, sunbrightness, brightness;

	COPYVECTOR(tmp, campos);
	sunbrightness = MODULE(campos);
	sunbrightness = CLAMP( AU / sunbrightness, 0.0 , 0.9);
	NORMALIZE(tmp);
	ADDVECTORS(tmp, tmp, cameye);
	lookatsun = MODULE(tmp);
	lookatsun *= (60.0 / fov);
	if (lookatsun > FLAREINTENSITY)
		return;


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(CLAMP_SELF(fov, 0.5, 120.0), width / (double) height,
				   0.5, 10.0);
	glMatrixMode(GL_MODELVIEW);


	COPYVECTOR(lightdir, campos);
	MULTVECTOR(lightdir, lightdir, -1.0);
	NORMALIZE(lightdir);
	if (SAMEVECTOR(cameye, lightdir))
		COPYVECTOR(dx, camup);
	else
		CROSSPRODUCT(dx, cameye, lightdir);
	NORMALIZE(dx);
	CROSSPRODUCT(dy, dx, cameye);
	MULTVECTOR(center, cameye, 0.5 + 2.0);




	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glDisable(GL_DEPTH_TEST);
	for (i=0; i<NUM_FLARES; i++) {
		MULTVECTOR(sx, dx, flaresdata[i].scale);
		MULTVECTOR(sy, dy, flaresdata[i].scale);
		glBindTexture(GL_TEXTURE_2D, flares[flaresdata[i].type]);
		brightness = flaresdata[i].intensity * (FLAREINTENSITY - lookatsun) / FLAREINTENSITY * sunbrightness;
		glColor3f(brightness, brightness, brightness * 0.95);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		MULTVECTOR(tmp, lightdir, flaresdata[i].loc);
		SUBVECTORS(tmp, center, tmp);
		ADDVECTORS(tmp, tmp, sx);
		ADDVECTORS(tmp,tmp,sy);
		glVertex3dv(tmp);
		glTexCoord2f(1.0, 0.0);
		MULTVECTOR(tmp, lightdir, flaresdata[i].loc);
		SUBVECTORS(tmp, center, tmp);
		SUBVECTORS(tmp, tmp, sx);
		ADDVECTORS(tmp,tmp,sy);
		glVertex3dv(tmp);
		glTexCoord2f(1.0, 1.0);
		MULTVECTOR(tmp, lightdir, flaresdata[i].loc);
		SUBVECTORS(tmp, center, tmp);
		SUBVECTORS(tmp, tmp, sx);
		SUBVECTORS(tmp,tmp,sy);
		glVertex3dv(tmp);
		glTexCoord2f(0.0, 1.0);
		MULTVECTOR(tmp, lightdir, flaresdata[i].loc);
		SUBVECTORS(tmp, center, tmp);
		ADDVECTORS(tmp, tmp, sx);
		SUBVECTORS(tmp,tmp,sy);
		glVertex3dv(tmp);
		glEnd();
	}
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}
