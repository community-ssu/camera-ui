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
#ifndef _CAMERA_UI2_DEFINES_H_
#define _CAMERA_UI2_DEFINES_H_
#include <glib.h>

const gchar* SCENE_MODE_ICON[][2] = 
  {{"camera_auto_mode", "camera_auto_mode_pressed"},
   {"camera_scene_mode_landscape", "camera_scene_mode_landscape_pressed"},
   {"camera_scene_mode_night", "camera_scene_mode_night_pressed"},
   {"camera_scene_mode_portrait", "camera_scene_mode_portrait_pressed"},
   {"camera_scene_mode_macro", "camera_scene_mode_macro_pressed"},
   {"camera_scene_mode_sport", "camera_scene_mode_sport_pressed"},
   {"camera_scene_mode_video", "camera_scene_mode_video_pressed"},
   {"camera_scene_mode_night_video", "camera_scene_mode_night_video_pressed"}
  };

const gchar* FLASH_MODE_ICON[][2] = 
  {{"camera_flash_auto", "camera_flash_auto_pressed"},
   {"camera_flash_fill", "camera_flash_fill_pressed"},
   {"camera_flash_off", "camera_flash_off_pressed"},
   {"camera_flash_redeye", "camera_flash_redeye_pressed"},
  };

const gchar* WHITE_BALANCE_ICON[][2] = 
  {{"camera_auto_mode", "camera_auto_mode_pressed"},
   {"camera_white_balance_sunny", "camera_white_balance_sunny_pressed"},
   {"camera_white_balance_cloudy", "camera_white_balance_cloudy_pressed"},
   {"camera_white_balance_flourescent", "camera_white_balance_flourescent_pressed"},
   {"camera_white_balance_incandescent", "camera_white_balance_incandescent_pressed"},
  };

const gchar* WHITE_BALANCE_NAME[] = 
  {"camera_bd_wb_automatic",
   "camera_bd_wb_sunny",
   "camera_bd_wb_cloudy",
   "camera_bd_wb_fluorescent",
   "camera_bd_wb_tungsten",
  };

const gchar* ISO_LEVEL_ICON[][2] = 
  {{"camera_isoAuto", "camera_isoAuto_pressed"},
   {"camera_iso100", "camera_iso100_pressed"},
   {"camera_iso200", "camera_iso200_pressed"},
   {"camera_iso400", "camera_iso400_pressed"},
   {"camera_iso800", "camera_iso800_pressed"},
   {"camera_iso1600", "camera_iso1600_pressed"},
  };

const gchar* ISO_LEVEL_NAME[] = 
  {"camera_bd_iso_automatic",
   "camera_bd_iso_100",
   "camera_bd_iso_200",
   "camera_bd_iso_400",
   "800",
   "1600",
  };

const gchar* SETTINGS_ICON[2] = 
  {"camera_camera_setting", "camera_camera_setting_pressed"};

const gchar* EXPOSURE_LEVEL_ICON[][2] =
  {{"camera_exposure_minus2","camera_exposure_minus2_pressed"},
   {"camera_exposure_minus1half","camera_exposure_minus1half_pressed"},
   {"camera_exposure_minus1","camera_exposure_minus1_pressed"},
   {"camera_exposure_minus0half","camera_exposure_minus0half_pressed"},
   {"camera_exposure_0","camera_exposure_0_pressed"},
   {"camera_exposure_plus0half","camera_exposure_plus0half_pressed"},
   {"camera_exposure_plus1","camera_exposure_plus1_pressed"},
   {"camera_exposure_plus1half","camera_exposure_plus1half_pressed"},
   {"camera_exposure_2","camera_exposure_2_pressed"},
  };

const gchar* EXPOSURE_LEVEL_NAME[] =
  {"-2.0",
   "-1.5",
   "-1.0",
   "-0.5",
   "0.0",
   "0.5",
   "1.0",
   "1.5",
   "2.0",
  };

const gchar* STILL_RESOLUTION_SIZE_ICON[][2] = 
  {{"camera_image_size1_3mpixel","camera_image_size1_3mpixel_pressed"},
   {"camera_image_size3mpixel","camera_image_size3mpixel_pressed"},
   {"camera_image_size5mpixel","camera_image_size5mpixel_pressed"},
   {"camera_image_size3_5mpixel","camera_image_size3_5mpixel_pressed"},
  };

const gchar* STILL_RESOLUTION_SIZE_NAME[] =
  {"camera_bd_resolution_low",
   "camera_bd_resolution_medium",
   "camera_bd_resolution_high",
   "camera_bd_resolution_wide",
  };

const gchar* VIDEO_RESOLUTION_SIZE_ICON[][2] = 
  {{"camera_video_resolution_qvga","camera_video_resolution_qvga_pressed"},
   {"camera_video_resolution_vga","camera_video_resolution_vga_pressed"},
   {"camera_video_resolution_wide_screen","camera_video_resolution_wide_screen_pressed"},
   {"camera_video_resolution_vga","camera_video_resolution_vga_pressed"},
   {"camera_video_resolution_wide_screen","camera_video_resolution_wide_screen_pressed"},
   {"camera_video_resolution_vga","camera_video_resolution_vga_pressed"},
   {"camera_video_resolution_wide_screen","camera_video_resolution_wide_screen_pressed"},
  };

const gchar* VIDEO_RESOLUTION_SIZE_NAME[] =
  {"camera_bd_resolution_video_low",
   "camera_bd_resolution_video_fine",
   "camera_bd_resolution_video_high",
   "camera_bd_resolution_video_dvd_4x3",
   "camera_bd_resolution_video_dvd_16x9",
   "camera_bd_resolution_video_hd_4x3",
   "camera_bd_resolution_video_hd_16x9"
  };

const gchar* VIDEO_MIC_MODE_ICON[][2] = 
  {{"camera_volume_unmuted","camera_volume_unmuted_pressed"},
   {"camera_volume_muted","camera_volume_muted_pressed"},
  };

const gchar* STORAGE_DEVICE_ICON[][2] =
  {{"camera_removable_memory_card","camera_removable_memory_card_pressed"},
   {"camera_internal_memory","camera_internal_memory_pressed"},
   {"camera_internal_memory","camera_internal_memory_pressed"},
  };

const gchar* STORAGE_DEVICE_NAME[] =
  {"card", "camera_va_memory_internal", "camera_va_memory_internal"};

const gchar* VIDEO_STATE_ICON_NAME[][2] =
  {{"camera_video_stop", "camera_video_stop_pressed"},
   {"camera_video_pause", "camera_video_pause_pressed"},
   {"camera_video_recording", "camera_video_recording_pressed"}
  };

#endif
