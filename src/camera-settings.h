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
#ifndef _CAMERA_SETTINGS_H_
#define _CAMERA_SETTINGS_H_
#include <glib.h>

typedef struct _capture_data_t
{
  gchar* filename;
  gint orientation_mode;
  gdouble longitude;
  gdouble latitude;
  gdouble altitude;
  gchar* country;
  gchar* city;
  gchar* suburb;
  gchar* author;
  gchar* classification_id;
} capture_data_t;

typedef struct _AppSettings
{
  gboolean show_on_lenscover_open;
  gboolean hide_on_lenscover_close;
  gboolean show_on_focus_button_press;
  gboolean enable_sound_effects;
} AppSettings;

typedef struct _CameraSettings CameraSettings;

typedef enum {
  CAM_SCENE_MODE_AUTO = 0,
  CAM_SCENE_MODE_LANDSCAPE = 1,
  CAM_SCENE_MODE_NIGHT = 2,
  CAM_SCENE_MODE_PORTRAIT = 3,
  CAM_SCENE_MODE_MACRO = 4,
  CAM_SCENE_MODE_ACTION = 5,
  CAM_SCENE_MODE_VIDEO = 6,
  CAM_SCENE_MODE_NIGHT_VIDEO = 7
} CamSceneMode;

typedef enum {
  CAM_FLASH_MODE_AUTO = 0,
  CAM_FLASH_MODE_ON = 1,
  CAM_FLASH_MODE_OFF = 2,
  CAM_FLASH_MODE_REDEYE = 3,
} CamFlashMode;

typedef enum {
  CAM_WHITE_BALANCE_AUTO = 0,
  CAM_WHITE_BALANCE_SUNNY = 1,
  CAM_WHITE_BALANCE_CLOUDY = 2,
  CAM_WHITE_BALANCE_FLOURESCENT = 3,
  CAM_WHITE_BALANCE_INCANDESCENT = 4,
} CamWhiteBalance;

typedef enum {
  CAM_ISO_LEVEL_AUTO = 0,
  CAM_ISO_LEVEL_100 = 1,
  CAM_ISO_LEVEL_200 = 2,
  CAM_ISO_LEVEL_400 = 3,
  CAM_ISO_LEVEL_800 = 4,
  CAM_ISO_LEVEL_1600 = 5,
} CamIsoLevel;

typedef enum {
  CAM_STILL_RESOLUTION_LOW = 0,
  CAM_STILL_RESOLUTION_MEDIUM = 1,
  CAM_STILL_RESOLUTION_HIGH = 2,
  CAM_STILL_RESOLUTION_WIDE = 3
} CamStillResolution;

typedef enum {
  CAM_VIDEO_RESOLUTION_LOW = 6,
  CAM_VIDEO_RESOLUTION_MEDIUM = 7,
  CAM_VIDEO_RESOLUTION_HIGH = 8,
  CAM_VIDEO_RESOLUTION_HD_4X3 = 9,
  CAM_VIDEO_RESOLUTION_HD_16X9 = 10,
  CAM_VIDEO_RESOLUTION_DVD_4X3 = 11,
  CAM_VIDEO_RESOLUTION_DVD_16X9 = 12,
} CamVideoResolution;

typedef enum {
  CAM_STORAGE_EXTERN = 0,
  CAM_STORAGE_INTERN = 1,
  CAM_STORAGE_EXTERN_UNAVAILABLE = 2
} CamStorageDevice;

typedef enum {
  CAM_VIDEO_MIC_ON = 0,
  CAM_VIDEO_MIC_OFF =1,
} CamMicMode;

typedef enum {
  CAM_EXPOSURE_COMP_M2 = 0,
  CAM_EXPOSURE_COMP_M15 = 1,
  CAM_EXPOSURE_COMP_M1 = 2,
  CAM_EXPOSURE_COMP_M05 = 3,
  CAM_EXPOSURE_COMP_0 = 4,
  CAM_EXPOSURE_COMP_P05 = 5,
  CAM_EXPOSURE_COMP_P1 = 6,
  CAM_EXPOSURE_COMP_P15 = 7,
  CAM_EXPOSURE_COMP_P2 = 8,
} CamExposureComp;

typedef enum {
  CAM_PREVIEW_MODE_2SEC = 0,
  CAM_PREVIEW_MODE_4SEC = 1,
  CAM_PREVIEW_MODE_6SEC = 2,
  CAM_PREVIEW_MODE_NO_TIMEOUT = 3,
  CAM_PREVIEW_MODE_NO_PREVIEW = 4,
} CamPreviewMode;

typedef enum {
  CAM_VIDEO_STATE_STOPPED = 0,
  CAM_VIDEO_STATE_PAUSED = 1,
  CAM_VIDEO_STATE_RECORDING = 2,
} VideoState;

typedef enum {
  CAM_BACK_CAM = 0,
  CAM_FRONT_CAM = 1,
} Camera;

typedef enum {
  CAM_PRIORITY_IDLE = 0,
  CAM_PRIORITY_RECORDING = 1,
} CamPriority;

struct _CameraSettings
{
  gint scene_mode;
  gint flash_mode;
  gint white_balance;
  gint iso_level;
  gint exposure_level;
  gint storage_device;
  gint still_resolution_size;
  gint video_resolution_size;
  gint color_mode;
  gint mic_mode;
  gint preview_mode;
  gint video_state;
  gchar* author;
};

#endif
