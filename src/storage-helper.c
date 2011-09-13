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
#include "storage-helper.h"
#include "camera-ui2-gconf-settings.h"
#include <locale.h>
#include <libintl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glib.h>

gchar*
storage_helper_get_mmc_name()
{
  GnomeVFSVolumeMonitor* vfs_monitor = gnome_vfs_get_volume_monitor();
  GnomeVFSVolume* mmc_volume = 
    gnome_vfs_volume_monitor_get_volume_for_path(vfs_monitor,
						 g_getenv("MMC_MOUNTPOINT"));
  gchar* name = gnome_vfs_volume_get_display_name(mmc_volume);
  gnome_vfs_volume_unref(mmc_volume);
  return name;
}

static gchar*
format_time_string()
{
  time_t timestamp = time(NULL);
  struct tm* t = localtime(&timestamp);
  gchar filename[255];
  strftime(filename, 255, "%Y%m%d", t);
  return g_strdup(filename);
}

static gchar* 
_find_unused_filename(const gchar* path,
		      const gchar* time_prefix,
		      const gboolean is_video)
{
  gchar* filename = NULL;
  int trials = 0;
  FILE* fd = NULL;
  struct stat stat_buf;
  int stat_ret = 0;

  do {
    guint last_media_id = camera_ui2_get_gconf_last_media_id();
    trials++;

    if(is_video)
      filename = g_strdup_printf("%s/DCIM/%s_%03d.mp4", 
				 path,
				 time_prefix,
				 last_media_id+1);
    else
      filename = g_strdup_printf("%s/DCIM/%s_%03d.jpg",
				 path,
				 time_prefix,
				 last_media_id+1);
    stat_ret = stat(filename, &stat_buf);
    // file already exists?
    if(stat_ret == 0)
    {
      // prepare next try
      g_free(filename);
      filename = NULL;
      camera_ui2_increment_gconf_last_media_id();
    }
  }while(stat_ret == 0 && trials < 99);

  // found nonexistent or running out of trials?
  if(stat_ret == 0)
  {
    g_free(filename);
    filename = NULL;
  }
  return filename;
}

gchar*
storage_helper_create_filename(CamStorageDevice storage_device,
			       CamSceneMode scene_mode)
{
  gchar* filename = NULL;
  gchar* path = NULL;
  if(storage_device == CAM_STORAGE_INTERN &&
     camera_ui2_internal_mmc_gconf_available())
  {
    path = g_strdup(g_getenv("INTERNAL_MMC_MOUNTPOINT"));
  }
  else if(storage_device == CAM_STORAGE_EXTERN &&
	  camera_ui2_mmc_gconf_available())
  {
    path = g_strdup(g_getenv("MMC_MOUNTPOINT"));
  }
  if(path == NULL)
  {
    hildon_banner_show_information(NULL, NULL, dgettext("osso-camera-ui", "camera_ib_no_memory_accessible"));
    return NULL;
  }
  gchar* time_prefix = format_time_string();
  guint last_media_id = camera_ui2_get_gconf_last_media_id();
  filename = _find_unused_filename(path, time_prefix, is_video_mode(scene_mode));
  g_free(path);
  g_free(time_prefix);
  return filename;
}

gchar*
storage_helper_create_unique_id()
{
  gchar random[51];
  FILE* f = fopen("/dev/urandom", "rb");
  int i=0;
  for(i=0;i<25;++i)
  {
    unsigned char c;
    fread(&c, 1, 1, f);
    g_sprintf(random+2*i,"%.2x", c);
  }
  fclose(f);
  random[50] = '\0';
  gchar* device_id = camera_ui2_get_gconf_device_id();
  gchar* key = g_strdup_printf("%s%s", device_id, random);
  gchar* hash = g_compute_checksum_for_string(G_CHECKSUM_SHA1, key, -1);
  g_free(device_id);
  device_id = g_strdup_printf("noki://0/%s%s", hash, random);
  g_free(hash);
  g_free(key);
  return device_id;
}

GnomeVFSFileSize
storage_helper_free_space(CamStorageDevice storage_device)
{
  GnomeVFSFileSize size = 0;
  gchar* volume_path = NULL;
  GnomeVFSURI* volume_uri = NULL;

  if(storage_device == CAM_STORAGE_INTERN)
  {
    volume_path = gnome_vfs_get_uri_from_local_path(g_getenv("INTERNAL_MMC_MOUNTPOINT"));
  }
  else if(storage_device == CAM_STORAGE_EXTERN && camera_ui2_mmc_gconf_available())
  {
    volume_path = gnome_vfs_get_uri_from_local_path(g_getenv("MMC_MOUNTPOINT"));
  }
  else
  {
    return size;
  }
  volume_uri = 
    gnome_vfs_uri_new(volume_path);
  g_free(volume_path);

  if (volume_uri != NULL)
  {
    GnomeVFSResult ret = 
      gnome_vfs_get_volume_free_space(volume_uri,
				      &size);
    gnome_vfs_uri_unref(volume_uri);
  }
  return size;
}
