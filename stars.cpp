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

float star_size = 40.0;


void InitStars(float magcorr, int read_file)
{
	char datapath[100] = DATA_DIR;
	static char line[100], *tmp;
	FILE *f = NULL;
	static float dimension, old_mag, magnitude;
	static double p1[3], p2[3], p3[3], p4[3];

	int i, j;

	if (read_file) {
		sprintf(line, "%s/stars.dat", datapath);
		f = fopen(line, "rt");
		if (f == NULL) {
			error("Couldn't find star catalog file");
			shutdown(1);
		}
	}
	for (i = 0; i < NUMSTARS; i++) {
		if (read_file) {
			fgets(line, 100, f);
			if (feof(f))
				break;
			while (line[0] == '#')
				fgets(line, 100, f);
			if (feof(f))
				break;
			j = 0;
			while (line[j] != ',')
				j++;
			line[j] = '\0';
			tmp = line + j + 1;
			strcpy(stars[i].Name, line);
			sscanf(tmp, "%f,%f,%f,%c\n", &stars[i].ascension,
				   &stars[i].declination, &stars[i].magnitude,
				   &stars[i].type);

			/* Translate declination/ascension coord into x,y,z */
			INITVECTOR(stars[i].pos,
					   UNIVERSERADIUS * cos(-stars[i].ascension) *
					   cos(stars[i].declination),
					   UNIVERSERADIUS * sin(stars[i].declination),
					   UNIVERSERADIUS * sin(-stars[i].ascension) *
					   cos(stars[i].declination));
		}
		red = 0;
		/* Print some stars in red color for accuracy testing */
		if (!strcmp(stars[i].Name, "Polaris")) {
			red = 1;
			polaris = i;
		}
		if (!strcmp(stars[i].Name, "Dubhe"))
			red = 1;
		if (!strcmp(stars[i].Name, "Merak"))
			red = 1;
		if (!strcmp(stars[i].Name, "Phecda"))
			red = 1;
		if (!strcmp(stars[i].Name, "Megrez"))
			red = 1;
		if (!strcmp(stars[i].Name, "Alioth"))
			red = 1;
		if (!strcmp(stars[i].Name, "Mizar"))
			red = 1;


		/* Translate magnitude scale into 0-1 color scale.  This is a bit 
		   tricky and didn't find the right translation yet (it's very
		   monitor gamma dependant) */
/*
 *	stars[i].color=1.0/(1.46+stars[i].magnitude)+0.2; 
 *	stars[i].color=pow(1.0/(stars[i].magnitude+2.46),0.5);
 *	stars[i].color=0.8/log(1.46+stars[i].magnitude);
 */
		magnitude = magcorr * 2.42 / ((2.46 + stars[i].magnitude) * 2.42);
		if (magnitude > 1.0)
			magnitude = 1.0;

		/* Only 20 magnitudes levels to speed up rendering process */
		magnitude = (int) ceil(magnitude * 20.0) / 20.0;
		switch (stars[i].type) {
		case 'O':
			stars[i].color[0] = 0.8;
			stars[i].color[1] = 0.8;
			stars[i].color[2] = 1.0;
			break;
		case 'B':
			stars[i].color[0] = 0.9;
			stars[i].color[1] = 0.9;
			stars[i].color[2] = 1.0;
			break;
		case 'A':
			stars[i].color[0] = 1.0;
			stars[i].color[1] = 1.0;
			stars[i].color[2] = 1.0;
			break;
		case 'F':
			stars[i].color[0] = 1.0;
			stars[i].color[1] = 1.0;
			stars[i].color[2] = 0.8;
			break;
		case 'G':
			stars[i].color[0] = 1.0;
			stars[i].color[1] = 1.0;
			stars[i].color[2] = 0.7;
			break;
		case 'K':
			stars[i].color[0] = 1.0;
			stars[i].color[1] = 0.9;
			stars[i].color[2] = 0.8;
			break;
		case 'M':
		case 'C':
		case 'S':
			stars[i].color[0] = 1.0;
			stars[i].color[1] = 0.6;
			stars[i].color[2] = 0.6;
			break;
		}
		stars[i].color[0] *= magnitude;
		stars[i].color[1] *= magnitude;
		stars[i].color[2] *= magnitude;
	}
	if (read_file)
		fclose(f);


	old_mag = 100.0;
	i = 0;
	StarsDL = glGenLists(1);
	glNewList(StarsDL, GL_COMPILE);
	glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, planets[0].textures[0]);
	while (stars[i].magnitude < magcorr) {

		dimension = star_size + 100.0 / (stars[i].magnitude + 2.4);
		dimension = 0.0035 + 0.001 / (stars[i].magnitude + 2.4);
		INITVECTOR(p1,
				   UNIVERSERADIUS *
				   cos(-(stars[i].ascension + dimension)) *
				   cos(stars[i].declination - dimension),
				   UNIVERSERADIUS * sin(stars[i].declination - dimension),
				   UNIVERSERADIUS *
				   sin(-(stars[i].ascension + dimension)) *
				   cos(stars[i].declination - dimension));

		INITVECTOR(p2,
				   UNIVERSERADIUS *
				   cos(-(stars[i].ascension - dimension)) *
				   cos(stars[i].declination - dimension),
				   UNIVERSERADIUS * sin(stars[i].declination - dimension),
				   UNIVERSERADIUS *
				   sin(-(stars[i].ascension - dimension)) *
				   cos(stars[i].declination - dimension));

		INITVECTOR(p3,
				   UNIVERSERADIUS *
				   cos(-(stars[i].ascension - dimension)) *
				   cos(stars[i].declination + dimension),
				   UNIVERSERADIUS * sin(stars[i].declination + dimension),
				   UNIVERSERADIUS *
				   sin(-(stars[i].ascension - dimension)) *
				   cos(stars[i].declination + dimension));

		INITVECTOR(p4,
				   UNIVERSERADIUS *
				   cos(-(stars[i].ascension + dimension)) *
				   cos(stars[i].declination + dimension),
				   UNIVERSERADIUS * sin(stars[i].declination + dimension),
				   UNIVERSERADIUS *
				   sin(-(stars[i].ascension + dimension)) *
				   cos(stars[i].declination + dimension));


		glColor3f(stars[i].color[0], stars[i].color[1], stars[i].color[2]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(p1[X], p1[Y], p1[Z]);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(p2[X], p2[Y], p2[Z]);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(p3[X], p3[Y], p3[Z]);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(p4[X], p4[Y], p4[Z]);
		glEnd();
		i++;
	}
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_POINTS);
	while (i < NUMSTARS) {
		glColor3f(stars[i].color[0], stars[i].color[1], stars[i].color[2]);
		glVertex3dv(stars[i].pos);
		i++;
	}
	glEnd();
	glEndList();
}
