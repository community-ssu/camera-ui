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
#include <hildon/hildon.h>
#include <gtk/gtk.h>
#include "camera-settings.h"
#include "storage-helper.h"
#include "camera-ui2-helper.h"
#include "camera-ui2-settings-dialog.h"


static void
_check_one_of(HildonCheckButton* button, gpointer user_data)
{
  if(!hildon_check_button_get_active(HILDON_CHECK_BUTTON(button)))
  {
    hildon_check_button_set_active(HILDON_CHECK_BUTTON(user_data), TRUE);
  }
}

static void
_enable_disable(HildonCheckButton* button, gpointer user_data)
{
  gtk_widget_set_sensitive(GTK_WIDGET(user_data), 
			   hildon_check_button_get_active(button));
}

static void
_close_dialog(GtkButton* button, gpointer user_data)
{
  gtk_dialog_response(GTK_DIALOG(user_data), GTK_RESPONSE_OK);
}

static GtkWidget*
_create_radio_button(const gchar* label, const gchar* icon_name, gboolean active, GtkWidget* w)
{
  GtkWidget* radio_button = NULL;
  if(w == NULL)
    radio_button = hildon_gtk_radio_button_new(HILDON_SIZE_FINGER_HEIGHT, NULL);
  else
    radio_button = hildon_gtk_radio_button_new_from_widget(HILDON_SIZE_FINGER_HEIGHT,
							   GTK_RADIO_BUTTON(w));
  gtk_button_set_label(GTK_BUTTON(radio_button), label);
  gtk_button_set_image(GTK_BUTTON(radio_button), gtk_image_new_from_icon_name(icon_name, HILDON_ICON_SIZE_FINGER));
  gtk_button_set_alignment(GTK_BUTTON(radio_button), 0, 0.5);
  gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(radio_button), FALSE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button), active);
  return radio_button;
}

guint
show_capture_timer_dialog()
{
  guint ret = 0;
  GtkWidget* timer_picker = hildon_picker_dialog_new(NULL);
  GtkWidget* selector = hildon_touch_selector_new_text();
  hildon_touch_selector_append_text(HILDON_TOUCH_SELECTOR(selector), "3");
  hildon_touch_selector_append_text(HILDON_TOUCH_SELECTOR(selector), "6");
  hildon_touch_selector_append_text(HILDON_TOUCH_SELECTOR(selector), "10");
  hildon_touch_selector_append_text(HILDON_TOUCH_SELECTOR(selector), "15");
  hildon_touch_selector_append_text(HILDON_TOUCH_SELECTOR(selector), "20");
  hildon_picker_dialog_set_selector(HILDON_PICKER_DIALOG(timer_picker),
				    HILDON_TOUCH_SELECTOR(selector));
  gtk_window_set_title(GTK_WINDOW(timer_picker), "Timer");
  gtk_widget_show_all(timer_picker);
  if(gtk_dialog_run(GTK_DIALOG(timer_picker)) == GTK_RESPONSE_OK)
  {
    int index = hildon_touch_selector_get_active(HILDON_TOUCH_SELECTOR(selector),
						 0);
    if(index == 0)
      ret = 3;
    else if(index == 1)
      ret = 6;
    else if(index == 2)
      ret = 10;
    else if(index == 3)
      ret = 10;
    else if(index == 4)
      ret = 20;
  }
  gtk_widget_destroy(timer_picker);
  return ret;
}

void
show_app_settings_dialog(AppSettings* app_settings)
{
  GtkWidget* dialog = gtk_dialog_new_with_buttons(dgettext("osso-camera-ui", "camera_me_general_settings"),
						  NULL,
						  GTK_DIALOG_MODAL | GTK_DIALOG_NO_SEPARATOR,
						  dgettext("hildon-libs", "wdgt_bd_save"), GTK_RESPONSE_OK,
						  NULL);
  GtkWidget* table_layout = gtk_table_new(3, 1, TRUE);

  GtkWidget* show_on_lenscover_open_button = hildon_check_button_new(HILDON_SIZE_FINGER_HEIGHT);
  GtkWidget* hide_on_lenscover_close_button = hildon_check_button_new(HILDON_SIZE_FINGER_HEIGHT);
  GtkWidget* show_on_focus_button_press_button = hildon_check_button_new(HILDON_SIZE_FINGER_HEIGHT);

  gtk_button_set_label(GTK_BUTTON(show_on_lenscover_open_button), "Show on Lenscover open");
  gtk_button_set_label(GTK_BUTTON(hide_on_lenscover_close_button), "Hide on Lenscover close");
  gtk_button_set_label(GTK_BUTTON(show_on_focus_button_press_button), "Show on Focusbutton press");

  hildon_check_button_set_active(HILDON_CHECK_BUTTON(show_on_lenscover_open_button), app_settings->show_on_lenscover_open);
  hildon_check_button_set_active(HILDON_CHECK_BUTTON(hide_on_lenscover_close_button), app_settings->hide_on_lenscover_close);
  hildon_check_button_set_active(HILDON_CHECK_BUTTON(show_on_focus_button_press_button), app_settings->show_on_focus_button_press);

  gtk_table_attach_defaults(GTK_TABLE(table_layout), show_on_lenscover_open_button, 0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), hide_on_lenscover_close_button, 0, 1, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), show_on_focus_button_press_button, 0, 1, 2, 3);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), table_layout, TRUE, TRUE, 0);
  gtk_widget_show_all(dialog);

  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
  {
    app_settings->show_on_lenscover_open = hildon_check_button_get_active(HILDON_CHECK_BUTTON(show_on_lenscover_open_button));
    app_settings->hide_on_lenscover_close = hildon_check_button_get_active(HILDON_CHECK_BUTTON(hide_on_lenscover_close_button));
    app_settings->show_on_focus_button_press = hildon_check_button_get_active(HILDON_CHECK_BUTTON(show_on_focus_button_press_button));
  }
  gtk_widget_destroy(dialog);
}


