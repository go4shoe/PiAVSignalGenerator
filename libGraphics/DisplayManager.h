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
//       $File: //depot/pi/libGraphics/DispMan.h $
//
//   $Revision: #2 $
//
//       $Date: 2017/09/25 $
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DISPLAY_MANAGER_H__
#define __DISPLAY_MANAGER_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include "EGL/egl.h"
#include "GLES/gl.h"

///////////////////////////////////////////////////////////////////////////////
class DisplayManager
///////////////////////////////////////////////////////////////////////////////
{
public:
   DisplayManager();

   bool Init();
   bool Deinit();
   bool UpdateResolution();

   uint32_t GetWidth() const;

   uint32_t GetHeight() const;

   EGL_DISPMANX_WINDOW_T *GetNativeWindow();

private:

   //
   // Graphics state
   //
   uint32_t m_nWidth;
   uint32_t m_nHeight;

   //
   // Disp Max objects
   //
   DISPMANX_ELEMENT_HANDLE_T m_dispmanElement;
   DISPMANX_DISPLAY_HANDLE_T m_dispmanDisplay;
   EGL_DISPMANX_WINDOW_T     m_nativeWindow;
};

#endif // __DISP_MAN_H__
