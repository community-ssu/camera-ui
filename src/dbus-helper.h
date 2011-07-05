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
#ifndef _DBUS_HELPER_H_
#define _DBUS_HELPER_H_

#include <libosso.h>
#include <libhal.h>

gboolean
dbus_helper_is_camera_launcher_button_pressed(osso_context_t* osso);

gboolean
dbus_helper_is_camera_focus_button_pressed(osso_context_t* osso);

gboolean
dbus_helper_is_camera_lenscover_closed(osso_context_t* osso);

/*
  start image viewer (all images)
*/
void
dbus_helper_start_image_viewer(osso_context_t* osso);

/*
  start media player (category view)
*/
void
dbus_helper_start_media_player(osso_context_t* osso);

gint
dbus_helper_get_device_orientation_mode(osso_context_t* osso);

gint
dbus_helper_get_device_locked(osso_context_t* osso);

gboolean
dbus_helper_register_hal_cam_property(LibHalContext* hal_context);

void
dbus_helper_unregister_hal_cam_property(LibHalContext* hal_context);

/*
  for image files, start the image
*/
void
dbus_helper_start_viewer(osso_context_t* osso, const gchar* media_file);

/*
  for video files, start mediaplayer and start playing the file
*/
void
dbus_helper_start_mime_viewer(osso_context_t* osso, const gchar* media_file, const gchar* mime_type);

#endif
