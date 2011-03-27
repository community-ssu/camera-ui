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

#include "camera-ui2-preview-window.h"
#include "dbus-helper.h"
#include <locale.h>
#include <libintl.h>
#include <sharing-dialog.h>
#include <math.h>

struct _CameraUI2PreviewWindowPrivate
{
  osso_context_t* osso;
  GtkWidget* hide_window_button;
  GtkWidget* hide_window_image;
  GtkWidget* sharing_button;
  GtkWidget* sharing_image;
  GtkWidget* image_viewer_button;
  GtkWidget* image_viewer_image;
  GtkWidget* media_player_button;
  GtkWidget* media_player_image;
  GtkWidget* delete_image_button;
  GtkWidget* delete_image_image;
  GtkWidget* preview_button;
  GtkWidget* geo_tag_box;
  GtkWidget* geo_tag_label;
  GtkWidget* content;
  GdkPixbuf* preview;
  gchar* current_file;
  gboolean is_fullscreen; 
  guint preview_hide_timer;
};

#define CAMERA_UI2_PREVIEW_WINDOW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), CAMERA_UI2_PREVIEW_TYPE_WINDOW, CameraUI2PreviewWindowPrivate))

G_DEFINE_TYPE(CameraUI2PreviewWindow, camera_ui2_preview_window, HILDON_TYPE_STACKABLE_WINDOW)


static void
_remove_preview_window_timer(CameraUI2PreviewWindow* self)
{
  if(self->priv->preview_hide_timer)
  {
    g_source_remove(self->priv->preview_hide_timer);
    self->priv->preview_hide_timer = 0;
  }
}

static gboolean
_close_preview_window(CameraUI2PreviewWindow* self)
{
  gtk_widget_hide(GTK_WIDGET(self));
  return FALSE;
}

static void
_start_preview_window_timer(CameraUI2PreviewWindow* self, guint seconds)
{
  _remove_preview_window_timer(self);
  self->priv->preview_hide_timer = g_timeout_add_seconds(seconds,
							 (GSourceFunc)_close_preview_window,
							 self);
}

static gboolean
_on_hide_window_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2PreviewWindow* self = CAMERA_UI2_PREVIEW_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->hide_window_image), 
			       "camera_overlay_back_pressed", HILDON_ICON_SIZE_THUMB);
  return TRUE;
}

static gboolean
_on_hide_window_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2PreviewWindow* self = CAMERA_UI2_PREVIEW_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->hide_window_image), 
			       "camera_overlay_back", HILDON_ICON_SIZE_THUMB);
  gtk_widget_hide(GTK_WIDGET(self));
  return TRUE;
}

static gboolean
_on_sharing_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2PreviewWindow* self = CAMERA_UI2_PREVIEW_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->sharing_image), 
			       "general_share_pressed", HILDON_ICON_SIZE_THUMB);
  return TRUE;
}

static gboolean
_on_sharing_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2PreviewWindow* self = CAMERA_UI2_PREVIEW_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->sharing_image), 
			       "general_share", HILDON_ICON_SIZE_THUMB);
  sharing_dialog_with_file(self->priv->osso, GTK_WINDOW(self), self->priv->current_file);
  return TRUE;
}

static gboolean
_on_image_viewer_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2PreviewWindow* self = CAMERA_UI2_PREVIEW_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->image_viewer_image), 
			       "camera_imageviewer_pressed", HILDON_ICON_SIZE_THUMB);
  return TRUE;
}

static gboolean
_on_image_viewer_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2PreviewWindow* self = CAMERA_UI2_PREVIEW_WINDOW(user_data);
  dbus_helper_start_viewer(self->priv->osso, self->priv->current_file);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->image_viewer_image), 
			       "camera_imageviewer", HILDON_ICON_SIZE_THUMB);
  return TRUE;
}

static gboolean
_on_media_player_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2PreviewWindow* self = CAMERA_UI2_PREVIEW_WINDOW(user_data);
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->media_player_image), 
			       "camera_playback_pressed", HILDON_ICON_SIZE_THUMB);
  return TRUE;
}

