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
#include "sound-helper.h"
#include <canberra.h>
#include <glib.h>
#include <libprofile.h>

struct _SoundPlayerHelper
{
  ca_context* ca_ctxt;
  ca_proplist* ca_pl;
  gboolean enabled;
};


#if 0
static void
sound_done(ca_context *c,
	   uint32_t id,
	   int error_code,
	   void *userdata)
{
  // ???
}
#endif

static void
profile_active_cb(const char *profile, 
		  const char *key, 
		  const char *val, 
		  const char *type, void *user_data)
{
  g_warning("profile %s\n", profile);
  g_warning("key %s\n", key);
  g_warning("value %s\n", val);
  g_warning("type %s\n", type);
  if(g_strcmp0(key, "system.sound.level") == 0)
  {
    ((SoundPlayerHelper*)user_data)->enabled =
      (g_strcmp0(val,"0")!=0);
  }
}

SoundPlayerHelper*
sound_player_create()
{
  SoundPlayerHelper* helper = g_new0(SoundPlayerHelper,1);
  
  char* current_profile = profile_get_profile();
  helper->enabled = (profile_get_value_as_int(current_profile, "system.sound.level")!=0);
  g_free(current_profile);
  ca_context_create(&helper->ca_ctxt);
  ca_proplist_create(&helper->ca_pl);
  ca_proplist_sets(helper->ca_pl, CA_PROP_MEDIA_FILENAME, "/usr/share/sounds/camera_snd_title_3.wav");
  ca_proplist_setf(helper->ca_pl, CA_PROP_CANBERRA_VOLUME, "%f", 0.0);
  
  profile_track_add_active_cb(profile_active_cb,
			      helper,
			      NULL);
  profile_tracker_init();
  if(helper->ca_ctxt)
    ca_context_open(helper->ca_ctxt);
  return helper;
}

void
sound_player_destroy(SoundPlayerHelper* sound_player)
{
  profile_tracker_quit();
  ca_proplist_destroy(sound_player->ca_pl);
  sound_player->ca_pl = NULL;
  if(sound_player->ca_ctxt)
    ca_context_destroy(sound_player->ca_ctxt);
  sound_player->ca_ctxt = NULL;
  g_free(sound_player);
}


void sound_player_capture_sound(SoundPlayerHelper* sound_player)
{
  if(sound_player->enabled)
  {
    ca_proplist_sets(sound_player->ca_pl, CA_PROP_MEDIA_FILENAME, "/usr/share/sounds/camera_snd_title_1.wav");
    ca_context_play_full(sound_player->ca_ctxt, 0, sound_player->ca_pl, NULL, NULL);
  }
}

void sound_player_focus_done_sound(SoundPlayerHelper* sound_player)
{
  if(sound_player->enabled)
  {
    ca_proplist_sets(sound_player->ca_pl, CA_PROP_MEDIA_FILENAME, "/usr/share/sounds/camera_snd_title_3.wav");
    ca_context_play_full(sound_player->ca_ctxt, 0, sound_player->ca_pl, NULL, NULL);
  }
}

void sound_player_start_recording_sound(SoundPlayerHelper* sound_player)
{
  if(sound_player->enabled)
  {
    ca_proplist_sets(sound_player->ca_pl, CA_PROP_MEDIA_FILENAME, "/usr/share/sounds/camera_snd_title_2.wav");
    ca_context_play_full(sound_player->ca_ctxt, 0, sound_player->ca_pl, NULL, NULL);
  }
}
