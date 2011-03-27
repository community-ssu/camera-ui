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
#ifndef _CAMERA_UI2_HELPER_H_
#define _CAMERA_UI2_HELPER_H_

#include <gtk/gtk.h>
#include "camera-settings.h"

const gchar*
scene_mode_icon_name(gint scene_mode, gboolean pressed);

const gchar*
flash_mode_icon_name(gint scene_mode, gboolean pressed);

const gchar*
settings_icon_name(gboolean pressed);

const gchar*
white_balance_mode_icon_name(gint white_balance_mode, gboolean pressed);

const gchar*
white_balance_mode_name(gint white_balance_mode);

const gchar*
iso_level_icon_name(gint iso_level, gboolean pressed);

const gchar*
iso_level_name(gint iso_level);

const gchar*
exposure_level_icon_name(gint exposure_level, gboolean pressed);

const gchar*
exposure_level_name(gint exposure_level);

const gchar*
still_resolution_size_icon_name(gint still_resolution_size, gboolean pressed);

const gchar*
still_resolution_size_name(gint still_resolution);

const gchar*
video_resolution_size_icon_name(gint video_resolution_size, gboolean pressed);

const gchar*
video_resolution_size_name(gint video_resolution);

const gchar*
video_mic_mode_icon_name(gint mic_mode, gboolean pressed);

const gchar*
storage_device_name(gint storage);

const gchar* 
storage_device_icon_name(gint storage, gboolean pressed);

const gchar*
video_state_icon_name(gint video_state, gboolean pressed);

gboolean
is_video_mode(CamSceneMode scene_mode);
#endif
