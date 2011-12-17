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
#include "camera-interface.h"
#include <gst/interfaces/photography.h>
#include <gdigicam/gst-camerabin/gdigicam-camerabin.h>
#include <gdigicam/gdigicam-manager.h>
#include <glib/gstdio.h>
#include <linux/videodev2.h>

#include <string.h>
static const gchar* VIDEO_SOURCE = "v4l2camsrc";
static const gchar* VIDEO_ENC    = "dspmp4venc";
static const gchar* VIDEO_MUX    = "hantromp4mux";
static const gchar* AUDIO_SOURCE = "pulsesrc";
static const gchar* AUDIO_ENC    = "nokiaaacenc";
static const gchar* IMAGE_ENC    = "jpegenc";
static const gchar* IMAGE_PP     = NULL;
static const gchar* XIMAGE_SINK  = "xvimagesink";

static guint8* raw_image_buffer;

struct _CameraInterface
{
  GDigicamManager* manager;
  gint colorkey;
  gulong viewfinder_window_id;
  gint video_fd;
  guint event_probe_id;
  CameraCaptureData* camera_capture_data;
};

static GstBin*
_get_camerabin(CameraInterface* camera_interface)
{
  GstElement* bin = NULL;
  g_digicam_manager_get_gstreamer_bin(camera_interface->manager,
				      &bin,
				      NULL);
  return GST_BIN(bin);
}

static void
_remove_tag_listener(CameraInterface* camera_interface)
{
  if(camera_interface->event_probe_id!=0)
  {
    GstBin* bin = _get_camerabin(camera_interface);
    GstElement* tagger = gst_bin_get_by_name(bin, "v4l2camsrc0");
    GstPad* pad = gst_element_get_pad(tagger,"src");
    gst_pad_remove_event_probe(pad, camera_interface->event_probe_id);
    camera_interface->event_probe_id = 0;
    gst_object_unref(pad);
    gst_object_unref(tagger);
    gst_object_unref(bin);
  }
}

static gboolean
_had_capture_data(GstTagList* tag_list, CameraInterface* camera_interface)
{
  if(gst_tag_list_is_empty(tag_list))
    return FALSE;
  gboolean got_data = FALSE;
  gint iso_speed = 0;
  guint flash = 0;
  guint gain = 0;
  const GValue* value = NULL;

  if(gst_tag_list_get_int(tag_list, 
			  "capture-iso-speed-ratings", 
			  &iso_speed))
  {  
    camera_interface->camera_capture_data->capture_iso_speed = iso_speed;
    got_data = TRUE;
  }
  if((value = gst_tag_list_get_value_index(tag_list, "capture-aperture", 0))!=NULL)
  {
    gint numerator = gst_value_get_fraction_numerator(value);
    gint denominator = gst_value_get_fraction_denominator(value);
    camera_interface->camera_capture_data->capture_aperture_n = numerator;
    camera_interface->camera_capture_data->capture_aperture_d = denominator;
    got_data = TRUE;
  }
  if((value = gst_tag_list_get_value_index(tag_list, "capture-shutter-speed", 0))!=NULL)
  {
    gint numerator = gst_value_get_fraction_numerator(value);
    gint denominator = gst_value_get_fraction_denominator(value);
    camera_interface->camera_capture_data->capture_shutter_speed_n = numerator;
    camera_interface->camera_capture_data->capture_shutter_speed_d = denominator;
    got_data = TRUE;
  }
  if((value = gst_tag_list_get_value_index(tag_list, "capture-exposure-time", 0))!=NULL)
  {
    gint numerator = gst_value_get_fraction_numerator(value);
    gint denominator = gst_value_get_fraction_denominator(value);
    camera_interface->camera_capture_data->capture_exposure_time_n = numerator;
    camera_interface->camera_capture_data->capture_exposure_time_d = denominator;
    got_data = TRUE;
  }
  if(gst_tag_list_get_uint(tag_list, 
		       "capture-flash", 
		       &flash))
  {  
    camera_interface->camera_capture_data->capture_flash = flash;
    got_data = TRUE;
  }
  if((value = gst_tag_list_get_value_index(tag_list, "capture-fnumber", 0))!=NULL)
  {
    gint numerator = gst_value_get_fraction_numerator(value);
    gint denominator = gst_value_get_fraction_denominator(value);
    camera_interface->camera_capture_data->capture_fnumber_n = numerator;
    camera_interface->camera_capture_data->capture_fnumber_d = denominator;
    got_data = TRUE;
  }
  if((value = gst_tag_list_get_value_index(tag_list, "capture-focal-len", 0))!=NULL)
  {
    gint numerator = gst_value_get_fraction_numerator(value);
    gint denominator = gst_value_get_fraction_denominator(value);
    camera_interface->camera_capture_data->capture_focal_length_n = numerator;
    camera_interface->camera_capture_data->capture_focal_length_d = denominator;
    got_data = TRUE;
  }
  if(gst_tag_list_get_uint(tag_list, 
			   "capture-gain", 
			   &gain))
  {  
    camera_interface->camera_capture_data->capture_gain = gain;
    got_data = TRUE;
  }
  gchar* date_time = NULL;
  if(got_data)
  {
    GstBin* bin = _get_camerabin(camera_interface);
    const GstTagList* tag_list2 = gst_tag_setter_get_tag_list(GST_TAG_SETTER(bin));
    if(gst_tag_list_get_string(tag_list2,
			     "date-time-original",
			     &date_time))
    {
      if(camera_interface->camera_capture_data->date_time_stamp)
	g_free(camera_interface->camera_capture_data->date_time_stamp);
      camera_interface->camera_capture_data->date_time_stamp = date_time;
      got_data = TRUE;
    }
    gst_object_unref(bin);
  }
  return got_data;
}

static gboolean
_tag_event_cb(GstPad* pad,
	      GstEvent* event,
	      gpointer user_data)
{
  if(GST_EVENT_TYPE(event) == GST_EVENT_TAG)
  { 
    GstTagList* taglist = NULL;
    gst_event_parse_tag (event, &taglist);
    if(taglist && _had_capture_data(taglist, (CameraInterface*)user_data))
    {
      //
    }
  }
  return TRUE;
}

/*
  Camera parameter capture data (exposure time, iso speed) are bypassed to the
  metadata mux, I want to use this data as well in my raw exporter, so I have
  to register a pad event listener on the gstv4l2camsrc element.
  (Wasn't easy to find out)
 */
