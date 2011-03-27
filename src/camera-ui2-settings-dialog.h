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
#ifndef _CAMERA_UI2_SETTINGS_DIALOG_H_
#define _CAMERA_UI2_SETTINGS_DIALOG_H_
#include "camera-settings.h"
#include "geotagging-helper.h"


guint
show_capture_timer_dialog();

void
show_app_settings_dialog(AppSettings* app_settings);

void
show_scene_mode_selection_dialog(CamSceneMode* scene_mode);

void
show_flash_mode_selection_dialog(CamFlashMode* flash_mode);

void
show_color_mode_selection_dialog(gint* color_mode);

void
show_white_balance_mode_selection_dialog(CamWhiteBalance* white_balance);

void
show_iso_level_selection_dialog(CamIsoLevel* iso_level);

void
show_geotagging_selection_dialog(GeoTagMode* mode);
#endif
