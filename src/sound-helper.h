/*
 *  camera ui2
 *  Copyright (C) 2010 Nicolai Hess
 *  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef _SOUND_HELPER_H_
#define _SOUND_HELPER_H_

typedef struct _SoundPlayerHelper SoundPlayerHelper;

SoundPlayerHelper* sound_player_create();
void sound_player_destroy(SoundPlayerHelper* sound_player);

void sound_player_capture_sound(SoundPlayerHelper* sound_player);
void sound_player_focus_done_sound(SoundPlayerHelper* sound_player);
void sound_player_start_recording_sound(SoundPlayerHelper* sound_player);


#endif