void
show_storage_settings_dialog(CameraSettings* settings)
{
  GtkWidget* dialog = gtk_dialog_new_with_buttons(dgettext("osso-camera-ui", "camera_me_general_settings"),
						  NULL,
						  GTK_DIALOG_MODAL | GTK_DIALOG_NO_SEPARATOR,
						  dgettext("hildon-libs", "wdgt_bd_save"), GTK_RESPONSE_OK,
						  NULL);
  GtkWidget* storage_button = hildon_picker_button_new(HILDON_SIZE_FINGER_HEIGHT,
						       HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);
  GtkWidget* author_box = gtk_hbox_new(TRUE, 0);
  GtkWidget* author_label = gtk_label_new(dgettext("osso-camera-ui", "camera_fi_creator"));
  GtkWidget* author_entry = hildon_entry_new(HILDON_SIZE_FINGER_HEIGHT);
  GtkWidget* preview_button = hildon_picker_button_new(HILDON_SIZE_FINGER_HEIGHT,
						       HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);
  GtkWidget* storage_selector = hildon_touch_selector_new_text();
  GtkWidget* preview_selector = hildon_touch_selector_new_text();
  gchar* mmc_card_name = storage_helper_get_mmc_name();
  
  hildon_touch_selector_append_text(HILDON_TOUCH_SELECTOR(storage_selector), mmc_card_name);
  g_free(mmc_card_name);
  hildon_touch_selector_append_text(HILDON_TOUCH_SELECTOR(storage_selector), 
				    dgettext("osso-camera-ui", "camera_va_memory_internal"));


  gtk_button_set_alignment(GTK_BUTTON(storage_button), 0, 0.5);
  gtk_button_set_alignment(GTK_BUTTON(preview_button), 0, 0.5);
  gtk_misc_set_alignment(GTK_MISC(author_label), 0, 0.5);

  gchar* preview_text2 = g_strdup_printf(dgettext("osso-camera-ui", "camera_va_show_post_capture_seconds"), 2);
  gchar* preview_text4 = g_strdup_printf(dgettext("osso-camera-ui", "camera_va_show_post_capture_seconds"), 4);
  gchar* preview_text6 = g_strdup_printf(dgettext("osso-camera-ui", "camera_va_show_post_capture_seconds"), 6);

  hildon_touch_selector_append_text(HILDON_TOUCH_SELECTOR(preview_selector), 
				    preview_text2);
  hildon_touch_selector_append_text(HILDON_TOUCH_SELECTOR(preview_selector), 
				    preview_text4);
  hildon_touch_selector_append_text(HILDON_TOUCH_SELECTOR(preview_selector), 
				    preview_text6);
  hildon_touch_selector_append_text(HILDON_TOUCH_SELECTOR(preview_selector), 
				    dgettext("osso-camera-ui", "camera_va_show_post_capture_manual"));
  hildon_touch_selector_append_text(HILDON_TOUCH_SELECTOR(preview_selector), 
				    dgettext("osso-camera-ui", "camera_va_show_post_capture_no"));

  g_free(preview_text2);
  g_free(preview_text4);
  g_free(preview_text6);
  
  hildon_button_set_title(HILDON_BUTTON(storage_button), dgettext("osso-camera-ui", "camera_fi_memory_in_use"));
  if(settings->scene_mode == CAM_SCENE_MODE_VIDEO ||
     settings->scene_mode == CAM_SCENE_MODE_NIGHT_VIDEO)
    hildon_button_set_title(HILDON_BUTTON(preview_button), dgettext("osso-camera-ui", "camera_fi_show_post_capture_video"));
  else
    hildon_button_set_title(HILDON_BUTTON(preview_button), dgettext("osso-camera-ui", "camera_fi_show_post_capture"));

  hildon_picker_button_set_selector(HILDON_PICKER_BUTTON(storage_button), HILDON_TOUCH_SELECTOR(storage_selector));
  hildon_picker_button_set_selector(HILDON_PICKER_BUTTON(preview_button), HILDON_TOUCH_SELECTOR(preview_selector));
			  
  hildon_touch_selector_set_active(HILDON_TOUCH_SELECTOR(storage_selector), 0, settings->storage_device);
  hildon_touch_selector_set_active(HILDON_TOUCH_SELECTOR(preview_selector), 0, settings->preview_mode);
  if(settings->author != NULL)
  {
    gtk_entry_set_text(GTK_ENTRY(author_entry), settings->author);
  }
  hildon_button_set_title(HILDON_BUTTON(preview_button), dgettext("osso-camera-ui", "camera_fi_show_post_capture"));
  gtk_box_pack_start(GTK_BOX(author_box), author_label, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(author_box), author_entry, TRUE, TRUE, 0);
  
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), storage_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), author_box, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), preview_button, TRUE, FALSE, 0);
  gtk_widget_show_all(dialog);
  
  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
  {
    g_free(settings->author);
    settings->author = g_strdup(gtk_entry_get_text(GTK_ENTRY(author_entry)));
    settings->storage_device = hildon_touch_selector_get_active(HILDON_TOUCH_SELECTOR(storage_selector), 0);
    settings->preview_mode = hildon_touch_selector_get_active(HILDON_TOUCH_SELECTOR(preview_selector), 0);
  }
  gtk_widget_destroy(dialog);
}

