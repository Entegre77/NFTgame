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

event events[MAXEVENTS];
int numevents = 0, currevent = 0, eventrecord = 0, nexteventin;
double eventrecordstart;
double eventstartcampos[3], eventstartcameye[3], eventstartcamup[3];
void EventTimerFunc(int);


void AutoEvents(void)
{
	char eventpath[100] = EVENTS_DIR;
	static char line[200];
	FILE *f = NULL;

	sprintf(line, "%s/autoevent.dat", eventpath);
	f = fopen(line, "rt");
	if (f == NULL) {
		error("Couldn't find autoevent file");
		shutdown(1);
	}

	currevent = -1;
	for (;;) {
		fgets(line, 200, f);
		if (feof(f))
			break;
		while (line[0] == '#')
			fgets(line, 200, f);
		if (feof(f))
			break;
		if (!strncmp(line, "keystrokes", 10)) {
			currevent = 0;
			continue;
		}
		if (currevent < 0) {
			if (!strncmp(line, "days", 4))
				sscanf(line + 4, "%lf", &days);
			if (!strncmp(line, "timefactor", 10))
				sscanf(line + 10, "%d", &timefactor);
			if (!strncmp(line, "paused", 6))
				sscanf(line + 6, "%d", &paused);
			if (!strncmp(line, "cmode", 5))
				sscanf(line + 5, "%d", &cmode);
			if (!strncmp(line, "campos", 6))
				sscanf(line + 6, "%d", &campos[X], &campos[Y], &campos[Z]);
			if (!strncmp(line, "cameye", 6))
				sscanf(line + 6, "%d", &cameye[X], &cameye[Y], &cameye[Z]);
			if (!strncmp(line, "camup", 5))
				sscanf(line + 5, "%d", &camup[X], &camup[Y], &camup[Z]);
		} else {
			sscanf(line, "%d,%d,%d\n",
				   &events[currevent].key, &events[currevent].special,
				   &events[currevent].when);
			currevent++;
		}
	}
	fclose(f);
	if (currevent > 0)
		numevents = currevent;
	else
		numevents = 0;
	currevent = 0;
	nexteventin = 0;
}

void DumpEvents(void)
{
	char eventpath[100] = EVENTS_DIR;
	static char line[200];
	FILE *f = NULL;
	int i;

	sprintf(line, "%s/dumpevent.dat", eventpath);
	f = fopen(line, "wt");
	if (f == NULL) {
		error("Couldn't open dumpevent file");
		shutdown(1);
	}

	fprintf(f, "days %f\n", eventrecordstart);
	fprintf(f, "timefactor %d\n", timefactor);
	fprintf(f, "paused %d\n", paused);
	fprintf(f, "cmode %d\n", cmode);
	fprintf(f, "campos %f %f %f\n", eventstartcampos[X],
			eventstartcampos[Y], eventstartcampos[Z]);
	fprintf(f, "cameye %f %f %f\n", eventstartcameye[X],
			eventstartcameye[Y], eventstartcameye[Z]);
	fprintf(f, "camup %f %f %f\n", eventstartcamup[X], eventstartcamup[Y],
			eventstartcamup[Z]);
	fprintf(f, "keystrokes only beyond this point\n");
	for (i = 0; i < numevents; i++) {
		fprintf(f, "%d,%d,%d\n",
				events[i].key, events[i].special, events[i].when);
	}
	fclose(f);
}

void NewEvents()
{
	if (currevent == numevents)
		return;

	if (!nexteventin) {
		if (events[currevent].key)
			Key(events[currevent].key, 0, 0);
		if (events[currevent].special)
			Special(events[currevent].special, 0, 0);
		if (currevent < (numevents - 1))
			nexteventin =
				events[currevent + 1].when - events[currevent].when;
		currevent++;
	} else
		nexteventin--;
}