static void
_add_tag_listener(CameraInterface* camera_interface)
{
  GstBin* bin = _get_camerabin(camera_interface);
  GstElement* tagger = gst_bin_get_by_name(bin, "v4l2camsrc0");
  GstPad* pad = gst_element_get_pad(tagger,"src");
  camera_interface->event_probe_id = 
    gst_pad_add_event_probe(pad, G_CALLBACK(_tag_event_cb), camera_interface);
  gst_object_unref(pad);
  gst_object_unref(tagger);
  gst_object_unref(bin);
}

/*
  Can be used to calculate bitrate based on resolution and target quality.  
  Coefficients retrieved from gst-dsp and adopted for HD codecs.
*/
static guint
calc_hd_bitrate(GstPad * pad)
{
  float bits_per_pixel = 0.2, scale;
  guint ref_bitrate,bitrate = 0;
  const guint reference_fps = 15;
  const float quality = 2.8;
  guint height = 0, width = 0;
  float framerate=0;
  GstCaps *caps;
  GstStructure *str; 
  const GValue *g_framerate = NULL;

  caps = gst_pad_get_negotiated_caps(pad);
  str = gst_caps_get_structure (caps, 0);

  if(gst_structure_get_int (str, "height", &height) &&
     gst_structure_get_int (str, "width", &width) &&
      (g_framerate = gst_structure_get_value (str, "framerate")))
  {

    framerate = ((float)gst_value_get_fraction_numerator(g_framerate))/
                (float)gst_value_get_fraction_denominator(g_framerate);

    ref_bitrate = (width * height) / bits_per_pixel;
    
    scale = 1 + ((float) framerate / reference_fps - 1) * quality;
    
    bitrate = ref_bitrate * scale;
    g_debug("Setting bitrate = %.2f for width=%u,height=%u,framerate=%.2f\n",(float)bitrate,width,height,framerate);
  }
  else
    g_debug("Unable to get src pad caps\n");

  return bitrate;
}

/*
  I want to handle the bus messages our self. So I can catch the raw image buffer
  from the camdriver. All the rest is copied from gdigicams handle_bus_message_func,
  as we can only replace and not extend that function.
 */
static gboolean
_handle_bus_message_func(GDigicamManager *manager,
			 gpointer user_data)
{
  const GstStructure *structure = NULL;
  const GstMessage* msg;
  GstElement* bin = NULL;

  msg = GST_MESSAGE(user_data);
  switch(GST_MESSAGE_TYPE(GST_MESSAGE(user_data)))
  {
  case GST_MESSAGE_STATE_CHANGED:
    if(g_digicam_manager_get_gstreamer_bin(manager,
					   &bin,
					   NULL))
    {
      if(GST_ELEMENT(GST_MESSAGE_SRC(GST_MESSAGE(user_data))) == bin)
      {
	GstState old = 0;
	GstState new = 0;
	GstState pending = 0;
	gst_message_parse_state_changed(GST_MESSAGE(user_data),
					&old, &new, &pending);
	if(GST_STATE_PLAYING == new)
	{
	  GDigicamMode mode;
	  if(g_digicam_manager_get_mode(manager,
					&mode,
					NULL))
	  {
	    switch(mode)
	    {
	      case G_DIGICAM_MODE_STILL:
		g_object_set(bin, "mode", 0, NULL);
		break;
	      case G_DIGICAM_MODE_VIDEO:
		g_object_set(bin, "mode", 1, NULL);
	    
		  break;
	    }
	  }
	}
	else if(GST_STATE_READY == new)
	{
	  GDigicamMode mode;
	  if(g_digicam_manager_get_mode(manager,
					&mode,
					NULL))
	  {
	    switch(mode)
	    {
	      case G_DIGICAM_MODE_VIDEO:
	      {
		GstElement* videoenc = 0;
		GstElement* videosrc = 0;
		
		g_object_get(bin,"videoenc",&videoenc,NULL);
		if(videoenc)
		{
		  g_object_get(bin,"videosrc",&videosrc,NULL);
		  if(videosrc)
		  {
		    GstPad * srcpad=gst_element_get_static_pad (videosrc,"src");
		    if(srcpad)
		    {
		      gst_object_unref (GST_OBJECT (srcpad));
		      gchar * videoenc_name = NULL;
		      g_object_get(videoenc,"name",&videoenc_name,NULL);
		      if(videoenc_name)
		      {
			if(g_str_has_prefix(videoenc_name,"dsphdmp4venc"))
			{
			  guint bitrate = calc_hd_bitrate(srcpad);
			  g_object_set(videoenc,
				       "intra-refresh",1,
				       NULL);
			  g_object_set(videoenc,
				       "max-bitrate",bitrate,
				       NULL);
			  g_object_set(videoenc,
				       "bitrate",bitrate,
				       NULL);
			}
			else
			  g_object_set(videoenc,
				       "bitrate",0,
				       NULL);
			g_free (videoenc_name);			
		      }
		      else
		      {
			g_debug("Unable to get videoenc name\n");
			g_object_set(videoenc,
				     "bitrate",0,
				     NULL);
		      }
		    }
		    else
		      g_debug("Unable to get src pad\n");
		    gst_object_unref(videosrc);
		  }
		  else
		    g_debug("Unable to get videosrc element\n");
		  gst_object_unref(videoenc);

		}
		else
		  g_debug("Unable to get videoenc element\n");
		break;
	      }
	    }
	  }
	}
      }
      if(NULL != bin)
      {
	gst_object_unref(bin);
      }
    }
    break;
  case GST_MESSAGE_ELEMENT:
    structure = gst_message_get_structure(GST_MESSAGE(user_data));
    const gchar* message_name = gst_structure_get_name(structure);
    if(g_strcmp0(message_name, GST_PHOTOGRAPHY_AUTOFOCUS_DONE) == 0)
    {
      gint status = GST_PHOTOGRAPHY_FOCUS_STATUS_NONE;
      gst_structure_get_int(structure, "status", &status);
      switch(status)
      {
      case GST_PHOTOGRAPHY_FOCUS_STATUS_FAIL:
	g_signal_emit_by_name(manager,
			      "focus-done", 
			      G_DIGICAM_FOCUSMODESTATUS_UNABLETOREACH);
	break;
      case GST_PHOTOGRAPHY_FOCUS_STATUS_SUCCESS:
	g_signal_emit_by_name(manager,
			      "focus-done",
			      G_DIGICAM_FOCUSMODESTATUS_REACHED);
	break;
      case GST_PHOTOGRAPHY_FOCUS_STATUS_NONE:
	g_signal_emit_by_name(manager,
			      "focus-done",
			      G_DIGICAM_FOCUSMODESTATUS_REQUEST);
	break;
      case GST_PHOTOGRAPHY_FOCUS_STATUS_RUNNING:
	break;
      }
      return TRUE;
    }
    else if(g_strcmp0(message_name, "raw-image") == 0)
    {
      if(gst_structure_has_field_typed(structure, "buffer", GST_TYPE_BUFFER))
      {
	const GValue* image = gst_structure_get_value(structure, "buffer");
	if(image)
	{
	  if(raw_image_buffer)
          {
	    memcpy(raw_image_buffer,GST_BUFFER_DATA(gst_value_get_buffer(image)),(2576*1960*2));
          }
	}
      }
    }
    break;
  }
  return TRUE;
}