void
show_scene_mode_selection_dialog(CamSceneMode* scene_mode)
{
  GtkWidget* dialog = gtk_dialog_new();
  GtkWidget* mode_box1 = gtk_vbox_new(TRUE, 0);
  GtkWidget* mode_box2 = gtk_vbox_new(TRUE, 0);
  GtkWidget* mode_box3 = gtk_vbox_new(TRUE, 0);
  GtkWidget* still_mode_box = gtk_hbox_new(TRUE, 0);
  GtkWidget* video_mode_box = gtk_hbox_new(TRUE, 0);
  GtkWidget* video_mode_label = gtk_label_new(dgettext("osso-camera-ui", "camera_ti_video_mode"));
  GtkWidget* mode_box4 = gtk_vbox_new(TRUE, 0);

  GtkWidget* auto_mode_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_mode_automatic"),
			 "camera_auto_mode",
			 (*scene_mode) == 0,
			 NULL);

  GtkWidget* landscape_mode_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_mode_landscape"),
			 "camera_scene_mode_landscape",
			 (*scene_mode)==1,
			 auto_mode_button);

  GtkWidget* night_mode_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_mode_night"),
			 "camera_scene_mode_night",
			 (*scene_mode)==2,
			 landscape_mode_button);

  GtkWidget* portrait_mode_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_mode_portrait"),
			 "camera_scene_mode_portrait",
			 (*scene_mode)==3,
			 night_mode_button);

  GtkWidget* macro_mode_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_mode_macro"),
			 "camera_scene_mode_macro",
			 (*scene_mode)==4,
			 portrait_mode_button);
  
  GtkWidget* sports_mode_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_mode_action"),
			 "camera_scene_mode_sport",
			 (*scene_mode)==5,
			 macro_mode_button);

  GtkWidget* video_mode_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_mode_automatic_video"),
			 "camera_scene_mode_video",
			 (*scene_mode)==6,
			 sports_mode_button);

  GtkWidget* night_video_mode_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_mode_night_video"),
			 "camera_scene_mode_night_video",
			 (*scene_mode)==7,
			 video_mode_button);
  
  gtk_box_pack_start(GTK_BOX(mode_box1), auto_mode_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(mode_box1), macro_mode_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(mode_box1), portrait_mode_button, TRUE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(mode_box2), night_mode_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(mode_box2), landscape_mode_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(mode_box2), sports_mode_button, TRUE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(mode_box3), video_mode_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(mode_box4), night_video_mode_button, TRUE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(still_mode_box), mode_box1, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(still_mode_box), mode_box2, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), still_mode_box, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), video_mode_label, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(video_mode_box), mode_box3, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(video_mode_box), mode_box4, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), video_mode_box, TRUE, TRUE, 0);

  g_signal_connect(auto_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(night_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(macro_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(landscape_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(portrait_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(sports_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(video_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(night_video_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);

  gtk_window_set_title(GTK_WINDOW(dialog),(dgettext("osso-camera-ui", "camera_ti_camera_mode")));
  gtk_widget_show_all(dialog);
  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
  {
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(auto_mode_button)))
      (*scene_mode) = 0;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(landscape_mode_button)))
      (*scene_mode) = 1;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(night_mode_button)))
      (*scene_mode) = 2;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(portrait_mode_button)))
      (*scene_mode) = 3;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(macro_mode_button)))
      (*scene_mode) = 4;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sports_mode_button)))
      (*scene_mode) = 5;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(video_mode_button)))
      (*scene_mode) = 6;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(night_video_mode_button)))
      (*scene_mode) = 7;
  }
  gtk_widget_destroy(dialog);
}

void
show_flash_mode_selection_dialog(CamFlashMode* flash_mode)
{
  GtkWidget* dialog = gtk_dialog_new();
  GtkWidget* mode_box1 = gtk_vbox_new(TRUE, 0);
  GtkWidget* mode_box2 = gtk_vbox_new(TRUE, 0);
  GtkWidget* hbox = gtk_hbox_new(TRUE, 0);

  GtkWidget* automatic_flash_mode_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_flash_automatic"),
			 "camera_flash_auto",
			 (*flash_mode) == 0,
			 NULL);
  GtkWidget* redeye_flash_mode_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_flash_red_eye"),
			 "camera_flash_redeye",
			 (*flash_mode) == 3,
			 automatic_flash_mode_button);
  GtkWidget* always_on_flash_mode_button  = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_flash_always_on"),
			 "camera_flash_fill",
			 (*flash_mode) == 1,
			 redeye_flash_mode_button);
  GtkWidget* always_off_flash_mode_button  = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_flash_always_off"),
			 "camera_flash_off",
			 (*flash_mode) == 2,
			 always_on_flash_mode_button);
  

  gtk_box_pack_start(GTK_BOX(mode_box1), automatic_flash_mode_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(mode_box1), always_on_flash_mode_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(mode_box2), redeye_flash_mode_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(mode_box2), always_off_flash_mode_button, TRUE, FALSE, 0);
  
  gtk_box_pack_start(GTK_BOX(hbox), mode_box1, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), mode_box2, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 0);

  gtk_window_set_title(GTK_WINDOW(dialog),(dgettext("osso-camera-ui", "camera_ti_flash")));
  gtk_widget_show_all(dialog);

  g_signal_connect(automatic_flash_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(redeye_flash_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(always_on_flash_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(always_off_flash_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);

  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
  {
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(automatic_flash_mode_button)))
      (*flash_mode) = 0;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(always_on_flash_mode_button)))
      (*flash_mode) = 1;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(always_off_flash_mode_button)))
      (*flash_mode) = 2;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(redeye_flash_mode_button)))
      (*flash_mode) = 3;
  }
  gtk_widget_destroy(dialog);
}

