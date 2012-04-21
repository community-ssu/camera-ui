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
#include "camera-ui2-window.h"
#include "camera-ui2-preview-window.h"
#include "camera-ui2-gconf-settings.h"
#include "camera-ui2-settings-dialog.h"
#include "camera-settings.h"
#include "camera-ui2-helper.h"
#include "camera-interface.h"
#include <gdk/gdkx.h>
#include <libintl.h>
#include <locale.h>
#include <string.h>
#include "dbus-helper.h"
#include "geotagging-helper.h"
#include "storage-helper.h"
#include "sound-helper.h"
#include <gdigicam/gdigicam-manager.h>
#include <glib/gstdio.h>
#include "pmdw.h"
#include <mce/dbus-names.h>
#include <mce/mode-names.h>

struct _CameraUI2WindowPrivate
{
  osso_context_t* osso;
  
  GtkWidget* view_finder;
  GtkWidget* scene_mode_button;
  GtkWidget* scene_mode_image;
  GtkWidget* flash_mode_button;
  GtkWidget* flash_mode_image;
  GtkWidget* color_mode_button;
  GtkWidget* color_mode_image;
  GtkWidget* still_settings_button;
  GtkWidget* still_settings_image;
  GtkWidget* video_settings_button;
  GtkWidget* video_settings_image;
  GtkWidget* video_state_button;
  GtkWidget* video_state_image;
  GtkWidget* video_stop_button;
  GtkWidget* video_stop_image;
  GtkWidget* still_capture_button;
  GtkWidget* still_capture_image;
  GtkWidget* image_viewer_button;
  GtkWidget* image_viewer_image;
  GtkWidget* media_player_button;
  GtkWidget* media_player_image;
  GtkWidget* image_counter_label;
  GtkWidget* focus_label;
  GtkWidget* raw_indicator_label;
  GtkWidget* storage_button;
  GtkWidget* storage_image;
  GtkWidget* still_resolution_button;
  GtkWidget* still_resolution_image;
  GtkWidget* video_resolution_button;
  GtkWidget* video_resolution_image;
  GtkWidget* video_mic_mode_button;
  GtkWidget* video_mic_mode_image;
  GtkWidget* white_balance_button;
  GtkWidget* white_balance_image;
  GtkWidget* iso_level_button;
  GtkWidget* iso_level_image;
  GtkWidget* close_window_button;
  GtkWidget* close_window_image;
  GtkWidget* close_standby_window_button;
  GtkWidget* close_standby_window_image;
  GtkWidget* geotagging_button;
  GtkWidget* geotagging_image;
  GtkWidget* capture_timer_button;
  GtkWidget* capture_timer_image;
  GtkWidget* capture_timer_label;
  GtkWidget* crosshair;
  GtkWidget* zoom_slider;
  GtkWidget* right_button_box;
  GtkWidget* bottom_button_box;
  GtkWidget* top_button_box;
  GtkWidget* left_button_box;
  GtkWidget* root_container;
  GtkWidget* standby_image;
  GtkWidget* standby_view;
  GtkWidget* standby_label;
  GtkWidget* recording_time_label;

  CameraUI2PreviewWindow* preview_window;
  HildonProgram* program;
  gboolean is_fullscreen;
  gboolean in_capture_phase;
  gboolean save_raw_image;
  gboolean disable_show_on_lenscover_open;
  gboolean disable_hide_on_lenscover_close;
  gboolean disable_show_on_focus_pressed;
  gboolean with_sound_effects;
  CameraSettings camera_settings;
  CameraInterface* camera_interface;
  GeotaggingHelper* geotagging_helper;
  GeotaggingSettings geotagging_settings;
  SoundPlayerHelper* sound_player;
  Camera cam;
  guint delayed_focus_timer;
  guint capture_timer;
  guint capture_timer_countdown;
  guint recording_timer;
  guint recording_time;
  capture_data_t ccapture_data;  
  gboolean lenscover_open;
  DBusConnection *sysbus_conn;
  gdouble max_zoom;
};

#define CAMERA_UI2_WINDOW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), CAMERA_UI2_TYPE_WINDOW, CameraUI2WindowPrivate))


#define REMAINING_RECORDING_TIME_FMT "% 2d:%02d"
#define REMAINING_IMAGECOUNT_FMT "% 3d"
#define RECORDING_TIME_MARKUP_FMT "<span color=\"red\">%02d:%02d</span>"
#define RECORDING_TIME_ZERO_MARKUP "<span color=\"red\">00:00</span>"
#define TIMER_COUNTDOWN_FMT "%2d"
#define FOCUS_DISTANCE_CM_FMT "%0.2f cm"
#define FOCUS_DISTANCE_M_FMT "%0.2f m"

G_DEFINE_TYPE(CameraUI2Window, camera_ui2_window, HILDON_TYPE_STACKABLE_WINDOW)

static void _show_hide_ui(CameraUI2Window* self, gboolean show);
static void _capture_image(CameraUI2Window* self);
static void _stop_recording(CameraUI2Window* self);

static void
_remove_delayed_focus_timer(CameraUI2Window* self)
{
  if(self->priv->delayed_focus_timer)
  {
    g_source_remove(self->priv->delayed_focus_timer);
    self->priv->delayed_focus_timer = 0;
  }
}

static void
_activate_scene_mode_ui(CameraUI2Window* self)
{
  gtk_widget_hide(self->priv->recording_time_label);
  gtk_widget_show_all(self->priv->scene_mode_button);
  gtk_widget_show_all(self->priv->storage_button);
  if(is_video_mode(self->priv->camera_settings.scene_mode))
  {
    gtk_widget_hide(self->priv->flash_mode_button);
    gtk_widget_show_all(self->priv->video_mic_mode_button);
    gtk_widget_hide(self->priv->still_settings_button);
    gtk_widget_show_all(self->priv->video_settings_button);
    gtk_widget_hide(self->priv->image_viewer_button);
    gtk_widget_show_all(self->priv->media_player_button);
    gtk_widget_hide(self->priv->still_resolution_button);
    gtk_widget_hide(self->priv->capture_timer_button);
    gtk_widget_show_all(self->priv->video_resolution_button);
    gtk_widget_show_all(self->priv->video_state_button);
    gtk_widget_hide(self->priv->still_capture_image);
  }
  else
  {
    gtk_widget_hide(self->priv->video_mic_mode_button);
    gtk_widget_show_all(self->priv->flash_mode_button);
    gtk_widget_hide(self->priv->video_settings_button);
    gtk_widget_show_all(self->priv->still_settings_button);
    gtk_widget_show_all(self->priv->capture_timer_button);
    gtk_widget_hide(self->priv->media_player_button);
    gtk_widget_show_all(self->priv->image_viewer_button);
    gtk_widget_hide(self->priv->video_resolution_button);
    gtk_widget_show_all(self->priv->still_resolution_button);
    gtk_widget_hide(self->priv->video_state_button);
    gtk_widget_show_all(self->priv->still_capture_button);
  }
}

static void
_activate_recording_ui(CameraUI2Window* self)
{
  gtk_widget_hide(self->priv->scene_mode_button);
  gtk_widget_hide(self->priv->video_settings_button);
  gtk_widget_hide(self->priv->video_resolution_button);
  gtk_widget_hide(self->priv->crosshair);
  gtk_widget_hide(self->priv->storage_button);
  gtk_widget_show(self->priv->recording_time_label);
}

static gboolean
_is_topmost_window(CameraUI2Window* self)
{
  gboolean is_topmost;
  g_object_get(self, "is-topmost", &is_topmost, NULL);
  return is_topmost;
}

static gboolean
_is_visible(CameraUI2Window* self)
{
  gboolean is_visible;
  g_object_get(self, "visible", &is_visible, NULL);
  return is_visible;
}

static int
_update_remaining_count_indicator(CameraUI2Window* self)
{
  int size = (int)(storage_helper_free_space(self->priv->camera_settings.storage_device) / 1024);

  size = MAX(size-1024, 0);

  if(is_video_mode(self->priv->camera_settings.scene_mode))
  {
    switch(self->priv->camera_settings.video_resolution_size)
    {
    case CAM_VIDEO_RESOLUTION_LOW:
      size /= 120;
      break;
    case CAM_VIDEO_RESOLUTION_MEDIUM:
      size /= 405;
      break;
    case CAM_VIDEO_RESOLUTION_HIGH:
      size /= 470;
      break;
    case CAM_VIDEO_RESOLUTION_DVD_4X3:
      size /= 896;
      break;
    case CAM_VIDEO_RESOLUTION_DVD_16X9:
      size /= 1014;
      break;
    case CAM_VIDEO_RESOLUTION_HD_4X3:
      size /= 1293;
      break;
    case CAM_VIDEO_RESOLUTION_HD_16X9:
      size /= 1498;      break;
    }
    gchar* count_str = g_strdup_printf(REMAINING_RECORDING_TIME_FMT, (int)(size/60.0), (size%60));
    gtk_label_set_text(GTK_LABEL(self->priv->image_counter_label), count_str);
    g_free(count_str);
  }
  else
  {
    switch(self->priv->camera_settings.still_resolution_size)
    {
    case CAM_STILL_RESOLUTION_LOW:
      size /= ((self->priv->save_raw_image)? 202 + 11000 : 202);
      break;
    case CAM_STILL_RESOLUTION_MEDIUM:
      size /= ((self->priv->save_raw_image)? 405 + 11000 : 405);
      break;
    case CAM_STILL_RESOLUTION_HIGH:
      size /= ((self->priv->save_raw_image)? 760 + 11000 : 760);
      break;
    case CAM_STILL_RESOLUTION_WIDE:
      size /= ((self->priv->save_raw_image)? 510 + 11000 : 510);
      break;
    }
    gchar* count_str = g_strdup_printf(REMAINING_IMAGECOUNT_FMT, size);
    gtk_label_set_text(GTK_LABEL(self->priv->image_counter_label), count_str);
    g_free(count_str);
  }
  return (int)size;
}

static gboolean
_update_recording_timer(CameraUI2Window* self)
{
  if(self->priv->in_capture_phase)
  {
    self->priv->recording_time++;
    guint minutes = (guint)(self->priv->recording_time/60.0);
    guint seconds = self->priv->recording_time % 60;
    gchar* time_label = g_strdup_printf(RECORDING_TIME_MARKUP_FMT, minutes, seconds);
    gtk_label_set_markup(GTK_LABEL(self->priv->recording_time_label), time_label);
    g_free(time_label);
    if((seconds % 2) == 0 && _update_remaining_count_indicator(self) < 1)
    {
      hildon_banner_show_information(NULL, NULL, dgettext("osso-camera-ui", "camera_ib_recording_stopped"));
      _stop_recording(self);
    }
    return TRUE;
  }
  return FALSE;
}

static void
_set_white_balance_mode(CameraUI2Window* self, CamWhiteBalance white_balance)
{
  if(camera_interface_set_white_balance_mode(self->priv->camera_interface,
					     white_balance))
  {
    self->priv->camera_settings.white_balance = white_balance;
    camera_ui2_set_gconf_white_balance_mode(white_balance);
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->white_balance_image), 
				 white_balance_mode_icon_name(white_balance, FALSE), HILDON_ICON_SIZE_STYLUS);
  }
}

static void
_set_preview_mode(CameraUI2Window* self, CamPreviewMode preview_mode)
{
  // we need the preview image when using raw image mode as the
  // preview image is used as embedded thumbnail.
  // So don't disable the preview mode in the camera interface when
  // raw mode is active
  if(camera_interface_enable_preview(self->priv->camera_interface,
				     self->priv->save_raw_image ||
				     preview_mode != CAM_PREVIEW_MODE_NO_PREVIEW))
  {
    self->priv->camera_settings.preview_mode = preview_mode;
    camera_ui2_set_gconf_preview_mode(preview_mode);
  }
}
static void
_update_zoom_slider_range(CameraUI2Window * self)
{
  gdouble max_video_zoom_levels[CAM_VIDEO_RESOLUTION_DVD_16X9-CAM_VIDEO_RESOLUTION_LOW+1]=
    { 6 , 6 , 6 , 5.2 , 3.6, 6 , 4.8 };
  
  gdouble max_still_image_zoom_levels[CAM_STILL_RESOLUTION_WIDE+1]=
    { 6 , 6 , 6 , 6 };
  
  if(is_video_mode(self->priv->camera_settings.scene_mode))
    self->priv->max_zoom=max_video_zoom_levels[self->priv->camera_settings.video_resolution_size-CAM_VIDEO_RESOLUTION_LOW];
  else
    self->priv->max_zoom=max_still_image_zoom_levels[self->priv->camera_settings.still_resolution_size];
  if(GTK_IS_RANGE(self->priv->zoom_slider))
  {
    gtk_range_set_value(GTK_RANGE(self->priv->zoom_slider), 1.0);
    gtk_range_set_range(GTK_RANGE(self->priv->zoom_slider), 1.0, self->priv->max_zoom);
    gtk_range_set_increments(GTK_RANGE(self->priv->zoom_slider),
                             (self->priv->max_zoom-1.0)/50.0,0);
  }
}