static gboolean
_on_media_player_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2PreviewWindow* self = CAMERA_UI2_PREVIEW_WINDOW(user_data);
  dbus_helper_start_mime_viewer(self->priv->osso, self->priv->current_file, "video/mp4");
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->media_player_image), 
			       "camera_playback", HILDON_ICON_SIZE_THUMB);
  return TRUE;
}

static gboolean
_on_delete_image_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2PreviewWindow* self = CAMERA_UI2_PREVIEW_WINDOW(user_data);
  _remove_preview_window_timer(self);
  if(!self->priv->current_file)
    return TRUE;

  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->delete_image_image), 
			       "camera_delete_pressed", HILDON_ICON_SIZE_THUMB);
  return TRUE;
}

static gboolean
_on_delete_image_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2PreviewWindow* self = CAMERA_UI2_PREVIEW_WINDOW(user_data);
  if(!self->priv->current_file)
    return TRUE;
  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->delete_image_image), 
			       "camera_delete", HILDON_ICON_SIZE_THUMB);
  gchar* filename = g_path_get_basename(self->priv->current_file);
  gchar* confirmation_text = g_strdup_printf(dgettext("osso-camera-ui", "camera_nc_delete_imageclip"),
					     filename);
  GtkWidget* ask_user = hildon_note_new_confirmation(GTK_WINDOW(self),
						     confirmation_text);
  g_free(filename);
  g_free(confirmation_text);
  int confirmation = gtk_dialog_run(GTK_DIALOG(ask_user));
  gtk_widget_destroy(ask_user);
  if(confirmation == GTK_RESPONSE_OK)
  {
    g_unlink(self->priv->current_file);
    g_free(self->priv->current_file);
    self->priv->current_file = NULL;
    gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->delete_image_image), 
				 "camera_delete_dimmed", HILDON_ICON_SIZE_THUMB);
    gtk_widget_hide(GTK_WIDGET(self));
  }
  
  return TRUE;
}

static gboolean
_on_preview_expose(GtkWidget* widget, GdkEventExpose* event, gpointer user_data)
{
  CameraUI2PreviewWindow* self = CAMERA_UI2_PREVIEW_WINDOW(user_data);
  if(self->priv->preview)
  {
    int preview_width = gdk_pixbuf_get_width(self->priv->preview);
    int preview_height = gdk_pixbuf_get_height(self->priv->preview);
    int offset_x = MAX(0, (event->area.width - preview_width)/2);
    int offset_y = MAX(0, (event->area.height - preview_height)/2);
    gdk_pixbuf_render_to_drawable(self->priv->preview, widget->window, widget->style->fg_gc[GTK_STATE_NORMAL],
				  0, 0,offset_x, offset_y,
				  MIN(event->area.width, preview_width), 
				  MIN(event->area.height, gdk_pixbuf_get_height(self->priv->preview)),
				  GDK_RGB_DITHER_NONE, 0, 0);
  }
  return FALSE;
}

static gboolean
_on_preview_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
  CameraUI2PreviewWindow* self = CAMERA_UI2_PREVIEW_WINDOW(user_data);
  _remove_preview_window_timer(self);
  if(!self->priv->is_fullscreen)
  {
    gtk_widget_show(self->priv->hide_window_button);
    gtk_window_fullscreen(GTK_WINDOW(self));
  }
  else
  {
    gtk_widget_hide(self->priv->hide_window_button);
    gtk_window_unfullscreen(GTK_WINDOW(self));
  }
  self->priv->is_fullscreen = !self->priv->is_fullscreen;
  gtk_widget_queue_draw(GTK_WIDGET(self));
  return TRUE;
}

static void
_init_hide_window_button(CameraUI2PreviewWindow* self)
{
  self->priv->hide_window_button = gtk_event_box_new();
  gtk_widget_set_size_request(self->priv->hide_window_button, 64, 64);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->hide_window_button),
				   FALSE);
  self->priv->hide_window_image = gtk_image_new_from_icon_name("camera_overlay_back", HILDON_ICON_SIZE_FINGER);
  gtk_container_add(GTK_CONTAINER(self->priv->hide_window_button),
		    self->priv->hide_window_image);
  g_signal_connect(self->priv->hide_window_button, "button-press-event",
		   G_CALLBACK(_on_hide_window_button_press), self);
  g_signal_connect(self->priv->hide_window_button, "button-release-event",
		   G_CALLBACK(_on_hide_window_button_release), self);
}