/*
  debug helper
 */
static
_print_result_message(gboolean result, GError** error, const gchar* operation)
{
  if(!result) 
  {
    if((*error) != NULL) 
    {
      g_warning("Error %s Value: %s\n", operation, (*error)->message);
      g_error_free(*error);
    } 
    else 
    {
      g_warning("Error setting the %s "
		"Value: Internal error !!!\n",operation);
    }
  } 
  /* else  */
  /* { */
  /*   g_warning("%s successfully.\n", operation); */
  /* } */
}

static
GDigicamDescriptor*
_new_g_digicam_camerabin_descriptor(GstElement* bin)
{
  GDigicamDescriptor* descriptor = 
    g_digicam_camerabin_descriptor_new(bin);
  // guess values
  descriptor->max_zoom_macro_enabled  = 6;
  descriptor->max_zoom_macro_disabled = 6;
  descriptor->max_digital_zoom        = 6;
  descriptor->supported_modes =
    descriptor->supported_modes |
    G_DIGICAM_MODE_STILL        |
    G_DIGICAM_MODE_VIDEO;
  descriptor->supported_iso_sensitivity_modes =
    descriptor->supported_iso_sensitivity_modes |
    G_DIGICAM_ISOSENSITIVITYMODE_MANUAL         |
    G_DIGICAM_ISOSENSITIVITYMODE_AUTO;
  descriptor->supported_white_balance_modes =
    descriptor->supported_white_balance_modes |
    G_DIGICAM_WHITEBALANCEMODE_MANUAL         |
    G_DIGICAM_WHITEBALANCEMODE_AUTO           |
    G_DIGICAM_WHITEBALANCEMODE_SUNLIGHT       |
    G_DIGICAM_WHITEBALANCEMODE_CLOUDY         |
    G_DIGICAM_WHITEBALANCEMODE_SHADE          |
    G_DIGICAM_WHITEBALANCEMODE_TUNGSTEN       |
    G_DIGICAM_WHITEBALANCEMODE_FLUORESCENT    |
    G_DIGICAM_WHITEBALANCEMODE_INCANDESCENT   |
    G_DIGICAM_WHITEBALANCEMODE_FLASH          |
    G_DIGICAM_WHITEBALANCEMODE_SUNSET;
  descriptor->supported_audio_states =
    descriptor->supported_audio_states |
    G_DIGICAM_AUDIO_RECORDON           |
    G_DIGICAM_AUDIO_RECORDOFF;
  descriptor->supported_preview_modes =
    descriptor->supported_preview_modes |
    G_DIGICAM_PREVIEW_ON |
    G_DIGICAM_PREVIEW_OFF;
  descriptor->supported_features = descriptor->supported_features |
    G_DIGICAM_CAPABILITIES_VIEWFINDER                   |
    G_DIGICAM_CAPABILITIES_RESOLUTION                   |
    G_DIGICAM_CAPABILITIES_ASPECTRATIO                  |
    G_DIGICAM_CAPABILITIES_MANUALFOCUS                  |
    G_DIGICAM_CAPABILITIES_METERING                     |
    G_DIGICAM_CAPABILITIES_QUALITY                      |
    G_DIGICAM_CAPABILITIES_AUTOFOCUS                    |
    G_DIGICAM_CAPABILITIES_MACROFOCUS                   |
    G_DIGICAM_CAPABILITIES_DIGITALZOOM                  |
    G_DIGICAM_CAPABILITIES_MANUALEXPOSURE               |
    G_DIGICAM_CAPABILITIES_AUTOEXPOSURE                 |
    G_DIGICAM_CAPABILITIES_AUTOWHITEBALANCE             |
    G_DIGICAM_CAPABILITIES_MANUALWHITEBALANCE           |
    G_DIGICAM_CAPABILITIES_AUTOISOSENSITIVITY           |
    G_DIGICAM_CAPABILITIES_MANUALISOSENSITIVITY         |
    G_DIGICAM_CAPABILITIES_FLASH                        |
    G_DIGICAM_CAPABILITIES_AUDIO                        |
    G_DIGICAM_CAPABILITIES_PREVIEW;
  descriptor->supported_flash_modes =
    descriptor->supported_flash_modes       |
    G_DIGICAM_FLASHMODE_OFF                 |
    G_DIGICAM_FLASHMODE_ON                  |
    G_DIGICAM_FLASHMODE_AUTO                |
    G_DIGICAM_FLASHMODE_REDEYEREDUCTION     |
    G_DIGICAM_FLASHMODE_REDEYEREDUCTIONAUTO |
    G_DIGICAM_FLASHMODE_FILLIN;
  descriptor->supported_resolutions =
    descriptor->supported_resolutions |
    G_DIGICAM_RESOLUTION_HD           |
    G_DIGICAM_RESOLUTION_HIGH         |
    G_DIGICAM_RESOLUTION_MEDIUM       |
    G_DIGICAM_RESOLUTION_LOW;
  descriptor->supported_aspect_ratios =
    descriptor->supported_aspect_ratios |
    G_DIGICAM_ASPECTRATIO_4X3           |
    G_DIGICAM_ASPECTRATIO_16X9;
  descriptor->handle_bus_message_func = _handle_bus_message_func;
  return descriptor;
}

static void
_get_zoom_level(GDigicamManager* manager, gdouble* zoom, gboolean* digital)
{
  GError* error = NULL;
  g_digicam_manager_get_zoom(manager, zoom, digital, &error);
  if(error)
  {
    g_error_free(error);
    error = NULL;
  }
}

static gboolean
_set_zoom_level(GDigicamManager* manager, gdouble zoom, gboolean digital)
{
  GDigicamCamerabinZoomHelper *helper = NULL;
  GError *error = NULL;
  gboolean result = FALSE;
  helper = g_slice_new(GDigicamCamerabinZoomHelper);
  helper->value = zoom;
  result = g_digicam_manager_set_zoom(manager,
				      helper->value,
				      &digital,
				      &error,
				      helper);
  g_slice_free(GDigicamCamerabinZoomHelper,
	       helper);
  return result;
}