static void
_set_still_resolution_size(CameraUI2Window* self, CamStillResolution size)
{
  if(camera_interface_set_still_resolution(self->priv->camera_interface,
					   size))
  {
    self->priv->camera_settings.still_resolution_size = size;
    camera_ui2_set_gconf_still_resolution_size(size);
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->still_resolution_image),
				 still_resolution_size_icon_name(size, FALSE), HILDON_ICON_SIZE_STYLUS);
    _update_remaining_count_indicator(self);
    _update_zoom_slider_range(self);
  }
}

static void
_set_video_resolution_size(CameraUI2Window* self, CamVideoResolution size)
{
  if(camera_interface_set_video_resolution(self->priv->camera_interface,
						size))
  { 
    gtk_widget_queue_draw(self->priv->view_finder);
    self->priv->camera_settings.video_resolution_size = size;
    camera_ui2_set_gconf_video_resolution_size(size);
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->video_resolution_image),
				 video_resolution_size_icon_name(size, FALSE), HILDON_ICON_SIZE_STYLUS);
    _update_remaining_count_indicator(self);
    _update_zoom_slider_range(self);
  }
}

static void
_set_iso_level(CameraUI2Window* self, CamIsoLevel iso_level)
{
  if(camera_interface_set_iso_level(self->priv->camera_interface,
				    iso_level))
  {
    self->priv->camera_settings.iso_level = iso_level;
    camera_ui2_set_gconf_iso_level(iso_level);
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->iso_level_image), 
				 iso_level_icon_name(iso_level, FALSE), HILDON_ICON_SIZE_STYLUS);
    gtk_widget_show(self->priv->iso_level_image);
  }
}

static void
_set_exposure_comp(CameraUI2Window* self, CamExposureComp exposure_comp)
{
  if(camera_interface_set_exposure_comp(self->priv->camera_interface,
					exposure_comp))
  {
    self->priv->camera_settings.exposure_level = exposure_comp;
    camera_ui2_set_gconf_exposure_level(exposure_comp);
  }
}

static void
_set_flash_mode(CameraUI2Window* self, CamFlashMode flash_mode)
{

  if(camera_interface_set_flash_mode(self->priv->camera_interface,
				     flash_mode))
  {
    self->priv->camera_settings.flash_mode = flash_mode;
    camera_ui2_set_gconf_flash_mode(flash_mode);
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->flash_mode_image), 
				 flash_mode_icon_name(flash_mode, FALSE), HILDON_ICON_SIZE_FINGER);
  }
}

static void
_set_scene_mode(CameraUI2Window* self, CamSceneMode scene_mode)
{
  gboolean change_scene_mode_ui =
    (is_video_mode(scene_mode) &&
     !is_video_mode(self->priv->camera_settings.scene_mode)) ||
    (!is_video_mode(scene_mode) &&
     is_video_mode(self->priv->camera_settings.scene_mode));

  if(!is_video_mode(scene_mode) &&
     is_video_mode(self->priv->camera_settings.scene_mode) &&
     self->priv->cam == CAM_FRONT_CAM)
  {
    camera_interface_backcamera_mode(self->priv->camera_interface);
    gtk_widget_queue_draw(GTK_WIDGET(self));
  }
  if(camera_interface_set_scene_mode(self->priv->camera_interface,
				     scene_mode))
  {
    self->priv->camera_settings.scene_mode = scene_mode;
    camera_ui2_set_gconf_scene_mode(scene_mode);
    if(scene_mode == CAM_SCENE_MODE_AUTO ||
       scene_mode == CAM_SCENE_MODE_PORTRAIT ||
       scene_mode == CAM_SCENE_MODE_NIGHT ||
       scene_mode == CAM_SCENE_MODE_MACRO)
    {
      _set_flash_mode(self, CAM_FLASH_MODE_AUTO);
      _set_iso_level(self, CAM_ISO_LEVEL_AUTO);
      _set_white_balance_mode(self, CAM_WHITE_BALANCE_AUTO);
    }
    else if(scene_mode == CAM_SCENE_MODE_LANDSCAPE ||
	    scene_mode == CAM_SCENE_MODE_ACTION)
    {
      _set_flash_mode(self, CAM_FLASH_MODE_OFF);
      _set_iso_level(self, CAM_ISO_LEVEL_AUTO);
      _set_white_balance_mode(self, CAM_WHITE_BALANCE_AUTO);
    }
    if(is_video_mode(scene_mode))
    {
      _set_iso_level(self, CAM_ISO_LEVEL_AUTO);
      _set_video_resolution_size(self, self->priv->camera_settings.video_resolution_size);
      camera_interface_set_audio_mode(self->priv->camera_interface, self->priv->camera_settings.mic_mode);
    }
    else
    {
      _set_still_resolution_size(self, self->priv->camera_settings.still_resolution_size);
      camera_interface_enable_preview(self->priv->camera_interface,
				      self->priv->save_raw_image || 
				      self->priv->camera_settings.preview_mode != CAM_PREVIEW_MODE_NO_PREVIEW);
    }
    _set_exposure_comp(self, CAM_EXPOSURE_COMP_0);
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->scene_mode_image), 
				 scene_mode_icon_name(scene_mode, FALSE), HILDON_ICON_SIZE_FINGER);
    switch(self->priv->camera_settings.scene_mode)
    {
    case CAM_SCENE_MODE_AUTO:
    case CAM_SCENE_MODE_PORTRAIT:
    case CAM_SCENE_MODE_MACRO:
    case CAM_SCENE_MODE_VIDEO:
      gtk_widget_show(self->priv->crosshair);
      gtk_widget_show(self->priv->focus_label);
      break;
    default:
      gtk_widget_hide(self->priv->crosshair);
      gtk_widget_hide(self->priv->focus_label);
    }
    if(change_scene_mode_ui)
    {
      _activate_scene_mode_ui(self);
      _update_remaining_count_indicator(self);
    }
  }
}

static gboolean
_set_ccapture_data(CameraUI2Window* self)
{
  if(_update_remaining_count_indicator(self)<1)
  {
    hildon_banner_show_information(NULL, NULL, dgettext("osso-camera-ui", "camera_ib_out_of_storage"));
    return FALSE;
  }
  if(self->priv->ccapture_data.classification_id)
    g_free(self->priv->ccapture_data.classification_id);

  if(is_video_mode(self->priv->camera_settings.scene_mode))
  {
    self->priv->ccapture_data.classification_id = storage_helper_create_unique_id();
  }
  
  if(self->priv->ccapture_data.filename)
    g_free(self->priv->ccapture_data.filename);

  self->priv->ccapture_data.filename = storage_helper_create_filename(self->priv->camera_settings.storage_device,
								      self->priv->camera_settings.scene_mode);
  if(g_strcmp0(self->priv->ccapture_data.author,
	       self->priv->camera_settings.author)!=0)
  {
    if(self->priv->ccapture_data.author != NULL)
      g_free(self->priv->ccapture_data.author);
    self->priv->ccapture_data.author = g_strdup(self->priv->camera_settings.author);
  }
  if(self->priv->ccapture_data.filename == NULL)
  {
    return FALSE;
  }
  self->priv->ccapture_data.orientation_mode = 
    dbus_helper_get_device_orientation_mode(self->priv->osso);
  self->priv->ccapture_data.longitude = G_MAXDOUBLE;
  self->priv->ccapture_data.latitude = G_MAXDOUBLE;
  self->priv->ccapture_data.altitude = G_MAXDOUBLE;
  if(self->priv->ccapture_data.country)
  {
    g_free(self->priv->ccapture_data.country);
    self->priv->ccapture_data.country = NULL;
  }
  if(self->priv->ccapture_data.city)
  {
    g_free(self->priv->ccapture_data.city);
    self->priv->ccapture_data.city = NULL;
  }
  if(self->priv->ccapture_data.suburb)
  {
    g_free(self->priv->ccapture_data.suburb);
    self->priv->ccapture_data.suburb = NULL;
  }
  geotagging_helper_fill_tagging_data(self->priv->geotagging_helper,
				      &self->priv->ccapture_data.longitude,
				      &self->priv->ccapture_data.latitude,
				      &self->priv->ccapture_data.altitude,
				      &self->priv->ccapture_data.country,
				      &self->priv->ccapture_data.city,
				      &self->priv->ccapture_data.suburb);
  return TRUE;
}
  
static void
_capture_image(CameraUI2Window* self)
{
  if(_set_ccapture_data(self))
  {
    if(self->priv->with_sound_effects)
      sound_player_capture_sound(self->priv->sound_player);
    if(camera_interface_capture_image(self->priv->camera_interface, self->priv->ccapture_data))
    {
      camera_ui2_increment_gconf_last_media_id();
    }
  }
}

void
_update_countdown_label(CameraUI2Window* self)
{
  gchar* time_str = g_strdup_printf(TIMER_COUNTDOWN_FMT, self->priv->capture_timer_countdown);
  gtk_label_set_text(GTK_LABEL(self->priv->capture_timer_label), time_str);
  g_free(time_str);
}

void
_update_focus_label(CameraUI2Window* self)
{
  gint focus = camera_interface_get_focus(self->priv->camera_interface);
  gfloat diopters = 0.0315*(focus-227);
  if(diopters == 0)
    gtk_widget_hide(self->priv->focus_label);
  else
    gtk_widget_show(self->priv->focus_label);

  gchar* focus_str = NULL;

  if(diopters > 1)
    focus_str = g_strdup_printf(FOCUS_DISTANCE_CM_FMT, 100/diopters);
  else
    focus_str = g_strdup_printf(FOCUS_DISTANCE_M_FMT, 1/diopters);

  gtk_label_set_text(GTK_LABEL(self->priv->focus_label), focus_str);
  g_free(focus_str);
}

static void
_start_recording_timer(CameraUI2Window* self)
{
  self->priv->recording_timer = g_timeout_add_seconds(1,
						      (GSourceFunc)_update_recording_timer,
						      self);
}

static void
_remove_capture_timer(CameraUI2Window* self)
{
  if(self->priv->capture_timer)
  {
    g_source_remove(self->priv->capture_timer);
    self->priv->capture_timer = 0;
    if(gtk_bin_get_child(GTK_BIN(self->priv->capture_timer_button)) == 
       self->priv->capture_timer_label)
    {
      gtk_container_remove(GTK_CONTAINER(self->priv->capture_timer_button),
			   self->priv->capture_timer_label);
      gtk_container_add(GTK_CONTAINER(self->priv->capture_timer_button),
			self->priv->capture_timer_image);
      gtk_widget_show_all(self->priv->capture_timer_button);
    }
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->capture_timer_image), 
				 "general_clock", HILDON_ICON_SIZE_FINGER);
  }
}

static gboolean
_countdown_capture_timer(CameraUI2Window* self)
{
  if(self->priv->capture_timer &&
     self->priv->capture_timer_countdown>0)
  {
    self->priv->capture_timer_countdown--;
    if(self->priv->capture_timer_countdown == 0)
    {
      _remove_capture_timer(self);
      _capture_image(self);
      _show_hide_ui(self, TRUE);
    }
    else
    {
      _update_countdown_label(self);
      return TRUE;
    }
  }
  return FALSE;
}

static void
_prevent_display_blanking (CameraUI2Window* self)
{
  DBusMessage *msg;
  dbus_bool_t b;

  if (self->priv->sysbus_conn == NULL) {
    g_warning ("%s: no D-Bus system bus connection", __func__);
    return;
  }
  msg = dbus_message_new_method_call(MCE_SERVICE, MCE_REQUEST_PATH,
                                     MCE_REQUEST_IF, MCE_PREVENT_BLANK_REQ);
  if (msg == NULL) {
    g_warning ("%s: could not create message", __func__);
    return;
  }

  b = dbus_connection_send(self->priv->sysbus_conn, msg, NULL);
  if (!b) {
    g_warning ("%s: dbus_connection_send() failed", __func__);
  } else {
    dbus_connection_flush(self->priv->sysbus_conn);
  }
  dbus_message_unref(msg);
}

static gboolean
_display_timeout_f (gpointer self)
{
  _prevent_display_blanking (self);
  return TRUE;
}

static void
_disable_display_blanking (gboolean disable, CameraUI2Window* self)
{
  static guint timeout_f = 0;

  if (disable)
    {
      _prevent_display_blanking (self);
      if (!timeout_f)
        timeout_f = g_timeout_add (30 * 1000, _display_timeout_f, self);
    }
  else if (timeout_f)
    {
      g_source_remove (timeout_f);
      timeout_f = 0;
    }
}

static void
_start_capture_timer(CameraUI2Window* self)
{
  _remove_capture_timer(self);
  _show_hide_ui(self, FALSE);
  if(gtk_bin_get_child(GTK_BIN(self->priv->capture_timer_button)) == 
     self->priv->capture_timer_image)
  {
    gtk_container_remove(GTK_CONTAINER(self->priv->capture_timer_button),
			 self->priv->capture_timer_image);
    gtk_container_add(GTK_CONTAINER(self->priv->capture_timer_button),
		      self->priv->capture_timer_label);
    gtk_widget_show(self->priv->left_button_box);
    gtk_widget_show_all(self->priv->capture_timer_button);
  }
  _update_countdown_label(self);
  self->priv->capture_timer = g_timeout_add_seconds(1,
						    (GSourceFunc)_countdown_capture_timer,
						    self);
}