static void
_init_sharing_button(CameraUI2PreviewWindow* self)
{
  self->priv->sharing_button = gtk_event_box_new();
  gtk_widget_set_size_request(self->priv->sharing_button, 64, 64);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->sharing_button),
				   FALSE);
  self->priv->sharing_image = gtk_image_new_from_icon_name("general_share", HILDON_ICON_SIZE_FINGER);
  gtk_container_add(GTK_CONTAINER(self->priv->sharing_button),
		    self->priv->sharing_image);
  g_signal_connect(self->priv->sharing_button, "button-press-event",
		   G_CALLBACK(_on_sharing_button_press), self);
  g_signal_connect(self->priv->sharing_button, "button-release-event",
		   G_CALLBACK(_on_sharing_button_release), self);
}


static void
_init_image_viewer_button(CameraUI2PreviewWindow* self)
{
  self->priv->image_viewer_button = gtk_event_box_new();
  gtk_widget_set_size_request(self->priv->image_viewer_button, 64, 64);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->image_viewer_button),
				   FALSE);
  self->priv->image_viewer_image = gtk_image_new_from_icon_name("camera_imageviewer", HILDON_ICON_SIZE_FINGER);
  gtk_container_add(GTK_CONTAINER(self->priv->image_viewer_button),
		    self->priv->image_viewer_image);
  g_signal_connect(self->priv->image_viewer_button, "button-press-event",
		   G_CALLBACK(_on_image_viewer_button_press), self);
  g_signal_connect(self->priv->image_viewer_button, "button-release-event",
		   G_CALLBACK(_on_image_viewer_button_release), self);
}

static void
_init_media_player_button(CameraUI2PreviewWindow* self)
{
  self->priv->media_player_button = gtk_event_box_new();
  gtk_widget_set_size_request(self->priv->media_player_button, 64, 64);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->media_player_button),
				   FALSE);
  self->priv->media_player_image = gtk_image_new_from_icon_name("camera_playback", HILDON_ICON_SIZE_FINGER);
  gtk_container_add(GTK_CONTAINER(self->priv->media_player_button),
		    self->priv->media_player_image);
  g_signal_connect(self->priv->media_player_button, "button-press-event",
		   G_CALLBACK(_on_media_player_button_press), self);
  g_signal_connect(self->priv->media_player_button, "button-release-event",
		   G_CALLBACK(_on_media_player_button_release), self);
}


static void
_init_delete_image_button(CameraUI2PreviewWindow* self)
{
  self->priv->delete_image_button = gtk_event_box_new();
  gtk_widget_set_size_request(self->priv->delete_image_button, 64, 64);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->delete_image_button),
				   FALSE);
  self->priv->delete_image_image = gtk_image_new_from_icon_name("camera_delete", HILDON_ICON_SIZE_FINGER);
  gtk_container_add(GTK_CONTAINER(self->priv->delete_image_button),
		    self->priv->delete_image_image);
  g_signal_connect(self->priv->delete_image_button, "button-press-event",
		   G_CALLBACK(_on_delete_image_button_press), self);
  g_signal_connect(self->priv->delete_image_button, "button-release-event",
		   G_CALLBACK(_on_delete_image_button_release), self);
}

static void 
_on_hide(GtkWidget *widget, gpointer user_data)
{
  CameraUI2PreviewWindow* self = CAMERA_UI2_PREVIEW_WINDOW(user_data);
  _remove_preview_window_timer(self);
}

