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
//       $File: //depot/pi/libGraphics/VideoCore.cpp $
//
//   $Revision: #8 $
//
//       $Date: 2017/09/25 $
//
///////////////////////////////////////////////////////////////////////////////

#include "VideoCore.h"

#include <vchiq_if.h>


///////////////////////////////////////////////////////////////////////////////
VideoCore::VideoCore()
///////////////////////////////////////////////////////////////////////////////
{
   m_pCallback = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// exit_func cleans up
///////////////////////////////////////////////////////////////////////////////
VideoCore::~VideoCore()
///////////////////////////////////////////////////////////////////////////////
{
   Deinit();
}

///////////////////////////////////////////////////////////////////////////////
bool VideoCore::Init()
///////////////////////////////////////////////////////////////////////////////
{
   bool bResult = true;

   //
   // register callback
   //
   vc_tv_register_callback(VideoCoreCallbackStatic,this);

   return bResult;
}

///////////////////////////////////////////////////////////////////////////////
bool VideoCore::Deinit()
///////////////////////////////////////////////////////////////////////////////
{
   bool bResult = true;

   //
   // register callback
   //
   vc_tv_unregister_callback(VideoCoreCallbackStatic);
   
   return bResult;
}

///////////////////////////////////////////////////////////////////////////////
bool VideoCore::UpdateResolution()
///////////////////////////////////////////////////////////////////////////////
{
   bool bResult = true;

   HDMI_RES_GROUP_T group = HDMI_RES_GROUP_DMT;

   HDMI_RES_GROUP_T preferGroup = HDMI_RES_GROUP_INVALID;

   uint32_t preferMode;

   TV_SUPPORTED_MODE_NEW_T * pSupportedModes = NULL;


   int maxSupportedModes = vc_tv_hdmi_get_supported_modes_new( group,
                                                               NULL,
                                                               0,
                                                               & preferGroup,
                                                               & preferMode);

   if (maxSupportedModes > 0)
   {
      pSupportedModes = new TV_SUPPORTED_MODE_NEW_T[maxSupportedModes];
   }

   uint32_t numModes = 0;

   if(pSupportedModes)
   {
       numModes = vc_tv_hdmi_get_supported_modes_new( group,
                                                      pSupportedModes,
                                                      maxSupportedModes,
                                                      &preferGroup,
                                                      &preferMode);

       bResult = numModes > 0;
   }

   vc_tv_hdmi_power_on_explicit_new(HDMI_MODE_HDMI,preferGroup,preferMode);

   if(pSupportedModes)
   {
      delete [] pSupportedModes;

      pSupportedModes = NULL;
   }
   
   return bResult;
}


///////////////////////////////////////////////////////////////////////////////
// static
///////////////////////////////////////////////////////////////////////////////
void VideoCore::VideoCoreCallbackStatic
///////////////////////////////////////////////////////////////////////////////
(
   void *pCallbackData,
   uint32_t reason,
   uint32_t param1,
   uint32_t param2
)
///////////////////////////////////////////////////////////////////////////////
{
   if(pCallbackData)
   {
      VideoCore * pVideoCore = (VideoCore*) pCallbackData;

      pVideoCore->VideoCoreCallback( reason, param1, param2);
   }
}

///////////////////////////////////////////////////////////////////////////////
struct ReasonEntry
///////////////////////////////////////////////////////////////////////////////
{
   const char * m_sText;
   uint32_t     m_eCode;
};

///////////////////////////////////////////////////////////////////////////////
static const ReasonEntry g_reasonEntry[] =
///////////////////////////////////////////////////////////////////////////////
{
   { "VC_HDMI_UNPLUGGED",         VC_HDMI_UNPLUGGED },
   { "VC_HDMI_ATTACHED",          VC_HDMI_ATTACHED },
   { "VC_HDMI_DVI",               VC_HDMI_DVI },
   { "VC_HDMI_HDMI",              VC_HDMI_HDMI },
   { "VC_HDMI_HDCP_UNAUTH",       VC_HDMI_HDCP_UNAUTH },
   { "VC_HDMI_HDCP_AUTH",         VC_HDMI_HDCP_AUTH },
   { "VC_HDMI_HDCP_KEY_DOWNLOAD", VC_HDMI_HDCP_KEY_DOWNLOAD },
   { "VC_HDMI_HDCP_SRM_DOWNLOAD", VC_HDMI_HDCP_SRM_DOWNLOAD },
   { "VC_HDMI_CHANGING_MODE",     VC_HDMI_CHANGING_MODE },
};

///////////////////////////////////////////////////////////////////////////////
// Callback reason and arguments from HDMI middleware
///////////////////////////////////////////////////////////////////////////////
// Each callback comes with two optional uint32_t parameters.
//
// Reason                     param1       param2      remark
///////////////////////////////////////////////////////////////////////////////
// VC_HDMI_UNPLUGGED            -            -         cable is unplugged
// VC_HDMI_ATTACHED           CEA/DMT      mode code   cable is plugged in and peripheral powered off (preferred mode sent back if available)
// VC_HDMI_DVI                CEA/DMT      mode code   DVI mode is active at said resolution
// VC_HDMI_HDMI               CEA(3D)/DMT  mode code   HDMI mode is active at said resolution (in 3D mode if CEA3D)
// VC_HDMI_HDCP_UNAUTH        HDCP_ERROR_T  retry?     HDCP is inactive, the error can be none if we actively disable HDCP, if retry is non-zero, HDCP will attempt to reauthenticate
// VC_HDMI_HDCP_AUTH            -            -         HDCP is active
// VC_HDMI_HDCP_KEY_DOWNLOAD  success?       -         HDCP key download success (zero) or not (non zero)
// VC_HDMI_HDCP_SRM_DOWNLOAD  no. of keys    -         HDCP revocation list download set no. of keys (zero means failure)
// VC_HDMI_CHANGING_MODE        0            0         No information is supplied in this callback
//

///////////////////////////////////////////////////////////////////////////////
static const char * GetResonString(uint32_t eReason)
///////////////////////////////////////////////////////////////////////////////
{
   const char * sReasonString = "<unknown>";

   for(size_t i=0; i < (sizeof(g_reasonEntry) / sizeof(g_reasonEntry[0])) ; ++i)
   {
      if( eReason == g_reasonEntry[i].m_eCode)
      {
         sReasonString = g_reasonEntry[i].m_sText;
         break;
      }
   }

   return sReasonString;
}

///////////////////////////////////////////////////////////////////////////////
void VideoCore::VideoCoreCallback
///////////////////////////////////////////////////////////////////////////////
(
   uint32_t reason,
   uint32_t param1,
   uint32_t param2
)
///////////////////////////////////////////////////////////////////////////////
{
   if(m_pCallback)
   {
      m_pCallback->ConnectionStatusChange(reason,param1,param2,GetResonString(reason));
   }
}

///////////////////////////////////////////////////////////////////////////////
void VideoCore::SetCallback
///////////////////////////////////////////////////////////////////////////////
(
   IVideoCoreCallback * pCallback
)
///////////////////////////////////////////////////////////////////////////////
{
   m_pCallback = pCallback;
}