static gboolean
_start_recording(CameraUI2Window* self)
{
  if(_set_ccapture_data(self))
  {
    if(self->priv->with_sound_effects)
      sound_player_start_recording_sound(self->priv->sound_player);
    if(camera_interface_start_recording(self->priv->camera_interface, self->priv->ccapture_data))
    {
      camera_ui2_increment_gconf_last_media_id();
      self->priv->camera_settings.video_state = CAM_VIDEO_STATE_RECORDING;
      self->priv->in_capture_phase = TRUE;
      gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->video_state_image), 
				   video_state_icon_name(CAM_VIDEO_STATE_PAUSED, FALSE), 
				   HILDON_ICON_SIZE_FINGER);
      gtk_widget_show_all(self->priv->video_stop_button);
      _disable_display_blanking (TRUE, self);
      _start_recording_timer(self);
      _activate_recording_ui(self);
    }
    return TRUE;
  }
  return FALSE;
}

static void
_resume_recording(CameraUI2Window* self)
{
  if(camera_interface_resume_recording(self->priv->camera_interface))
  {
    self->priv->camera_settings.video_state = CAM_VIDEO_STATE_RECORDING;
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->video_state_image), 
				 video_state_icon_name(CAM_VIDEO_STATE_PAUSED, FALSE), 
				 HILDON_ICON_SIZE_FINGER);
    _start_recording_timer(self);
  }
}

static void
_pause_recording(CameraUI2Window* self)
{
  if(self->priv->camera_settings.video_state == CAM_VIDEO_STATE_RECORDING &&
     camera_interface_pause_recording(self->priv->camera_interface))
  {
    self->priv->camera_settings.video_state = CAM_VIDEO_STATE_PAUSED;
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->video_state_image), 
				 video_state_icon_name(CAM_VIDEO_STATE_RECORDING, FALSE), 
				 HILDON_ICON_SIZE_FINGER);
    if(self->priv->recording_timer)
    {
      g_source_remove(self->priv->recording_timer);
    }
  }
}

static void
_stop_recording(CameraUI2Window* self)
{
  if(self->priv->camera_settings.video_state == CAM_VIDEO_STATE_PAUSED)
    camera_interface_resume_recording(self->priv->camera_interface);
  if(camera_interface_stop_recording(self->priv->camera_interface))
  {
    _disable_display_blanking (FALSE, self);
    self->priv->camera_settings.video_state = CAM_VIDEO_STATE_STOPPED;
    self->priv->in_capture_phase = FALSE;
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->video_state_image), 
				 video_state_icon_name(CAM_VIDEO_STATE_RECORDING, FALSE), 
				 HILDON_ICON_SIZE_FINGER);
    if(self->priv->recording_timer)
    {
      g_source_remove(self->priv->recording_timer);
      self->priv->recording_timer = 0;
    }
    gtk_widget_hide(self->priv->video_stop_button);
    _activate_scene_mode_ui(self);
    self->priv->recording_time = 0;
    gtk_label_set_markup(GTK_LABEL(self->priv->recording_time_label), 
                         RECORDING_TIME_ZERO_MARKUP);
  }
}

static gboolean
_on_close_window_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->close_window_image), 
			       "camera_overlay_close", HILDON_ICON_SIZE_FINGER);
  if(event->x >= 0 && event->y >= 0 && event->x < 84 && event->y < 84)
    gtk_widget_hide(GTK_WIDGET(self));
  return TRUE;
}

static gboolean
_on_close_window_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->close_window_image), 
			       "camera_overlay_close_pressed", HILDON_ICON_SIZE_FINGER);
  return TRUE;
}


static gboolean
_on_close_standby_window_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->close_standby_window_image), 
			       "camera_overlay_close", HILDON_ICON_SIZE_FINGER);
  if(event->x >= 0 && event->y >= 0 && event->x < 84 && event->y < 84)
    gtk_widget_hide(GTK_WIDGET(self));
  return TRUE;
}

static gboolean
_on_close_standby_window_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->close_standby_window_image), 
			       "camera_overlay_close_pressed", HILDON_ICON_SIZE_FINGER);
  return TRUE;
}

static gboolean
_on_scene_mode_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  CamSceneMode scene_mode = self->priv->camera_settings.scene_mode;
  if(event->x >= 0 && event->y >= 0 && event->x < 64 && event->y < 64)
    show_scene_mode_selection_dialog(&scene_mode);
  if(scene_mode != self->priv->camera_settings.scene_mode)
  {
    _set_scene_mode(self, scene_mode);
  }
  else
  {
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->scene_mode_image), 
				 scene_mode_icon_name(scene_mode, FALSE), HILDON_ICON_SIZE_FINGER);
  }
  return TRUE;
}

static gboolean
_on_scene_mode_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gint scene_mode = self->priv->camera_settings.scene_mode;
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->scene_mode_image), 
			       scene_mode_icon_name(scene_mode, TRUE), HILDON_ICON_SIZE_THUMB);
  return TRUE;
}

static gboolean
_on_flash_mode_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);

  CamFlashMode flash_mode = self->priv->camera_settings.flash_mode;
  if(event->x >= 0 && event->y >= 0 && event->x < 64 && event->y < 64)
    show_flash_mode_selection_dialog(&flash_mode);
  if(flash_mode != self->priv->camera_settings.flash_mode)
  {
    _set_flash_mode(self, flash_mode);
  }
  else
  {
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->flash_mode_image), 
				 flash_mode_icon_name(flash_mode, FALSE), HILDON_ICON_SIZE_FINGER);
  }
  return TRUE;
}

static gboolean
_on_flash_mode_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gint flash_mode = self->priv->camera_settings.flash_mode;
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->flash_mode_image), 
			       flash_mode_icon_name(flash_mode, TRUE), HILDON_ICON_SIZE_THUMB);
  return TRUE;
}

static gboolean
_on_mic_mode_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gint mic_mode = self->priv->camera_settings.mic_mode;
  if(mic_mode == CAM_VIDEO_MIC_ON)
  {
    mic_mode = CAM_VIDEO_MIC_OFF;
  }
  else
  {
    mic_mode = CAM_VIDEO_MIC_ON;
  }
  if(event->x >= 0 && event->y >= 0 && event->x < 64 && event->y < 64)
    if(camera_interface_set_audio_mode(self->priv->camera_interface,
				       mic_mode))
    {
      self->priv->camera_settings.mic_mode = mic_mode;
    }

  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->video_mic_mode_image), 
			       video_mic_mode_icon_name(self->priv->camera_settings.mic_mode, FALSE), 
			       HILDON_ICON_SIZE_FINGER);
  return TRUE;
}

static gboolean
_on_mic_mode_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->video_mic_mode_image), 
			       video_mic_mode_icon_name(self->priv->camera_settings.mic_mode, TRUE), 
			       HILDON_ICON_SIZE_THUMB);
  return TRUE;
}

static gboolean
_on_white_balance_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  CamWhiteBalance white_balance = self->priv->camera_settings.white_balance;
  if(event->x >= 0 && event->y >= 0 && event->x < 64 && event->y < 64)
    show_white_balance_mode_selection_dialog(&white_balance);
  if(white_balance != self->priv->camera_settings.white_balance)
  {
    _set_white_balance_mode(self, white_balance);
  }
  else
  {
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->white_balance_image), 
				 white_balance_mode_icon_name(white_balance, FALSE), HILDON_ICON_SIZE_STYLUS);
  }
  return TRUE;
}

static gboolean
_on_white_balance_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gint white_balance_mode = self->priv->camera_settings.white_balance;
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->white_balance_image), 
			       white_balance_mode_icon_name(white_balance_mode, TRUE), HILDON_ICON_SIZE_FINGER);
  return TRUE;
}

static gboolean
_on_video_state_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  if(event->x < 0 || event->y < 0 || event->x >= 64 || event->y >= 64)
    return TRUE;
  if(self->priv->camera_settings.video_state == CAM_VIDEO_STATE_RECORDING)
  {
    _pause_recording(self);
  }
  else if(self->priv->camera_settings.video_state == CAM_VIDEO_STATE_STOPPED)
  {
    if(!_start_recording(self))
    {
      gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->video_state_image), 
				   video_state_icon_name(CAM_VIDEO_STATE_RECORDING, FALSE), 
				   HILDON_ICON_SIZE_FINGER);
    }
  }
  else if(self->priv->camera_settings.video_state == CAM_VIDEO_STATE_PAUSED)
  {
    _resume_recording(self);
  }
  return TRUE;
}

static gboolean
_on_video_state_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->video_state_image), 
			       video_state_icon_name(self->priv->camera_settings.video_state, TRUE), HILDON_ICON_SIZE_THUMB);
  return TRUE;
}

static gboolean
_on_video_stop_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  if(self->priv->camera_settings.video_state == CAM_VIDEO_STATE_RECORDING ||
     self->priv->camera_settings.video_state == CAM_VIDEO_STATE_PAUSED)
  {
    _stop_recording(self);
    self->priv->camera_settings.video_state = CAM_VIDEO_STATE_STOPPED;
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->video_stop_image), 
				 video_state_icon_name(CAM_VIDEO_STATE_STOPPED, FALSE), 
				 HILDON_ICON_SIZE_FINGER);
  }
  return TRUE;
}

static gboolean
_on_video_stop_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->video_stop_image), 
			       video_state_icon_name(CAM_VIDEO_STATE_STOPPED, TRUE), HILDON_ICON_SIZE_THUMB);
  return TRUE;
}


static gboolean
_on_still_resolution_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  CamStillResolution resolution_size = self->priv->camera_settings.still_resolution_size;
  if(event->x >= 0 && event->y >= 0 && event->x < 48 && event->y < 48)
    show_still_resolution_size_selection_dialog(&resolution_size);
  if(resolution_size != self->priv->camera_settings.still_resolution_size)
  {
    _set_still_resolution_size(self, resolution_size);
  }
  else
  {
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->still_resolution_image), 
				 still_resolution_size_icon_name(resolution_size, FALSE), 
				 HILDON_ICON_SIZE_STYLUS);
  }
  return TRUE;
}

static gboolean
_on_still_resolution_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gint still_resolution_size = self->priv->camera_settings.still_resolution_size;
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->still_resolution_image), 
			       still_resolution_size_icon_name(still_resolution_size, TRUE), HILDON_ICON_SIZE_FINGER);
  return TRUE;
}

static gboolean
_on_video_resolution_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gint video_resolution_size = self->priv->camera_settings.video_resolution_size;
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->video_resolution_image), 
			       video_resolution_size_icon_name(video_resolution_size, TRUE), HILDON_ICON_SIZE_FINGER);
  return TRUE;
}

static gboolean
_on_video_resolution_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  CameraSettings settings;
  settings.scene_mode = self->priv->camera_settings.scene_mode;
  settings.flash_mode = self->priv->camera_settings.flash_mode;
  settings.white_balance = self->priv->camera_settings.white_balance;
  settings.iso_level = self->priv->camera_settings.iso_level;
  settings.exposure_level = self->priv->camera_settings.exposure_level;
  settings.video_resolution_size = self->priv->camera_settings.video_resolution_size;

  if(event->x >= 0 && event->y >= 0 && event->x < 48 && event->y < 48)
    show_video_resolution_size_selection_dialog(&settings);

  if(settings.video_resolution_size != self->priv->camera_settings.video_resolution_size)
  {
    _set_video_resolution_size(self, settings.video_resolution_size);
  }
  else
  {
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->video_resolution_image), 
				 video_resolution_size_icon_name(settings.video_resolution_size, FALSE), 
				 HILDON_ICON_SIZE_STYLUS);
  }
  return TRUE;
}

static gboolean
_on_image_viewer_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);

  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->image_viewer_image), 
			       "camera_imageviewer", HILDON_ICON_SIZE_FINGER);
  if(event->x >= 0 && event->y >= 0 && event->x < 64 && event->y < 64)
    dbus_helper_start_image_viewer(self->priv->osso);
  return TRUE;
}

static gboolean
_on_image_viewer_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->image_viewer_image), 
			       "camera_imageviewer_pressed", HILDON_ICON_SIZE_THUMB);
  return TRUE;
}

static gboolean
_on_media_player_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->media_player_image), 
			       "camera_playback", HILDON_ICON_SIZE_FINGER);
  if(event->x >= 0 && event->y >= 0 && event->x < 64 && event->y < 64)
    dbus_helper_start_media_player(self->priv->osso);
  return TRUE;
}

static gboolean
_on_media_player_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->media_player_image), 
			       "camera_playback_pressed", HILDON_ICON_SIZE_THUMB);
  return TRUE;
}

static gboolean
_on_iso_level_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  CamIsoLevel iso_level = self->priv->camera_settings.iso_level;
  if(event->x >= 0 && event->y >= 0 && event->x < 48 && event->y < 48)
    show_iso_level_selection_dialog(&iso_level);
  if(iso_level != self->priv->camera_settings.iso_level)
  {
    _set_iso_level(self, iso_level);
  }
  else
  {
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->iso_level_image), 
				 iso_level_icon_name(iso_level, FALSE), HILDON_ICON_SIZE_STYLUS);
  }
  return TRUE;
}