CameraInterface*
create_camera_interface()
{
  GDigicamManager* manager = NULL;
  GDigicamDescriptor* descriptor = NULL;
  GstElement* bin = NULL;
  
  GError* error = NULL;
  gint colorkey;
  g_digicam_init(0, NULL);
  manager = g_digicam_manager_new();
  if(manager == NULL)
  {
    g_debug("can not create gdigicam manager\n");
    return NULL;
  }
  
  bin = g_digicam_camerabin_element_new(VIDEO_SOURCE,
					VIDEO_ENC,
					VIDEO_MUX,
					AUDIO_SOURCE,
					AUDIO_ENC,
					IMAGE_ENC,
					IMAGE_PP,
					XIMAGE_SINK,
					&colorkey);
  if(bin == NULL)
  {
    gst_object_unref(manager);
    manager = NULL;
    return NULL;
  }
  descriptor = _new_g_digicam_camerabin_descriptor(bin);
  if(!g_digicam_manager_set_gstreamer_bin(manager,
					  bin,
					  descriptor,
					  &error))
  {
    if(error)
    {
      g_debug("set gstreamer bin failed: %s\n", error->message);
      g_error_free(error);
      error = NULL;
    }
    else
    {
      g_debug("set gstreamer bin failed\n");
    }
    g_object_unref(bin);
    bin = NULL;
    g_object_unref(descriptor);
    descriptor = NULL;
    g_object_unref(manager);
    manager = NULL;
    return NULL;
  }
  CameraInterface* camera_interface = g_new0(CameraInterface, 1);
  camera_interface->manager = manager;
  camera_interface->colorkey = colorkey;
  camera_interface->camera_capture_data = g_new0(CameraCaptureData,1);
  raw_image_buffer = NULL;
  return camera_interface;
}

gint
camera_interface_get_color_key(CameraInterface* camera_interface)
{
  return camera_interface->colorkey;
}

gboolean
camera_interface_set_scene_mode(CameraInterface* camera_interface, CamSceneMode scene_mode)
{
  GDigicamCamerabinModeHelper* still_video_helper = NULL;
  GDigicamCamerabinExposureModeHelper* helper = NULL;
  GDigicamCamerabinFocusModeHelper *focus_helper = NULL;

  GError* error = NULL;
  gboolean result = FALSE;
  still_video_helper = g_slice_new(GDigicamCamerabinModeHelper);
  helper = g_slice_new(GDigicamCamerabinExposureModeHelper);
  focus_helper = g_slice_new(GDigicamCamerabinFocusModeHelper);
  focus_helper->macro_enabled = FALSE;
  focus_helper->focus_mode = G_DIGICAM_FOCUSMODE_AUTO;

  camera_interface_set_focus_mode(camera_interface, TRUE);
  still_video_helper->mode = G_DIGICAM_MODE_STILL;
  switch(scene_mode)
  {
    case CAM_SCENE_MODE_AUTO:
      helper->exposure_mode = G_DIGICAM_EXPOSUREMODE_AUTO;
      break;
    case CAM_SCENE_MODE_LANDSCAPE:
      helper->exposure_mode = G_DIGICAM_EXPOSUREMODE_LANDSCAPE;
      focus_helper->focus_mode = G_DIGICAM_FOCUSMODE_NONE;
      break;
    case CAM_SCENE_MODE_NIGHT:
      helper->exposure_mode = G_DIGICAM_EXPOSUREMODE_NIGHT;
      break;
    case CAM_SCENE_MODE_PORTRAIT:
      helper->exposure_mode = G_DIGICAM_EXPOSUREMODE_PORTRAIT;
      break;
    case CAM_SCENE_MODE_MACRO:
      focus_helper->macro_enabled = TRUE;
      helper->exposure_mode = G_DIGICAM_EXPOSUREMODE_PORTRAIT;
      break;
    case CAM_SCENE_MODE_ACTION:
      helper->exposure_mode = G_DIGICAM_EXPOSUREMODE_AUTO;
      focus_helper->focus_mode = G_DIGICAM_FOCUSMODE_NONE;
      break;
    case CAM_SCENE_MODE_VIDEO:
      helper->exposure_mode = G_DIGICAM_EXPOSUREMODE_PORTRAIT;
      still_video_helper->mode = G_DIGICAM_MODE_VIDEO;
      break;
    case CAM_SCENE_MODE_NIGHT_VIDEO:
      helper->exposure_mode = G_DIGICAM_EXPOSUREMODE_NIGHT;
      still_video_helper->mode = G_DIGICAM_MODE_VIDEO;
  }

  result = g_digicam_manager_set_mode(camera_interface->manager,
				      still_video_helper->mode,
				      &error,
				      still_video_helper);
  _print_result_message(result, &error, " set scene mode");
  result = g_digicam_manager_set_exposure_mode(camera_interface->manager,
					       helper->exposure_mode,
					       NULL,
					       &error,
					       helper);
  _print_result_message(result, &error, "set exposure mode");
  result = g_digicam_manager_set_focus_mode(camera_interface->manager,
					    focus_helper->focus_mode,
					    focus_helper->macro_enabled,
					    &error,
					    focus_helper);
  _print_result_message(result, &error, "set focus mode");
  g_slice_free(GDigicamCamerabinModeHelper, still_video_helper);
  g_slice_free(GDigicamCamerabinExposureModeHelper, helper);
  g_slice_free(GDigicamCamerabinFocusModeHelper, focus_helper);
  return result;
}

gboolean
camera_interface_set_still_resolution(CameraInterface* camera_interface, CamStillResolution resolution)
{
  GDigicamCamerabinAspectRatioResolutionHelper* helper = NULL;
  GError* error = NULL;
  helper = g_slice_new(GDigicamCamerabinAspectRatioResolutionHelper);
  if(resolution == CAM_STILL_RESOLUTION_LOW)
  {
    helper->resolution = G_DIGICAM_RESOLUTION_LOW;
    helper->aspect_ratio = G_DIGICAM_ASPECTRATIO_4X3;
  }
  else if(resolution == CAM_STILL_RESOLUTION_MEDIUM)
  {
    helper->resolution = G_DIGICAM_RESOLUTION_MEDIUM;
    helper->aspect_ratio = G_DIGICAM_ASPECTRATIO_4X3;
  }
  else if(resolution == CAM_STILL_RESOLUTION_HIGH)
  {
    helper->resolution = G_DIGICAM_RESOLUTION_HIGH;
    helper->aspect_ratio = G_DIGICAM_ASPECTRATIO_4X3;
  }
  else if(resolution == CAM_STILL_RESOLUTION_WIDE)
  {
    helper->resolution = G_DIGICAM_RESOLUTION_HIGH;
    helper->aspect_ratio = G_DIGICAM_ASPECTRATIO_16X9;
  }
  
  gboolean result = g_digicam_manager_set_aspect_ratio_resolution(camera_interface->manager,
								  helper->aspect_ratio,
								  helper->resolution,
								  &error,
								  helper);
  _print_result_message(result, &error, " set still image resolution");
  g_slice_free(GDigicamCamerabinAspectRatioResolutionHelper,
	       helper);
  return result;
}

