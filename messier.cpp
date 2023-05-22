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

int messier = 1, messier_texsize = 0;

void InitMessier()
{
	char datapath[100] = DATA_DIR;
	char line[100], texname[20];
	int i, j, height, components, width;
	FILE *f = NULL;
	float dimension;
	double p1[3], p2[3], p3[3], p4[3];
	unsigned char *image;

	sprintf(line, "%s/messier.dat", datapath);
	f = fopen(line, "rt");
	if (f == NULL) {
		error("Couldn't find messier catalog file");
		shutdown(1);
	}

	i = 0;
	while (1) {
		fgets(line, 100, f);
		while (line[0] == '#')
			fgets(line, 100, f);
		if (feof(f))
			break;
		sscanf(line, "%s %s %f %f %f %f\n",
			   messierobjs[i].Name, texname,
			   &messierobjs[i].ascension, &messierobjs[i].declination,
			   &messierobjs[i].magnitude, &messierobjs[i].size);

		INITVECTOR(messierobjs[i].pos,
				   UNIVERSERADIUS * cos(-messierobjs[i].ascension) *
				   cos(messierobjs[i].declination),
				   UNIVERSERADIUS * sin(messierobjs[i].declination),
				   UNIVERSERADIUS * sin(-messierobjs[i].ascension) *
				   cos(messierobjs[i].declination));

		image = read_JPEG_file(texname, &width, &height, &components);
		if (!image) {
			sprintf(line, "Couldn't read image mw.jpg\n");
			error(line);
			shutdown(1);
		}

		image = texture_LOD(image, &width, &height, components);

		if ((width > 256) || (height > 256)) {
			error
				("Messier textures larger than 256x256 are not supported");
			shutdown(1);
		}

		if (components == 1)
			j = 1;
		else
			j = color_depth / 8;
		messier_texsize += (width * height * j);

		glGenTextures(1, messierobjs[i].textures);

		glBindTexture(GL_TEXTURE_2D, messierobjs[i].textures[0]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 components == 3 ? cd_rgb : cd_lum,
					 width, height,
					 0,
					 components == 3 ? GL_RGB : GL_LUMINANCE,
					 GL_UNSIGNED_BYTE, image);

		free(image);
		i++;
	}

	fclose(f);


	num_messier = i;

	i = 0;
	MessierDL = glGenLists(1);
	glNewList(MessierDL, GL_COMPILE);
	while (i < num_messier) {

		glBindTexture(GL_TEXTURE_2D, messierobjs[i].textures[0]);
		dimension = messierobjs[i].size / 2.0;

		INITVECTOR(p1,
				   UNIVERSERADIUS *
				   cos(-(messierobjs[i].ascension + dimension)) *
				   cos(messierobjs[i].declination - dimension),
				   UNIVERSERADIUS * sin(messierobjs[i].declination -
										dimension),
				   UNIVERSERADIUS *
				   sin(-(messierobjs[i].ascension + dimension)) *
				   cos(messierobjs[i].declination - dimension));

		INITVECTOR(p2,
				   UNIVERSERADIUS *
				   cos(-(messierobjs[i].ascension - dimension)) *
				   cos(messierobjs[i].declination - dimension),
				   UNIVERSERADIUS * sin(messierobjs[i].declination -
										dimension),
				   UNIVERSERADIUS *
				   sin(-(messierobjs[i].ascension - dimension)) *
				   cos(messierobjs[i].declination - dimension));

		INITVECTOR(p3,
				   UNIVERSERADIUS *
				   cos(-(messierobjs[i].ascension - dimension)) *
				   cos(messierobjs[i].declination + dimension),
				   UNIVERSERADIUS * sin(messierobjs[i].declination +
										dimension),
				   UNIVERSERADIUS *
				   sin(-(messierobjs[i].ascension - dimension)) *
				   cos(messierobjs[i].declination + dimension));

		INITVECTOR(p4,
				   UNIVERSERADIUS *
				   cos(-(messierobjs[i].ascension + dimension)) *
				   cos(messierobjs[i].declination + dimension),
				   UNIVERSERADIUS * sin(messierobjs[i].declination +
										dimension),
				   UNIVERSERADIUS *
				   sin(-(messierobjs[i].ascension + dimension)) *
				   cos(messierobjs[i].declination + dimension));

		if (messierobjs[i].magnitude > 4.0)
			glColor3f(4.0 / messierobjs[i].magnitude,
					  4.0 / messierobjs[i].magnitude,
					  4.0 / messierobjs[i].magnitude);
		else
			glColor3f(1.0, 1.0, 1.0);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(p1[X], p1[Y], p1[Z]);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(p2[X], p2[Y], p2[Z]);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(p3[X], p3[Y], p3[Z]);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(p4[X], p4[Y], p4[Z]);
		glEnd();
		i++;
	}
	glEndList();
}