static gboolean
_on_iso_level_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gint iso_level = self->priv->camera_settings.iso_level;
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->iso_level_image), 
			       iso_level_icon_name(iso_level, TRUE), HILDON_ICON_SIZE_FINGER);
  return TRUE;
}

static gboolean
_on_color_mode_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  if(event->x >= 0 && event->y >= 0 && event->x < 64 && event->y < 64)
  {
    show_color_mode_selection_dialog(&self->priv->camera_settings.color_mode);
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->color_mode_image), 
				 "control_led", HILDON_ICON_SIZE_FINGER);
    camera_interface_set_colour_tone_mode(self->priv->camera_interface, self->priv->camera_settings.color_mode);
  }
  return TRUE;
}

static gboolean
_on_still_settings_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  CameraSettings settings;
  settings.scene_mode = self->priv->camera_settings.scene_mode;
  settings.flash_mode = self->priv->camera_settings.flash_mode;
  settings.white_balance = self->priv->camera_settings.white_balance;
  settings.iso_level = self->priv->camera_settings.iso_level;
  settings.exposure_level = self->priv->camera_settings.exposure_level;
  settings.still_resolution_size = self->priv->camera_settings.still_resolution_size;
  
  if(event->x >= 0 && event->y >= 0 && event->x < 64 && event->y < 64)
    show_still_settings_dialog(&settings);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->still_settings_image), 
			       settings_icon_name(FALSE), HILDON_ICON_SIZE_FINGER);

  if(settings.still_resolution_size != self->priv->camera_settings.still_resolution_size)
  {
    _set_still_resolution_size(self, settings.still_resolution_size);
  }
  if(settings.white_balance != self->priv->camera_settings.white_balance)
  {
    _set_white_balance_mode(self, settings.white_balance);
  }
  if(settings.iso_level != self->priv->camera_settings.iso_level)
  {
    _set_iso_level(self, settings.iso_level);
  }
  if(settings.exposure_level != self->priv->camera_settings.exposure_level)
  {
    _set_exposure_comp(self, settings.exposure_level);
  }
  return TRUE;
}

static gboolean
_on_still_settings_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->still_settings_image), 
			       settings_icon_name(TRUE), HILDON_ICON_SIZE_THUMB);
  return TRUE;
}

static gboolean
_on_storage_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->storage_image), 
			       storage_device_icon_name(self->priv->camera_settings.storage_device,TRUE), HILDON_ICON_SIZE_FINGER);
  return TRUE;
}

static gboolean
_on_storage_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  CameraSettings settings;
  settings.author = g_strdup(self->priv->camera_settings.author);
  settings.storage_device = self->priv->camera_settings.storage_device;
  settings.preview_mode = self->priv->camera_settings.preview_mode;
  settings.scene_mode = self->priv->camera_settings.scene_mode;
  if(event->x >= 0 && event->y >= 0 && event->x < 48 && event->y < 48)
    show_storage_settings_dialog(&settings);
  if(settings.preview_mode != self->priv->camera_settings.preview_mode)
  {
    _set_preview_mode(self, settings.preview_mode);
  }
  if(g_strcmp0(settings.author, self->priv->camera_settings.author) != 0)
  {
    g_free(self->priv->camera_settings.author);
    self->priv->camera_settings.author = g_strdup(settings.author);
    camera_ui2_set_gconf_author(settings.author);
  }
  if(self->priv->camera_settings.storage_device != CAM_STORAGE_EXTERN_UNAVAILABLE)
  {
    camera_ui2_set_gconf_storage_device(settings.storage_device);
    self->priv->camera_settings.storage_device = settings.storage_device;
    _update_remaining_count_indicator(self);
  }
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->storage_image), 
			       storage_device_icon_name(self->priv->camera_settings.storage_device, FALSE), 
			       HILDON_ICON_SIZE_STYLUS);
  g_free(settings.author);
  return TRUE;
}

static gboolean
_on_capture_timer_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->capture_timer_image), 
			       "general_clock", HILDON_ICON_SIZE_THUMB);
  return TRUE;
}

static gboolean
_on_capture_timer_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  if(self->priv->capture_timer!=0)
  {
    _remove_capture_timer(self);
    _show_hide_ui(self, TRUE);
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->capture_timer_image), 
				 "general_clock", HILDON_ICON_SIZE_FINGER);
  }
  else
  {
    int seconds = show_capture_timer_dialog();
    if(seconds!=0)
    {
      self->priv->capture_timer_countdown = seconds;
      _start_capture_timer(self);
    }
    else
    {
      gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->capture_timer_image), 
				   "general_clock", HILDON_ICON_SIZE_FINGER);
    }
  }
}

static gboolean
_on_still_capture_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->still_capture_image), 
			       "tasklaunch_default_application", HILDON_ICON_SIZE_THUMB);
  return TRUE;
}

static gboolean
_on_still_capture_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  if(event->x >= 0 && event->y >= 0 && event->x < 64 && event->y < 64)
  {
    if(!is_video_mode(self->priv->camera_settings.scene_mode))
    {
      _capture_image(self);
    }
  }
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->still_capture_image), 
			       "general_tickmark_unchecked", HILDON_ICON_SIZE_FINGER);
  return TRUE;
}

static gboolean
_on_geotagging_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->geotagging_image), 
			       "camera_gps_location_pressed", HILDON_ICON_SIZE_FINGER);
  return TRUE;
}

static gboolean
_on_geotagging_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  GeoTagMode tag_mode = geotagging_helper_get_mode(self->priv->geotagging_helper);
  
  if(event->x >= 0 && event->y >= 0 && event->x < 48 && event->y < 48)
    show_geotagging_settings_dialog(&tag_mode);

  if(tag_mode != GEO_TAG_NONE)
  {
    geotagging_helper_run(self->priv->geotagging_helper);
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->geotagging_image), 
				 "camera_gps_location", 
				 HILDON_ICON_SIZE_STYLUS);
  }
  else
  {
    geotagging_helper_stop(self->priv->geotagging_helper);
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->geotagging_image), 
				 "gps_searching", 
				 HILDON_ICON_SIZE_STYLUS);
  }
  geotagging_helper_set_mode(self->priv->geotagging_helper, tag_mode);
  return TRUE;
}

static gboolean
_on_video_settings_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  CameraSettings settings;
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->video_settings_image), 
                               settings_icon_name(FALSE), HILDON_ICON_SIZE_FINGER);
  if(event->x < 0 || event->y < 0 || event->x >= 64 || event->y >= 64)
  {
    return TRUE;
  }
  settings.scene_mode = self->priv->camera_settings.scene_mode;
  settings.flash_mode = self->priv->camera_settings.flash_mode;
  settings.white_balance = self->priv->camera_settings.white_balance;
  settings.iso_level = self->priv->camera_settings.iso_level;
  settings.exposure_level = self->priv->camera_settings.exposure_level;
  settings.video_resolution_size = self->priv->camera_settings.video_resolution_size;
  show_video_settings_dialog(&settings);
  if(settings.video_resolution_size != self->priv->camera_settings.video_resolution_size)
  {
    _set_video_resolution_size(self, settings.video_resolution_size);
  }
  if(settings.white_balance != self->priv->camera_settings.white_balance)
  {
    _set_white_balance_mode(self, settings.white_balance);
  }
  if(settings.iso_level != self->priv->camera_settings.iso_level)
  {
    _set_iso_level(self, settings.iso_level);
  }
  if(settings.exposure_level != self->priv->camera_settings.exposure_level)
  {
    _set_exposure_comp(self, settings.exposure_level);
  }
  return TRUE;
}

static gboolean
_on_video_settings_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->video_settings_image), 
			       settings_icon_name(TRUE), HILDON_ICON_SIZE_THUMB);
  return TRUE;
}

static gboolean
_on_viewfinder_pressed(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  if(self->priv->capture_timer!=0)
  {
    _remove_capture_timer(self);
  }
  else
  {
    if(self->priv->is_fullscreen)
    {
      gtk_window_unfullscreen(GTK_WINDOW(self));
      gtk_widget_hide(self->priv->close_window_button);
      gtk_widget_hide(self->priv->close_standby_window_button);
    }
    else
    {
      gtk_window_fullscreen(GTK_WINDOW(self));
      gtk_widget_show(self->priv->close_window_button);
      gtk_widget_show(self->priv->close_standby_window_button);
    }
    self->priv->is_fullscreen = !self->priv->is_fullscreen;
  }
  return TRUE;
}

static gboolean
_on_key_pressed(GtkWidget* widget, GdkEventKey* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  if(event->keyval == HILDON_HARDKEY_INCREASE)
  {
    gdouble new_zoom = camera_interface_increase_zoom(self->priv->camera_interface,self->priv->max_zoom);
    gtk_range_set_value(GTK_RANGE(self->priv->zoom_slider), new_zoom);
    return TRUE;
  }
  else if(event->keyval == HILDON_HARDKEY_DECREASE)
  {
    gdouble new_zoom = camera_interface_decrease_zoom(self->priv->camera_interface);
    gtk_range_set_value(GTK_RANGE(self->priv->zoom_slider), new_zoom);
    return TRUE;
  }
  else
  {
    switch (event->keyval) 
    {
    case GDK_f:
      camera_interface_decrease_focus(self->priv->camera_interface, 1);
      _update_focus_label(self);
      break;
    case GDK_F:
      camera_interface_increase_focus(self->priv->camera_interface, 1);
      _update_focus_label(self);      
      break;
    case GDK_g:
      camera_interface_decrease_focus(self->priv->camera_interface, 50);
      _update_focus_label(self);      
      break;
    case GDK_G:
      camera_interface_increase_focus(self->priv->camera_interface, 50);
      _update_focus_label(self);      
      break;
    case GDK_z:
      {
	gdouble new_zoom = camera_interface_decrease_zoom(self->priv->camera_interface);
	gtk_range_set_value(GTK_RANGE(self->priv->zoom_slider), new_zoom);
      }
      break;
    case GDK_Z:
      {
	gdouble new_zoom = camera_interface_increase_zoom(self->priv->camera_interface,self->priv->max_zoom);
	gtk_range_set_value(GTK_RANGE(self->priv->zoom_slider), new_zoom);
      }
      break;
    }
  }
  return FALSE;
}

static gboolean
_on_key_released(GtkWidget* widget, GdkEventKey* event, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  switch (event->keyval) {
  case GDK_a:
    {
      if(!self->priv->in_capture_phase)
	_set_scene_mode(self, CAM_SCENE_MODE_AUTO);
      break;
    }
  case GDK_b:
    {
      if(is_video_mode(self->priv->camera_settings.scene_mode) &&
	 !self->priv->in_capture_phase)
      {
	_set_video_resolution_size(self, CAM_VIDEO_RESOLUTION_LOW);
	if(camera_interface_frontcamera_mode(self->priv->camera_interface))
	{
	  gtk_widget_queue_draw(GTK_WIDGET(self));
	  self->priv->cam = CAM_FRONT_CAM;
	}
      }
      break;
    }
  case GDK_B:
    {
      if(is_video_mode(self->priv->camera_settings.scene_mode) &&
	 !self->priv->in_capture_phase)
      {
	if(camera_interface_backcamera_mode(self->priv->camera_interface))
	{
	  _set_video_resolution_size(self, self->priv->camera_settings.video_resolution_size);
	  gtk_widget_queue_draw(GTK_WIDGET(self));
	  self->priv->cam = CAM_BACK_CAM;
	}
      }
      break;
    }
  case GDK_c:
    {
      if(!is_video_mode(self->priv->camera_settings.scene_mode))
      {
	_capture_image(self);
      }
      break;
    }
  case GDK_R:
    {
      if(!is_video_mode(self->priv->camera_settings.scene_mode))
      {
	camera_interface_set_raw_image_mode(self->priv->camera_interface, TRUE);
        self->priv->save_raw_image = TRUE;
	camera_interface_enable_preview(self->priv->camera_interface, 
					TRUE);
	_update_remaining_count_indicator(self);
	gtk_widget_show(self->priv->raw_indicator_label);

      }
      break;
    }
  case GDK_r:
    {
      if(is_video_mode(self->priv->camera_settings.scene_mode) &&
	 !self->priv->in_capture_phase)
      {
	_start_recording(self);
      }
      else if(is_video_mode(self->priv->camera_settings.scene_mode) &&
	 self->priv->in_capture_phase)
      {
	_stop_recording(self);
      }
      else if(!is_video_mode(self->priv->camera_settings.scene_mode))
      {
	camera_interface_set_raw_image_mode(self->priv->camera_interface, FALSE);
        self->priv->save_raw_image = FALSE;
	_update_remaining_count_indicator(self);
	camera_interface_enable_preview(self->priv->camera_interface, 
					self->priv->camera_settings.preview_mode != CAM_PREVIEW_MODE_NO_PREVIEW);
	gtk_widget_hide(self->priv->raw_indicator_label);
      }
      break;
    }
  case GDK_t:
    {
      camera_interface_toggle_light(self->priv->camera_interface);
      break;
    }
  case GDK_h:
    {
      camera_interface_toggle_privacy_light(self->priv->camera_interface);
      break;
    }
  case GDK_p:
    {
      if(is_video_mode(self->priv->camera_settings.scene_mode))
      {
	_pause_recording(self);
      }
      else if(!self->priv->in_capture_phase)
      {
	_set_scene_mode(self, CAM_SCENE_MODE_PORTRAIT);
      }
      break;
    }
  case GDK_m:
    {
      if(!self->priv->in_capture_phase)
	_set_scene_mode(self, CAM_SCENE_MODE_MACRO);
      break;
    }
  case GDK_l:
    {
      if(!self->priv->in_capture_phase)
	_set_scene_mode(self, CAM_SCENE_MODE_LANDSCAPE);
      break;
    }
  case GDK_n:
    {
      if(!self->priv->in_capture_phase)
	_set_scene_mode(self, CAM_SCENE_MODE_NIGHT);
      break;
    }
  case GDK_v:
    {
      if(!self->priv->in_capture_phase)
	_set_scene_mode(self, CAM_SCENE_MODE_VIDEO);
      break;
    }
  case GDK_V:
    {
      if(!self->priv->in_capture_phase)
	_set_scene_mode(self, CAM_SCENE_MODE_NIGHT_VIDEO);
      break;
    }
  case GDK_i:
    {
      switch(self->priv->camera_settings.iso_level)
      {
      case CAM_ISO_LEVEL_AUTO:
	_set_iso_level(self, CAM_ISO_LEVEL_100);
	break;
      case CAM_ISO_LEVEL_100:
	_set_iso_level(self, CAM_ISO_LEVEL_200);
	break;
      case CAM_ISO_LEVEL_200:
	_set_iso_level(self, CAM_ISO_LEVEL_400);
	break;
      case CAM_ISO_LEVEL_400:
	_set_iso_level(self, CAM_ISO_LEVEL_800);
	break;
      case CAM_ISO_LEVEL_800:
	_set_iso_level(self, CAM_ISO_LEVEL_1600);
	break;
      default:
	_set_iso_level(self, CAM_ISO_LEVEL_AUTO);
      }
      break;
    }
  case GDK_w:
    {
      switch(self->priv->camera_settings.white_balance)
      {
      case CAM_WHITE_BALANCE_AUTO:
	_set_white_balance_mode(self, CAM_WHITE_BALANCE_SUNNY);
	break;
      case CAM_WHITE_BALANCE_SUNNY:
	_set_white_balance_mode(self, CAM_WHITE_BALANCE_CLOUDY);
	break;
      case CAM_WHITE_BALANCE_CLOUDY:
	_set_white_balance_mode(self, CAM_WHITE_BALANCE_FLOURESCENT);
	break;
      case CAM_WHITE_BALANCE_FLOURESCENT:
	_set_white_balance_mode(self, CAM_WHITE_BALANCE_INCANDESCENT);
	break;
      default:
	_set_white_balance_mode(self, CAM_WHITE_BALANCE_AUTO);
      }
      break;
    }
  case GDK_1:
    camera_interface_decrease_focus(self->priv->camera_interface, 1023);
    _update_focus_label(self);
    break;
  case GDK_0:
    camera_interface_increase_focus(self->priv->camera_interface, 1023);
    _update_focus_label(self);
    break;
  }
  return FALSE;
}