gboolean
camera_interface_set_video_resolution(CameraInterface* camera_interface, CamVideoResolution resolution)
{
  GDigicamCamerabinAspectRatioResolutionHelper* helper = NULL;
  GError* error = NULL;
  gboolean digital=TRUE;
  helper = g_slice_new(GDigicamCamerabinAspectRatioResolutionHelper);
  if(resolution == CAM_VIDEO_RESOLUTION_LOW)
  {
    helper->resolution = G_DIGICAM_RESOLUTION_LOW;
    helper->aspect_ratio = G_DIGICAM_ASPECTRATIO_4X3;
  }
  else if(resolution == CAM_VIDEO_RESOLUTION_MEDIUM)
  {
    helper->resolution = G_DIGICAM_RESOLUTION_MEDIUM;
    helper->aspect_ratio = G_DIGICAM_ASPECTRATIO_4X3;
  }
  else if(resolution == CAM_VIDEO_RESOLUTION_HIGH)
  {
    helper->resolution = G_DIGICAM_RESOLUTION_HIGH;
    helper->aspect_ratio = G_DIGICAM_ASPECTRATIO_16X9;
  }
  else if(resolution == CAM_VIDEO_RESOLUTION_DVD_4X3)
  {
    helper->resolution = G_DIGICAM_RESOLUTION_DVD;
    helper->aspect_ratio = G_DIGICAM_ASPECTRATIO_4X3;
  }
  else if(resolution == CAM_VIDEO_RESOLUTION_DVD_16X9)
  {
    helper->resolution = G_DIGICAM_RESOLUTION_DVD;
    helper->aspect_ratio = G_DIGICAM_ASPECTRATIO_16X9;
  }
  else if(resolution == CAM_VIDEO_RESOLUTION_HD_4X3)
  {
    helper->resolution = G_DIGICAM_RESOLUTION_HD;
    helper->aspect_ratio = G_DIGICAM_ASPECTRATIO_4X3;
  }
  else if(resolution == CAM_VIDEO_RESOLUTION_HD_16X9)
  {
    helper->resolution = G_DIGICAM_RESOLUTION_HD;
    helper->aspect_ratio = G_DIGICAM_ASPECTRATIO_16X9;
  }
  _set_zoom_level(camera_interface->manager,1.0,&digital);
  g_digicam_manager_stop_bin(camera_interface->manager, &error);
  gboolean result = g_digicam_manager_set_aspect_ratio_resolution(camera_interface->manager,
								  helper->aspect_ratio,
								  helper->resolution,
								  &error,
								  helper);
  if(result && camera_interface->viewfinder_window_id)
    g_digicam_manager_play_bin(camera_interface->manager, camera_interface->viewfinder_window_id, &error);
  _print_result_message(result, &error, " set video resolution ");
  g_slice_free(GDigicamCamerabinAspectRatioResolutionHelper, helper);
  return result;
}

gboolean
camera_interface_set_white_balance_mode(CameraInterface* camera_interface, CamWhiteBalance white_balance)
{
  GDigicamCamerabinWhitebalanceModeHelper *helper = NULL;
  GError *error = NULL;
  gboolean result = FALSE;
  helper = g_slice_new(GDigicamCamerabinWhitebalanceModeHelper);
  switch(white_balance)
  {
  case CAM_WHITE_BALANCE_AUTO :
    helper->wb_mode = G_DIGICAM_WHITEBALANCEMODE_AUTO;
    break;
  case CAM_WHITE_BALANCE_SUNNY :
    helper->wb_mode = G_DIGICAM_WHITEBALANCEMODE_SUNLIGHT;
    break;
  case CAM_WHITE_BALANCE_CLOUDY :
    helper->wb_mode = G_DIGICAM_WHITEBALANCEMODE_CLOUDY;
    break;
  case CAM_WHITE_BALANCE_FLOURESCENT :
    helper->wb_mode = G_DIGICAM_WHITEBALANCEMODE_FLUORESCENT;
    break;
  case CAM_WHITE_BALANCE_INCANDESCENT :
    helper->wb_mode = G_DIGICAM_WHITEBALANCEMODE_INCANDESCENT;
    break;
  }    
  result = g_digicam_manager_set_white_balance_mode(camera_interface->manager,
						    helper->wb_mode,
						    0,
						    &error,
						    helper);
  _print_result_message(result, &error, " set white balance mode ");
  g_slice_free(GDigicamCamerabinWhitebalanceModeHelper, helper);
  return result;
}

gboolean
camera_interface_set_flash_mode(CameraInterface* camera_interface, CamFlashMode flash_mode)
{
  GDigicamCamerabinFlashModeHelper* helper = NULL;
  GError* error = NULL;
  gboolean result = FALSE;
  helper = g_slice_new(GDigicamCamerabinFlashModeHelper);
  switch(flash_mode)
  {
  case CAM_FLASH_MODE_AUTO:
    helper->flash_mode = G_DIGICAM_FLASHMODE_AUTO;
    break;
  case CAM_FLASH_MODE_ON:
    helper->flash_mode = G_DIGICAM_FLASHMODE_ON;
    break;
  case CAM_FLASH_MODE_OFF:
    helper->flash_mode = G_DIGICAM_FLASHMODE_OFF;
    break;
  case CAM_FLASH_MODE_REDEYE:
    helper->flash_mode = G_DIGICAM_FLASHMODE_REDEYEREDUCTION;
    break;
  }
  result = g_digicam_manager_set_flash_mode(camera_interface->manager,
                                            helper->flash_mode,
                                            &error, 
                                            helper);
  _print_result_message(result, &error, " set flash mode");
  g_slice_free(GDigicamCamerabinFlashModeHelper, helper);
  return result;
}

