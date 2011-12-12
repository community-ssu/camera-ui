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
#ifndef _CAMERA_INTERFACE_H_
#define _CAMERA_INTERFACE_H_
#include "camera-settings.h"
#include <gst/interfaces/photography.h>

typedef struct _CameraCaptureData
{
  gint capture_iso_speed;
  gint capture_aperture_n;
  gint capture_aperture_d;
  gint capture_contrast;
  gint capture_saturation;
  gint capture_shutter_speed_n;
  gint capture_shutter_speed_d;
  gint capture_exposure_time_n;
  gint capture_exposure_time_d;
  guint capture_flash;
  gint capture_fnumber_n;
  gint capture_fnumber_d;
  gint capture_focal_length_n;
  gint capture_focal_length_d;
  guint capture_gain;
  guint capture_light_source;
  gchar* date_time_stamp;
}CameraCaptureData;

typedef struct _CameraInterface CameraInterface;

CameraInterface* create_camera_interface();

void
camera_interface_set_settings(CameraInterface* camera_interface, CameraSettings* settings);

gboolean
camera_interface_set_scene_mode(CameraInterface* camera_interface, CamSceneMode scene_mode);

gboolean
camera_interface_set_still_resolution(CameraInterface* camera_interface, CamStillResolution resolution);

gboolean
camera_interface_set_video_resolution(CameraInterface* camera_interface, CamVideoResolution resolution);

gboolean
camera_interface_set_white_balance_mode(CameraInterface* camera_interface, CamWhiteBalance white_balance);

gboolean
camera_interface_set_flash_mode(CameraInterface* camera_interface, CamFlashMode flash_mode);

gboolean
camera_interface_set_iso_level(CameraInterface* camera_interface, CamIsoLevel iso_level);

gboolean
camera_interface_set_exposure_comp(CameraInterface* camera_interface, CamExposureComp comp);

gboolean
camera_interface_set_colour_tone_mode(CameraInterface* camera_interface, GstColourToneMode mode);

gint
camera_interface_get_color_key(CameraInterface* camera_interface);

gboolean
camera_interface_open_viewfinder(CameraInterface* camera_interface, gulong viewfinder_window_id);

gboolean
camera_interface_close_viewfinder(CameraInterface* camera_interface);

gboolean
camera_interface_capture_image(CameraInterface* camera_interface, capture_data_t capture_data);

gboolean
camera_interface_start_recording(CameraInterface* camera_interface, capture_data_t capture_data);

gboolean
camera_interface_stop_recording(CameraInterface* camera_interface);

gboolean
camera_interface_pause_recording(CameraInterface* camera_interface);

gboolean
camera_interface_resume_recording(CameraInterface* camera_interface);

gboolean
camera_interface_enable_preview(CameraInterface* camera_interface, gboolean enable);

gdouble
camera_interface_increase_zoom(CameraInterface* camera_interface);

gdouble
camera_interface_decrease_zoom(CameraInterface* camera_interface);

gboolean
camera_interface_autofocus(CameraInterface* camera_interface, gboolean start_stop);

gboolean
camera_interface_set_focus_mode(CameraInterface* camera_interface, gboolean start_stop);

gboolean
camera_interface_set_audio_mode(CameraInterface* camera_interface, CamMicMode mic_mode);

gboolean
camera_interface_frontcamera_mode(CameraInterface* camera_interface);

gboolean
camera_interface_backcamera_mode(CameraInterface* camera_interface);

gulong
camera_interface_signal_connect(CameraInterface* camera_interface, const gchar* signal, GCallback call_back, gpointer user_data);

void
camera_interface_toggle_light(CameraInterface* camera_interface);

void
camera_interface_toggle_privacy_light(CameraInterface* camera_interface);

void
camera_interface_increase_focus(CameraInterface* camera_interface, gint value);

void
camera_interface_decrease_focus(CameraInterface* camera_interface, gint value);

void
camera_interface_set_raw_image_mode(CameraInterface* camera_interface, gboolean enable);

gint
camera_interface_get_focus(CameraInterface* camera_interface);

guint8*
camera_interface_get_raw_data();

void
camera_interface_get_camera_capture_data(CameraInterface* camera_interface, CameraCaptureData** cc);

void
camera_interface_set_priority(CamPriority camera_priority);

#endif
