#include <hildon/hildon.h>
#include "camera-ui2-window.h"

#include <libosso.h>
#include <libhal.h>
#include <libintl.h>
#include <locale.h>
#include <libgnomevfs/gnome-vfs.h>

#define OSSO_SERVICE "com.nokia.cameraui2"
#define VERSION "0.0.1"

typedef struct _AppData AppData;

struct _AppData
{
  HildonProgram* program;
  CameraUI2Window* window;
};

static void 
on_main_window_hide(GtkWidget *widget, gpointer user_data)
{
  //gtk_main_quit();
}

static void
on_is_topmost_changed(HildonProgram* program,
		      GParamSpec* psp,
		      gpointer user_data)
{
  AppData* data = (AppData*)user_data;
  if(hildon_program_get_is_topmost(program))
  {
    camera_ui2_window_show_ui(data->window);
  }
  else
  {
    camera_ui2_window_hide_ui(data->window);
  }
}

int main(int argc, char** argv)
{
  osso_context_t* osso = NULL;
  LibHalContext* hal_context = NULL;

  AppData *data = g_new0 (AppData, 1);
  setlocale(LC_ALL, "");
  hildon_gtk_init(&argc, &argv);
  gnome_vfs_init();
  if(!g_thread_supported())
  {
    g_thread_init(NULL);
  }

  data->program = HILDON_PROGRAM(hildon_program_get_instance());
  osso = osso_initialize(OSSO_SERVICE, VERSION, TRUE, NULL);
  hal_context = libhal_ctx_new();
  DBusError error;
  dbus_error_init(&error);
  libhal_ctx_set_dbus_connection(hal_context, (DBusConnection*)osso_get_sys_dbus_connection(osso));
  if(libhal_ctx_init(hal_context, &error))
  {
    data->window = camera_ui2_window_new(osso, data->program);
    gtk_window_set_title(GTK_WINDOW(data->window), dgettext("osso-camera-ui", "camera_ap_camera"));
    libhal_ctx_set_user_data(hal_context, data->window);
    if(!dbus_helper_register_top_application_callback(osso, data->window))
    {
      g_print("can not register top_application callback\n");
    }
    if(!dbus_helper_register_hal_cam_property(hal_context))
    {
      g_print("can not register  hal cam callbacks\n");
    }

    g_signal_connect(data->window, "hide", G_CALLBACK(&on_main_window_hide), NULL);
    g_signal_connect(data->window, "delete-event", G_CALLBACK(&gtk_widget_hide_on_delete), NULL);
    g_signal_connect(data->program, "notify::is-topmost", G_CALLBACK(on_is_topmost_changed), data);
    gtk_main();
    gtk_widget_destroy (GTK_WIDGET (data->window));
  }
  libhal_ctx_shutdown(hal_context, &error);
  libhal_ctx_free(hal_context);
  g_free(data);
  return 0;
}