gboolean
camera_interface_set_iso_level(CameraInterface* camera_interface, CamIsoLevel iso_level)
{
  GDigicamCamerabinIsoSensitivityHelper *helper = NULL;
  GError *error = NULL;
  gboolean result = FALSE;

  helper = g_slice_new(GDigicamCamerabinIsoSensitivityHelper);
  switch(iso_level)
  {
  case CAM_ISO_LEVEL_AUTO :
    helper->iso_sensitivity_mode = G_DIGICAM_ISOSENSITIVITYMODE_AUTO;
    helper->iso_value = 0;
    break;
  case CAM_ISO_LEVEL_100:
    helper->iso_sensitivity_mode = G_DIGICAM_ISOSENSITIVITYMODE_MANUAL;    
    helper->iso_value = 100;
    break;
  case CAM_ISO_LEVEL_200:
    helper->iso_sensitivity_mode = G_DIGICAM_ISOSENSITIVITYMODE_MANUAL;    
    helper->iso_value = 200;
    break;
   case CAM_ISO_LEVEL_400:
    helper->iso_sensitivity_mode = G_DIGICAM_ISOSENSITIVITYMODE_MANUAL;    
    helper->iso_value = 400;
    break;
   case CAM_ISO_LEVEL_800:
    helper->iso_sensitivity_mode = G_DIGICAM_ISOSENSITIVITYMODE_MANUAL;    
    helper->iso_value = 800;
    break;
   case CAM_ISO_LEVEL_1600:
    helper->iso_sensitivity_mode = G_DIGICAM_ISOSENSITIVITYMODE_MANUAL;    
    helper->iso_value = 1600;
    break;
  }

  result = g_digicam_manager_set_iso_sensitivity_mode(camera_interface->manager,
						      helper->iso_sensitivity_mode,
						      helper->iso_value,
						      &error,
						      helper);
  _print_result_message(result, &error, "set iso level");
  g_slice_free(GDigicamCamerabinIsoSensitivityHelper, helper);
  return result;
}

gboolean
camera_interface_set_exposure_comp(CameraInterface* camera_interface,  CamExposureComp exposure_level)
{
  GDigicamCamerabinExposureCompHelper *helper = NULL;
  GError *error = NULL;
  gboolean result = FALSE;
  
  /* Create operation helper. */
  gdouble exposure_comp = 0.0;
  switch(exposure_level)
  {
    case CAM_EXPOSURE_COMP_M2:
      exposure_comp = -2;
      break;
    case CAM_EXPOSURE_COMP_M15:
      exposure_comp = -1.5;
      break;
    case CAM_EXPOSURE_COMP_M1:
      exposure_comp = -1;
      break;
    case CAM_EXPOSURE_COMP_M05:
      exposure_comp = -0.5;
      break;
    case CAM_EXPOSURE_COMP_0:
      exposure_comp = 0;
      break;
    case CAM_EXPOSURE_COMP_P05:
      exposure_comp = 0.5;
      break;
    case CAM_EXPOSURE_COMP_P1:
      exposure_comp = 1.0;
      break;
    case CAM_EXPOSURE_COMP_P15:
      exposure_comp = 1.5;
      break;
    case CAM_EXPOSURE_COMP_P2:
      exposure_comp = 2;
      break;
  }
  helper = g_slice_new(GDigicamCamerabinExposureCompHelper);
  helper->exposure_comp = exposure_comp;
  
  result = g_digicam_manager_set_exposure_comp(camera_interface->manager,
					       helper->exposure_comp,
					       &error,
					       helper);
  _print_result_message(result, &error, " set exposure comp");
  g_slice_free(GDigicamCamerabinExposureCompHelper, helper);
  return result;
}


gboolean
camera_interface_set_colour_tone_mode(CameraInterface* camera_interface, GstColourToneMode mode)
{
  GstElement* bin = NULL;
  if(g_digicam_manager_get_gstreamer_bin(camera_interface->manager,
					 &bin,
					 NULL))
  {
    if(mode != GST_PHOTOGRAPHY_COLOUR_TONE_MODE_NORMAL)
    {
      gst_photography_set_colour_tone_mode(GST_PHOTOGRAPHY(bin), GST_PHOTOGRAPHY_COLOUR_TONE_MODE_NORMAL);
    }
    gst_photography_set_colour_tone_mode(GST_PHOTOGRAPHY(bin), mode);
    return TRUE;
  }
  return FALSE;
}

void
camera_interface_set_settings(CameraInterface* camera_interface, CameraSettings* settings)
{
  camera_interface_set_scene_mode(camera_interface, settings->scene_mode);
  camera_interface_set_still_resolution(camera_interface, settings->still_resolution_size);
  camera_interface_set_white_balance_mode(camera_interface, settings->white_balance);
  camera_interface_set_flash_mode(camera_interface, settings->flash_mode);
  camera_interface_set_iso_level(camera_interface, settings->iso_level);
}

gboolean
camera_interface_open_viewfinder(CameraInterface* camera_interface, gulong viewfinder_window_id)
{
  GError* error = NULL;
  camera_interface->viewfinder_window_id = viewfinder_window_id;

  gboolean result = 
    g_digicam_manager_play_bin(camera_interface->manager, viewfinder_window_id, &error);
  {
    GstElement* bin = NULL;
    if(result && g_digicam_manager_get_gstreamer_bin(camera_interface->manager,
					   &bin,
					   NULL))
      {
	GstElement* videosrc = NULL;
	gint device_fd = -1;
	g_object_get(bin, "videosrc", &videosrc, NULL);
	if(videosrc)
	{
	  g_object_get(videosrc, "device-fd", &device_fd, NULL);
	  g_object_set(videosrc, "queue-size", 16, NULL);
	  camera_interface->video_fd = device_fd;
	  gst_object_unref(videosrc);
	}
	else
	  g_debug("Unable to get videoenc element\n");
	gst_object_unref(bin);
	/* restart bin so queue size change to take effect */
	g_digicam_manager_stop_bin(camera_interface->manager, &error);
	g_digicam_manager_play_bin(camera_interface->manager, viewfinder_window_id, &error);
      }
      else
	g_debug("Unable to get camera bin\n");
  }
  _add_tag_listener(camera_interface);
  _print_result_message(result, &error, " open viewfinder ");
  return result;
}

gboolean
camera_interface_close_viewfinder(CameraInterface* camera_interface)
{
  GError* error = NULL;
  _remove_tag_listener((CameraInterface*)camera_interface);
  gboolean result = 
    g_digicam_manager_stop_bin(camera_interface->manager, &error);

  _print_result_message(result, &error, " close view finder ");
  if(result)
  {
    camera_interface->video_fd = -1;
  }
  return result;
}

