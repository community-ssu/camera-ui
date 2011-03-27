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
#ifndef _STORAGE_HELPER_H_
#define _STORAGE_HELPER_H_

#include "camera-settings.h"
#include <libgnomevfs/gnome-vfs.h>

/*
  create a filename based on the date and the
  last media id from /apps/camera/settings-basic/settings
  The path is choosen based on the storage mode 
  (internal / external memory card)
 */
gchar*
storage_helper_create_filename(CamStorageDevice storage,
			       CamSceneMode scene_mode);

/*
  create a 90 digit hex key.
  The algorithm:
  - create a 50 digit random hex key (R1)
  - concat R1 with the 50 digit device-id 
    (gconf /apps/camera/settings/basic-settings/device-id)
  - create a 40 digit sha1 hash key of this 100 digit string (S1)
  unique-id is then the concatenation of S1 and R1.
 */
gchar*
storage_helper_create_unique_id();

/*
  Find out how much free space is on the memory card
  (internal / external based on the storage mode)
 */
GnomeVFSFileSize
storage_helper_free_space(CamStorageDevice storage);

/*
  name of memorycard
 */
gchar*
storage_helper_get_mmc_name();

#endif