void
show_color_mode_selection_dialog(gint* color_mode)
{
  GtkWidget* dialog = gtk_dialog_new();
  GtkWidget* table_layout = gtk_table_new(3, 2, TRUE);

  GtkWidget* normal_color_mode_button = 
    _create_radio_button("Normal", "control_led", (*color_mode)==0, NULL);
  GtkWidget* sepia_color_mode_button = 
    _create_radio_button("Sepia", "control_led", (*color_mode)==1, normal_color_mode_button);
  GtkWidget* negative_color_mode_button  = 
    _create_radio_button("Negative", "control_led", (*color_mode)==2, sepia_color_mode_button);
  GtkWidget* grayscale_color_mode_button  = 
    _create_radio_button("Grayscale", "control_led", (*color_mode)==3, negative_color_mode_button );
  GtkWidget* natural_color_mode_button  = 
    _create_radio_button("natural", "control_led", (*color_mode)==4, grayscale_color_mode_button );
  GtkWidget* vivid_color_mode_button  = 
    _create_radio_button("vivid", "control_led", (*color_mode)==5, natural_color_mode_button );
  GtkWidget* colorswap_color_mode_button  = 
    _create_radio_button("Colorswap", "control_led", (*color_mode)==6, vivid_color_mode_button );
  GtkWidget* solarize_color_mode_button  = 
    _create_radio_button("Solarize", "control_led", (*color_mode)==7, colorswap_color_mode_button );
  GtkWidget* out_of_focus_color_mode_button  = 
    _create_radio_button("out_of_focus", "control_led", (*color_mode)==8, solarize_color_mode_button );


  gtk_table_attach_defaults(GTK_TABLE(table_layout), normal_color_mode_button, 0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), sepia_color_mode_button, 1, 2, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), negative_color_mode_button, 0, 1, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), grayscale_color_mode_button, 1, 2, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), colorswap_color_mode_button, 0, 1, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), solarize_color_mode_button, 1, 2, 2, 3);
/*
  gtk_table_attach_defaults(GTK_TABLE(table_layout), natural_color_mode_button, 0, 1, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), vivid_color_mode_button, 1, 2, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), out_of_focus_color_mode_button, 0, 1, 4, 5);
*/
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), table_layout, TRUE, TRUE, 0);

  gtk_window_set_title(GTK_WINDOW(dialog),"Color");
  gtk_widget_show_all(dialog);

  g_signal_connect(normal_color_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(sepia_color_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(negative_color_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(grayscale_color_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(natural_color_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(vivid_color_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(colorswap_color_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(solarize_color_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(out_of_focus_color_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);


  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
  {
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(normal_color_mode_button)))
      (*color_mode) = 0;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sepia_color_mode_button)))
      (*color_mode) = 1;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(negative_color_mode_button)))
      (*color_mode) = 2;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(grayscale_color_mode_button)))
      (*color_mode) = 3;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(natural_color_mode_button)))
      (*color_mode) = 4;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(vivid_color_mode_button)))
      (*color_mode) = 5;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(colorswap_color_mode_button)))
      (*color_mode) = 6;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(solarize_color_mode_button)))
      (*color_mode) = 7;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(out_of_focus_color_mode_button)))
      (*color_mode) = 8;
  }
  gtk_widget_destroy(dialog);
}

void
show_white_balance_mode_selection_dialog(CamWhiteBalance* white_balance)
{
  GtkWidget* dialog = gtk_dialog_new();
  GtkWidget* table_layout = gtk_table_new(3, 2, TRUE);

  GtkWidget* auto_mode_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_wb_automatic"),
			 "camera_auto_mode",
			 (*white_balance) == 0,
			 NULL);
  GtkWidget* sunny_mode_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_wb_sunny"),
			 "camera_white_balance_sunny",
			 (*white_balance) == 1,
			 auto_mode_button);
  GtkWidget* cloudy_mode_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_wb_cloudy"),
			 "camera_white_balance_cloudy",
			 (*white_balance) == 2,
			 sunny_mode_button);
  GtkWidget* fluorescent_mode_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_wb_fluorescent"),
			 "camera_white_balance_flourescent",
			 (*white_balance) == 3,
			 cloudy_mode_button);
  GtkWidget* incandescent_mode_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_wb_tungsten"),
			 "camera_white_balance_incandescent",
			 (*white_balance) == 4,
			 fluorescent_mode_button);

  gtk_table_attach_defaults(GTK_TABLE(table_layout), auto_mode_button, 0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), sunny_mode_button, 1, 2, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), cloudy_mode_button, 0, 1, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), incandescent_mode_button, 1, 2, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), fluorescent_mode_button, 0, 1, 2, 3);

  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), table_layout, TRUE, TRUE, 0);

  g_signal_connect(auto_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(cloudy_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(sunny_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(fluorescent_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(incandescent_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);

  gtk_window_set_title(GTK_WINDOW(dialog),(dgettext("osso-camera-ui", "camera_ti_white_balance")));
  gtk_widget_show_all(dialog);
  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
  {
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(auto_mode_button)))
      (*white_balance) = 0;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sunny_mode_button)))
      (*white_balance) = 1;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cloudy_mode_button)))
      (*white_balance) = 2;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fluorescent_mode_button)))
      (*white_balance) = 3;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(incandescent_mode_button)))
      (*white_balance) = 4;
  }
  gtk_widget_destroy(dialog);
}

void
show_iso_level_selection_dialog(CamIsoLevel* iso_level)
{
  GtkWidget* dialog = gtk_dialog_new();
  GtkWidget* mode_box1 = gtk_vbox_new(TRUE, 0);
  GtkWidget* mode_box2 = gtk_vbox_new(TRUE, 0);
  GtkWidget* hbox = gtk_hbox_new(TRUE, 0);

  GtkWidget* auto_mode_button =
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_wb_automatic"),
			 "camera_auto_mode",
			 (*iso_level) == 0,
			 NULL);
  GtkWidget* iso_100_button =
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_iso_100"),
			 "camera_iso100",
			 (*iso_level) == 1,
			 auto_mode_button);
  GtkWidget* iso_200_button =
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_iso_200"),
			 "camera_iso200",
			 (*iso_level) == 2,
			 iso_100_button);
  GtkWidget* iso_400_button =
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_iso_400"),
			 "camera_iso400",
			 (*iso_level) == 3,
			 iso_200_button);

  gtk_box_pack_start(GTK_BOX(mode_box1), auto_mode_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(mode_box1), iso_200_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(mode_box2), iso_100_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(mode_box2), iso_400_button, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(hbox), mode_box1, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), mode_box2, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 0);

  g_signal_connect(auto_mode_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(iso_100_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(iso_200_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(iso_400_button, "toggled", G_CALLBACK(_close_dialog), dialog);

  gtk_window_set_title(GTK_WINDOW(dialog),(dgettext("osso-camera-ui", "camera_ti_iso")));
  gtk_widget_show_all(dialog);
  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
  {
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(auto_mode_button)))
      (*iso_level) = 0;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(iso_100_button)))
      (*iso_level) = 1;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(iso_200_button)))
      (*iso_level) = 2;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(iso_400_button)))
      (*iso_level) = 3;
  }
  gtk_widget_destroy(dialog);
}

