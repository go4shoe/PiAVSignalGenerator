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
//       $File: //depot/pi/libGraphics/VideoCore.h $
//
//   $Revision: #6 $
//
//       $Date: 2017/09/25 $
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VIDEO_CORE_H__
#define __VIDEO_CORE_H__

///////////////////////////////////////////////////////////////////////////////
//
// OpenVG wrapper
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "bcm_host.h"
#include "vc_tvservice.h"


///////////////////////////////////////////////////////////////////////////////
class IVideoCoreCallback
///////////////////////////////////////////////////////////////////////////////
{
public:
   virtual void ConnectionStatusChange
   (
      uint32_t     reason,
      uint32_t     param1,
      uint32_t     param2,
      const char * sReasonString
   ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
class VideoCore
///////////////////////////////////////////////////////////////////////////////
{
public:
   VideoCore();
   ~VideoCore();
   
   bool Init();

   bool Deinit();

   bool UpdateResolution();

   static void VideoCoreCallbackStatic
   (
       void *callback_data,
       uint32_t reason,
       uint32_t param1,
       uint32_t param2
   );

   void VideoCoreCallback
   (
       uint32_t reason,
       uint32_t param1,
       uint32_t param2
   );

   void SetCallback
   (
      IVideoCoreCallback * pCallback
   );

private:
   IVideoCoreCallback * m_pCallback;
};

#endif // __VIDEO_CORE_H__
