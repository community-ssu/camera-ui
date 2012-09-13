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
#ifndef _CAMERA_UI2_GCONF_SETTINGS_H_
#define _CAMERA_UI2_GCONF_SETTINGS_H_
#include <glib.h>

gint
camera_ui2_get_gconf_scene_mode();

void
camera_ui2_set_gconf_scene_mode(gint scene_mode);

gint
camera_ui2_get_gconf_flash_mode();
  
void
camera_ui2_set_gconf_flash_mode(gint flash_mode);
  
gint
camera_ui2_get_gconf_white_balance_mode();

void
camera_ui2_set_gconf_white_balance_mode(gint white_balance);

gint
camera_ui2_get_gconf_iso_level();
  
void
camera_ui2_set_gconf_iso_level(gint iso_level);
  
gint
camera_ui2_get_gconf_exposure_level();
  
void
camera_ui2_set_gconf_exposure_level(gint exposure_level);
  
gint
camera_ui2_get_gconf_still_resolution_size();
  
void
camera_ui2_set_gconf_still_resolution_size(gint size);
  
gint
camera_ui2_get_gconf_video_resolution_size();
  
void
camera_ui2_set_gconf_video_resolution_size(gint size);
  
gint
camera_ui2_get_gconf_video_mic_mode_size();
  
void
camera_ui2_set_gconf_video_mic_mode_size(gint mic_mode);

gint
camera_ui2_get_gconf_srorage_device();
  
void
camera_ui2_set_gconf_storage_device(gint storage_device);

gint
camera_ui2_get_gconf_preview_mode();
  
void
camera_ui2_set_gconf_preview_mode(gint preview_mode);
 
gchar*
camera_ui2_get_gconf_author();
  
void
camera_ui2_set_gconf_author(gchar* author);
 
gint
camera_ui2_get_gconf_last_media_id();
 
void
camera_ui2_increment_gconf_last_media_id();
 
gboolean
camera_ui2_mmc_gconf_available();

gboolean
camera_ui2_internal_mmc_gconf_available();

gchar*
camera_ui2_get_gconf_device_id();

gboolean
camera_ui2_get_gconf_show_on_lenscover_open();

void
camera_ui2_set_gconf_show_on_lenscover_open(gboolean enable);

gboolean
camera_ui2_get_gconf_hide_on_lenscover_close();

void
camera_ui2_set_gconf_hide_on_lenscover_close(gboolean enable);

gboolean
camera_ui2_get_gconf_show_on_focus_pressed();

void
camera_ui2_set_gconf_show_on_focus_pressed(gboolean enable);

gboolean
camera_ui2_get_gconf_enabled_sound_effects();

void
camera_ui2_set_gconf_enabled_sound_effects(gboolean enable);

int
camera_ui2_get_gconf_storage_device();

int
camera_ui2_get_gconf_video_mic_mode();

#endif
