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
#ifndef _CAMERA_UI2_WINDOW_H_
#define _CAMERA_UI2_WINDOW_H_

#include <hildon/hildon.h>
#include <libosso.h>

G_BEGIN_DECLS

#define CAMERA_UI2_TYPE_WINDOW camera_ui2_window_get_type()
#define CAMERA_UI2_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), CAMERA_UI2_TYPE_WINDOW, CameraUI2Window))
#define CAMERA_UI2_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), CAMERA_UI2_TYPE_WINDOW, CameraUI2WindowClass))
#define CAMERA_UI2_IS_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), CAMERA_UI2_TYPE_WINDOW))
#define CAMERA_UI2_IS_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), CAMERA_UI2_TYPE_WINDOW))
#define CAMERA_UI2_WINDOW_GET_CLASS(obj) (G_TYPE_CHECK_GET_CLASS((obj), CAMERA_UI2_TYPE_WINDOW, CameraUI2WindowClass))

typedef struct _CameraUI2Window CameraUI2Window;
typedef struct _CameraUI2WindowClass CameraUI2WindowClass;
typedef struct _CameraUI2WindowPrivate CameraUI2WindowPrivate;

struct _CameraUI2Window
{
  HildonStackableWindow parent;
  CameraUI2WindowPrivate* priv;
};

struct _CameraUI2WindowClass
{
  HildonStackableWindowClass parent_class;
};

GType camera_ui2_window_get_type(void);

CameraUI2Window* camera_ui2_window_new(osso_context_t* osso, HildonProgram* program);
void camera_ui2_window_show_ui(CameraUI2Window* self);
void camera_ui2_window_hide_ui(CameraUI2Window* self);
void camera_ui2_window_top_application(CameraUI2Window* self);
void camera_ui2_window_lenscover_closed(CameraUI2Window* self);
void camera_ui2_window_lenscover_opened(CameraUI2Window* self);
void camera_ui2_window_focus_button_pressed(CameraUI2Window* self);
void camera_ui2_window_focus_button_released(CameraUI2Window* self);
void camera_ui2_window_capture_button_pressed(CameraUI2Window* self);
void camera_ui2_window_capture_button_released(CameraUI2Window* self);

G_END_DECLS

#endif
