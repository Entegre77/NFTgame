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

#ifdef HAVE_GLUI

GLUI *glui_rwin = NULL, *glui_trailwin, *glui_optwin = NULL;
GLUI_Listbox *list_source, *list_target;
GLUI_Panel *control_panel;
GLUI_Translation *head_trans, *roll_trans;
GLUI_Checkbox *glui_follow;
GLUI_RadioGroup *radio;
GLUI_RadioButton *cam1, *cam2, *cam3, *cam4;
GLUI_Translation *trans_zoom;
int fullscreeninGUI = 0, rebuildStarsDL = 0;

/*
    GLUI callback
*/

void update_cmode(void)
{
	int type;


	type = ISASPACECRAFT(currtarget);

	if (type)
		glui_follow->enable();
	else
		glui_follow->disable();


	if (follow && type) {
		cam1->set_name("Behind");
		cam2->set_name("Top");
		cam3->set_name("Right");
		cam4->set_name("Left");
		list_target->enable();
		trans_zoom->enable();
		head_trans->enable();
		roll_trans->enable();
	} else {
		cam1->set_name("Free");
		cam2->set_name("Linked");
		cam3->set_name("Body to Body");
		cam4->set_name("Orbiter");
		switch (cmode) {
		case 0:
		case 1:
			trans_zoom->disable();
			head_trans->disable();
			roll_trans->disable();
			break;
		case 2:
		case 3:
			trans_zoom->enable();
			break;
		}
	}


}

