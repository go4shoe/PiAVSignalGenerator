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
//       $File: //depot/pi/libGraphics/DispMan.cpp $
//
//   $Revision: #2 $
//
//       $Date: 2017/09/25 $
//
///////////////////////////////////////////////////////////////////////////////

#include "DisplayManager.h"
#include <bcm_host.h>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
DisplayManager::DisplayManager()
///////////////////////////////////////////////////////////////////////////////
{
   m_dispmanElement = DISPMANX_NO_HANDLE;
   m_dispmanDisplay = DISPMANX_NO_HANDLE;

   m_nativeWindow.element = DISPMANX_NO_HANDLE;
   m_nativeWindow.width   = 0;
   m_nativeWindow.height  = 0;

   m_nWidth  = 0;
   m_nHeight = 0;
}

///////////////////////////////////////////////////////////////////////////////
bool DisplayManager::Init()
///////////////////////////////////////////////////////////////////////////////
{
   bool bResult = true;

   VC_RECT_T rectDestination;
   VC_RECT_T rectSource;

   int success = graphics_get_display_size(0 /* LCD */, &m_nWidth, &m_nHeight);

   bResult = success == 0;

   rectDestination.x = 0;
   rectDestination.y = 0;

   rectDestination.width  = m_nWidth;
   rectDestination.height = m_nHeight;
   
   rectSource.x = 0;
   rectSource.y = 0;
   rectSource.width  = m_nWidth << 16;
   rectSource.height = m_nHeight << 16;


   m_dispmanDisplay = vc_dispmanx_display_open( 0 ); // 0 => LCD

   DISPMANX_UPDATE_HANDLE_T dispmanUpdate  = vc_dispmanx_update_start( 0 );

   m_dispmanElement = vc_dispmanx_element_add(  dispmanUpdate,
                                                m_dispmanDisplay,
                                                0, // layer
                                              & rectDestination,
                                                0, // src
                                              & rectSource,
                                                DISPMANX_PROTECTION_NONE,
                                                0 /*alpha*/,
                                                0 /*clamp*/,
                                                (DISPMANX_TRANSFORM_T) 0 /*transform*/);

   vc_dispmanx_update_submit_sync( dispmanUpdate );
   
   m_nativeWindow.element = m_dispmanElement;
   m_nativeWindow.width   = m_nWidth;
   m_nativeWindow.height  = m_nHeight;
   
   return bResult;
}

///////////////////////////////////////////////////////////////////////////////
bool DisplayManager::UpdateResolution()
///////////////////////////////////////////////////////////////////////////////
{
   bool bResult = true;

   VC_RECT_T rectDestination;
   VC_RECT_T rectSource;

   int success = graphics_get_display_size(0 /* LCD */, &m_nWidth, &m_nHeight);

   assert(success == 0);

   bResult = success == 0;

   std::cerr << "DispMan::UpdateResolution : diplay size: " << m_nWidth << "x" << m_nHeight << std::endl;

   rectDestination.x = 0;
   rectDestination.y = 0;

   rectDestination.width  = m_nWidth;
   rectDestination.height = m_nHeight;
   
   rectSource.x = 0;
   rectSource.y = 0;
   rectSource.width  = m_nWidth << 16;
   rectSource.height = m_nHeight << 16;
   
   DISPMANX_UPDATE_HANDLE_T dispmanUpdate  = vc_dispmanx_update_start( 0 );

   success = vc_dispmanx_element_change_attributes(   dispmanUpdate,
                                                      m_dispmanElement,
                                                      0,
                                                      0,
                                                      0,
                                                      &rectDestination,
                                                      &rectSource,
                                                      0,
                                                      DISPMANX_NO_ROTATE);

   assert(success == 0);

   bResult = success == 0;

   vc_dispmanx_update_submit_sync( dispmanUpdate );
   
   m_nativeWindow.width  = m_nWidth;
   m_nativeWindow.height = m_nHeight;

   return bResult;
}


///////////////////////////////////////////////////////////////////////////////
uint32_t DisplayManager::GetWidth() const
///////////////////////////////////////////////////////////////////////////////
{
   return m_nWidth;
}

///////////////////////////////////////////////////////////////////////////////
uint32_t DisplayManager::GetHeight() const
///////////////////////////////////////////////////////////////////////////////
{
   return m_nHeight;
}

///////////////////////////////////////////////////////////////////////////////
EGL_DISPMANX_WINDOW_T * DisplayManager::GetNativeWindow()
///////////////////////////////////////////////////////////////////////////////
{
   return & m_nativeWindow;
}

///////////////////////////////////////////////////////////////////////////////
bool DisplayManager::Deinit()
///////////////////////////////////////////////////////////////////////////////
{
   bool bResult = true;

   m_nativeWindow.element = DISPMANX_NO_HANDLE;
   m_nativeWindow.width   = 0;
   m_nativeWindow.height  = 0;
   
   //
   // destroy Disp Max window
   //
   DISPMANX_UPDATE_HANDLE_T dispman_update = vc_dispmanx_update_start(0);

   if (m_dispmanElement != DISPMANX_NO_HANDLE)
   {
      vc_dispmanx_element_remove(dispman_update, m_dispmanElement);
      m_dispmanElement = DISPMANX_NO_HANDLE;
   }

   vc_dispmanx_update_submit_sync(dispman_update);

   if (m_dispmanDisplay != DISPMANX_NO_HANDLE)
   {
      vc_dispmanx_display_close(m_dispmanDisplay);
      m_dispmanDisplay = DISPMANX_NO_HANDLE;
   }

   return bResult;
}
