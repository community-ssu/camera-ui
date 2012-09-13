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
#include "geotagging-helper.h"


#include <location/location-gps-device.h>
#include <location/location-gpsd-control.h>
#include <navigation/navigation-provider.h>

struct _GeotaggingHelper
{
  LocationGPSDControl* location_control;
  LocationGPSDevice* location_device;
  FixChangedCallback fix_listener;
  FixChangedCallback fix_lost_listener;
  gpointer fix_listener_data;
  gpointer fix_lost_listener_data;
  LocationGPSDeviceStatus current_status;
  NavigationAddress* address;
  GeoTagMode mode;
};

static void
location_to_address_cb(NavigationProvider *provider, 
		       NavigationAddress  *address,
		       GError             *error,
		       gpointer           userdata)
{
  GeotaggingHelper* helper = (GeotaggingHelper*)userdata;
  if(helper->address)
  {
    navigation_address_free(helper->address);    
    helper->address = NULL;
  }
  g_print("got address\n");
  if(error)
  {
    g_print("error in callback %s\n", error->message);
    g_error_free(error);
    error = NULL;
    if(helper->mode == GEO_TAG_POS_AND_LOC)
      helper->mode = GEO_TAG_POSITION;
    if(helper->mode == GEO_TAG_LOCATION)
      helper->mode = GEO_TAG_NONE;
  }
  else if(address != NULL)
  {
    g_print("address 0x%p\n", address);
    g_print("address %s %s %s\n", address->country, address->town, address->suburb);
    helper->address = address;
  }
}

static void
_request_location_data(GeotaggingHelper* helper)
{
  // request location initally
  GError* error = NULL;
  NavigationLocation location;
  location.latitude = helper->location_device->fix->latitude;
  location.longitude = helper->location_device->fix->longitude;
  NavigationProvider* navigation_provider = navigation_provider_new_default();
  if(!navigation_provider_location_to_address_verbose(navigation_provider,
						      &location,
						      location_to_address_cb,
						      helper,
						      &error))
  {
    g_print("error on location to address\n");
    if(error)
    {
      g_print("provider error %s\n", error->message);
      g_error_free(error);
      error = NULL;
    }
  }
}

static void
on_location_device_changed(LocationGPSDevice *device, gpointer user_data)
{
  GeotaggingHelper* helper = (GeotaggingHelper*)user_data;
  g_print("location device changed\n");
  if (!device)
    return;
  
  if(device->fix) 
  {
    if(device->fix->fields & LOCATION_GPS_DEVICE_LATLONG_SET)
    {
      g_print("lat = %f, long = %f", device->fix->latitude, device->fix->longitude);
    }
    if(device->fix->fields & LOCATION_GPS_DEVICE_ALTITUDE_SET)
      g_print("alt = %f", device->fix->altitude);
    
    g_print("horizontal accuracy: %f meters", device->fix->eph/100);
  }
  if(helper->current_status != device->status )
  {
    helper->current_status = device->status;
    if(device->status == LOCATION_GPS_DEVICE_STATUS_FIX)
    {
      if(helper->mode == GEO_TAG_LOCATION ||
	 helper->mode == GEO_TAG_POS_AND_LOC)
      {
	_request_location_data(helper);
      }
      if(helper->fix_listener)
	helper->fix_listener(helper->fix_listener_data);
    }
    else
    {
      if(helper->fix_lost_listener)
	helper->fix_lost_listener(helper->fix_lost_listener_data);
    }
  }
}

static void on_error(LocationGPSDControl *control, LocationGPSDControlError error, gpointer user_data)
{ 
  switch (error) {
  case LOCATION_ERROR_USER_REJECTED_DIALOG:
    g_print("User didn't enable requested methods");
    break;
  case LOCATION_ERROR_USER_REJECTED_SETTINGS:
    g_print("User changed settings, which disabled location");
    break;
  case LOCATION_ERROR_BT_GPS_NOT_AVAILABLE:
    g_print("Problems with BT GPS");
    break;
  case LOCATION_ERROR_METHOD_NOT_ALLOWED_IN_OFFLINE_MODE:
    g_print("Requested method is not allowed in offline mode");
    break;
  case LOCATION_ERROR_SYSTEM:
    g_print("System error");
    break;
  }
}