void glui_callback(int cmd)
{

#define CX(s) (s->ball->down_pt[0]-s->ball->center[0])
#define CY(s) (s->ball->down_pt[1]-s->ball->center[1])

	static GLUI_Spinner *spinner;
	static int i;

	switch (cmd) {
	case OPT_OPEN:
		if (glui_optwin) {
			glui_optwin->show();
		} else {
			glui_optwin = GLUI_Master.create_glui("Options");
			glui_optwin->add_checkbox("Demo", &demomode);
			glui_optwin->add_checkbox("Pause", &paused);
			glui_optwin->add_separator();
			glui_optwin->add_checkbox("Atmosphere", &atmospheres);
			glui_optwin->add_checkbox("Lighting", &lighting);
			glui_optwin->add_checkbox("Textures", &texture);
			glui_optwin->add_checkbox("Stars", &drawstars);
			glui_optwin->add_checkbox("Milky Way", &milkyway);
			glui_optwin->add_checkbox("Messier Objects", &messier);
			glui_optwin->add_separator();
			glui_optwin->add_checkbox("Info", &info);
			glui_optwin->add_checkbox("Top View Map", &radar);
			glui_optwin->add_checkbox("Stars labels", &slabels);
			glui_optwin->add_checkbox("Bodies labels", &plabels);
			glui_optwin->add_separator();
			glui_optwin->add_checkbox("Joystick", &joyactive);
			glui_optwin->add_separator();
			glui_optwin->add_checkbox("Real time", &realtime);
			glui_optwin->add_separator();
			spinner =
				glui_optwin->add_spinner("Star brightness:",
										 GLUI_SPINNER_FLOAT, &star_mag,
										 OPT_STARMAG,
										 (GLUI_Update_CB) glui_callback);
			spinner->set_float_limits(0.0, 10.0);
			glui_optwin->add_button("Trails", TRAILS_OPEN,
									(GLUI_Update_CB) glui_callback);
			glui_optwin->add_button("Close", OPT_HIDE,
									(GLUI_Update_CB) glui_callback);
		}
		break;
	case OPT_HIDE:
		glui_optwin->hide();
		break;
	case TRAILS_OPEN:
		if (glui_trailwin) {
			glui_trailwin->show();
		} else {
			glui_trailwin = GLUI_Master.create_glui("Trails");
			for (i = 0; i < NUMBODIES; i++)
				if ((planets[i].Type == PLANET) ||
					(planets[i].Type == ASTEROID) ||
					(planets[i].Type == COMET))
						glui_trailwin->add_checkbox(planets[i].Name,
													&planets[i].
													TrailEnabled);
			glui_trailwin->add_button("Close", TRAILS_HIDE,
									  (GLUI_Update_CB) glui_callback);
		}
		break;
	case TRAILS_HIDE:
		glui_trailwin->hide();
		break;
	case CAM_NEAR:
		Key('n', 0, 0);
		break;
	case CAM_CMODE:
		i = follow && ISASPACECRAFT(currtarget);
		switch (cmode) {
		case 0:
			list_target->disable();
			control_panel->enable();
			trans_zoom->disable();
			break;
		case 1:
			transition = 1;
			if (!i) {
				list_target->enable();
				control_panel->disable();
			}
			break;
		case 2:
			transition = 1;
			if (!i) {
				list_target->enable();
				control_panel->disable();
			}
			break;
		case 3:
			transition = 1;
			if (!i) {
				list_target->enable();
				control_panel->disable();
			}
			break;
		default:
			break;
		}
		update_cmode();
		break;
	case CAM_FOLLOW:
		update_cmode();
		break;
	case CAM_SOURCE:
		if (currsource == currtarget) {
			currsource = (currsource + 1) % NUMBODIES;
			while (planets[currsource].Type >= RING)
				currsource = (currsource + 1) % NUMBODIES;
		}
		transition = 1;
		break;
	case CAM_TARGET:
		if (currsource == currtarget) {
			currtarget = (currtarget + 1) % NUMBODIES;
			while (planets[currtarget].Type >= RING)
				currtarget = (currtarget + 1) % NUMBODIES;
		}
		update_cmode();
		transition = 1;
		break;
	case CAM_HEADING:
		if (ISASPACECRAFT(currtarget)) {
			planets[currtarget].rot[X] = (int) head_trans->get_x();
			planets[currtarget].rot[Y] = (int) head_trans->get_y();
			CLAMP_SELF(planets[currtarget].rot[X], -64, 64);
			CLAMP_SELF(planets[currtarget].rot[Y], -64, 64);
			head_trans->set_x((float) planets[currtarget].rot[X]);
			head_trans->set_y((float) planets[currtarget].rot[Y]);
		} else {
			camrot[X] = (int) head_trans->get_x();
			camrot[Y] = (int) head_trans->get_y();
			CLAMP_SELF(camrot[X], -64, 64);
			CLAMP_SELF(camrot[Y], -64, 64);
			head_trans->set_x((float) camrot[X]);
			head_trans->set_y((float) camrot[Y]);
		}
		break;
	case CAM_ROLL:
		if (ISASPACECRAFT(currtarget)) {
			planets[currtarget].rot[Z] = (int) roll_trans->get_x();
			CLAMP_SELF(planets[currtarget].rot[Z], -64, 64);
			roll_trans->set_x((float) planets[currtarget].rot[Z]);
		} else {
			camrot[Z] = (int) roll_trans->get_x();
			CLAMP_SELF(camrot[Z], -64, 64);
			roll_trans->set_x((float) camrot[Z]);
		}
		if (roll_trans->get_y() > 0.0)
			Special(GLUT_KEY_PAGE_UP, 0, 0);
		if (roll_trans->get_y() < 0.0)
			Special(GLUT_KEY_PAGE_DOWN, 0, 0);
		roll_trans->set_y(0.0);
		break;
	case GUI_HIDE:
		glui_rwin->hide();
		fullscreeninGUI = 1;
		Reshape(glutGet((GLenum) GLUT_WINDOW_WIDTH),
				glutGet((GLenum) GLUT_WINDOW_HEIGHT));
		break;
	case GUI_QUIT:
		Key(27, 0, 0);
		break;
	case OPT_STARMAG:
		rebuildStarsDL = 1;
		break;
	default:
		break;
	}
}


#endif


void GUI_Viewport(int *width, int *height)
{
#ifdef HAVE_GLUI
	int tx, ty, tw, th;

	if (fullscreeninGUI)
		glViewport(0, 0, *width, *height);
	else {

		GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
		GLUI_Master.auto_set_viewport();
		*width = tw;
		*height = th;
	}
#else
	glViewport(0, 0, *width, *height);
#endif

}


