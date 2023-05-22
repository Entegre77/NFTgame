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

#ifdef HAVE_XAUDIO

#include "xaudio.h"
#include "decoder.h"
#include "player.h"
#include "control.h"
#include <io.h>

int *player;


void MP3_InitPlayer(void)
{
	char s[100];
	struct _finddata_t c_file;
	long hfile;

	/* Find first .c file in current directory */
	sprintf(s, "%s/*.mp3", MP3_DIR);
	if ((hfile = _findfirst(s, &c_file)) == -1L)
		printf("No *.c files in current directory!\n");
	else {
		/* Find the rest of the .c files */
		while (_findnext(hfile, &c_file) == 0) {
		}
		_findclose(hfile);
	}

	if (player_new((void **) &player, NULL) != XA_SUCCESS)
		error("XAPLAYER: cannot not create decoder thread");
	sprintf(s, "%s/%s", MP3_DIR, "2001.mp3");
	control_message_send(player, XA_MSG_COMMAND_INPUT_OPEN, s);
	control_message_send(player, XA_MSG_COMMAND_PLAY);
}

void MP3_FlushMessages(void)
{
	XA_Message message;

	while (control_message_get(player, &message)>0) {	/* Flush message queue */
		if (message.code < 0)
			break;
		if (message.code == XA_MSG_NOTIFY_INPUT_POSITION)
			if (message.data.position.offset >= message.data.position.range)
				shutdown(0);
	}

}

#endif
