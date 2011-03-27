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

#include "dbus-helper.h"
#include "camera-ui2-window.h"

#include <mce/mode-names.h>
#include <mce/dbus-names.h>
#include <hildon-mime.h>

#define COM_NOKIA_CAMERAUI2_SERVICE "com.nokia.cameraui"
#define COM_NOKIA_CAMERAUI2_PATH "com/nokia/cameraui"
#define COM_NOKIA_CAMERAUI2_IF "com.nokia.cameraui"

#define OFH_SERVICE "org.freedesktop.Hal"
#define OFH_CAM_SHUTTER_PATH "/org/freedesktop/Hal/devices/platform_cam_shutter"
#define OFH_CAM_LAUNCH_PATH "/org/freedesktop/Hal/devices/platform_cam_launch"
#define OFH_CAM_FOCUS_PATH "/org/freedesktop/Hal/devices/platform_cam_focus"
#define OFH_DEVICE_IF        "org.freedesktop.Hal.Device"
#define GET_PROPERTY_METHOD "GetProperty"
#define PROPERTY_MODIFIED_METHOD "PropertyModified"
#define BUTTON_STATE_VALUE "button.state.value"
#define HD_APP_MGR_DBUS_NAME "com.nokia.HildonDesktop.AppMgr"
#define HD_APP_MGR_DBUS_PATH "/com/nokia/HildonDesktop/AppMgr"
#define HD_APP_MGR_DBUS_IFACE "com.nokia.HildonDesktop.AppMgr"
#define HAL_UDI_CAM_SHUTTER "/org/freedesktop/Hal/devices/platform_cam_shutter"
#define HAL_UDI_CAM_FOCUS "/org/freedesktop/Hal/devices/platform_cam_focus"
#define HAL_UDI_CAM_CAPTURE "/org/freedesktop/Hal/devices/platform_cam_launch"
#define HD_APP_MGR_LAUNCH_APPLICATION "LaunchApplication"
#define TOP_APPLICATION "top_application"
#define IMAGE_VIEWER "image_viewer"
#define MEDIAPLAYER "mediaplayer"
#define OPEN_MEDIAPLAYER_CATEGORY "open_mp_video_category"

gboolean
dbus_helper_is_camera_launcher_button_pressed(osso_context_t* osso)
{
  osso_return_t ret;
  osso_rpc_t return_value;
  ret = osso_rpc_run_system(osso,
			    OFH_SERVICE, OFH_CAM_LAUNCH_PATH, OFH_DEVICE_IF,
			    GET_PROPERTY_METHOD,
			    &return_value,
			    DBUS_TYPE_STRING,
			    BUTTON_STATE_VALUE,
			    DBUS_TYPE_INVALID);
  gboolean closed = TRUE;
  if(ret == OSSO_OK)
  {
    closed = return_value.value.b;
    osso_rpc_free_val(&return_value);
  }
  return closed;
}

gboolean
dbus_helper_is_camera_focus_button_pressed(osso_context_t* osso)
{
  osso_return_t ret;
  osso_rpc_t return_value;
  ret = osso_rpc_run_system(osso,
			    OFH_SERVICE, OFH_CAM_FOCUS_PATH, OFH_DEVICE_IF,
			    GET_PROPERTY_METHOD,
			    &return_value,
			    DBUS_TYPE_STRING,
			    BUTTON_STATE_VALUE,
			    DBUS_TYPE_INVALID);
  gboolean closed = TRUE;
  if(ret == OSSO_OK)
  {
    closed = return_value.value.b;
    osso_rpc_free_val(&return_value);
  }
  return closed;
}

gboolean
dbus_helper_is_camera_lenscover_closed(osso_context_t* osso)
{
  osso_return_t ret;
  osso_rpc_t return_value;
  ret = osso_rpc_run_system(osso,
			    OFH_SERVICE, OFH_CAM_SHUTTER_PATH, OFH_DEVICE_IF,
			    GET_PROPERTY_METHOD,
			    &return_value,
			    DBUS_TYPE_STRING,
			    BUTTON_STATE_VALUE,
			    DBUS_TYPE_INVALID);
  gboolean closed = TRUE;
  if(ret == OSSO_OK)
  {
    closed = return_value.value.b;
    osso_rpc_free_val(&return_value);
  }
  return closed;
}

void
dbus_helper_start_image_viewer(osso_context_t* osso)
{
  osso_rpc_run_with_defaults(osso,
			     IMAGE_VIEWER,
			     TOP_APPLICATION,
			     NULL,
			     DBUS_TYPE_INVALID);
}

void
dbus_helper_start_media_player(osso_context_t* osso)
{
  osso_rpc_run_with_defaults(osso,
			     MEDIAPLAYER,
			     OPEN_MEDIAPLAYER_CATEGORY,
			     NULL,
			     DBUS_TYPE_INVALID);
}