void GUI_Init(void)
{
#ifdef HAVE_GLUI
	int i;
	char sbuf[80];

	if (fullscreen)
		return;

	GLUI_Master.set_glutKeyboardFunc(Key);
	GLUI_Master.set_glutSpecialFunc(Special);
	GLUI_Master.set_glutMouseFunc(Mouse);
	glutIdleFunc(NULL);
	GLUI_Master.set_glutIdleFunc(Idle);

	glui_rwin =
		GLUI_Master.create_glui_subwindow(main_window,
										  GLUI_SUBWINDOW_RIGHT);
	glui_rwin->set_main_gfx_window(main_window);


	list_source =
		glui_rwin->add_listbox("Source:", &currsource, CAM_SOURCE,
							   (GLUI_Update_CB) glui_callback);
	for (i = 0; i < NUMBODIES; i++)
		switch (planets[i].Type) {
		case RING:
		case ATMOSPHERE:
			break;
		case SATELLITE:
		case ORBITER:
			sprintf(sbuf, "  %s", planets[i].Name);
			list_source->add_item(i, sbuf);
			break;
		default:
			list_source->add_item(i, planets[i].Name);
			break;

		}
	list_source->enabled = true;


	list_target =
		glui_rwin->add_listbox("Target:", &currtarget, CAM_TARGET,
							   (GLUI_Update_CB) glui_callback);
	for (i = 0; i < NUMBODIES; i++)
		switch (planets[i].Type) {
		case RING:
		case ATMOSPHERE:
			break;
		case SATELLITE:
		case ORBITER:
			sprintf(sbuf, "  %s", planets[i].Name);
			list_target->add_item(i, sbuf);
			break;
		default:
			list_target->add_item(i, planets[i].Name);
			break;

		}

	glui_rwin->add_button("Go there!", CAM_NEAR,
						  (GLUI_Update_CB) glui_callback);

	GLUI_Panel *camera_panel = glui_rwin->add_panel("Camera Mode");
	radio =
		glui_rwin->add_radiogroup_to_panel(camera_panel, &cmode, CAM_CMODE,
										   (GLUI_Update_CB) glui_callback);
	cam1 = glui_rwin->add_radiobutton_to_group(radio, "Free");
	cam2 = glui_rwin->add_radiobutton_to_group(radio, "Linked");
	cam3 = glui_rwin->add_radiobutton_to_group(radio, "Body to Body");
	cam4 = glui_rwin->add_radiobutton_to_group(radio, "Orbiter");
	glui_follow =
		glui_rwin->add_checkbox_to_panel(camera_panel, "Follow", &follow,
										 CAM_FOLLOW,
										 (GLUI_Update_CB) glui_callback);
	if (ISASPACECRAFT(currtarget))
		glui_follow->disable();

	control_panel = glui_rwin->add_panel("", GLUI_PANEL_NONE);

	head_trans =
		glui_rwin->add_translation_to_panel(control_panel, "Heading",
											GLUI_TRANSLATION_XY, NULL,
											CAM_HEADING,
											(GLUI_Update_CB)
											glui_callback);

	glui_rwin->add_column_to_panel(control_panel, false);


	roll_trans =
		glui_rwin->add_translation_to_panel(control_panel, "Roll/Speed",
											GLUI_TRANSLATION_XY, NULL,
											CAM_ROLL,
											(GLUI_Update_CB)
											glui_callback);

	GLUI_Panel *lens_panel = glui_rwin->add_panel("", GLUI_PANEL_NONE);

/* Insert extra blank spaces to  avoid misalignment */
	GLUI_Translation *trans_fov =
		glui_rwin->add_translation_to_panel(lens_panel, "  FOV  ",
											GLUI_TRANSLATION_Z, &fov);
	trans_fov->set_speed(0.1);
	trans_fov->set_z(fov);

	glui_rwin->add_column_to_panel(lens_panel, false);

	trans_zoom =
		glui_rwin->add_translation_to_panel(lens_panel, "   ZOOM   ",
											GLUI_TRANSLATION_Z, &zoom);
	trans_zoom->set_speed(0.1);
	trans_zoom->set_z(zoom);

	GLUI_Spinner *glui_tf =
		glui_rwin->add_spinner("Timefactor:", GLUI_SPINNER_INT,
							   &timefactor);

	glui_tf->set_int_limits(-5000, 5000);
	glui_tf->set_speed(0.001);

	glui_rwin->add_button("Options", OPT_OPEN,
						  (GLUI_Update_CB) glui_callback);
	glui_rwin->add_button("Hide GUI", GUI_HIDE,
						  (GLUI_Update_CB) glui_callback);
	glui_rwin->add_button("Quit", GUI_QUIT,
						  (GLUI_Update_CB) glui_callback);
	if (!cmode)
		control_panel->disable();

	update_cmode();
#endif
}
