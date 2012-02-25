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
#include "camera-ui2-gconf-settings.h"
#include "camera-settings.h"
#include <time.h>
#include <gconf/gconf-client.h>

#define GC_SCENE_MODE_KEY "/apps/camera/settings/basic-settings/scene-mode"
#define GC_FLASH_MODE_KEY "/apps/camera/settings/still-settings/flash-mode"
#define GC_WHITE_BALANCE_KEY "/apps/camera/settings/basic-settings/wb-mode"
#define GC_ISO_LEVEL_KEY "/apps/camera/settings/basic-settings/iso-level"
#define GC_EXPOSURE_LEVEL_KEY "/apps/camera/settings/basic-settings/ev-level"
#define GC_STILL_RESOLUTION_SIZE_KEY "/apps/camera/settings/still-settings/size"
#define GC_VIDEO_RESOLUTION_SIZE_KEY "/apps/camera/settings/video-settings/size"
#define GC_VIDEO_MIC_MODE_KEY "/apps/camera/settings/video-settings/sound-mode"
#define GC_STORAGE_DEVICE_KEY "/apps/camera/settings/basic-settings/storage-device"
#define GC_AUTHOR_KEY "/apps/camera/settings/basic-settings/author"
#define GC_DEVICE_ID_KEY "/apps/camera/settings/basic-settings/device-id"
#define GC_PREVIEW_MODE_KEY "/apps/camera/settings/basic-settings/post-capture-timeout"
#define GC_LAST_MEDIA_ID_KEY "/apps/camera/settings/basic-settings/last-media-id"
#define GC_LAST_DATE_KEY "/apps/camera/settings/basic-settings/last-date"

#define GC_EXTRA_SHOW_ON_LENSCOVER_KEY "/apps/camera/settings/extra-settings/disable-show-on-lenscover-open"
#define GC_EXTRA_HIDE_ON_LENSCOVER_KEY "/apps/camera/settings/extra-settings/disable-hide-on-lenscover-close"
#define GC_EXTRA_SHOW_ON_FOCUS_PRESSED_KEY "/apps/camera/settings/extra-settings/disable-show-on-focus-pressed"
#define GC_EXTRA_ENABLED_SOUND_EFFECTS "/apps/camera/settings/extra-settings/enable-sound-effects"

#define GC_MMC_PRESENT_KEY "/system/osso/af/mmc-device-present"
#define GC_MMC_USED_KEY "/system/osso/af/mmc-used-over-usb"
#define GC_MMC_COVER_OPEN_KEY "/system/osso/af/mmc-cover-open"
#define GC_MMC_CORRUPTED_KEY "/system/osso/af/mmc/mmc-corrupted"

#define GC_INTERNAL_MMC_PRESENT_KEY "/system/osso/af/internal-mmc-device-present"
#define GC_INTERNAL_MMC_USED_KEY "/system/osso/af/internal-mmc-used-over-usb"
#define GC_INTERNAL_MMC_COVER_OPEN_KEY "/system/osso/af/internal-mmc-cover-open"

static gboolean
_new_day(time_t last_time)
{
  time_t now = time(NULL);
  struct tm* now_date = localtime(&now);
  guint day = now_date->tm_mday;
  guint month = now_date->tm_mon;
  guint year = now_date->tm_year;
  struct tm* last_date = localtime(&last_time);
  if(day != last_date->tm_mday ||
     month != last_date->tm_mon ||
     year != last_date->tm_year)
  {
    return TRUE;
  }
  return FALSE;
}

gint
camera_ui2_get_gconf_scene_mode()
{
  GConfClient* client = gconf_client_get_default();
  gint ret = 0;
  g_assert(GCONF_IS_CLIENT(client));
  ret = gconf_client_get_int(client, GC_SCENE_MODE_KEY, NULL);
  g_object_unref(client);
  return ret;
}

void
camera_ui2_set_gconf_scene_mode(gint scene_mode)
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gconf_client_set_int(client, GC_SCENE_MODE_KEY, scene_mode, NULL);
  g_object_unref(client);
}

gint
camera_ui2_get_gconf_flash_mode()
{
  GConfClient* client = gconf_client_get_default();
  gint ret = 0;
  g_assert(GCONF_IS_CLIENT(client));
  ret = gconf_client_get_int(client, GC_FLASH_MODE_KEY, NULL);
  g_object_unref(client);
  return ret;
}

void
camera_ui2_set_gconf_flash_mode(gint flash_mode)
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gconf_client_set_int(client, GC_FLASH_MODE_KEY, flash_mode, NULL);
  g_object_unref(client);
}

gint
camera_ui2_get_gconf_white_balance_mode()
{
  GConfClient* client = gconf_client_get_default();
  gint ret = 0;
  g_assert(GCONF_IS_CLIENT(client));

  ret = gconf_client_get_int(client, GC_WHITE_BALANCE_KEY, NULL);
  g_object_unref(client);
  return ret;
}