void
_is_topmost_changed(HildonStackableWindow* window,
		    GParamSpec* psp,
		    gpointer user_data)
{
  CameraUI2PreviewWindow* self = CAMERA_UI2_PREVIEW_WINDOW(user_data);
  gboolean is_topmost;
  g_object_get(GTK_WINDOW(self), "is-topmost", &is_topmost, NULL);
  if(is_topmost)
  {
    g_print("preview window topmost TRUE\n");
  }
  else
  {
    g_print("preview window topmost FALSE\n");
  }
  if(is_topmost && self->priv->preview)
  {
    GdkPixmap* background = gdk_pixmap_new(GTK_WIDGET(self)->window, 800, 480, -1); 

    int preview_width = gdk_pixbuf_get_width(self->priv->preview);
    int preview_height = gdk_pixbuf_get_height(self->priv->preview);
    if(preview_width < 640 && preview_height < 480)
    {
      GdkPixbuf* tmp = gdk_pixbuf_scale_simple(self->priv->preview, 640, 480, GDK_INTERP_BILINEAR);
      g_object_unref(self->priv->preview);
      self->priv->preview = tmp;
    }
    preview_width = gdk_pixbuf_get_width(self->priv->preview);
    preview_height = gdk_pixbuf_get_height(self->priv->preview);
    int offset_x = MAX(0, (800 - preview_width)/2);
    int offset_y = MAX(0, (480 - preview_height)/2);
    gdk_pixbuf_render_to_drawable(self->priv->preview, background, GTK_WIDGET(self)->style->fg_gc[GTK_STATE_NORMAL],
				  0, 0,offset_x, offset_y,
				  MIN(800, preview_width), 
				  MIN(480, preview_height),
				  GDK_RGB_DITHER_NONE, 0, 0);
    GtkStyle * style = gtk_style_copy(GTK_WIDGET(self)->style);
    style->bg_pixmap[GTK_STATE_NORMAL] = background;
    gtk_widget_set_style(GTK_WIDGET(self), style);
  }
}

static void
camera_ui2_preview_window_show_preview(CameraUI2PreviewWindow* self, 
				       gboolean fullscreen,
				       CamPreviewMode preview_mode,
				       capture_data_t* capture_data,
				       GdkPixbuf* preview)
{
  if(self->priv->preview)
    g_object_unref(self->priv->preview);
  self->priv->preview = preview;

  gtk_image_set_from_icon_name(GTK_IMAGE(self->priv->delete_image_image), 
			       "camera_delete", HILDON_ICON_SIZE_THUMB);

  self->priv->is_fullscreen = fullscreen;
  if(self->priv->current_file)
  {
    g_free(self->priv->current_file);
    self->priv->current_file = NULL;
  }
  self->priv->current_file = g_strdup(capture_data->filename);

  if(fullscreen)
  {
    gtk_widget_show(self->priv->hide_window_button);
    gtk_window_fullscreen(GTK_WINDOW(self));
  }
  else
  {
    gtk_widget_hide(self->priv->hide_window_button);
    gtk_window_unfullscreen(GTK_WINDOW(self));
  }
  if(capture_data->country &&
     capture_data->city &&
     capture_data->suburb)
  {
    gchar* geo_tag_text = g_strdup_printf("%s %s %s",
					  capture_data->country,
					  capture_data->city,
					  capture_data->suburb);
    
    gtk_label_set_text(GTK_LABEL(self->priv->geo_tag_label), geo_tag_text);
    g_free(geo_tag_text);
    gtk_widget_show(self->priv->geo_tag_box);
  }
  else
  {
    gtk_widget_hide(self->priv->geo_tag_box);
  }
  if(preview_mode != CAM_PREVIEW_MODE_NO_TIMEOUT)
  {
    guint seconds = 2;
    if(preview_mode == CAM_PREVIEW_MODE_6SEC)
    {
      seconds = 6;
    }
    if(preview_mode == CAM_PREVIEW_MODE_4SEC)
    {
      seconds = 4;
    }
    _start_preview_window_timer(self, seconds);
  }
//      gtk_widget_show(self->priv->geo_tag_box);

//  gtk_widget_show(GTK_WIDGET(self));
/*   GdkPixmap* background; */
/*   gdk_pixbuf_render_pixmap_and_mask(preview, &background, NULL, 0); */
/*   gdk_window_set_back_pixmap(GDK_WINDOW(GTK_WIDGET(self->priv->preview_button)->window), background, FALSE); */


}

void 
camera_ui2_preview_window_show_image_preview(CameraUI2PreviewWindow* self, 
					     gboolean fullscreen,
					     CamPreviewMode preview_mode,
					     capture_data_t* capture_data,
					     GdkPixbuf* preview)
{
  gtk_widget_hide(self->priv->media_player_button);
  gtk_widget_show(self->priv->image_viewer_button);
  camera_ui2_preview_window_show_preview(self, fullscreen, preview_mode, capture_data, preview);
}