static void
_init_geotagging_button(CameraUI2Window* self)
{
  self->priv->geotagging_button = gtk_event_box_new();
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->geotagging_button),
				   FALSE);
  self->priv->geotagging_image = gtk_image_new_from_icon_name("gps_searching", HILDON_ICON_SIZE_STYLUS);

  gtk_container_add(GTK_CONTAINER(self->priv->geotagging_button),
		    self->priv->geotagging_image);

  gtk_widget_set_size_request(self->priv->geotagging_button, 64, 64);
  g_signal_connect(GTK_CONTAINER(self->priv->geotagging_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_geotagging_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->geotagging_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_geotagging_button_release),
		   self);
}

static void
_init_capture_timer_button(CameraUI2Window* self)
{
  self->priv->capture_timer_button = gtk_event_box_new();
  self->priv->capture_timer_label = gtk_label_new("");
  hildon_helper_set_logical_font(self->priv->capture_timer_label,
				 "SmallSystemFont");
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->capture_timer_button),
				   FALSE);
  self->priv->capture_timer_image = gtk_image_new_from_icon_name("general_clock", HILDON_ICON_SIZE_FINGER);
  g_object_ref(self->priv->capture_timer_image);
  g_object_ref(self->priv->capture_timer_label);
  gtk_container_add(GTK_CONTAINER(self->priv->capture_timer_button),
		    self->priv->capture_timer_image);

  gtk_widget_set_size_request(self->priv->capture_timer_button, 64, 64);
  g_signal_connect(GTK_CONTAINER(self->priv->capture_timer_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_capture_timer_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->capture_timer_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_capture_timer_button_release),
		   self);
}

static void
_init_still_capture_button(CameraUI2Window* self)
{
  self->priv->still_capture_button = gtk_event_box_new();
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->still_capture_button),
				   FALSE);
  self->priv->still_capture_image = gtk_image_new_from_icon_name("general_tickmark_unchecked", HILDON_ICON_SIZE_FINGER);
  gtk_container_add(GTK_CONTAINER(self->priv->still_capture_button),
		    self->priv->still_capture_image);

  gtk_widget_set_size_request(self->priv->still_capture_button, 64, 64);
  g_signal_connect(GTK_CONTAINER(self->priv->still_capture_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_still_capture_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->still_capture_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_still_capture_button_release),
		   self);
}

static void
_init_storage_device_button(CameraUI2Window* self)
{
  self->priv->storage_button = gtk_event_box_new();
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->storage_button),
				   FALSE);
  self->priv->storage_image = gtk_image_new_from_icon_name(storage_device_icon_name(self->priv->camera_settings.storage_device, FALSE), HILDON_ICON_SIZE_STYLUS);

  gtk_container_add(GTK_CONTAINER(self->priv->storage_button),
		    self->priv->storage_image);

  gtk_widget_set_size_request(self->priv->storage_button, 64, 64);
  g_signal_connect(GTK_CONTAINER(self->priv->storage_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_storage_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->storage_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_storage_button_release),
		   self);
}

static void
_on_preview_window_hide(GtkWidget *widget, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  if(hildon_program_get_is_topmost(self->priv->program))
  {
    gtk_widget_hide(self->priv->standby_view); 
    gtk_widget_show(self->priv->view_finder); 
    camera_interface_open_viewfinder(self->priv->camera_interface, 
				     GDK_WINDOW_XWINDOW(self->priv->view_finder->window)); 
    gtk_widget_queue_draw(self->priv->view_finder);
  }
}

static void
_on_general_settings_button_pressed(GtkButton* button, gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  AppSettings app_settings;
  app_settings.show_on_lenscover_open = !self->priv->disable_show_on_lenscover_open;
  app_settings.hide_on_lenscover_close = !self->priv->disable_hide_on_lenscover_close;
  app_settings.show_on_focus_button_press = !self->priv->disable_show_on_focus_pressed;
  app_settings.enable_sound_effects = self->priv->with_sound_effects;
  show_app_settings_dialog(&app_settings);
  self->priv->disable_show_on_lenscover_open = !app_settings.show_on_lenscover_open;
  self->priv->disable_hide_on_lenscover_close = !app_settings.hide_on_lenscover_close;
  self->priv->disable_show_on_focus_pressed = !app_settings.show_on_focus_button_press;
  self->priv->with_sound_effects = app_settings.enable_sound_effects;
  camera_ui2_set_gconf_show_on_lenscover_open(self->priv->disable_show_on_lenscover_open);
  camera_ui2_set_gconf_hide_on_lenscover_close(self->priv->disable_hide_on_lenscover_close);
  camera_ui2_set_gconf_show_on_focus_pressed(self->priv->disable_show_on_focus_pressed);
  camera_ui2_set_gconf_enabled_sound_effects(self->priv->with_sound_effects);
}

static void
_init_preview_window(CameraUI2Window* self)
{
  self->priv->preview_window = camera_ui2_preview_window_new(self->priv->osso);
  gtk_window_set_title(GTK_WINDOW(self->priv->preview_window), dgettext("osso-camera-ui", "camera_ap_camera"));
  g_signal_connect(self->priv->preview_window, "hide", G_CALLBACK(&_on_preview_window_hide), self);
}

static void
_init_close_window_button(CameraUI2Window* self)
{
  self->priv->close_window_image = gtk_image_new_from_icon_name("camera_overlay_close", HILDON_ICON_SIZE_FINGER);
  self->priv->close_window_button = gtk_event_box_new();
  gtk_container_set_border_width(GTK_CONTAINER(self->priv->close_window_button), 0);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->close_window_button),
				   FALSE);
  gtk_container_add(GTK_CONTAINER(self->priv->close_window_button), self->priv->close_window_image);
  gtk_widget_set_size_request(self->priv->close_window_button, 80, 64);
  g_signal_connect(GTK_CONTAINER(self->priv->close_window_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_close_window_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->close_window_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_close_window_button_release),
		   self);
}

static void
_init_close_standby_window_button(CameraUI2Window* self)
{
  self->priv->close_standby_window_image = gtk_image_new_from_icon_name("camera_overlay_close", HILDON_ICON_SIZE_FINGER);
  self->priv->close_standby_window_button = gtk_event_box_new();
  gtk_container_set_border_width(GTK_CONTAINER(self->priv->close_standby_window_button), 0);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->close_standby_window_button),
				   FALSE);
  gtk_container_add(GTK_CONTAINER(self->priv->close_standby_window_button), self->priv->close_standby_window_image);
  gtk_widget_set_size_request(self->priv->close_standby_window_button, 80, 64);
  g_signal_connect(GTK_CONTAINER(self->priv->close_standby_window_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_close_standby_window_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->close_standby_window_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_close_standby_window_button_release),
		   self);
}

static void
_init_scene_mode_button(CameraUI2Window* self)
{
  self->priv->scene_mode_button = gtk_event_box_new();
  gtk_widget_set_size_request(self->priv->scene_mode_button, 64, 64);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->scene_mode_button),
				   FALSE);
  gint scene_mode = self->priv->camera_settings.scene_mode;
  self->priv->scene_mode_image = 
    gtk_image_new_from_icon_name(scene_mode_icon_name(scene_mode, FALSE), HILDON_ICON_SIZE_FINGER);

  gtk_container_add(GTK_CONTAINER(self->priv->scene_mode_button),
		    self->priv->scene_mode_image);
  g_signal_connect(GTK_CONTAINER(self->priv->scene_mode_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_scene_mode_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->scene_mode_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_scene_mode_button_release),
		   self);
}

static void
_init_color_mode_button(CameraUI2Window* self)
{
  self->priv->color_mode_button = gtk_event_box_new();
  gtk_widget_set_size_request(self->priv->color_mode_button, 64, 64);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->color_mode_button),
				   FALSE);
  gint color_mode = self->priv->camera_settings.color_mode;
  self->priv->color_mode_image = 
    gtk_image_new_from_icon_name("control_led", HILDON_ICON_SIZE_FINGER);

  gtk_container_add(GTK_CONTAINER(self->priv->color_mode_button),
		    self->priv->color_mode_image);
  g_signal_connect(GTK_CONTAINER(self->priv->color_mode_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_color_mode_button_release),
		   self);
}

static void
_init_flash_mode_button(CameraUI2Window* self)
{
  self->priv->flash_mode_button = gtk_event_box_new();
  gtk_widget_set_size_request(self->priv->flash_mode_button, 64, 64);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->flash_mode_button),
				   FALSE);
  gint flash_mode = self->priv->camera_settings.flash_mode;
  self->priv->flash_mode_image = 
    gtk_image_new_from_icon_name(flash_mode_icon_name(flash_mode, FALSE), HILDON_ICON_SIZE_FINGER);

  gtk_container_add(GTK_CONTAINER(self->priv->flash_mode_button),
		    self->priv->flash_mode_image);

  g_signal_connect(GTK_CONTAINER(self->priv->flash_mode_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_flash_mode_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->flash_mode_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_flash_mode_button_release),
		   self);
}

static void
_init_video_mic_mode_button(CameraUI2Window* self)
{
  self->priv->video_mic_mode_button = gtk_event_box_new();
  gtk_widget_set_size_request(self->priv->video_mic_mode_button, 64, 64);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->video_mic_mode_button),
				   FALSE);
  gint mic_mode = self->priv->camera_settings.mic_mode;
  self->priv->video_mic_mode_image = 
    gtk_image_new_from_icon_name(video_mic_mode_icon_name(mic_mode, FALSE), HILDON_ICON_SIZE_FINGER);
  gtk_container_add(GTK_CONTAINER(self->priv->video_mic_mode_button),
		    self->priv->video_mic_mode_image);
  g_signal_connect(GTK_CONTAINER(self->priv->video_mic_mode_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_mic_mode_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->video_mic_mode_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_mic_mode_button_release),
		   self);
}

static void
_init_still_settings_button(CameraUI2Window* self)
{
  self->priv->still_settings_button = gtk_event_box_new();
  gtk_widget_set_size_request(self->priv->still_settings_button, 64, 64);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->still_settings_button),
				   FALSE);
  self->priv->still_settings_image = gtk_image_new_from_icon_name("camera_camera_setting", HILDON_ICON_SIZE_FINGER);

  gtk_container_add(GTK_CONTAINER(self->priv->still_settings_button),
		    self->priv->still_settings_image);
  g_signal_connect(GTK_CONTAINER(self->priv->still_settings_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_still_settings_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->still_settings_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_still_settings_button_release),
		   self);
}

