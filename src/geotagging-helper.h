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
#ifndef _GEOTAGGING_HELPER_H_
#define _GEOTAGGING_HELPER_H_

#include <glib.h>
#include <glib-object.h>

typedef enum {
  GEO_TAG_NONE = 0,
  GEO_TAG_POSITION = 1,
  GEO_TAG_LOCATION = 2,
  GEO_TAG_POS_AND_LOC = 3,
} GeoTagMode;

typedef struct _GeotaggingSettings
{
  gboolean tag_position;
  gboolean tag_location;
} GeotaggingSettings;

typedef struct _GeotaggingHelper GeotaggingHelper;
typedef void(*FixChangedCallback)(gpointer user_data);

GeotaggingHelper* 
geotagging_helper_create();

void
geotagging_helper_run(GeotaggingHelper* geotagging_helper);

void
geotagging_helper_stop(GeotaggingHelper* geotagging_helper);

void
geotagging_helper_set_mode(GeotaggingHelper* geotagging_helper, GeoTagMode tag_mode);

GeoTagMode
geotagging_helper_get_mode(GeotaggingHelper* geotagging_helper);

void
geotagging_helper_register_fix_listener(GeotaggingHelper* geotagging_helper, FixChangedCallback call_back, gpointer user_data);

void
geotagging_helper_register_fix_lost_listener(GeotaggingHelper* geotagging_helper, FixChangedCallback call_back, gpointer user_data);

void
geotagging_helper_fill_tagging_data(GeotaggingHelper* geotagging_helper,
				    gdouble* longitude,
				    gdouble* latitude,
				    gdouble* altitude,
				    gchar** country,
				    gchar** city,
				    gchar** suburb);

#endif
