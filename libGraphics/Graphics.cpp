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
//       $File: //depot/pi/libGraphics/Graphics.cpp $
//
//   $Revision: #13 $
//
//       $Date: 2017/09/30 $
//
///////////////////////////////////////////////////////////////////////////////

#include "Graphics.h"
#include "DisplayManager.h"
#include "libFont/FontEntry.h"

#include <bcm_host.h>

///////////////////////////////////////////////////////////////////////////////
Graphics::Graphics()
///////////////////////////////////////////////////////////////////////////////
{
   //
   // OpenGL ES objects
   //
   m_eglDisplay = EGL_NO_DISPLAY;
   m_eglSurface = NULL;
   m_eglContext = NULL;

   m_pDispMan = NULL;

}

///////////////////////////////////////////////////////////////////////////////
// exit_func cleans up
///////////////////////////////////////////////////////////////////////////////
Graphics::~Graphics()
///////////////////////////////////////////////////////////////////////////////
{
   Deinit();
}

///////////////////////////////////////////////////////////////////////////////
// init() sets the display, OpenGL|ES context and screen information
// state holds the OGLES model information
///////////////////////////////////////////////////////////////////////////////
bool Graphics::Init
///////////////////////////////////////////////////////////////////////////////
(
   DisplayManager * pDispMan
)
///////////////////////////////////////////////////////////////////////////////
{
   bool bResult = true;

   m_pDispMan = pDispMan;

   EGLBoolean result = EGL_FALSE;
   EGLint num_config = 0;

   static const EGLint attribute_list[] =
   {
      EGL_RED_SIZE,     8,
      EGL_GREEN_SIZE,   8,
      EGL_BLUE_SIZE,    8,
      EGL_ALPHA_SIZE,   8,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_NONE
   };

   EGLConfig config;

   //
   // get an EGL display connection
   //
   m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   assert(m_eglDisplay!=EGL_NO_DISPLAY);

   //
   // initialize the EGL display connection
   //
   result = eglInitialize(m_eglDisplay, NULL, NULL);
   assert(EGL_FALSE != result);

   //
   // bind OpenVG API
   //
   eglBindAPI(EGL_OPENVG_API);

   //
   // get an appropriate EGL frame buffer configuration
   //
   result = eglChooseConfig(m_eglDisplay, attribute_list, &config, 1, &num_config);
   assert(EGL_FALSE != result);

   //
   // create an EGL rendering context
   //
   m_eglContext = eglCreateContext(m_eglDisplay, config, EGL_NO_CONTEXT, NULL);
   assert(m_eglContext!=EGL_NO_CONTEXT);

   //
   // create an EGL window surface
   //
   
   m_eglSurface = eglCreateWindowSurface( m_eglDisplay, config, m_pDispMan->GetNativeWindow(), NULL );
   assert(m_eglSurface != EGL_NO_SURFACE);

   //
   // connect the context to the surface
   //
   result = eglMakeCurrent(   m_eglDisplay,
                              m_eglSurface,
                              m_eglSurface,
                              m_eglContext);

   assert(EGL_FALSE != result);

   //
   // set up screen ratio
   //
   glViewport( 0,
               0,
               (GLsizei)m_pDispMan->GetWidth(),
               (GLsizei)m_pDispMan->GetHeight());

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   float ratio = (float)m_pDispMan->GetWidth() / (float)m_pDispMan->GetHeight();

   glFrustumf( -ratio,
               ratio,
               -1.0f,
               1.0f,
               1.0f,
               10.0f);

   FontEntry::Init();

   return bResult;
}