void
camera_ui2_set_gconf_white_balance_mode(gint white_balance)
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gconf_client_set_int(client, GC_WHITE_BALANCE_KEY, white_balance, NULL);
  g_object_unref(client);
}

gint
camera_ui2_get_gconf_iso_level()
{
  GConfClient* client = gconf_client_get_default();
  gint ret = 0;
  g_assert(GCONF_IS_CLIENT(client));

  ret = gconf_client_get_int(client, GC_ISO_LEVEL_KEY, NULL);
  g_object_unref(client);
  return ret;
}

void
camera_ui2_set_gconf_iso_level(gint iso_level)
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gconf_client_set_int(client, GC_ISO_LEVEL_KEY, iso_level, NULL);
  g_object_unref(client);
}

gint
camera_ui2_get_gconf_exposure_level()
{
  GConfClient* client = gconf_client_get_default();
  gint ret = 0;
  g_assert(GCONF_IS_CLIENT(client));

  ret = gconf_client_get_int(client, GC_EXPOSURE_LEVEL_KEY, NULL);
  g_object_unref(client);
  return ret;
}
  
void
camera_ui2_set_gconf_exposure_level(gint exposure_level)
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gconf_client_set_int(client, GC_EXPOSURE_LEVEL_KEY, exposure_level, NULL);
  g_object_unref(client);
}
  
gint
camera_ui2_get_gconf_still_resolution_size()
{
  GConfClient* client = gconf_client_get_default();
  gint ret = 0;
  g_assert(GCONF_IS_CLIENT(client));

  ret = gconf_client_get_int(client, GC_STILL_RESOLUTION_SIZE_KEY, NULL);
  g_object_unref(client);
  return ret;
}

void
camera_ui2_set_gconf_still_resolution_size(gint size)
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gconf_client_set_int(client, GC_STILL_RESOLUTION_SIZE_KEY, size, NULL);
  g_object_unref(client);
}
  
int
camera_ui2_get_gconf_video_resolution_size()
{
  GConfClient* client = gconf_client_get_default();
  gint ret = 0;
  g_assert(GCONF_IS_CLIENT(client));

  ret = gconf_client_get_int(client, GC_VIDEO_RESOLUTION_SIZE_KEY, NULL);
  g_object_unref(client);
  return ret;
}

void
camera_ui2_set_gconf_video_resolution_size(gint size)
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gconf_client_set_int(client, GC_VIDEO_RESOLUTION_SIZE_KEY, size, NULL);
  g_object_unref(client);
}
int
camera_ui2_get_gconf_video_mic_mode()
{
  GConfClient* client = gconf_client_get_default();
  gint ret = 0;
  g_assert(GCONF_IS_CLIENT(client));

  ret = gconf_client_get_int(client, GC_VIDEO_MIC_MODE_KEY, NULL);
  if(ret == -1)
    ret = 0;
  g_object_unref(client);
  return ret;
}

void
camera_ui2_set_gconf_video_mic_mode(gint size)
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gconf_client_set_int(client, GC_VIDEO_MIC_MODE_KEY, size, NULL);
  g_object_unref(client);
}
   
int
camera_ui2_get_gconf_storage_device()
{
  GConfClient* client = gconf_client_get_default();
  gint ret = 0;
  g_assert(GCONF_IS_CLIENT(client));
  ret = gconf_client_get_int(client, GC_STORAGE_DEVICE_KEY, NULL);
  if(ret < 0)
    ret = CAM_STORAGE_INTERN;
  g_object_unref(client);
  return ret;
}

void
camera_ui2_set_gconf_storage_device(gint storage_device)
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gconf_client_set_int(client, GC_STORAGE_DEVICE_KEY, storage_device, NULL);
  g_object_unref(client);
}
  
int
camera_ui2_get_gconf_preview_mode()
{
  GConfClient* client = gconf_client_get_default();
  gint ret = 0;
  g_assert(GCONF_IS_CLIENT(client));
  ret = gconf_client_get_int(client, GC_PREVIEW_MODE_KEY, NULL);
  g_object_unref(client);
  return ret;
}

void
camera_ui2_set_gconf_preview_mode(gint preview_mode)
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gconf_client_set_int(client, GC_PREVIEW_MODE_KEY, preview_mode, NULL);
  g_object_unref(client);
}

gchar*
camera_ui2_get_gconf_author()
{
  GConfClient* client = gconf_client_get_default();
  gchar* ret = NULL;
  g_assert(GCONF_IS_CLIENT(client));

  ret = gconf_client_get_string(client, GC_AUTHOR_KEY, NULL);
  g_object_unref(client);
  return ret;
}

