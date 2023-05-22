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
#include "gui.h"

void TimerFunc(int i)
{

	switch (i) {
	case 0:
		if (demomode) {
			cmode = (rand() % 3) + 1;
			if (cmode > 1)
				do {
					currtarget = rand() % NUMBODIES;
					currsource = rand() % NUMBODIES;
				} while ((currsource == currtarget)
						 || (planets[currsource].Type >= RING)
						 || (planets[currtarget].Type >= RING));
			zoom = rand() % 5 + 5.0;
#ifdef HAVE_GLUI
			if (!fullscreeninGUI)
				update_cmode();
#endif
			transition = 1;
			dosmooth = 1;
			if (!(rand() % 5))
				logo = 1;
		}
		glutTimerFunc(demostep * 1000, TimerFunc, 0);
		break;
	case 1:
		help = 0;
		break;
	}
}