gboolean
camera_interface_capture_image(CameraInterface* camera_interface, capture_data_t capture_data)
{
  GDigicamCamerabinPictureHelper* helper = NULL;
  GError* error = NULL;
  gboolean result;
  helper = g_slice_new(GDigicamCamerabinPictureHelper);
  helper->file_path = capture_data.filename;
  helper->metadata = g_slice_new(GDigicamCamerabinMetadata);
  helper->metadata->model = g_strdup("N900");
  helper->metadata->make = g_strdup("Nokia");
  helper->metadata->author = g_strdup(capture_data.author);
  helper->metadata->orientation = capture_data.orientation_mode;
  helper->metadata->longitude = capture_data.longitude;
  helper->metadata->latitude = capture_data.latitude;
  helper->metadata->altitude = capture_data.altitude;
  helper->metadata->country_name = g_strdup(capture_data.country);
  helper->metadata->city_name = g_strdup(capture_data.city);
  helper->metadata->suburb_name = g_strdup(capture_data.suburb);
  result = g_digicam_manager_capture_still_picture(camera_interface->manager, helper->file_path, &error, helper);
  _print_result_message(result, &error, "capture still picture");
  g_slice_free(GDigicamCamerabinMetadata,
	       helper->metadata);
  g_slice_free(GDigicamCamerabinPictureHelper,
	       helper);
  return result;
}

void camera_interface_set_priority(CamPriority camera_priority)
{
  if(camera_priority == CAM_PRIORITY_RECORDING)
    system("sudo /usr/bin/camera-ui-set-priority 1");
  else
    system("sudo /usr/bin/camera-ui-set-priority 0");
}

gboolean
camera_interface_start_recording(CameraInterface* camera_interface, capture_data_t capture_data)
{
  GDigicamCamerabinVideoHelper* helper = NULL;
  GError* error = NULL;
  gboolean result;
  camera_interface_set_priority(CAM_PRIORITY_RECORDING);
  helper = g_slice_new(GDigicamCamerabinVideoHelper);
  helper->file_path = capture_data.filename;
  helper->metadata = g_slice_new(GDigicamCamerabinMetadata);
  helper->metadata->model = g_strdup("N900");
  helper->metadata->make = g_strdup("Nokia");
  helper->metadata->author = g_strdup(capture_data.author);
  helper->metadata->unique_id = g_strdup(capture_data.classification_id);
  helper->metadata->orientation = capture_data.orientation_mode;
  helper->metadata->longitude = capture_data.longitude;
  helper->metadata->latitude = capture_data.latitude;
  helper->metadata->altitude = capture_data.altitude;
  helper->metadata->country_name = g_strdup(capture_data.country);
  helper->metadata->city_name = g_strdup(capture_data.city);
  helper->metadata->suburb_name = g_strdup(capture_data.suburb);
  result = g_digicam_manager_start_recording_video(camera_interface->manager,
						   &error, 
						  helper);
  _print_result_message(result, &error, " start recording ");
  g_slice_free(GDigicamCamerabinMetadata,
	       helper->metadata);
  g_slice_free(GDigicamCamerabinVideoHelper, helper);
  return result;
}

gboolean
camera_interface_stop_recording(CameraInterface* camera_interface)
{
  GDigicamCamerabinVideoHelper* helper = NULL;
  GError* error = NULL;
  gboolean result;
  helper = g_slice_new(GDigicamCamerabinVideoHelper);
  result = g_digicam_manager_finish_recording_video(camera_interface->manager,
						    &error, 
						    helper);
  _print_result_message(result, &error, " stop recording ");
  g_slice_free(GDigicamCamerabinVideoHelper, helper);
  camera_interface_set_priority(CAM_PRIORITY_IDLE);
  return result;
}

gboolean
camera_interface_pause_recording(CameraInterface* camera_interface)
{
  GDigicamCamerabinVideoHelper* helper = NULL;
  GError* error = NULL;
  gboolean result;
  helper = g_slice_new(GDigicamCamerabinVideoHelper);
  helper->resume = FALSE;
  result = g_digicam_manager_pause_recording_video(camera_interface->manager,
						   helper->resume,
						   &error, 
						   helper);
  _print_result_message(result, &error, " pause recording ");
  g_slice_free(GDigicamCamerabinVideoHelper, helper);
  return result;
}

gboolean
camera_interface_resume_recording(CameraInterface* camera_interface)
{
  GDigicamCamerabinVideoHelper* helper = NULL;
  GError* error = NULL;
  gboolean result;
  helper = g_slice_new(GDigicamCamerabinVideoHelper);
  helper->resume = TRUE;
  result = g_digicam_manager_pause_recording_video(camera_interface->manager,
						   helper->resume,
						   &error, 
						   helper);
  _print_result_message(result, &error, " resume recording ");
  g_slice_free(GDigicamCamerabinVideoHelper, helper);
  return result;
}

gboolean
camera_interface_enable_preview(CameraInterface* camera_interface, gboolean enable)
{
  GDigicamCamerabinPreviewHelper * helper = NULL;
  GError *error = NULL;
  gboolean result = FALSE;
  helper = g_slice_new(GDigicamCamerabinPreviewHelper);
  if(enable)
    helper->mode = G_DIGICAM_PREVIEW_ON;
  else
    helper->mode = G_DIGICAM_PREVIEW_OFF;

  result = g_digicam_manager_set_preview_mode(camera_interface->manager,
					      helper->mode,
					      &error,
					      helper);
  _print_result_message(result, &error, " change preview mode");

  g_slice_free(GDigicamCamerabinPreviewHelper,
		helper);
}

gdouble
camera_interface_increase_zoom(CameraInterface* camera_interface,gdouble max_zoom)
{
  gdouble zoom = max_zoom;
  gboolean digital = TRUE;
  _get_zoom_level(camera_interface->manager, &zoom, &digital);

  if(max_zoom>zoom)
  {
    zoom += (max_zoom-1.0)/50.0;
    _set_zoom_level(camera_interface->manager, zoom, digital);
  }
  return zoom;
}

gdouble
camera_interface_decrease_zoom(CameraInterface* camera_interface)
{
  gdouble zoom = 1;
  gboolean digital = TRUE;
  _get_zoom_level(camera_interface->manager, &zoom, &digital);
  if(zoom-0.1>=1)
  {
    zoom-=0.1;
    _set_zoom_level(camera_interface->manager, zoom, digital);
  }
  return zoom;
}

gboolean
camera_interface_set_audio_mode(CameraInterface* camera_interface, CamMicMode mic_mode)
{
  GDigicamCamerabinVideoHelper * helper = NULL;
  GError *error = NULL;
  gboolean result = FALSE;

  helper = g_slice_new(GDigicamCamerabinVideoHelper);
  if(mic_mode == CAM_VIDEO_MIC_ON)
    helper->audio = G_DIGICAM_AUDIO_RECORDON;
  else
    helper->audio = G_DIGICAM_AUDIO_RECORDOFF;

  result = g_digicam_manager_set_audio(camera_interface->manager,
				       helper->audio,
				       &error,
				       helper);
  _print_result_message(result, &error, " set audio mode");
  g_slice_free(GDigicamCamerabinVideoHelper,
	       helper);
  return result;
}