///////////////////////////////////////////////////////////////////////////////
bool Graphics::Deinit()
///////////////////////////////////////////////////////////////////////////////
{
   bool bResult = true;

   FontEntry::Deinit();

   //
   // clear screen
   //
   glClear( GL_COLOR_BUFFER_BIT );
   eglSwapBuffers(m_eglDisplay, m_eglSurface);

   //
   // Release OpenGL resources
   //
   eglMakeCurrent( m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
   
   eglDestroySurface( m_eglDisplay, m_eglSurface );
   m_eglSurface = NULL;
   
   eglDestroyContext( m_eglDisplay, m_eglContext );
   m_eglContext = NULL;

   eglTerminate( m_eglDisplay );
   m_eglDisplay = NULL;

   

   return bResult;
}

///////////////////////////////////////////////////////////////////////////////
uint32_t Graphics::GetWidth() const
///////////////////////////////////////////////////////////////////////////////
{
   return m_pDispMan->GetWidth();
}

///////////////////////////////////////////////////////////////////////////////
uint32_t Graphics::GetHeight() const
///////////////////////////////////////////////////////////////////////////////
{
   return m_pDispMan->GetHeight();
}


///////////////////////////////////////////////////////////////////////////////
// setfill sets the fill color
///////////////////////////////////////////////////////////////////////////////
void Graphics::Setfill
///////////////////////////////////////////////////////////////////////////////
(
   float color[4]
)
///////////////////////////////////////////////////////////////////////////////
{
    VGPaint fillPaint = vgCreatePaint();
    vgSetParameteri(fillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
    vgSetParameterfv(fillPaint, VG_PAINT_COLOR, 4, color);
    vgSetPaint(fillPaint, VG_FILL_PATH);
    vgDestroyPaint(fillPaint);
}

///////////////////////////////////////////////////////////////////////////////
// setstroke sets the stroke color and width
///////////////////////////////////////////////////////////////////////////////
void Graphics::Setstroke
///////////////////////////////////////////////////////////////////////////////
(
   float color[4],
   float width
)
///////////////////////////////////////////////////////////////////////////////
{
    VGPaint strokePaint = vgCreatePaint();
    vgSetParameteri(strokePaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
    vgSetParameterfv(strokePaint, VG_PAINT_COLOR, 4, color);
    vgSetPaint(strokePaint, VG_STROKE_PATH);
    vgSetf(VG_STROKE_LINE_WIDTH, width);
    vgSeti(VG_STROKE_CAP_STYLE, VG_CAP_BUTT);
    vgSeti(VG_STROKE_JOIN_STYLE, VG_JOIN_MITER);
    vgDestroyPaint(strokePaint);
}

///////////////////////////////////////////////////////////////////////////////
void Graphics::RenderText
///////////////////////////////////////////////////////////////////////////////
(
   float        xPos,
   float        yPos,
   const char * sText,
   int          ptSize,
   const char * sFont,
   float        fill[4]
)
///////////////////////////////////////////////////////////////////////////////
{
   FontEntry *pFontEntry = FontEntry::Get(sFont);

   Setfill(fill);

   if(pFontEntry)
   {
      pFontEntry->RenderText( xPos,
                              yPos,
                              sText,
                              ptSize);
   }
}

///////////////////////////////////////////////////////////////////////////////
void Graphics::RenderTextRight
///////////////////////////////////////////////////////////////////////////////
(
   float        xPos,
   float        yPos,
   const char * sText,
   int          ptSize,
   const char * sFont,
   float        fill[4]
)
///////////////////////////////////////////////////////////////////////////////
{
   FontEntry *pFontEntry = FontEntry::Get(sFont);

   Setfill(fill);

   if(pFontEntry)
   {
      pFontEntry->RenderTextRight( xPos,
                                   yPos,
                                   sText,
                                   ptSize);
   }
}

///////////////////////////////////////////////////////////////////////////////
void Graphics::RenderTextCentered
///////////////////////////////////////////////////////////////////////////////
(
   float        xPos,
   float        yPos,
   const char * sText,
   int          ptSize,
   const char * sFont,
   float        fill[4]
)
///////////////////////////////////////////////////////////////////////////////
{
   FontEntry *pFontEntry = FontEntry::Get(sFont);

   Setfill(fill);

   if(pFontEntry)
   {
      pFontEntry->RenderTextCentered( xPos,
                                      yPos,
                                      sText,
                                      ptSize);
   }
}

///////////////////////////////////////////////////////////////////////////////
// rect makes a rectangle  at the specified location and dimensions, applying style
///////////////////////////////////////////////////////////////////////////////
void Graphics::Rect
///////////////////////////////////////////////////////////////////////////////
(
   float x, 
   float y, 
   float w, 
   float h, 
   float sw, 
   float fill[4], 
   float stroke[4]
)
///////////////////////////////////////////////////////////////////////////////
{
    VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
    vguRect(path, x, y, w, h);
    Setfill(fill);
    Setstroke(stroke, sw);
    vgDrawPath(path, VG_FILL_PATH | VG_STROKE_PATH);
    vgDestroyPath(path);
}

///////////////////////////////////////////////////////////////////////////////
// Line makes a line, applying style
///////////////////////////////////////////////////////////////////////////////
void Graphics::Line
///////////////////////////////////////////////////////////////////////////////
(
   float x1,
   float y1,
   float x2,
   float y2,
   float sw,
   float stroke[4]
)
///////////////////////////////////////////////////////////////////////////////
{
   VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
   vguLine(path, x1, y1, x2, y2);
   Setstroke(stroke, sw);
   vgDrawPath(path, VG_STROKE_PATH);
   vgDestroyPath(path);
}

///////////////////////////////////////////////////////////////////////////////
// Roundrect makes an rounded rectangle at the specified location and dimensions, applying style
///////////////////////////////////////////////////////////////////////////////
void Graphics::Roundrect
///////////////////////////////////////////////////////////////////////////////
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
)
///////////////////////////////////////////////////////////////////////////////
{
    VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
    vguRoundRect(path, x, y, w, h, rw, rh);
    Setfill(fill);
    Setstroke(stroke, sw);
    vgDrawPath(path, VG_FILL_PATH | VG_STROKE_PATH);
    vgDestroyPath(path);
}

///////////////////////////////////////////////////////////////////////////////
// Ellipse makes an ellipse at the specified location and dimensions, applying style
///////////////////////////////////////////////////////////////////////////////
void Graphics::Ellipse
///////////////////////////////////////////////////////////////////////////////
(
   float x,
   float y,
   float w,
   float h,
   float sw,
   float fill[4],
   float stroke[4]
)
///////////////////////////////////////////////////////////////////////////////
{
    VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
    vguEllipse(path, x, y, w, h);
    Setfill(fill);
    Setstroke(stroke, sw);
    vgDrawPath(path, VG_FILL_PATH | VG_STROKE_PATH);
    vgDestroyPath(path);
}

///////////////////////////////////////////////////////////////////////////////
// Arc makes an ellipse at the specified location and dimensions, applying style
///////////////////////////////////////////////////////////////////////////////
void Graphics::Arc
///////////////////////////////////////////////////////////////////////////////
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
)
///////////////////////////////////////////////////////////////////////////////
{
    VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
    vguArc(path, x, y, w, h, startAngle, extentAngle,VGU_ARC_PIE);
    Setfill(fill);
    Setstroke(stroke, sw);
    vgDrawPath(path, VG_FILL_PATH | VG_STROKE_PATH);
    vgDestroyPath(path);
}

///////////////////////////////////////////////////////////////////////////////
// Start begins the picture, clearing a rectangluar region with a specified color
///////////////////////////////////////////////////////////////////////////////
void Graphics::Start()
///////////////////////////////////////////////////////////////////////////////
{
   VGfloat colorClear[4] = { 255, 255, 255, 1 };

   vgSetfv( VG_CLEAR_COLOR,
            4,
            colorClear);

   vgClear(0, 0, m_pDispMan->GetWidth(), m_pDispMan->GetHeight());

   VGfloat colorFillAndStroke[4] = { 0, 0, 0, 1 };

   Setfill(colorFillAndStroke);
   Setstroke(colorFillAndStroke,0);
	
   vgLoadIdentity();
}

///////////////////////////////////////////////////////////////////////////////
// End checks for errors, and renders to the display
///////////////////////////////////////////////////////////////////////////////
void Graphics::End()
///////////////////////////////////////////////////////////////////////////////
{
    assert(vgGetError() == VG_NO_ERROR);
    eglSwapInterval(m_eglDisplay,1);
    eglSwapBuffers(m_eglDisplay, m_eglSurface);
    assert(eglGetError() == EGL_SUCCESS);
}

///////////////////////////////////////////////////////////////////////////////
// randcolor returns a fraction of 255
///////////////////////////////////////////////////////////////////////////////
VGfloat Graphics::Randcolor()
///////////////////////////////////////////////////////////////////////////////
{
   return (VGfloat)(rand() % 256) / 255.0;
}

///////////////////////////////////////////////////////////////////////////////
// randf returns a floating point number bounded by n
///////////////////////////////////////////////////////////////////////////////
VGfloat Graphics::Randf
///////////////////////////////////////////////////////////////////////////////
(
   int n
)
///////////////////////////////////////////////////////////////////////////////
{
   return (VGfloat)(rand() % n);
}

///////////////////////////////////////////////////////////////////////////////
// RandomShapes draws shapes (rect and ellipse) with random colors, strokes, and sizes.
///////////////////////////////////////////////////////////////////////////////
void Graphics::RandomShapes
///////////////////////////////////////////////////////////////////////////////
(
   int n
)
///////////////////////////////////////////////////////////////////////////////
{
    float rcolor[4], scolor[4];
    scolor[3] = 1;                                // strokes are solid
    srand ( time(NULL) );
    Start();

    for (int i=0; i < n; i++)
    {
        rcolor[0] = Randcolor();
        rcolor[1] = Randcolor();
        rcolor[2] = Randcolor();
        rcolor[3] = Randcolor();

        scolor[1] = Randcolor();
        scolor[2] = Randcolor();
        scolor[3] = Randcolor();

        if (i%2 == 0)
        {
            Ellipse(Randf(m_pDispMan->GetWidth()), Randf(m_pDispMan->GetHeight()), Randf(200), Randf(100), Randf(10), rcolor, scolor);
        }
        else
        {
            Rect(Randf(m_pDispMan->GetWidth()), Randf(m_pDispMan->GetHeight()), Randf(200), Randf(100), Randf(10), rcolor, scolor);
        }
    }
    End();
}
