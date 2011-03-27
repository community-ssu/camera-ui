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
#ifndef _CAMERA_UI2_PREVIEW_WINDOW_H_
#define _CAMERA_UI2_PREVIEW_WINDOW_H_

#include <hildon/hildon.h>
#include <libosso.h>
#include "camera-settings.h"

G_BEGIN_DECLS

#define CAMERA_UI2_PREVIEW_TYPE_WINDOW camera_ui2_preview_window_get_type()
#define CAMERA_UI2_PREVIEW_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), CAMERA_UI2_PREVIEW_TYPE_WINDOW, CameraUI2PreviewWindow))
#define CAMERA_UI2_PREVIEW_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), CAMERA_UI2_PREVIEW_TYPE_WINDOW, CameraUI2PreviewWindowClass))
#define CAMERA_UI2_PREVIEW_IS_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), CAMERA_UI2_PREVIEW_TYPE_WINDOW))
#define CAMERA_UI2_PREVIEW_IS_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), CAMERA_UI2_PREVIEW_TYPE_WINDOW))
#define CAMERA_UI2_PREVIEW_WINDOW_GET_CLASS(obj) (G_TYPE_CHECK_GET_CLASS((obj), CAMERA_UI2_PREVIEW_TYPE_WINDOW, CameraUI2PreviewWindowClass))

typedef struct _CameraUI2PreviewWindow CameraUI2PreviewWindow;
typedef struct _CameraUI2PreviewWindowClass CameraUI2PreviewWindowClass;
typedef struct _CameraUI2PreviewWindowPrivate CameraUI2PreviewWindowPrivate;


struct _CameraUI2PreviewWindow
{
  HildonStackableWindow parent;
  CameraUI2PreviewWindowPrivate* priv;
};

struct _CameraUI2PreviewWindowClass
{
  HildonStackableWindowClass parent_class;
};

GType camera_ui2_preview_window_get_type(void);

CameraUI2PreviewWindow* camera_ui2_preview_window_new(osso_context_t* osso);

void camera_ui2_preview_window_show_image_preview(CameraUI2PreviewWindow* self,
						  gboolean fullscreen, 
						  CamPreviewMode preview_mode,
						  capture_data_t* capture_data,
						  GdkPixbuf* preview);

void camera_ui2_preview_window_show_video_preview(CameraUI2PreviewWindow* self, 
						  gboolean fullscreen,
						  CamPreviewMode preview_mode,
						  capture_data_t* capture_data,
						  GdkPixbuf* preview);

G_END_DECLS

#endif