gboolean
camera_interface_autofocus(CameraInterface* camera_interface, gboolean start_stop)
{
  GstElement* bin = NULL;
  if(g_digicam_manager_get_gstreamer_bin(camera_interface->manager,
					 &bin,
					 NULL))
  {
    if(start_stop)
      gst_photography_set_autofocus(GST_PHOTOGRAPHY(bin), TRUE);
    else
      gst_photography_set_autofocus(GST_PHOTOGRAPHY(bin), FALSE);
    gst_object_unref(bin);
    return TRUE;
  }
  return FALSE;
}

gboolean
camera_interface_set_focus_mode(CameraInterface* camera_interface, gboolean start_stop)
{
  GDigicamCamerabinFocusModeHelper * helper = NULL;
  GError *error = NULL;
  gboolean result = FALSE;
  helper = g_slice_new(GDigicamCamerabinFocusModeHelper);
  helper->macro_enabled = TRUE;
  helper->focus_mode = G_DIGICAM_FOCUSMODE_AUTO;


  result = g_digicam_manager_set_focus_mode(camera_interface->manager,
					    helper->focus_mode,
					    helper->macro_enabled,
					    &error,
					    helper);
  
  _print_result_message(result, &error, " set focus mode ");
  g_slice_free(GDigicamCamerabinFocusModeHelper,
	       helper);
  return result;
}

gulong
camera_interface_signal_connect(CameraInterface* camera_interface, const gchar* signal, GCallback call_back, gpointer user_data)
{
  return g_signal_connect(camera_interface->manager, signal, call_back, user_data);
}

void
camera_interface_toggle_light(CameraInterface* camera_interface)
{
  if(camera_interface->video_fd != -1)
  {
    struct v4l2_control control;
    memset(&control, 0, sizeof(control));
    control.id = V4L2_CID_TORCH_INTENSITY;
    ioctl(camera_interface->video_fd, VIDIOC_G_CTRL, &control);
    control.value = !control.value;
    ioctl(camera_interface->video_fd, VIDIOC_S_CTRL, &control);
  }
}

void
camera_interface_toggle_privacy_light(CameraInterface* camera_interface)
{
  if(camera_interface->video_fd != -1)
  {
    struct v4l2_control control;
    memset(&control, 0, sizeof(control));
    control.id = V4L2_CID_INDICATOR_INTENSITY;
    ioctl(camera_interface->video_fd, VIDIOC_G_CTRL, &control);
    control.value = !control.value;
    ioctl(camera_interface->video_fd, VIDIOC_S_CTRL, &control);
  }
}

gint
camera_interface_get_focus(CameraInterface* camera_interface)
{
  if(camera_interface->video_fd != -1)
  {
    struct v4l2_control control;
    memset(&control, 0, sizeof(control));
    control.id = V4L2_CID_FOCUS_ABSOLUTE;
    ioctl(camera_interface->video_fd, VIDIOC_G_CTRL, &control);
    return MAX(MIN(control.value, 862), 227);
  }
  return 0;
}


void
camera_interface_increase_focus(CameraInterface* camera_interface, gint value)
{
  if(camera_interface->video_fd != -1)
  {
    struct v4l2_control control;
    memset(&control, 0, sizeof(control));
    control.id = V4L2_CID_FOCUS_ABSOLUTE;
    ioctl(camera_interface->video_fd, VIDIOC_G_CTRL, &control);
    control.value = MIN(control.value +value, 862);
    ioctl(camera_interface->video_fd, VIDIOC_S_CTRL, &control);
  }
}

void
camera_interface_decrease_focus(CameraInterface* camera_interface, gint value)
{
  if(camera_interface->video_fd != -1)
  {
    struct v4l2_control control;
    memset(&control, 0, sizeof(control));
    control.id = V4L2_CID_FOCUS_ABSOLUTE;
    ioctl(camera_interface->video_fd, VIDIOC_G_CTRL, &control);
    control.value = MAX(control.value - value, 227);
    ioctl(camera_interface->video_fd, VIDIOC_S_CTRL, &control);
  }
}

gboolean
camera_interface_frontcamera_mode(CameraInterface* camera_interface)
{
  GstBin* bin = _get_camerabin(camera_interface);
  if(bin)
  {
    GstElement* videosrc = gst_bin_get_by_name(bin, "v4l2camsrc0");
    if(videosrc)
    {
      g_object_set(videosrc, "device", "/dev/video1", NULL);
      gst_object_unref(videosrc);
      GError* error = NULL;
      gboolean result = 
	g_digicam_manager_stop_bin(camera_interface->manager, &error) &&
	g_digicam_manager_play_bin(camera_interface->manager, camera_interface->viewfinder_window_id, &error);
      gst_object_unref(bin);
      return result;
    }
    gst_object_unref(bin);
  }
  return FALSE;
}


gboolean
camera_interface_backcamera_mode(CameraInterface* camera_interface)
{
  GstBin* bin = _get_camerabin(camera_interface);
  if(bin)
  {
    GstElement* videosrc = gst_bin_get_by_name(bin, "v4l2camsrc0");
    if(videosrc)
    {
      g_object_set(videosrc, "device", "/dev/video0", NULL);
      gst_object_unref(videosrc);
      gst_object_unref(bin);
      return 
	g_digicam_manager_stop_bin(camera_interface->manager, NULL) &&
	g_digicam_manager_play_bin(camera_interface->manager, camera_interface->viewfinder_window_id, NULL);
    }
    gst_object_unref(bin);
  }
  return FALSE;
}

void
camera_interface_set_raw_image_mode(CameraInterface* camera_interface, gboolean enable)
{
  if(enable)
  {
    raw_image_buffer = g_new(guint8,(2576*1969*2));
    g_setenv("CAMSRC_PUBLISH_RAW", "1", TRUE);
  }
  else
  {
    g_free(raw_image_buffer);
    raw_image_buffer = NULL;
    g_unsetenv("CAMSRC_PUBLISH_RAW");
  }
}

guint8*
camera_interface_get_raw_data()
{
  if(raw_image_buffer)
    return raw_image_buffer;
  return NULL;
}


void
camera_interface_get_camera_capture_data(CameraInterface* camera_interface, CameraCaptureData** cc)
{
  (*cc) = g_memdup(camera_interface->camera_capture_data,
		   sizeof(CameraCaptureData));
  (*cc)->date_time_stamp = g_strdup(camera_interface->camera_capture_data->date_time_stamp);
}
