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
#ifdef HAVE_GLUI
#include "gui.h"
#endif


void ParseCmdLine(int n, char **s)
{
	int i = 1, err = 0;

	while ((i < n) && (!err)) {
		err = 0;
		if (!strcmp(s[i], "-bench")) {
			bench = 1;
			i++;
			continue;
		}
		if (!strcmp(s[i], "-fullscreen")) {
			fullscreen = 1;
#ifdef HAVE_GLUI
			fullscreeninGUI = 1;
#endif
			if (s[i + 1][0] != '-') {
				strcpy(game_string, s[i + 1]);
				i++;
			}
			i++;
			continue;
		}
		err = 1;
	};
	if (err) {
		error("Valid command line options:\n");
		error("\n ou [-bench] [-fullscreen [mode_string]] \n\n");
		shutdown(1);
	}
}
