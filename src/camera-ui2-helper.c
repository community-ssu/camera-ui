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
#include <libintl.h>
#include <locale.h>
#include "camera-ui2-helper.h"
#include "camera-ui2-defines.h"
#include "camera-settings.h"

const gchar*
scene_mode_icon_name(gint scene_mode, gboolean pressed)
{
  if(pressed)
    return SCENE_MODE_ICON[scene_mode][1];
  else
    return SCENE_MODE_ICON[scene_mode][0];
}

const gchar*
flash_mode_icon_name(gint flash_mode, gboolean pressed)
{
  if(pressed)
    return FLASH_MODE_ICON[flash_mode][1];
  else
    return FLASH_MODE_ICON[flash_mode][0];
}

const gchar*
settings_icon_name(gboolean pressed)
{
  if(pressed)
    return SETTINGS_ICON[1];
  else
    return SETTINGS_ICON[0];
}

const gchar*
white_balance_mode_icon_name(gint white_balance_mode, gboolean pressed)
{
  if(pressed)
    return WHITE_BALANCE_ICON[white_balance_mode][1];
  else
    return WHITE_BALANCE_ICON[white_balance_mode][0];
}

const gchar*
iso_level_icon_name(gint iso_level, gboolean pressed)
{
  if(pressed)
    return ISO_LEVEL_ICON[iso_level][1];
  else
    return ISO_LEVEL_ICON[iso_level][0];
}

const gchar*
white_balance_mode_name(gint white_balance_mode)
{
  return dgettext("osso-camera-ui", WHITE_BALANCE_NAME[white_balance_mode]);
}

const gchar*
iso_level_name(gint iso_level)
{
  return dgettext("osso-camera-ui", ISO_LEVEL_NAME[iso_level]);
}

const gchar*
exposure_level_icon_name(gint exposure_level, gboolean pressed)
{
  if(pressed)
    return EXPOSURE_LEVEL_ICON[exposure_level][1];
  else
    return EXPOSURE_LEVEL_ICON[exposure_level][0];
}

const gchar*
exposure_level_name(gint exposure_level)
{
  return EXPOSURE_LEVEL_NAME[exposure_level];
}

const gchar*
still_resolution_size_icon_name(gint still_resolution_size, gboolean pressed)
{
  if(pressed)
    return STILL_RESOLUTION_SIZE_ICON[still_resolution_size][1];
  else
    return STILL_RESOLUTION_SIZE_ICON[still_resolution_size][0];
}


const gchar*
still_resolution_size_name(gint still_resolution_size)
{
  return dgettext("osso-camera-ui", STILL_RESOLUTION_SIZE_NAME[still_resolution_size]);
}

const gchar*
video_resolution_size_icon_name(gint video_resolution_size, gboolean pressed)
{
  if(video_resolution_size >=CAM_VIDEO_RESOLUTION_LOW && video_resolution_size <= CAM_VIDEO_RESOLUTION_DVD_16X9)
  {
    if(pressed)
      return VIDEO_RESOLUTION_SIZE_ICON[video_resolution_size - CAM_VIDEO_RESOLUTION_LOW][1];
    else
      return VIDEO_RESOLUTION_SIZE_ICON[video_resolution_size - CAM_VIDEO_RESOLUTION_LOW][0];
  }
  else
  {
    return VIDEO_RESOLUTION_SIZE_ICON[0][0];
  }
}

const gchar*
video_resolution_size_name(gint video_resolution_size)
{
  if(video_resolution_size >= CAM_VIDEO_RESOLUTION_LOW && video_resolution_size <= CAM_VIDEO_RESOLUTION_DVD_16X9)
    return dgettext("osso-camera-ui", VIDEO_RESOLUTION_SIZE_NAME[video_resolution_size-CAM_VIDEO_RESOLUTION_LOW]);
  else
    return NULL;
}

const gchar*
video_mic_mode_icon_name(gint mic_mode, gboolean pressed)
{
  if(pressed)
    return VIDEO_MIC_MODE_ICON[mic_mode][1];
  else
    return VIDEO_MIC_MODE_ICON[mic_mode][0];
}

const gchar*
storage_device_icon_name(gint storage_device, gboolean pressed)
{
  if(pressed)
    return STORAGE_DEVICE_ICON[storage_device][1];
  else
    return STORAGE_DEVICE_ICON[storage_device][0];
}

const gchar*
storage_device_name(gint storage_device)
{
  return STORAGE_DEVICE_NAME[storage_device];
}

gboolean
is_video_mode(CamSceneMode scene_mode)
{
  return scene_mode == CAM_SCENE_MODE_VIDEO ||
    scene_mode == CAM_SCENE_MODE_NIGHT_VIDEO;
}

const gchar*
video_state_icon_name(gint video_state, gboolean pressed)
{
  if(pressed)
    return VIDEO_STATE_ICON_NAME[video_state][1];
  else
    return VIDEO_STATE_ICON_NAME[video_state][0];
}