void
show_exposure_level_selection_dialog(CameraSettings* settings)
{
  GtkWidget* dialog = gtk_dialog_new();
  GtkWidget* table_layout = gtk_table_new(5, 2, TRUE);

  GtkWidget* exposure_m2_button = 
    _create_radio_button("-2.0",
			 "camera_exposure_minus2",
			 settings->exposure_level == 0,
			 NULL);
  GtkWidget* exposure_m15_button = 
    _create_radio_button("-1.5",
			 "camera_exposure_minus1half",
			 settings->exposure_level == 1,
			 exposure_m2_button);
  GtkWidget* exposure_m1_button = 
    _create_radio_button("-1.0",
			 "camera_exposure_minus1",
			 settings->exposure_level == 2,
			 exposure_m15_button);
  GtkWidget* exposure_m05_button = 
    _create_radio_button("-0.5",
			 "camera_exposure_minus0half",
			 settings->exposure_level == 3,
			 exposure_m1_button);
  GtkWidget* exposure_0_button = 
    _create_radio_button("0.0",
			 "camera_exposure_0",
			 settings->exposure_level == 4,
			 exposure_m05_button);
  GtkWidget* exposure_p05_button = 
    _create_radio_button("0.5",
			 "camera_exposure_plus0half",
			 settings->exposure_level == 5,
			 exposure_0_button);
  GtkWidget* exposure_p1_button = 
    _create_radio_button("1.0",
			 "camera_exposure_plus1",
			 settings->exposure_level == 6,
			 exposure_p05_button);
  GtkWidget* exposure_p15_button = 
    _create_radio_button("1.5",
			 "camera_exposure_plus1half",
			 settings->exposure_level == 7,
			 exposure_p1_button);
  GtkWidget* exposure_p2_button = 
    _create_radio_button("2.0",
			 "camera_exposure_2",
			 settings->exposure_level == 8,
			 exposure_p15_button);

  gtk_table_attach_defaults(GTK_TABLE(table_layout), exposure_0_button, 1, 2, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), exposure_p05_button, 1, 2, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), exposure_m05_button, 0, 1, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), exposure_p1_button, 1, 2, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), exposure_m1_button, 0, 1, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), exposure_p15_button, 1, 2, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), exposure_m15_button, 0, 1, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), exposure_p2_button, 1, 2, 4, 5);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), exposure_m2_button, 0, 1, 4, 5);

  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), table_layout, TRUE, TRUE, 0);

  g_signal_connect(exposure_m2_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(exposure_m15_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(exposure_m1_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(exposure_m05_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(exposure_0_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(exposure_p05_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(exposure_p1_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(exposure_p15_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(exposure_p2_button, "toggled", G_CALLBACK(_close_dialog), dialog);

  gtk_window_set_title(GTK_WINDOW(dialog),(dgettext("osso-camera-ui", "camera_ti_exposure")));
  gtk_widget_show_all(dialog);

  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
  {
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(exposure_m2_button)))
      settings->exposure_level = 0;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(exposure_m15_button)))
      settings->exposure_level = 1;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(exposure_m1_button)))
      settings->exposure_level = 2;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(exposure_m05_button)))
      settings->exposure_level = 3;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(exposure_0_button)))
      settings->exposure_level = 4;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(exposure_p05_button)))
      settings->exposure_level = 5;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(exposure_p1_button)))
      settings->exposure_level = 6;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(exposure_p15_button)))
      settings->exposure_level = 7;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(exposure_p2_button)))
      settings->exposure_level = 8;
  }
  gtk_widget_destroy(dialog);
}

void
show_still_resolution_size_selection_dialog(CamStillResolution* still_resolution_size)
{
  GtkWidget* dialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dialog),(dgettext("osso-camera-ui", "camera_ti_resolution")));
  GtkWidget* mode_box1 = gtk_vbox_new(TRUE, 0);
  GtkWidget* mode_box2 = gtk_vbox_new(TRUE, 0);
  GtkWidget* hbox = gtk_hbox_new(TRUE, 0);
  GtkWidget* resolution_size1_3m_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_resolution_low"),
			 "camera_image_size1_3mpixel",
			 (*still_resolution_size) == 0,
			 NULL);
  GtkWidget* resolution_size3m_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_resolution_medium"),
			 "camera_image_size3mpixel",
			 (*still_resolution_size) == 1,
			 resolution_size1_3m_button);
  GtkWidget* resolution_size5m_button  = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_resolution_high"),
			 "camera_image_size5mpixel",
			 (*still_resolution_size) == 2,
			 resolution_size3m_button);
  GtkWidget* resolution_size3_5m_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_resolution_wide"),
			 "camera_image_size3_5mpixel",
			 (*still_resolution_size) == 3,
			 resolution_size5m_button);

  gtk_box_pack_start(GTK_BOX(mode_box1), resolution_size5m_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(mode_box1), resolution_size1_3m_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(mode_box2), resolution_size3m_button, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(mode_box2), resolution_size3_5m_button, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(hbox), mode_box1, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), mode_box2, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 0);
  
  g_signal_connect(resolution_size5m_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(resolution_size1_3m_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(resolution_size3m_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(resolution_size3_5m_button, "toggled", G_CALLBACK(_close_dialog), dialog);

  
  gtk_widget_show_all(dialog);
  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
  {
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(resolution_size1_3m_button)))
      (*still_resolution_size) = 0;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(resolution_size3m_button)))
      (*still_resolution_size) = 1;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(resolution_size5m_button)))
      (*still_resolution_size) = 2;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(resolution_size3_5m_button)))
      (*still_resolution_size) = 3;    
  }
  gtk_widget_destroy(dialog);
}

