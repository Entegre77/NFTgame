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
#ifdef LINUXJOY
#include <linux/joystick.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#endif
#ifdef WIN32
#include <mmsystem.h>
#endif

/*
  And another function based on David Bucciarelli's demos.
*/

int joyavailable = 0, joyactive = 0, joy_fd;
char axes = 2;
char buttons = 2;
int version = 0x000800;
int *axis;
int *button;

void joystick(int *rot, double *dir, double *vel)
{
#ifdef WIN32
	MMRESULT res;
	JOYINFO joy;
	static double v1[3];
	static int x, y;

	res = joyGetPos(JOYSTICKID1, &joy);

	if (res == JOYERR_NOERROR) {
		joyavailable = 1;
		x = (joy.wXpos >> 12) - 8;
		y = (joy.wYpos >> 12) - 8;
		if ((x < 2) && (x > -2))
			x = 0;
		if ((y < 2) && (y > -2))
			y = 0;
		if (joyactive) {
			if (joy.wButtons & JOY_BUTTON4) {
				rot[Z] += x;
				CLAMP_SELF(rot[Z], -64, 64);
			} else {
				if (x) {
					rot[X] += x;
					CLAMP_SELF(rot[X], -64, 64);
				}
				if (y) {
					rot[Y] += y;
					CLAMP_SELF(rot[Y], -64, 64);
				}
			}

			if (joy.wButtons & JOY_BUTTON1) {
				DIVVECTOR(v1, dir, 10000.0);
				ADDVECTORS(vel, vel, v1);
			}
			if (joy.wButtons & JOY_BUTTON2) {
				DIVVECTOR(v1, dir, 10000.0);
				SUBVECTORS(vel, vel, v1);
			}
			if ((joy.wButtons & JOY_BUTTON1) & (joy.wButtons & JOY_BUTTON2)
				|| (joy.wButtons & JOY_BUTTON3))
				INITVECTOR(vel, 0.0, 0.0, 0.0);
		}
	} else
		joyavailable = 0;
#endif

#ifdef LINUXJOY
	static struct js_event js;
	static int status, x, y;
	static double v1[3];


	status = read(joy_fd, &js, sizeof(struct js_event));

	if (status > 0) {
		switch (js.type & ~JS_EVENT_INIT) {
		case JS_EVENT_BUTTON:
			button[js.number] = js.value;
			break;
		case JS_EVENT_AXIS:
			axis[js.number] = js.value;
			break;
		}
		x = axis[0] / 4096;		/* 16 levels */
		y = axis[1] / 4096;
	}

	if (joyactive) {
		if (button[4]) {
			rot[Z] += x;
			CLAMP_SELF(rot[Z], -64, 64);
		} else {
			if (x) {
				rot[X] += x;
				CLAMP_SELF(rot[X], -64, 64);
			}
			if (y) {
				rot[Y] += y;
				CLAMP_SELF(rot[Y], -64, 64);
			}
		}

		if (button[0]) {
			DIVVECTOR(v1, dir, 10000.0);
			ADDVECTORS(vel, vel, v1);
		}
		if (button[1]) {
			DIVVECTOR(v1, dir, 10000.0);
			SUBVECTORS(vel, vel, v1);
		}
		if ((button[0]) & (button[1]) || button[2])
			INITVECTOR(vel, 0.0, 0.0, 0.0);
	}
#endif
}


void joydetect(void)
{
#ifdef WIN32

	MMRESULT res;
	JOYINFO joy;

	res = joyGetPos(JOYSTICKID1, &joy);
	if (res == JOYERR_NOERROR)
		joyavailable = 1;
	else
		joyavailable = 0;

#endif

#ifdef LINUXJOY

	if ((joy_fd = open("/dev/js0", O_RDONLY)) < 0)
		joyavailable = 0;
	else {
		joyavailable = 1;
		ioctl(joy_fd, JSIOCGVERSION, &version);
		ioctl(joy_fd, JSIOCGAXES, &axes);
		ioctl(joy_fd, JSIOCGBUTTONS, &buttons);

		printf
			("%d axes %d buttons joystick detected (%d.%d.%d driver version)\n",
			 axes, buttons, version >> 16, (version >> 8) & 0xff,
			 version & 0xff);

		fcntl(joy_fd, F_SETFL, O_NONBLOCK);
		axis = (int *) calloc(axes, sizeof(int));
		button = (int *) calloc(buttons, sizeof(int));
	}
#endif

}
