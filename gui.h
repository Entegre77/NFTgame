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

#ifdef HAVE_GLUI

#include "glui.h"
#include "arcball.h"
#include "quaternion.h"

#define OPT_OPEN	100
#define OPT_HIDE	101
#define CAM_NEAR	103
#define CAM_CMODE	104
#define CAM_SOURCE	105
#define CAM_TARGET	106
#define GUI_HIDE	107
#define OPT_STARMAG	108
#define GUI_QUIT        110
#define CAM_FOLLOW      112
#define CAM_HEADING     113
#define CAM_ROLL        114
#define TRAILS_OPEN	115
#define TRAILS_HIDE	116


extern GLUI *glui_rwin, *glui_bwin, *glui_optwin;
extern GLUI_Listbox *list_source, *list_target;
extern GLUI_Rotation *head_rot, *roll_rot;
extern GLUI_Panel *control_panel;
extern GLUI_Checkbox *glui_follow;
extern GLUI_RadioGroup *radio;
extern int fullscreeninGUI, rebuildStarsDL;

void glui_callback(int);
void update_cmode(void);

#endif

void GUI_Viewport(int *, int *);
void GUI_Init(void);