void
show_video_resolution_size_selection_dialog(CameraSettings* settings)
{
  GtkWidget* dialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dialog),(dgettext("osso-camera-ui", "camera_ti_resolution")));
  GtkWidget* table_layout = gtk_table_new(2, 2, TRUE);
  GtkWidget* video_resolution_size_vga_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_resolution_video_low"),
			 "camera_video_resolution_vga",
			 settings->video_resolution_size == 6,
			 NULL);
  GtkWidget* video_resolution_size_qvga_button = 
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_resolution_video_fine"),
			 "camera_video_resolution_qvga",
			 settings->video_resolution_size == 7,
			 video_resolution_size_vga_button);
  GtkWidget* video_resolution_size_wide_screen_button =
    _create_radio_button(dgettext("osso-camera-ui", "camera_bd_resolution_video_high"),
			 "camera_video_resolution_wide_screen",
			 settings->video_resolution_size == 8,
			 video_resolution_size_qvga_button);

  gtk_table_attach_defaults(GTK_TABLE(table_layout), video_resolution_size_wide_screen_button, 0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), video_resolution_size_qvga_button, 1, 2, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), video_resolution_size_vga_button, 0, 1, 1, 2);

  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), table_layout, TRUE, TRUE, 0);
  
  g_signal_connect(video_resolution_size_vga_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(video_resolution_size_qvga_button, "toggled", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(video_resolution_size_wide_screen_button, "toggled", G_CALLBACK(_close_dialog), dialog);

  
  gtk_widget_show_all(dialog);
  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
  {
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(video_resolution_size_vga_button)))
     settings->video_resolution_size = 6;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(video_resolution_size_qvga_button)))
      settings->video_resolution_size = 7;
    else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(video_resolution_size_wide_screen_button)))
      settings->video_resolution_size = 8;
  }
  gtk_widget_destroy(dialog);
}

static void
_on_show_white_balance_mode_selection_dialog(GtkButton* button, gpointer user_data)
{
  CameraSettings* settings = (CameraSettings*)user_data;
  CamWhiteBalance white_balance = settings->white_balance;
  show_white_balance_mode_selection_dialog(&white_balance);
  settings->white_balance = white_balance;
}

static void
_on_show_iso_level_selection_dialog(GtkButton* button, gpointer user_data)
{
  CameraSettings* settings = (CameraSettings*)user_data;
  CamIsoLevel iso_level = settings->iso_level;
  show_iso_level_selection_dialog(&iso_level);
  settings->iso_level = iso_level;
}

static void
_on_show_exposure_level_selection_dialog(GtkButton* button, gpointer user_data)
{
  show_exposure_level_selection_dialog((CameraSettings*)user_data);
}

static void
_on_show_still_resolution_size_selection_dialog(GtkButton* button, gpointer user_data)
{
  CameraSettings* settings = (CameraSettings*)user_data;
  CamStillResolution resolution_size = settings->still_resolution_size;
  show_still_resolution_size_selection_dialog(&resolution_size);
  gtk_button_set_image(button, gtk_image_new_from_icon_name(still_resolution_size_icon_name(settings->still_resolution_size, FALSE), HILDON_SIZE_FINGER_HEIGHT));
  settings->still_resolution_size = resolution_size;
}

static void
_on_show_video_resolution_size_selection_dialog(GtkButton* button, gpointer user_data)
{
  CameraSettings* settings = (CameraSettings*)user_data;
  show_video_resolution_size_selection_dialog(settings);
  gtk_button_set_image(button, gtk_image_new_from_icon_name(video_resolution_size_icon_name(settings->video_resolution_size, FALSE), HILDON_SIZE_FINGER_HEIGHT));
}