static void
_init_video_settings_button(CameraUI2Window* self)
{
  self->priv->video_settings_button = gtk_event_box_new();
  gtk_widget_set_size_request(self->priv->video_settings_button, 64, 64);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->video_settings_button),
				   FALSE);
  self->priv->video_settings_image = gtk_image_new_from_icon_name("camera_camera_setting", HILDON_ICON_SIZE_FINGER);

  gtk_container_add(GTK_CONTAINER(self->priv->video_settings_button),
		    self->priv->video_settings_image);
  g_signal_connect(GTK_CONTAINER(self->priv->video_settings_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_video_settings_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->video_settings_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_video_settings_button_release),
		   self);
}

static void
_init_image_viewer_button(CameraUI2Window* self)
{
  self->priv->image_viewer_button = gtk_event_box_new();
  gtk_widget_set_size_request(self->priv->image_viewer_button, 64, 64);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->image_viewer_button),
				   FALSE);
  self->priv->image_viewer_image = gtk_image_new_from_icon_name("camera_imageviewer", HILDON_ICON_SIZE_FINGER);

  gtk_container_add(GTK_CONTAINER(self->priv->image_viewer_button),
		    self->priv->image_viewer_image);
  g_signal_connect(GTK_CONTAINER(self->priv->image_viewer_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_image_viewer_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->image_viewer_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_image_viewer_button_release),
		   self);
}

static void
_init_video_player_button(CameraUI2Window* self)
{
  self->priv->media_player_button = gtk_event_box_new();
  gtk_widget_set_size_request(self->priv->media_player_button, 64, 64);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->media_player_button),
				   FALSE);
  self->priv->media_player_image = gtk_image_new_from_icon_name("camera_playback", HILDON_ICON_SIZE_FINGER);

  gtk_container_add(GTK_CONTAINER(self->priv->media_player_button),
		    self->priv->media_player_image);
  g_signal_connect(GTK_CONTAINER(self->priv->media_player_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_media_player_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->media_player_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_media_player_button_release),
		   self);
}

static void
_init_video_state_button(CameraUI2Window* self)
{
  self->priv->video_state_button = gtk_event_box_new();
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->video_state_button),
				   FALSE);
  gtk_widget_set_size_request(self->priv->video_state_button, 48, 48);
  
  self->priv->video_state_image = 
    gtk_image_new_from_icon_name(video_state_icon_name(CAM_VIDEO_STATE_RECORDING, FALSE), HILDON_ICON_SIZE_FINGER);
  
  gtk_container_add(GTK_CONTAINER(self->priv->video_state_button),
		    self->priv->video_state_image);
  
  g_signal_connect(GTK_CONTAINER(self->priv->video_state_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_video_state_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->video_state_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_video_state_button_release),
		   self);

  self->priv->video_stop_button = gtk_event_box_new();
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->video_stop_button),
				   FALSE);
  gtk_widget_set_size_request(self->priv->video_stop_button, 48, 48);
  
  self->priv->video_stop_image = 
    gtk_image_new_from_icon_name(video_state_icon_name(CAM_VIDEO_STATE_STOPPED, FALSE), HILDON_ICON_SIZE_FINGER);
  
  gtk_container_add(GTK_CONTAINER(self->priv->video_stop_button),
		    self->priv->video_stop_image);
  
  g_signal_connect(GTK_CONTAINER(self->priv->video_stop_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_video_stop_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->video_stop_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_video_stop_button_release),
		   self);
}

static void
_init_white_balance_mode_button(CameraUI2Window* self)
{
  self->priv->white_balance_button = gtk_event_box_new();
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->white_balance_button),
				   FALSE);
  gtk_widget_set_size_request(self->priv->white_balance_button, 48, 48);
  gint white_balance = self->priv->camera_settings.white_balance;
  self->priv->white_balance_image = 
    gtk_image_new_from_icon_name(white_balance_mode_icon_name(white_balance, FALSE), HILDON_ICON_SIZE_STYLUS);

  gtk_container_add(GTK_CONTAINER(self->priv->white_balance_button),
		    self->priv->white_balance_image);

  g_signal_connect(GTK_CONTAINER(self->priv->white_balance_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_white_balance_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->white_balance_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_white_balance_button_release),
		   self);
}

static void
_init_still_resolution_size_button(CameraUI2Window* self)
{
  self->priv->still_resolution_button = gtk_event_box_new();
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->still_resolution_button),
				   FALSE);
  gtk_widget_set_size_request(self->priv->still_resolution_button, 48, 48);
  self->priv->still_resolution_image = gtk_image_new_from_icon_name(still_resolution_size_icon_name(self->priv->camera_settings.still_resolution_size, FALSE), 
								    HILDON_ICON_SIZE_STYLUS);

  gtk_container_add(GTK_CONTAINER(self->priv->still_resolution_button),
		    self->priv->still_resolution_image);
  g_signal_connect(GTK_CONTAINER(self->priv->still_resolution_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_still_resolution_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->still_resolution_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_still_resolution_button_release),
		   self);
}

static void
_init_video_resolution_size_button(CameraUI2Window* self)
{
  self->priv->video_resolution_button = gtk_event_box_new();
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->video_resolution_button),
				   FALSE);
  gtk_widget_set_size_request(self->priv->video_resolution_button, 48, 48);
  self->priv->video_resolution_image = gtk_image_new_from_icon_name(video_resolution_size_icon_name(self->priv->camera_settings.video_resolution_size, FALSE), HILDON_ICON_SIZE_STYLUS);

  gtk_container_add(GTK_CONTAINER(self->priv->video_resolution_button),
		    self->priv->video_resolution_image);

  g_signal_connect(GTK_CONTAINER(self->priv->video_resolution_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_video_resolution_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->video_resolution_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_video_resolution_button_release),
		   self);
}

static void
_init_iso_level_button(CameraUI2Window* self)
{
  self->priv->iso_level_button = gtk_event_box_new();
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->iso_level_button),
				   FALSE);
  gtk_widget_set_size_request(self->priv->iso_level_button, 48, 48);
  gint iso_level = self->priv->camera_settings.iso_level;
  self->priv->iso_level_image = 
    gtk_image_new_from_icon_name(iso_level_icon_name(iso_level, FALSE), HILDON_ICON_SIZE_STYLUS);
  
  gtk_container_add(GTK_CONTAINER(self->priv->iso_level_button),
		    self->priv->iso_level_image);
  
  g_signal_connect(GTK_CONTAINER(self->priv->iso_level_button), 
		   "button-press-event", 
		   G_CALLBACK(_on_iso_level_button_press),
		   self);
  g_signal_connect(GTK_CONTAINER(self->priv->iso_level_button), 
		   "button-release-event", 
		   G_CALLBACK(_on_iso_level_button_release),
		   self);
}

static void
_init_menu(CameraUI2Window* self)
{
  HildonAppMenu* menu = HILDON_APP_MENU(hildon_app_menu_new());
  GtkWidget* general_settings_button = hildon_gtk_button_new(HILDON_SIZE_FINGER_HEIGHT);
  gtk_button_set_label(GTK_BUTTON(general_settings_button), dgettext("osso-camera-ui", "camera_me_general_settings"));
  g_signal_connect(general_settings_button, "clicked", G_CALLBACK(_on_general_settings_button_pressed), self);
  hildon_app_menu_append(menu, GTK_BUTTON(general_settings_button));
  gtk_widget_show_all(GTK_WIDGET(menu));
  hildon_window_set_app_menu(HILDON_WINDOW(self), menu);
}

GtkWidget*
_create_zoom_slider(CameraUI2Window* self)
{
  GtkWidget* vslider = gtk_vscale_new_with_range(1,
                                                 self->priv->max_zoom,
                                                 (self->priv->max_zoom-1.0)/50.0);
  gtk_rc_parse_string(
		      "style \"fremantle-zoombar\" {\n"
		      "GtkScale::slider-length = 34\n"
		      "GtkScale::slider-width = 6\n"
		      "GtkRange::trough-border = 2\n"
		      "GtkRange::trough-side-details = 0\n"
		      "engine \"sapwood\" {\n"
		      "image {\n"
		      "function = BOX\n"
		      "detail = \"stepper\"\n"
		      "}\n"
		      "image {\n"
		      "function = BOX\n"
		      "detail = \"trough\"\n"
		      "file = \"../../../icons/hicolor/scalable/hildon/camera_zoom_background.png\"\n"
		      "border = {2, 2, 3, 3}\n"
		      "}\n"
		      "image {\n"
		      "function = SLIDER\n"
		      "orientation = VERTICAL\n"
		      "file = \"../../../icons/hicolor/scalable/hildon/camera_zoom_slider.png\"\n"
		      "}\n"
		      "}\n"
		      "}\n"
		      "class \"GtkVScale\" style \"fremantle-zoombar\"\n"
		      );
  gtk_widget_set_size_request(vslider, 8, 128);
  gtk_scale_set_draw_value(GTK_SCALE(vslider), FALSE);
  gtk_range_set_inverted(GTK_RANGE(vslider), TRUE);
  return vslider;
}

static void
_check_mmc_available(CameraUI2Window* self)
{
  if(!storage_helper_mmc_user_writable())
  {
    if(self->priv->camera_settings.storage_device == CAM_STORAGE_EXTERN)
    {
      self->priv->camera_settings.storage_device = CAM_STORAGE_EXTERN_UNAVAILABLE;
    }
  }
  else
  {
    if(self->priv->camera_settings.storage_device == CAM_STORAGE_EXTERN_UNAVAILABLE)
    {
      self->priv->camera_settings.storage_device = CAM_STORAGE_EXTERN;
    }
  }
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->storage_image), 
			       storage_device_icon_name(self->priv->camera_settings.storage_device, FALSE), 
			       HILDON_ICON_SIZE_STYLUS);
}

static void
_read_gconf_camera_settings(CameraUI2Window* self)
{
  self->priv->camera_settings.scene_mode = camera_ui2_get_gconf_scene_mode();
  self->priv->camera_settings.flash_mode = camera_ui2_get_gconf_flash_mode();
  self->priv->camera_settings.white_balance = camera_ui2_get_gconf_white_balance_mode();
  self->priv->camera_settings.iso_level = camera_ui2_get_gconf_iso_level();
  self->priv->camera_settings.exposure_level = camera_ui2_get_gconf_exposure_level();
  self->priv->camera_settings.still_resolution_size = camera_ui2_get_gconf_still_resolution_size();
  self->priv->camera_settings.video_resolution_size = camera_ui2_get_gconf_video_resolution_size();
  self->priv->camera_settings.color_mode=0;
  self->priv->camera_settings.storage_device = camera_ui2_get_gconf_storage_device();
  self->priv->camera_settings.author = camera_ui2_get_gconf_author();
  self->priv->camera_settings.mic_mode = camera_ui2_get_gconf_video_mic_mode();
  self->priv->camera_settings.video_state = CAM_VIDEO_STATE_STOPPED;
  self->priv->camera_settings.preview_mode = camera_ui2_get_gconf_preview_mode();
  self->priv->disable_show_on_lenscover_open = camera_ui2_get_gconf_show_on_lenscover_open();
  self->priv->disable_hide_on_lenscover_close = camera_ui2_get_gconf_hide_on_lenscover_close();
  self->priv->disable_show_on_focus_pressed = camera_ui2_get_gconf_show_on_focus_pressed();
  self->priv->with_sound_effects = camera_ui2_get_gconf_enabled_sound_effects();
  _check_mmc_available(self);
}

static void
_set_camera_settings(CameraUI2Window* self)
{
  camera_interface_set_scene_mode(self->priv->camera_interface, self->priv->camera_settings.scene_mode);
  camera_interface_set_white_balance_mode(self->priv->camera_interface, self->priv->camera_settings.white_balance);
  camera_interface_set_iso_level(self->priv->camera_interface, self->priv->camera_settings.iso_level);
  camera_interface_set_exposure_comp(self->priv->camera_interface, self->priv->camera_settings.exposure_level);
  if(is_video_mode(self->priv->camera_settings.scene_mode))
  {
    camera_interface_set_video_resolution(self->priv->camera_interface, self->priv->camera_settings.video_resolution_size);
    gtk_widget_queue_draw(self->priv->view_finder);
    camera_interface_set_audio_mode(self->priv->camera_interface, self->priv->camera_settings.mic_mode);
  }
  else
  {
    camera_interface_set_flash_mode(self->priv->camera_interface, self->priv->camera_settings.flash_mode);
    camera_interface_set_still_resolution(self->priv->camera_interface, self->priv->camera_settings.still_resolution_size);
  }
  camera_interface_set_colour_tone_mode(self->priv->camera_interface, self->priv->camera_settings.color_mode);
  camera_interface_enable_preview(self->priv->camera_interface, 
				  self->priv->save_raw_image || 
				  self->priv->camera_settings.preview_mode != CAM_PREVIEW_MODE_NO_PREVIEW);
  _update_zoom_slider_range(self);
}