GeotaggingHelper* 
geotagging_helper_create()
{
  GeotaggingHelper* helper = g_new0(GeotaggingHelper, 1);
  helper->mode = GEO_TAG_NONE;
  helper->address = NULL;
  helper->location_control = location_gpsd_control_get_default();
  helper->location_device = g_object_new(LOCATION_TYPE_GPS_DEVICE, NULL);
  helper->current_status = helper->location_device->status;
  g_object_set(G_OBJECT(helper->location_control), "preferred-method", LOCATION_METHOD_USER_SELECTED, NULL);
  g_object_set(G_OBJECT(helper->location_control), "preferred-interval", LOCATION_INTERVAL_20S, NULL);
  g_signal_connect(helper->location_device,  "changed", G_CALLBACK(on_location_device_changed), helper);
  g_signal_connect(helper->location_control, "error-verbose", G_CALLBACK(on_error), NULL);
  return helper;
}

void
geotagging_helper_run(GeotaggingHelper* geotagging_helper)
{
  g_print("run\n");
  if(geotagging_helper->location_device)
  {
    location_gpsd_control_start(geotagging_helper->location_control);
  g_print("run done\n");
  }
}


void
geotagging_helper_stop(GeotaggingHelper* geotagging_helper)
{
  if(geotagging_helper->location_device)
  {
    location_gpsd_control_stop(geotagging_helper->location_control);
  }
}

void
geotagging_helper_register_fix_listener(GeotaggingHelper* geotagging_helper, FixChangedCallback call_back, gpointer user_data)
{
  geotagging_helper->fix_listener = call_back;
  geotagging_helper->fix_listener_data = user_data;
}

void
geotagging_helper_register_fix_lost_listener(GeotaggingHelper* geotagging_helper, FixChangedCallback call_back, gpointer user_data)
{
  geotagging_helper->fix_lost_listener = call_back;
  geotagging_helper->fix_lost_listener_data = user_data;
}

void
geotagging_helper_fill_tagging_data(GeotaggingHelper* geotagging_helper,
				    gdouble* longitude,
				    gdouble* latitude,
				    gdouble* altitude,
				    gchar** country,
				    gchar** city,
				    gchar** suburb)
{
  if(geotagging_helper->mode == GEO_TAG_NONE)
    return;
  if(!geotagging_helper->location_device  ||
     geotagging_helper->location_device->status != LOCATION_GPS_DEVICE_STATUS_FIX)
    return;

  if(geotagging_helper->mode == GEO_TAG_POSITION ||
     geotagging_helper->mode == GEO_TAG_POS_AND_LOC)
  {
    if(geotagging_helper->location_device->fix->fields & LOCATION_GPS_DEVICE_LATLONG_SET)
    {
      (*latitude) = geotagging_helper->location_device->fix->latitude;
      (*longitude) = geotagging_helper->location_device->fix->longitude;
    }
    if(geotagging_helper->location_device->fix->fields & LOCATION_GPS_DEVICE_ALTITUDE_SET)
    {
      (*altitude) = geotagging_helper->location_device->fix->altitude;
    }
  }
  if(geotagging_helper->mode == GEO_TAG_LOCATION ||
     geotagging_helper->mode == GEO_TAG_POS_AND_LOC)
  {
    if(geotagging_helper->address)
    {
      (*country) = g_strdup(geotagging_helper->address->country);
      (*city) = g_strdup(geotagging_helper->address->town);
      (*suburb) = g_strdup(geotagging_helper->address->suburb);
    }
  }
}

GeoTagMode
geotagging_helper_get_mode(GeotaggingHelper* geotagging_helper)
{
  return geotagging_helper->mode;
}

void
geotagging_helper_set_mode(GeotaggingHelper* geotagging_helper, GeoTagMode mode)
{
  geotagging_helper->mode = mode;
  if(mode == GEO_TAG_LOCATION ||
     mode == GEO_TAG_POS_AND_LOC)
  {
    if(geotagging_helper->location_device  &&
       geotagging_helper->location_device->status == LOCATION_GPS_DEVICE_STATUS_FIX)
    {
      _request_location_data(geotagging_helper);
    }
  }
}