void
show_still_settings_dialog(CameraSettings* settings)
{
  GtkWidget* dialog = gtk_dialog_new();
  GtkWidget* setting_box1 = gtk_vbox_new(TRUE, 0);
  GtkWidget* setting_box2 = gtk_vbox_new(TRUE, 0);
  GtkWidget* hbox = gtk_hbox_new(TRUE, 0);
  GtkWidget* white_balance_button = hildon_button_new(HILDON_SIZE_FINGER_HEIGHT,
						      HILDON_BUTTON_ARRANGEMENT_VERTICAL);
  GtkWidget* exposure_button = hildon_button_new(HILDON_SIZE_FINGER_HEIGHT,
						 HILDON_BUTTON_ARRANGEMENT_VERTICAL);
  GtkWidget* iso_level_button = hildon_button_new(HILDON_SIZE_FINGER_HEIGHT,
						  HILDON_BUTTON_ARRANGEMENT_VERTICAL);
  GtkWidget* resolution_button = hildon_button_new(HILDON_SIZE_FINGER_HEIGHT,
						   HILDON_BUTTON_ARRANGEMENT_VERTICAL);

  hildon_button_set_title(HILDON_BUTTON(white_balance_button), dgettext("osso-camera-ui", "camera_bd_settings_wb"));
  hildon_button_set_title(HILDON_BUTTON(exposure_button), dgettext("osso-camera-ui", "camera_bd_settings_exposure"));
  hildon_button_set_title(HILDON_BUTTON(iso_level_button), dgettext("osso-camera-ui", "camera_bd_settings_iso"));
  hildon_button_set_title(HILDON_BUTTON(resolution_button), still_resolution_size_name(settings->still_resolution_size)); 

  gtk_button_set_alignment(GTK_BUTTON(white_balance_button), 0, 0.5);
  gtk_button_set_alignment(GTK_BUTTON(exposure_button), 0, 0.5);
  gtk_button_set_alignment(GTK_BUTTON(iso_level_button), 0, 0.5);
  gtk_button_set_alignment(GTK_BUTTON(resolution_button), 0, 0.5);

  hildon_button_set_style(HILDON_BUTTON(white_balance_button),
			  HILDON_BUTTON_STYLE_PICKER);
  hildon_button_set_style(HILDON_BUTTON(iso_level_button),
			  HILDON_BUTTON_STYLE_PICKER);
  hildon_button_set_style(HILDON_BUTTON(exposure_button),
			  HILDON_BUTTON_STYLE_PICKER);
  hildon_button_set_style(HILDON_BUTTON(resolution_button),
			  HILDON_BUTTON_STYLE_PICKER);

  hildon_button_set_value(HILDON_BUTTON(white_balance_button), 
			  white_balance_mode_name(settings->white_balance));
  hildon_button_set_value(HILDON_BUTTON(iso_level_button), 
			  iso_level_name(settings->iso_level));
  hildon_button_set_value(HILDON_BUTTON(exposure_button), 
			  exposure_level_name(settings->exposure_level));
  hildon_button_set_value(HILDON_BUTTON(resolution_button), 
			  still_resolution_size_name(settings->still_resolution_size));

  hildon_button_set_image(HILDON_BUTTON(white_balance_button), 
			  gtk_image_new_from_icon_name(white_balance_mode_icon_name(settings->white_balance, FALSE), HILDON_ICON_SIZE_FINGER));
  hildon_button_set_image(HILDON_BUTTON(iso_level_button), 
			  gtk_image_new_from_icon_name(iso_level_icon_name(settings->iso_level, FALSE), HILDON_ICON_SIZE_FINGER));
  hildon_button_set_image(HILDON_BUTTON(exposure_button), 
			  gtk_image_new_from_icon_name(exposure_level_icon_name(settings->exposure_level, FALSE), HILDON_ICON_SIZE_FINGER));
  hildon_button_set_image(HILDON_BUTTON(resolution_button), 
			  gtk_image_new_from_icon_name(still_resolution_size_icon_name(settings->still_resolution_size, FALSE), HILDON_ICON_SIZE_FINGER));
  
  gtk_box_pack_start(GTK_BOX(setting_box1), white_balance_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(setting_box1), iso_level_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(setting_box2), exposure_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(setting_box2), resolution_button, TRUE, FALSE, 0);
  
  gtk_box_pack_start(GTK_BOX(hbox), setting_box1, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), setting_box2, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 0);

  g_signal_connect(white_balance_button, "clicked", G_CALLBACK(_on_show_white_balance_mode_selection_dialog), settings);
  g_signal_connect(iso_level_button, "clicked", G_CALLBACK(_on_show_iso_level_selection_dialog), settings);
  g_signal_connect(exposure_button, "clicked", G_CALLBACK(_on_show_exposure_level_selection_dialog), settings);
  g_signal_connect(resolution_button, "clicked", G_CALLBACK(_on_show_still_resolution_size_selection_dialog), settings);

  g_signal_connect(white_balance_button, "clicked", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(iso_level_button, "clicked", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(exposure_button, "clicked", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(resolution_button, "clicked", G_CALLBACK(_close_dialog), dialog);

  gtk_window_set_title(GTK_WINDOW(dialog), dgettext("osso-camera-ui", "camera_ti_general"));
  gtk_widget_show_all(dialog);

  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
  {
  }
  gtk_widget_destroy(dialog);
}

void
show_video_settings_dialog(CameraSettings* settings)
{
  GtkWidget* dialog = gtk_dialog_new();
  GtkWidget* table_layout = gtk_table_new(2, 2, TRUE);
  GtkWidget* white_balance_button = hildon_button_new(HILDON_SIZE_FINGER_HEIGHT,
						      HILDON_BUTTON_ARRANGEMENT_VERTICAL);
  GtkWidget* exposure_button = hildon_button_new(HILDON_SIZE_FINGER_HEIGHT,
						 HILDON_BUTTON_ARRANGEMENT_VERTICAL);
  GtkWidget* resolution_button = hildon_button_new(HILDON_SIZE_FINGER_HEIGHT,
						   HILDON_BUTTON_ARRANGEMENT_VERTICAL);

  hildon_button_set_title(HILDON_BUTTON(white_balance_button), dgettext("osso-camera-ui", "camera_bd_settings_wb"));
  hildon_button_set_title(HILDON_BUTTON(exposure_button), dgettext("osso-camera-ui", "camera_bd_settings_exposure"));
  hildon_button_set_title(HILDON_BUTTON(resolution_button), video_resolution_size_name(settings->video_resolution_size)); 

  gtk_button_set_alignment(GTK_BUTTON(white_balance_button), 0, 0.5);
  gtk_button_set_alignment(GTK_BUTTON(exposure_button), 0, 0.5);
  gtk_button_set_alignment(GTK_BUTTON(resolution_button), 0, 0.5);

  hildon_button_set_style(HILDON_BUTTON(white_balance_button),
			  HILDON_BUTTON_STYLE_PICKER);
  hildon_button_set_style(HILDON_BUTTON(exposure_button),
			  HILDON_BUTTON_STYLE_PICKER);
  hildon_button_set_style(HILDON_BUTTON(resolution_button),
			  HILDON_BUTTON_STYLE_PICKER);

  hildon_button_set_value(HILDON_BUTTON(white_balance_button), 
			  white_balance_mode_name(settings->white_balance));
  hildon_button_set_value(HILDON_BUTTON(exposure_button), 
			  exposure_level_name(settings->exposure_level));
  hildon_button_set_value(HILDON_BUTTON(resolution_button), 
			  video_resolution_size_name(settings->video_resolution_size));

  hildon_button_set_image(HILDON_BUTTON(white_balance_button), 
			  gtk_image_new_from_icon_name(white_balance_mode_icon_name(settings->white_balance, FALSE), HILDON_ICON_SIZE_FINGER));
  hildon_button_set_image(HILDON_BUTTON(exposure_button), 
			  gtk_image_new_from_icon_name(exposure_level_icon_name(settings->exposure_level, FALSE), HILDON_ICON_SIZE_FINGER));
  hildon_button_set_image(HILDON_BUTTON(resolution_button), 
			  gtk_image_new_from_icon_name(video_resolution_size_icon_name(settings->video_resolution_size, FALSE), HILDON_ICON_SIZE_FINGER));
  
  gtk_table_attach_defaults(GTK_TABLE(table_layout), white_balance_button, 0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), exposure_button, 1, 2, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table_layout), resolution_button, 0, 1, 1, 2);
  
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), table_layout, TRUE, TRUE, 0);

  g_signal_connect(white_balance_button, "clicked", G_CALLBACK(_on_show_white_balance_mode_selection_dialog), settings);
  g_signal_connect(exposure_button, "clicked", G_CALLBACK(_on_show_exposure_level_selection_dialog), settings);
  g_signal_connect(resolution_button, "clicked", G_CALLBACK(_on_show_video_resolution_size_selection_dialog), settings);

  g_signal_connect(white_balance_button, "clicked", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(exposure_button, "clicked", G_CALLBACK(_close_dialog), dialog);
  g_signal_connect(resolution_button, "clicked", G_CALLBACK(_close_dialog), dialog);

  gtk_window_set_title(GTK_WINDOW(dialog), dgettext("osso-camera-ui", "camera_ti_general"));
  gtk_widget_show_all(dialog);

  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
  {
  }
  gtk_widget_destroy(dialog);
}