static void
_on_preview_image(GDigicamManager *digicam_manager,
		  GdkPixbuf *preview,
		  gpointer data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(data);
  GtkWidget* image = gtk_image_new_from_pixbuf(preview);
  int width = gdk_pixbuf_get_width(preview);
  int height = gdk_pixbuf_get_height(preview);
  if(!is_video_mode(self->priv->camera_settings.scene_mode) && self->priv->save_raw_image)
  {
    gchar* name = g_strdup_printf("%s.dng",self->priv->ccapture_data.filename);
    CameraCaptureData* camera_capture_data = NULL;
    camera_interface_get_camera_capture_data(self->priv->camera_interface, &camera_capture_data);
    raw_export_data_t* export_data = g_new0(raw_export_data_t, 1);
    export_data->raw_data = camera_interface_get_raw_data();
    export_data->raw_width = 2952;
    export_data->raw_height = 1968;
    export_data->preview_data = gdk_pixbuf_get_pixels(preview);
    export_data->preview_width = width;
    export_data->preview_height = height;
    export_data->file_name = name;
    export_data->camera_capture_data = camera_capture_data;
    export_image_data(export_data);
    g_free(export_data->camera_capture_data->date_time_stamp);
    g_free(export_data->camera_capture_data);
    g_free(export_data);
    g_free(name);
  }
  if(self->priv->camera_settings.preview_mode != CAM_PREVIEW_MODE_NO_PREVIEW  && _is_visible(self))
  {
    gtk_widget_hide(self->priv->view_finder);
    if(is_video_mode(self->priv->camera_settings.scene_mode))
    {
      camera_ui2_preview_window_show_video_preview(self->priv->preview_window, 
						   self->priv->is_fullscreen,
						   self->priv->camera_settings.preview_mode,
						   &self->priv->ccapture_data,
						   preview);
    }
    else
    {
      camera_ui2_preview_window_show_image_preview(self->priv->preview_window, 
						   self->priv->is_fullscreen,
						   self->priv->camera_settings.preview_mode,
						   &self->priv->ccapture_data,
						   preview);
    }
    gtk_widget_show(GTK_WIDGET(self->priv->preview_window));
  }
}

static void
_on_focus_done(GDigicamManager *digicam_manager,
	       guint status,
	       gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  switch(status)
  {
    case G_DIGICAM_FOCUSMODESTATUS_UNABLETOREACH:
      gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->crosshair), "camera_crosshair_red", 216);
      break;
    case G_DIGICAM_FOCUSMODESTATUS_REACHED:
      if(self->priv->with_sound_effects)
	sound_player_focus_done_sound(self->priv->sound_player);
      gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->crosshair), "camera_crosshair_green", 216);
      break;
    case G_DIGICAM_FOCUSMODESTATUS_REQUEST:
      gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->crosshair), "camera_crosshair_white", 216);
      break;
  }
  _update_focus_label(self);
}

static void
_on_pict_done(GDigicamManager* digicam_manager,
	      const gchar* filename,
	      gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  _update_remaining_count_indicator(self);
}

static void
_on_capture_start(GDigicamManager* digicam_manager,
		  gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  self->priv->in_capture_phase = TRUE;
}

static void
_on_capture_end(GDigicamManager* digicam_manager,
		gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
  self->priv->in_capture_phase = FALSE;
}

static void
_on_picture_got(GDigicamManager* digicam_manager,
		gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
}

static void
_show_hide_ui(CameraUI2Window* self, gboolean show)
{
  if(show)
  {
    gtk_widget_show(self->priv->right_button_box);
    gtk_widget_show(self->priv->bottom_button_box);
    gtk_widget_show(self->priv->left_button_box);
  }
  else
  {
    gtk_widget_hide(self->priv->right_button_box);
    gtk_widget_hide(self->priv->bottom_button_box);
    gtk_widget_hide(self->priv->left_button_box);
  }
}

void camera_ui2_window_show_ui(CameraUI2Window* self)
{
  self->priv->lenscover_open = !dbus_helper_is_camera_lenscover_closed(self->priv->osso);
  if(self->priv->lenscover_open)
  {
    gtk_widget_hide(self->priv->standby_view);
    gtk_widget_show(self->priv->view_finder);
    camera_interface_open_viewfinder(self->priv->camera_interface, 
				     GDK_WINDOW_XWINDOW(self->priv->view_finder->window)); 
    _check_mmc_available(self);
    _update_remaining_count_indicator(self);
    gtk_widget_queue_draw(self->priv->view_finder);
    if(geotagging_helper_get_mode(self->priv->geotagging_helper) != GEO_TAG_NONE)
    {
      geotagging_helper_run(self->priv->geotagging_helper);
    }
  }
  else
  {
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->standby_image),
				 "camera_lenscover", 216);
    gtk_label_set_text(GTK_LABEL(self->priv->standby_label), dgettext("osso-camera-ui", "camera_ia_open_lens_cover"));
  }
}

void camera_ui2_window_hide_ui(CameraUI2Window* self)
{
  if(self->priv->in_capture_phase)
  {
    _stop_recording(self);
  }
  camera_interface_close_viewfinder(self->priv->camera_interface); 
  gtk_widget_hide(self->priv->view_finder);
  if(!self->priv->lenscover_open)
  {
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->standby_image),
				 "camera_standby", 216);
    gtk_label_set_text(GTK_LABEL(self->priv->standby_label), dgettext("osso-camera-ui", "camera_ia_standby"));
  }
  gtk_widget_show(self->priv->standby_view);
}

static void
_on_top_changed(HildonWindow* window,
		GParamSpec* psp,
		gpointer user_data)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(user_data);
}

static gboolean
_start_delayed_focus(CameraUI2Window* self)
{
  if(self->priv->capture_timer == 0)
    _show_hide_ui(self, FALSE);
  camera_interface_autofocus(self->priv->camera_interface, TRUE);
  return FALSE;
}

static void
_init_delayed_focus(CameraUI2Window* self)
{
  _remove_delayed_focus_timer(self);
  self->priv->delayed_focus_timer = g_timeout_add(500,
						  (GSourceFunc)_start_delayed_focus,
						  self);
}

static void
_on_hide_window(CameraUI2Window* self)
{
  if(self->priv->in_capture_phase)
  {
    _stop_recording(self);
  }
  geotagging_helper_stop(self->priv->geotagging_helper);
}

static void
_init_view_finder(CameraUI2Window* self)
{
  GdkColor color;
  gint colorkey = camera_interface_get_color_key(self->priv->camera_interface);
  color.pixel = 0;
  color.red = (colorkey & 0xff0000) >> 8;
  color.green = (colorkey & 0x00ff00) >> 8;
  color.blue = (colorkey & 0x0000ff) << 8;
  gtk_widget_modify_bg(self->priv->view_finder,
		       GTK_STATE_NORMAL, &color);
}

static void
_on_gps_fix(gpointer user_data)
{
  g_warning("got fix\n");
}