void 
camera_ui2_preview_window_show_video_preview(CameraUI2PreviewWindow* self, 
					     gboolean fullscreen,
					     CamPreviewMode preview_mode,
					     capture_data_t* capture_data,
					     GdkPixbuf* preview)
{
  gtk_widget_hide(self->priv->image_viewer_button);
  gtk_widget_show(self->priv->media_player_button);
  camera_ui2_preview_window_show_preview(self, fullscreen, preview_mode, capture_data, preview);
}

CameraUI2PreviewWindow*
camera_ui2_preview_window_new(osso_context_t* osso)
{
  CameraUI2PreviewWindow* self = 
    CAMERA_UI2_PREVIEW_WINDOW(g_object_new(CAMERA_UI2_PREVIEW_TYPE_WINDOW, NULL));
  self->priv->osso = osso;
  return self;
}

static void
camera_ui2_preview_window_dispose(GObject* object)
{
  G_OBJECT_CLASS(camera_ui2_preview_window_parent_class)->dispose(object);
}

static void
camera_ui2_preview_window_finalize(GObject* object)
{
  G_OBJECT_CLASS(camera_ui2_preview_window_parent_class)->finalize(object);
}

static void
camera_ui2_preview_window_class_init(CameraUI2PreviewWindowClass* klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS(klass);
  g_type_class_add_private(klass, sizeof(CameraUI2PreviewWindowPrivate));
  object_class->dispose = camera_ui2_preview_window_dispose;
  object_class->finalize = camera_ui2_preview_window_finalize;
}

static void
camera_ui2_preview_window_init(CameraUI2PreviewWindow* self)
{
  self->priv = CAMERA_UI2_PREVIEW_WINDOW_GET_PRIVATE(self);
  self->priv->current_file = NULL;
  self->priv->preview_hide_timer = 0;
  _init_hide_window_button(self);
  _init_sharing_button(self);
  _init_image_viewer_button(self);
  _init_media_player_button(self);
  _init_delete_image_button(self);
  self->priv->content = gtk_hbox_new(FALSE, 0);
  self->priv->preview = NULL;
  self->priv->geo_tag_box = gtk_hbox_new(FALSE, 0);
  self->priv->geo_tag_label = gtk_label_new("country, city, suburb");
  gtk_box_pack_start(GTK_BOX(self->priv->geo_tag_box), gtk_image_new_from_icon_name("camera_gps_location", HILDON_ICON_SIZE_FINGER), FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(self->priv->geo_tag_box), self->priv->geo_tag_label, FALSE, FALSE, 0);
  
  self->priv->preview_button = gtk_event_box_new();
  GtkWidget* preview_box = gtk_vbox_new(FALSE, 0);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(self->priv->preview_button),
				   FALSE);

  GtkWidget* right_box = gtk_vbox_new(TRUE, 0);
  GtkWidget* root_right_box = gtk_hbox_new(TRUE, 0);
  gtk_box_pack_end(GTK_BOX(self->priv->content), root_right_box, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(root_right_box), right_box, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(self->priv->content), preview_box, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(preview_box), self->priv->preview_button, TRUE, TRUE, 0);
  gtk_box_pack_end(GTK_BOX(preview_box), self->priv->geo_tag_box, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(right_box), self->priv->hide_window_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(right_box), self->priv->sharing_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(right_box), self->priv->delete_image_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(right_box), self->priv->image_viewer_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(right_box), self->priv->media_player_button, TRUE, FALSE, 0);
  gtk_container_add(GTK_CONTAINER(self), self->priv->content);
  gtk_widget_show_all(self->priv->content);
  g_signal_connect(self->priv->preview_button, "button-release-event", G_CALLBACK(_on_preview_button_release), self);
//  g_signal_connect(self->priv->content, "expose-event", G_CALLBACK(_on_preview_expose), self);
  g_signal_connect(self, "notify::is-topmost", G_CALLBACK(_is_topmost_changed), self);
  g_signal_connect(self, "hide", G_CALLBACK(_on_hide), self);
  g_signal_connect(self, "delete-event", G_CALLBACK(&gtk_widget_hide_on_delete), NULL);
}
