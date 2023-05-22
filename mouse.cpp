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

static int oldmousex, oldmousey;

void Mouse(int button, int state, int x, int y)
{
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		oldmousex = x;
		oldmousey = y;
	}
}

/* 
    Moves camera with the mouse
*/
void Motion(int x, int y)
{
	static double v1[3];


	if (!cmode) {
		Rotation(-(x - oldmousex) / 1000.0, camup, cameye);
		CROSSPRODUCT(v1, camup, cameye);
		Rotation((y - oldmousey) / 1000.0, v1, cameye);
		Rotation((y - oldmousey) / 1000.0, v1, camup);
	}
	oldmousex = x;
	oldmousey = y;
}