void
camera_ui2_set_gconf_author(gchar* author)
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gconf_client_set_string(client, GC_AUTHOR_KEY, author, NULL);
  g_object_unref(client);
}
  
gint
camera_ui2_get_gconf_last_media_id()
{
  GConfClient* client = gconf_client_get_default();
  guint ret = 0;
  g_assert(GCONF_IS_CLIENT(client));
  if(_new_day(gconf_client_get_int(client, GC_LAST_DATE_KEY, NULL)))
  {
    gconf_client_set_int(client, GC_LAST_DATE_KEY, time(NULL), NULL);
    gconf_client_set_int(client, GC_LAST_MEDIA_ID_KEY, 0, NULL);
  }
  else
  {
    ret = gconf_client_get_int(client, GC_LAST_MEDIA_ID_KEY, NULL);
  }
  g_object_unref(client);
  return ret;
}
 
void
camera_ui2_increment_gconf_last_media_id()
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  guint last_media_id = gconf_client_get_int(client, GC_LAST_MEDIA_ID_KEY, NULL);
  gconf_client_set_int(client, GC_LAST_MEDIA_ID_KEY, last_media_id+1, NULL);
  g_object_unref(client);
}

gboolean
camera_ui2_mmc_gconf_available()
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gboolean ret = FALSE;
  if(gconf_client_get_bool(client, GC_MMC_PRESENT_KEY, NULL) &&
     !gconf_client_get_bool(client, GC_MMC_USED_KEY, NULL) &&
     !gconf_client_get_bool(client, GC_MMC_COVER_OPEN_KEY, NULL) &&
     !gconf_client_get_bool(client, GC_MMC_CORRUPTED_KEY, NULL))
    ret = TRUE;
  g_object_unref(client);
  return ret;
}

gboolean
camera_ui2_internal_mmc_gconf_available()
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gboolean ret = FALSE;
  if(gconf_client_get_bool(client, GC_INTERNAL_MMC_PRESENT_KEY, NULL) &&
     !gconf_client_get_bool(client, GC_INTERNAL_MMC_USED_KEY, NULL) &&
     !gconf_client_get_bool(client, GC_INTERNAL_MMC_COVER_OPEN_KEY, NULL))
    ret = TRUE;
  g_object_unref(client);
  return ret;
}

gchar*
camera_ui2_get_gconf_device_id()
{
  GConfClient* client = gconf_client_get_default();
  gchar* ret = NULL;
  g_assert(GCONF_IS_CLIENT(client));
  ret = gconf_client_get_string(client, GC_DEVICE_ID_KEY, NULL);
  g_object_unref(client);
  return ret;
}

gboolean
camera_ui2_get_gconf_show_on_lenscover_open()
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gboolean ret = gconf_client_get_bool(client, GC_EXTRA_SHOW_ON_LENSCOVER_KEY, NULL);
  g_object_unref(client);
  return ret;
}

void
camera_ui2_set_gconf_show_on_lenscover_open(gboolean disable)
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gconf_client_set_bool(client, GC_EXTRA_SHOW_ON_LENSCOVER_KEY, disable, NULL);
  g_object_unref(client);
}

gboolean
camera_ui2_get_gconf_hide_on_lenscover_close()
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gboolean ret = gconf_client_get_bool(client, GC_EXTRA_HIDE_ON_LENSCOVER_KEY, NULL);
  g_object_unref(client);
  return ret;
}

void
camera_ui2_set_gconf_hide_on_lenscover_close(gboolean disable)
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gconf_client_set_bool(client, GC_EXTRA_HIDE_ON_LENSCOVER_KEY, disable, NULL);
  g_object_unref(client);
}

gboolean
camera_ui2_get_gconf_show_on_focus_pressed()
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gboolean ret = gconf_client_get_bool(client, GC_EXTRA_SHOW_ON_FOCUS_PRESSED_KEY, NULL);
  g_object_unref(client);
  return ret;
}

void
camera_ui2_set_gconf_show_on_focus_pressed(gboolean disable)
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gconf_client_set_bool(client, GC_EXTRA_SHOW_ON_FOCUS_PRESSED_KEY, disable, NULL);
  g_object_unref(client);
}

gboolean
camera_ui2_get_gconf_enabled_sound_effects()
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gboolean ret = gconf_client_get_bool(client, GC_EXTRA_ENABLED_SOUND_EFFECTS, NULL);
  g_object_unref(client);
  return ret;
}

void
camera_ui2_set_gconf_enabled_sound_effects(gboolean enabled)
{
  GConfClient* client = gconf_client_get_default();
  g_assert(GCONF_IS_CLIENT(client));
  gconf_client_set_bool(client, GC_EXTRA_ENABLED_SOUND_EFFECTS, enabled, NULL);
  g_object_unref(client);
}