void
dbus_helper_start_viewer(osso_context_t* osso, const gchar* media_file)
{
  DBusConnection* connection = dbus_bus_get(DBUS_BUS_SESSION, NULL);
  gchar* uri = g_strdup_printf("file://%s", media_file);
  if(connection)
  {
    hildon_mime_open_file(connection, uri);
  }
  g_free(uri);
  dbus_connection_unref(connection);
}

void
dbus_helper_start_mime_viewer(osso_context_t* osso, const gchar* media_file, const gchar* mime_type)
{
  DBusConnection* connection = dbus_bus_get(DBUS_BUS_SESSION, NULL);
  gchar* uri = g_strdup_printf("file://%s", media_file);
  if(connection)
  {
    hildon_mime_open_file_with_mime_type(connection, uri, mime_type);
  }
  g_free(uri);
  dbus_connection_unref(connection);
}

gint
dbus_helper_get_device_orientation_mode(osso_context_t* osso)
{
  osso_return_t ret;
  osso_rpc_t return_value;
  ret = osso_rpc_run_system(osso,
			    MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IF,
			    MCE_DEVICE_ORIENTATION_GET,
			    &return_value,
			    DBUS_TYPE_INVALID);
  int mode = 1;
  if(ret == OSSO_OK)
  {
    if(g_strcmp0(return_value.value.s, MCE_ORIENTATION_PORTRAIT) == 0)
    {
      mode = 6;
    }
    else if(g_strcmp0(return_value.value.s, MCE_ORIENTATION_PORTRAIT_INVERTED) == 0)
    {
      mode = 8;
    }
    else if(g_strcmp0(return_value.value.s, MCE_ORIENTATION_LANDSCAPE_INVERTED) == 0)
    {
      mode = 3;
    }
    osso_rpc_free_val(&return_value);
  }
  return mode;
}

static gint 
top_camera_ui2_application(const gchar* interface, 
			   const gchar* method,
			   GArray* arguments, 
			   gpointer data,
			   osso_rpc_t * retval)
{
  if(g_strcmp0(method, TOP_APPLICATION)==0)
    camera_ui2_window_top_application(CAMERA_UI2_WINDOW(data));
  return OSSO_OK;
}

gboolean
dbus_helper_register_top_application_callback(osso_context_t* osso, gpointer user_data)
{
  osso_return_t result = osso_rpc_set_cb_f(osso, 
					   COM_NOKIA_CAMERAUI2_SERVICE,
					   COM_NOKIA_CAMERAUI2_PATH,
					   COM_NOKIA_CAMERAUI2_IF,
					   top_camera_ui2_application, user_data);
  return result == OSSO_OK;
}

static void
hal_cam_property_modified(LibHalContext* hal_context,
			  const char* udi,
			  const char* key,
			  dbus_bool_t is_removed, 
			  dbus_bool_t is_added)
{
  void* user_data = libhal_ctx_get_user_data(hal_context);
  if(user_data)
  {
    if(CAMERA_UI2_IS_WINDOW(user_data))
    {
      CameraUI2Window* camera_ui2_window = CAMERA_UI2_WINDOW(user_data);
      if(g_strcmp0(key, BUTTON_STATE_VALUE) == 0)
      {
	int state = libhal_device_get_property_bool(hal_context, udi, "button.state.value", NULL);
	
	if(g_strcmp0(udi, HAL_UDI_CAM_SHUTTER) == 0)
	{
	  if(state == 1)
	    camera_ui2_window_lenscover_closed(camera_ui2_window);
	  else
	    camera_ui2_window_lenscover_opened(camera_ui2_window);
	}
	if(g_strcmp0(udi, HAL_UDI_CAM_FOCUS) == 0)
	{
	  if(state == 1)
	    camera_ui2_window_focus_button_pressed(camera_ui2_window);
	  else
	    camera_ui2_window_focus_button_released(camera_ui2_window);
	}
	if(g_strcmp0(udi, HAL_UDI_CAM_CAPTURE) == 0)
	{
	  if(state == 1)
	    camera_ui2_window_capture_button_pressed(camera_ui2_window);
	  else
	    camera_ui2_window_capture_button_released(camera_ui2_window);
	}
      }
    }
  }
}

gboolean
dbus_helper_register_hal_cam_property(LibHalContext* hal_context)
{
  if(libhal_ctx_set_device_property_modified(hal_context, hal_cam_property_modified))
  {
    if(libhal_device_add_property_watch(hal_context, HAL_UDI_CAM_SHUTTER, NULL) &&
       libhal_device_add_property_watch(hal_context, HAL_UDI_CAM_FOCUS, NULL) &&
       libhal_device_add_property_watch(hal_context, HAL_UDI_CAM_CAPTURE, NULL))
    {
      return TRUE;
    }
  }
  return FALSE;
}

void
dbus_helper_uregister_hal_cam_property(LibHalContext* hal_context)
{
  libhal_device_remove_property_watch(hal_context, HAL_UDI_CAM_SHUTTER, NULL);
  libhal_device_remove_property_watch(hal_context, HAL_UDI_CAM_FOCUS, NULL);
  libhal_device_remove_property_watch(hal_context, HAL_UDI_CAM_CAPTURE, NULL);
}
