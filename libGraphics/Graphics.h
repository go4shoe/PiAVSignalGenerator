///////////////////////////////////////////////////////////////////////////////
//
// This file is part of "Pi AV Signal Generator".
//
// "Pi AV Signal Generator" is free software : you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// "Pi AV Signal Generator" is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with "Pi AV Signal Generator". If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2009-2017 Markus P Schumann (go4shoe@hotmail.com)
//
///////////////////////////////////////////////////////////////////////////////
//
//      mailto:go4shoe@hotmail.com
//
//     $Author: Markus $
//
//       $File: //depot/pi/libGraphics/Graphics.h $
//
//   $Revision: #12 $
//
//       $Date: 2017/09/30 $
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

///////////////////////////////////////////////////////////////////////////////
//
// OpenVG wrapper
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include "EGL/egl.h"
#include "GLES/gl.h"

///////////////////////////////////////////////////////////////////////////////
// forward
///////////////////////////////////////////////////////////////////////////////
class DisplayManager;

///////////////////////////////////////////////////////////////////////////////
class Graphics
///////////////////////////////////////////////////////////////////////////////
{
public:
   Graphics();
   ~Graphics();
   
   bool Init
   (
      DisplayManager *pDispMan
   );

   bool Deinit();

   uint32_t GetWidth() const;

   uint32_t GetHeight() const;

   //
   // setfill sets the fill color
   //
   static void Setfill
   (
      float color[4]
   );

   //
   // setstroke sets the stroke color and width
   //
   static void Setstroke
   (
      float color[4],
      float width
   );

   //
   // render text
   //
   void RenderText
   (
      VGfloat      xPos,
      VGfloat      yPos,
      const char * sText,
      int          ptSize,
      const char * sFont,
      float        fill[4] 
   );

   //
   // render text right aligned
   //
   void RenderTextRight
   (
      VGfloat      xPos,
      VGfloat      yPos,
      const char * sText,
      int          ptSize,
      const char * sFont,
      float        fill[4] 
   );

   //
   // render text centered
   //
   void RenderTextCentered
   (
      VGfloat      xPos,
      VGfloat      yPos,
      const char * sText,
      int          ptSize,
      const char * sFont,
      float        fill[4] 
   );

   //
   // rect makes a rectangle  at the specified location and dimensions, applying style
   //
   static void Rect
   (
      float x, 
      float y, 
      float w, 
      float h, 
      float sw, 
      float fill[4], 
      float stroke[4]
   );

   //
   // Line makes a line, applying style
   //
   static void Line
   (
      float x1,
      float y1,
      float x2,
      float y2,
      float sw,
      float stroke[4]
   );

   //
   // Roundrect makes an rounded rectangle at the specified location and dimensions, applying style
   //
   static void Roundrect
   (
      float x,
      float y,
      float w,
      float h,
      float rw,
      float rh,
      float sw,
      float fill[4],
      float stroke[4]
   );

   //
   // Ellipse makes an ellipse at the specified location and dimensions, applying style
   //
   void Ellipse
   (
      float x,
      float y,
      float w,
      float h,
      float sw,
      float fill[4],
      float stroke[4]
   );

   //
   // Arc makes an arc at teh specified location and dimensions, applying style
   //
   void Arc
   (
      float x,
      float y,
      float w,
      float h,
      float startAngle,
      float extentAngle,
      float sw,
      float fill[4],
      float stroke[4]
   );

   //
   // Start begins the picture, clearing a rectangluar region with a specified color
   //
   void Start();
   
   //
   // End checks for errors, and renders to the display
   //
   void End();

   //
   // randcolor returns a fraction of 255
   //
   static VGfloat Randcolor();

   //
   // randf returns a floating point number bounded by n
   //
   static VGfloat Randf
   (
      int n
   );

   //
   // RandomShapes draws shapes (rect and ellipse) with random colors, strokes, and sizes.
   //
   void RandomShapes
   (
      int n
   );

private:
   DisplayManager * m_pDispMan;

   //
   // OpenGL ES objects
   //
   EGLDisplay m_eglDisplay;
   EGLSurface m_eglSurface;
   EGLContext m_eglContext;

   
};

#endif // __GRAPHICS_H__