static DBusHandlerResult
_on_msg_recieved(DBusConnection* connection G_GNUC_UNUSED, DBusMessage* message, void* data)
{
    CameraUI2Window* window = (CameraUI2Window*)data;
    DBusMessageIter iter;
    const char* result = NULL;

    dbus_message_iter_init(message, &iter);
    dbus_message_iter_get_basic(&iter, &result);

    if (g_strcmp0(dbus_message_get_path(message), MCE_SIGNAL_PATH) == 0) {
        if ((g_strcmp0(result, MCE_DISPLAY_OFF_STRING) ==0) && _is_topmost_window(window)) {
            camera_ui2_window_hide_ui (window);
            return DBUS_HANDLER_RESULT_HANDLED;
        } else if ((g_strcmp0(result, MCE_DISPLAY_ON_STRING) ==0) && _is_topmost_window(window)) {
            camera_ui2_window_show_ui (window);
            return DBUS_HANDLER_RESULT_HANDLED;
        } else {
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        }
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static void
_on_gps_fix_lost(gpointer user_data)
{
  g_warning("fix lost\n");
}

CameraUI2Window*
camera_ui2_window_new(osso_context_t* osso, HildonProgram* program)
{
  CameraUI2Window* self = 
    CAMERA_UI2_WINDOW(g_object_new(CAMERA_UI2_TYPE_WINDOW, NULL));
  self->priv->osso = osso;
  _init_preview_window(self);
  hildon_program_add_window(program, HILDON_WINDOW(self));
  hildon_program_add_window(program, HILDON_WINDOW(self->priv->preview_window));
  self->priv->lenscover_open = !dbus_helper_is_camera_lenscover_closed(osso);
  self->priv->program = program;
  return self;
}

static void
camera_ui2_window_dispose(GObject* object)
{
  G_OBJECT_CLASS(camera_ui2_window_parent_class)->dispose(object);
}

static void
camera_ui2_window_finalize(GObject* object)
{
  CameraUI2Window* self = CAMERA_UI2_WINDOW(object);
  if(self->priv->with_sound_effects)
    sound_player_destroy(self->priv->sound_player);
  G_OBJECT_CLASS(camera_ui2_window_parent_class)->finalize(object);
}

static void
camera_ui2_window_class_init(CameraUI2WindowClass* klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS(klass);
  g_type_class_add_private(klass, sizeof(CameraUI2WindowPrivate));
  object_class->dispose = camera_ui2_window_dispose;
  object_class->finalize = camera_ui2_window_finalize;
}

static void
camera_ui2_window_init(CameraUI2Window* self)
{
  self->priv = CAMERA_UI2_WINDOW_GET_PRIVATE(self);
  self->priv->cam = CAM_BACK_CAM;
  self->priv->is_fullscreen = TRUE;
  self->priv->in_capture_phase = FALSE;
  self->priv->save_raw_image = FALSE;
  self->priv->lenscover_open = FALSE;
  self->priv->disable_show_on_lenscover_open = FALSE;
  self->priv->disable_hide_on_lenscover_close = FALSE;
  self->priv->disable_show_on_focus_pressed = FALSE;
  self->priv->with_sound_effects = FALSE;
  self->priv->delayed_focus_timer = 0;
  self->priv->capture_timer = 0;
  self->priv->capture_timer_countdown = 0;
  self->priv->recording_timer = 0;
  self->priv->recording_time = 0;
  self->priv->camera_interface = create_camera_interface();
  self->priv->ccapture_data.filename = NULL;
  self->priv->ccapture_data.orientation_mode = 1;
  self->priv->ccapture_data.longitude = G_MAXDOUBLE;
  self->priv->ccapture_data.latitude = G_MAXDOUBLE;
  self->priv->ccapture_data.country = NULL;
  self->priv->ccapture_data.city = NULL;
  self->priv->ccapture_data.suburb = NULL;
  self->priv->ccapture_data.author = NULL;
  self->priv->ccapture_data.classification_id = NULL;
  self->priv->geotagging_helper = geotagging_helper_create();
  self->priv->geotagging_settings.tag_position = FALSE;
  self->priv->geotagging_settings.tag_location = FALSE;
  self->priv->sound_player = sound_player_create();
  self->priv->view_finder = gtk_event_box_new();
  self->priv->image_counter_label = gtk_label_new("999");
  self->priv->focus_label = gtk_label_new("0");
  self->priv->raw_indicator_label = gtk_label_new("RAW");
  self->priv->recording_time_label = gtk_label_new("");
  self->priv->sysbus_conn = dbus_bus_get (DBUS_BUS_SYSTEM, NULL);
  gtk_label_set_markup(GTK_LABEL(self->priv->recording_time_label), RECORDING_TIME_ZERO_MARKUP);
  _read_gconf_camera_settings(self);
  _set_camera_settings(self);
  _init_close_window_button(self);
  _init_close_standby_window_button(self);
  _init_scene_mode_button(self);
  _init_flash_mode_button(self);
  _init_video_mic_mode_button(self);
  _init_still_settings_button(self);
  _init_video_settings_button(self);
  _init_color_mode_button(self);
  _init_white_balance_mode_button(self);
  _init_iso_level_button(self);
  _init_still_resolution_size_button(self);
  _init_video_resolution_size_button(self);
  _init_image_viewer_button(self);
  _init_video_player_button(self);
  _init_storage_device_button(self);
  _init_geotagging_button(self);
  _init_still_capture_button(self);
  _init_video_state_button(self);
  _init_capture_timer_button(self);
  _init_menu(self);

  self->priv->bottom_button_box = gtk_hbox_new(FALSE, 0);
  self->priv->top_button_box = gtk_hbox_new(FALSE, 0);
  self->priv->right_button_box = gtk_vbox_new(FALSE, 0);
  self->priv->left_button_box = gtk_vbox_new(FALSE, 0);
  self->priv->root_container = gtk_vbox_new(FALSE, 0);

  GtkWidget* center_box = gtk_vbox_new(FALSE, 0);
  GtkWidget* right_button_box2 = gtk_vbox_new(TRUE, 0);

  gtk_widget_set_size_request(self->priv->right_button_box, 84, -1);
  gtk_widget_set_size_request(self->priv->left_button_box, 84, -1);
  gtk_widget_set_size_request(self->priv->bottom_button_box, -1, 48);
  gtk_widget_set_size_request(self->priv->top_button_box, -1, 48);

  self->priv->crosshair = gtk_image_new_from_icon_name("camera_crosshair_white", 216);

  self->priv->zoom_slider = _create_zoom_slider(self);

  gtk_box_pack_start(GTK_BOX(self->priv->right_button_box), self->priv->close_window_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(right_button_box2), self->priv->scene_mode_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(right_button_box2), self->priv->flash_mode_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(right_button_box2), self->priv->video_mic_mode_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(right_button_box2), self->priv->still_settings_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(right_button_box2), self->priv->video_settings_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(right_button_box2), self->priv->image_viewer_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(right_button_box2), self->priv->media_player_button, TRUE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(self->priv->bottom_button_box), self->priv->image_counter_label, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(self->priv->bottom_button_box), self->priv->storage_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(self->priv->bottom_button_box), self->priv->still_resolution_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(self->priv->bottom_button_box), self->priv->video_resolution_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(self->priv->bottom_button_box), self->priv->white_balance_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(self->priv->bottom_button_box), self->priv->iso_level_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(self->priv->bottom_button_box), self->priv->geotagging_button, FALSE, FALSE, 0);


  gtk_box_pack_end(GTK_BOX(self->priv->top_button_box), self->priv->focus_label, FALSE, FALSE, 0);

  gtk_box_pack_end(GTK_BOX(self->priv->bottom_button_box), self->priv->recording_time_label, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(self->priv->bottom_button_box), self->priv->raw_indicator_label, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(self->priv->raw_indicator_label), 1, 0.5);
  
  GtkWidget* zoom_box = gtk_hbox_new(FALSE, 0);

  gtk_box_pack_start(GTK_BOX(zoom_box), self->priv->zoom_slider, FALSE, FALSE, 10);
  gtk_widget_show_all(zoom_box);
  gtk_box_pack_start(GTK_BOX(self->priv->left_button_box), zoom_box, FALSE, FALSE, 10);
  gtk_box_pack_start(GTK_BOX(self->priv->left_button_box), self->priv->capture_timer_button, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(self->priv->left_button_box), self->priv->color_mode_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(self->priv->left_button_box), self->priv->still_capture_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(self->priv->left_button_box), self->priv->video_state_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(self->priv->left_button_box), self->priv->video_stop_button, FALSE, FALSE, 0);


  gtk_box_pack_end(GTK_BOX(self->priv->right_button_box), right_button_box2, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(center_box), self->priv->crosshair, TRUE, TRUE, 0);

  GtkWidget* root_table = gtk_table_new(3, 3, FALSE);
  GtkWidget* frame_bottom_box = gtk_hbox_new(FALSE, 0);
  GtkWidget* frame_top_box = gtk_hbox_new(FALSE, 0);
  GtkWidget* frame_right_box = gtk_vbox_new(FALSE, 0);
  GtkWidget* frame_left_box = gtk_vbox_new(FALSE, 0);

  gtk_widget_show(frame_right_box);
  gtk_widget_show(frame_left_box);
  gtk_widget_show(frame_bottom_box);
  gtk_widget_show(frame_top_box);

  gtk_widget_set_size_request(frame_right_box, 84, -1);
  gtk_widget_set_size_request(frame_left_box, 84, -1);
  gtk_widget_set_size_request(frame_bottom_box, -1, 48);
  gtk_widget_set_size_request(frame_top_box, -1, 48);
  gtk_box_pack_start(GTK_BOX(frame_right_box), self->priv->right_button_box, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(frame_left_box), self->priv->left_button_box, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(frame_top_box), self->priv->top_button_box, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(frame_bottom_box), self->priv->bottom_button_box, TRUE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(self->priv->view_finder), root_table);
  //  gtk_table_attach(GTK_TABLE(root_table), self->priv->left_button_box, 0, 1, 0, 2, GTK_SHRINK, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_table_attach(GTK_TABLE(root_table), frame_left_box, 0, 1, 0, 2, GTK_SHRINK, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_table_attach(GTK_TABLE(root_table), center_box, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
  // gtk_table_attach(GTK_TABLE(root_table), self->priv->right_button_box, 2, 3, 0, 3, GTK_SHRINK, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_table_attach(GTK_TABLE(root_table), frame_right_box, 2, 3, 0, 3, GTK_SHRINK, GTK_EXPAND | GTK_FILL, 0, 0);
  //  gtk_table_attach(GTK_TABLE(root_table), self->priv->bottom_button_box, 0, 2, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(GTK_TABLE(root_table), frame_bottom_box, 0, 2, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
  //  gtk_table_attach(GTK_TABLE(root_table), self->priv->top_button_box, 0, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(GTK_TABLE(root_table), frame_top_box, 0, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);

  _init_view_finder(self);
  
  gtk_container_set_border_width(GTK_CONTAINER(self), 0);
  gtk_container_add(GTK_CONTAINER(self), self->priv->root_container);
  self->priv->standby_image = gtk_image_new_from_icon_name("camera_standby", 216);
  self->priv->standby_view = gtk_event_box_new();
  GtkWidget* standby_view_box = gtk_hbox_new(FALSE, 0);
  self->priv->standby_label = gtk_label_new(dgettext("osso-camera-ui", "camera_ia_open_lens_cover"));

  gtk_box_pack_start(GTK_BOX(standby_view_box), self->priv->standby_image, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(standby_view_box), self->priv->standby_label, TRUE, TRUE, 0);
  GtkWidget* alignment = gtk_alignment_new(1.0, 0, 0, 0);
  gtk_container_add(GTK_CONTAINER(alignment), self->priv->close_standby_window_button);
  gtk_box_pack_start(GTK_BOX(standby_view_box), alignment, TRUE, TRUE, 0);
  gtk_widget_show_all(standby_view_box);
  gtk_container_add(GTK_CONTAINER(self->priv->standby_view), standby_view_box);

  gtk_box_pack_start(GTK_BOX(self->priv->root_container), self->priv->view_finder, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(self->priv->root_container), self->priv->standby_view, TRUE, TRUE, 0);


  g_object_set(self->priv->view_finder, "can-focus", TRUE, NULL);
  g_signal_connect(self->priv->view_finder, "button-release-event", G_CALLBACK(_on_viewfinder_pressed), self);
  g_signal_connect(self->priv->standby_view, "button-release-event", G_CALLBACK(_on_viewfinder_pressed), self);

  g_signal_connect(GTK_WIDGET(self), "key-press-event", G_CALLBACK(_on_key_pressed), self);
  g_signal_connect(GTK_WIDGET(self), "key-release-event", G_CALLBACK(_on_key_released), self);
  g_signal_connect(GTK_WIDGET(self), "hide", G_CALLBACK(_on_hide_window), self);
  g_signal_connect(self, "notify::is-topmost", G_CALLBACK(_on_top_changed), self);

  hildon_gtk_window_enable_zoom_keys(GTK_WINDOW(self), TRUE);
  gtk_window_fullscreen(GTK_WINDOW(self));

  gtk_widget_show(self->priv->root_container);
  gtk_widget_show(self->priv->view_finder);
  gtk_widget_show(root_table);
  gtk_widget_show(center_box);

  gtk_widget_show(self->priv->left_button_box);
  gtk_widget_show(self->priv->right_button_box);
  gtk_widget_show(right_button_box2);
  gtk_widget_show(self->priv->bottom_button_box);
  gtk_widget_show(self->priv->top_button_box);
  gtk_widget_show_all(self->priv->scene_mode_button);
  gtk_widget_show_all(self->priv->color_mode_button);
  gtk_widget_show_all(self->priv->storage_button);
  gtk_widget_show_all(self->priv->geotagging_button);
  gtk_widget_show_all(self->priv->white_balance_button);
  gtk_widget_show_all(self->priv->iso_level_button);

  switch(self->priv->camera_settings.scene_mode)
  {
    case CAM_SCENE_MODE_AUTO:
    case CAM_SCENE_MODE_PORTRAIT:
    case CAM_SCENE_MODE_MACRO:
    case CAM_SCENE_MODE_VIDEO:
      gtk_widget_show(self->priv->crosshair);
      break;
    default:
      gtk_widget_hide(self->priv->crosshair);
  }

  gtk_widget_show(self->priv->image_counter_label);
  gtk_widget_hide(self->priv->focus_label);

  gtk_widget_show(self->priv->zoom_slider);
  gtk_widget_show_all(self->priv->close_window_button);

  _activate_scene_mode_ui(self);
  camera_interface_signal_connect(self->priv->camera_interface, 
				  "focus-done",
				  G_CALLBACK(_on_focus_done), self);
  camera_interface_signal_connect(self->priv->camera_interface, 
				  "pict-done",
				  G_CALLBACK(_on_pict_done), self);
  camera_interface_signal_connect(self->priv->camera_interface, 
				  "capture-start",
				  G_CALLBACK(_on_capture_start), self);
  camera_interface_signal_connect(self->priv->camera_interface, 
				  "capture-end",
				  G_CALLBACK(_on_capture_end), self);
  camera_interface_signal_connect(self->priv->camera_interface, 
				  "picture-got",
				  G_CALLBACK(_on_picture_got), self);
  camera_interface_signal_connect(self->priv->camera_interface, 
				  "image-preview", 
				  G_CALLBACK(_on_preview_image), self);

  dbus_bus_add_match (self->priv->sysbus_conn,
                      "type='signal', "
                      "interface='" MCE_SIGNAL_IF "', "
                      "member='" MCE_DISPLAY_SIG "'",
                      NULL);

  dbus_connection_add_filter (self->priv->sysbus_conn,
                              _on_msg_recieved,
                              self,
                              NULL);

  geotagging_helper_register_fix_listener(self->priv->geotagging_helper, _on_gps_fix, self);
  geotagging_helper_register_fix_lost_listener(self->priv->geotagging_helper, _on_gps_fix_lost, self);
}

void
camera_ui2_window_top_application(CameraUI2Window* self)
{
  gtk_widget_show(GTK_WIDGET(self));
}

void
camera_ui2_window_lenscover_closed(CameraUI2Window* self)
{
  self->priv->lenscover_open = FALSE;
  if(_is_visible(self))
  {
    camera_ui2_window_hide_ui(self);
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->standby_image),
				 "camera_lenscover", 216);
    gtk_label_set_text(GTK_LABEL(self->priv->standby_label), dgettext("osso-camera-ui", "camera_ia_open_lens_cover"));
  }
  if(!self->priv->disable_hide_on_lenscover_close)
    gtk_widget_hide(GTK_WIDGET(self));
}

void
camera_ui2_window_lenscover_opened(CameraUI2Window* self)
{
  self->priv->lenscover_open = TRUE;
  if(!_is_topmost_window(self))
  {
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->standby_image),
				 "camera_standby", 216);
    gtk_label_set_text(GTK_LABEL(self->priv->standby_label), dgettext("osso-camera-ui", "camera_ia_standby"));
  }
  if(_is_visible(self) && _is_topmost_window(self))
    camera_ui2_window_show_ui(self);
  if(!self->priv->disable_show_on_lenscover_open)
    gtk_widget_show(GTK_WIDGET(self));
}

void
camera_ui2_window_focus_button_pressed(CameraUI2Window* self)
{
  if(self->priv->lenscover_open)
  {
    if(_is_topmost_window(self) || !self->priv->disable_show_on_focus_pressed)
    {
      if(!_is_topmost_window(self))
	gtk_widget_show(GTK_WIDGET(self));

      if(self->priv->camera_settings.scene_mode == CAM_SCENE_MODE_VIDEO ||
	 self->priv->camera_settings.scene_mode == CAM_SCENE_MODE_PORTRAIT ||
	 self->priv->camera_settings.scene_mode == CAM_SCENE_MODE_AUTO ||
	 self->priv->camera_settings.scene_mode == CAM_SCENE_MODE_MACRO)
	_init_delayed_focus(self);
      else
      {
	if(self->priv->capture_timer == 0)
	  _show_hide_ui(self, FALSE);
      }
    }
    if(_is_topmost_window(self->priv->preview_window))
    {
      gtk_widget_hide(GTK_WIDGET(self->priv->preview_window));
    }
  }
}

void
camera_ui2_window_focus_button_released(CameraUI2Window* self)
{
  if(self->priv->lenscover_open)
  {
    _remove_delayed_focus_timer(self);
    if(self->priv->capture_timer == 0)
      _show_hide_ui(self, TRUE);
    camera_interface_autofocus(self->priv->camera_interface, FALSE);
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->crosshair), "camera_crosshair_white", 216);
  }
}

void
camera_ui2_window_capture_button_released(CameraUI2Window* self)
{
}

void
camera_ui2_window_capture_button_pressed(CameraUI2Window* self)
{
  if(!self->priv->lenscover_open)
  {
    if(!dbus_helper_get_device_locked(self->priv->osso) && 
       !_is_topmost_window(self) && 
       !self->priv->disable_show_on_focus_pressed)
    {
      gtk_widget_show(GTK_WIDGET(self));
      return;
    }
  }
  else if(_is_topmost_window(self))
  {
    _remove_delayed_focus_timer(self);
    if(!is_video_mode(self->priv->camera_settings.scene_mode))
    {
      _capture_image(self);
    }
    else
    {
      if(!self->priv->in_capture_phase)
      {
	_start_recording(self);
      }
      else
      {
	_stop_recording(self);
      }
    }
  }
}