void
show_geotagging_settings_dialog(GeoTagMode* geo_tag_mode)
{
  GtkWidget* dialog = gtk_dialog_new_with_buttons(dgettext("osso-camera-ui", "camera_ti_geotag"),
						  NULL,
						  GTK_DIALOG_MODAL | GTK_DIALOG_NO_SEPARATOR,
						  dgettext("hildon-libs", "wdgt_bd_done"), GTK_RESPONSE_OK,
						  NULL);
  GtkWidget* geotagging_information_text = hildon_text_view_new();
  GtkWidget* geotagging_check_button = hildon_check_button_new(HILDON_SIZE_FINGER_HEIGHT);
  GtkWidget* tag_position_check_button = hildon_check_button_new(HILDON_SIZE_FINGER_HEIGHT);
  GtkWidget* tag_location_check_button = hildon_check_button_new(HILDON_SIZE_FINGER_HEIGHT);
  gtk_button_set_label(GTK_BUTTON(geotagging_check_button), dgettext("osso-camera-ui", "camera_ti_geotag_checkbox"));
  gtk_button_set_label(GTK_BUTTON(tag_position_check_button), "GPS Position");
  gtk_button_set_label(GTK_BUTTON(tag_location_check_button), "Location");
  GtkTextBuffer* info_text = gtk_text_view_get_buffer(GTK_TEXT_VIEW(geotagging_information_text));
  gtk_text_buffer_set_text(info_text, dgettext("osso-camera-ui", "camera_ia_geotag"),-1);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(geotagging_information_text), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(geotagging_information_text), GTK_WRAP_WORD);
  GtkWidget* geotagging_information_text_label = gtk_label_new(dgettext("osso-camera-ui", "camera_ia_geotag"));
  gtk_label_set_line_wrap(GTK_LABEL(geotagging_information_text_label), TRUE);
  gtk_label_set_line_wrap_mode(GTK_LABEL(geotagging_information_text_label), GTK_WRAP_WORD);
  GtkWidget* box = gtk_hbox_new(TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), tag_position_check_button, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), tag_location_check_button, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), geotagging_information_text_label, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), geotagging_check_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), box, TRUE, FALSE, 0);

  if((*geo_tag_mode) != GEO_TAG_NONE)
  {
    hildon_check_button_set_active(HILDON_CHECK_BUTTON(geotagging_check_button), TRUE);
    hildon_check_button_set_active(HILDON_CHECK_BUTTON(tag_position_check_button), 
				   (*geo_tag_mode) == GEO_TAG_POSITION || 
				   (*geo_tag_mode) == GEO_TAG_POS_AND_LOC);
    hildon_check_button_set_active(HILDON_CHECK_BUTTON(tag_location_check_button), 
				   (*geo_tag_mode) == GEO_TAG_LOCATION || 
				   (*geo_tag_mode) == GEO_TAG_POS_AND_LOC);
  }
  else
  {
    gtk_widget_set_sensitive(box, FALSE);
  }

  g_signal_connect(tag_location_check_button, "toggled", G_CALLBACK(_check_one_of), tag_position_check_button);
  g_signal_connect(tag_position_check_button, "toggled", G_CALLBACK(_check_one_of), tag_location_check_button);
  g_signal_connect(geotagging_check_button, "toggled", G_CALLBACK(_enable_disable), box);
  //  gtk_window_set_default_size(GTK_WINDOW(dialog), -1, 350);
  gtk_widget_show_all(dialog);

  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
  {
    if(!hildon_check_button_get_active(HILDON_CHECK_BUTTON(geotagging_check_button)))
    {
      (*geo_tag_mode) = GEO_TAG_NONE;
    }
    else
    {
      if(hildon_check_button_get_active(HILDON_CHECK_BUTTON(tag_position_check_button)) &&
	 hildon_check_button_get_active(HILDON_CHECK_BUTTON(tag_location_check_button)))
      {
	(*geo_tag_mode) = GEO_TAG_POS_AND_LOC;
      }
      else if(hildon_check_button_get_active(HILDON_CHECK_BUTTON(tag_position_check_button)) &&
	      !hildon_check_button_get_active(HILDON_CHECK_BUTTON(tag_location_check_button)))
      {
	(*geo_tag_mode) = GEO_TAG_POSITION;
      }
      else if(!hildon_check_button_get_active(HILDON_CHECK_BUTTON(tag_position_check_button)) &&
	      hildon_check_button_get_active(HILDON_CHECK_BUTTON(tag_location_check_button)))
      {
	(*geo_tag_mode) = GEO_TAG_LOCATION;
      }
    }
  }
  gtk_widget_destroy(dialog);
}
