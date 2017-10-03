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
//       $File: //depot/pi/libFont/FontEntry.cpp $
//
//   $Revision: #4 $
//
//       $Date: 2017/09/25 $
//
///////////////////////////////////////////////////////////////////////////////

#include "FontEntry.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include "EGL/egl.h"
#include "GLES/gl.h"
#include "bcm_host.h"

///////////////////////////////////////////////////////////////////////////////
// include font data
///////////////////////////////////////////////////////////////////////////////
#include "DejaVuSans.inc"
#include "DejaVuSerif.inc"
#include "DejaVuSansMono.inc"
#include "DIN1451m.inc"

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
static const int g_nMaxFontPath = 256;


///////////////////////////////////////////////////////////////////////////////
// supported fonts
///////////////////////////////////////////////////////////////////////////////
static FontEntry SansTypeface;
static FontEntry SerifTypeface;
static FontEntry MonoTypeface;
static FontEntry DIN1451m;

///////////////////////////////////////////////////////////////////////////////
struct FontMapping
///////////////////////////////////////////////////////////////////////////////
{
   const char *sName;
   FontEntry  *pFontEntry;
};

///////////////////////////////////////////////////////////////////////////////
static const FontMapping g_aFontMappings[] =
///////////////////////////////////////////////////////////////////////////////
{
   { "SansTypeface",  & SansTypeface  },
   { "SerifTypeface", & SerifTypeface },
   { "MonoTypeface",  & MonoTypeface  },
   { "DIN1451m",      & DIN1451m  },
};

///////////////////////////////////////////////////////////////////////////////
//static
///////////////////////////////////////////////////////////////////////////////
FontEntry * FontEntry::Get
///////////////////////////////////////////////////////////////////////////////
(
   const char * sName
)
///////////////////////////////////////////////////////////////////////////////
{
   FontEntry * pFontEntry = NULL;

   for(  size_t i=0;
         i < (sizeof(g_aFontMappings) / sizeof(g_aFontMappings[0]) );
         ++i)
   {
      if( 0 == strcmp(sName,g_aFontMappings[i].sName))
      {
         pFontEntry = g_aFontMappings[i].pFontEntry;
         break;
      }
   }

   return pFontEntry;
}
   

///////////////////////////////////////////////////////////////////////////////
//static
///////////////////////////////////////////////////////////////////////////////
bool FontEntry::Init()
///////////////////////////////////////////////////////////////////////////////
{
   bool bResult = true;
   
   if(bResult)
   {
      bResult =   SansTypeface.Load
                  (
                     DejaVuSans_glyphPoints,
                     DejaVuSans_glyphPointIndices,
                     DejaVuSans_glyphInstructions,
                     DejaVuSans_glyphInstructionIndices,
                     DejaVuSans_glyphInstructionCounts,
                     DejaVuSans_glyphAdvances,
                     DejaVuSans_characterMap,
                     DejaVuSans_glyphCount
                  );
   }

   if(bResult)
   {
      bResult = SerifTypeface.Load
                (
                  DejaVuSerif_glyphPoints,
                  DejaVuSerif_glyphPointIndices,
                  DejaVuSerif_glyphInstructions,
                  DejaVuSerif_glyphInstructionIndices,
                  DejaVuSerif_glyphInstructionCounts,
                  DejaVuSerif_glyphAdvances,
                  DejaVuSerif_characterMap,
                  DejaVuSerif_glyphCount
                );
   }

   if(bResult)
   {
      bResult = MonoTypeface.Load
               (
                  DejaVuSansMono_glyphPoints,
                  DejaVuSansMono_glyphPointIndices,
                  DejaVuSansMono_glyphInstructions,
                  DejaVuSansMono_glyphInstructionIndices,
                  DejaVuSansMono_glyphInstructionCounts,
                  DejaVuSansMono_glyphAdvances,
                  DejaVuSansMono_characterMap,
                  DejaVuSansMono_glyphCount
               );
   }

   if(bResult)
   {
      bResult = DIN1451m.Load
               (
                  DIN1451m_glyphPoints,
                  DIN1451m_glyphPointIndices,
                  DIN1451m_glyphInstructions,
                  DIN1451m_glyphInstructionIndices,
                  DIN1451m_glyphInstructionCounts,
                  DIN1451m_glyphAdvances,
                  DIN1451m_characterMap,
                  DIN1451m_glyphCount
               );
   }
   
   return bResult;
}

///////////////////////////////////////////////////////////////////////////////
//static
///////////////////////////////////////////////////////////////////////////////
void FontEntry::Deinit()
///////////////////////////////////////////////////////////////////////////////
{
   SansTypeface.Unload();
   SerifTypeface.Unload();
   MonoTypeface.Unload();
}

///////////////////////////////////////////////////////////////////////////////
//
// Text renders a string of text at a specified location, size, using the specified font glyphs
// derived from http://web.archive.org/web/20070808195131/http://developer.hybrid.fi/font2openvg/renderFont.cpp.txt
//
///////////////////////////////////////////////////////////////////////////////
void FontEntry::RenderText
///////////////////////////////////////////////////////////////////////////////
(
   VGfloat      xPos,
   VGfloat      yPos,
   const char * sText,
   int          ptSize
)
///////////////////////////////////////////////////////////////////////////////
{
   VGfloat size = (VGfloat) ptSize;

   VGfloat xxPos = xPos;
   VGfloat mm[9];
   
   vgGetMatrix(mm);
   
   for (int i = 0; i < (int)strlen(sText); i++)
   {
      unsigned int character = (unsigned int)sText[i];

      int glyph = m_pCharacterMap[character];

      if (glyph == -1)
      {
         //
         // glyph is undefined
         //
         continue;            
      }
      
      VGfloat mat[9] = 
      {
         size,   0.0f,    0.0f,
         0.0f,   size,    0.0f,
         xxPos,  yPos,    1.0f
      };

      vgLoadMatrix(mm);
      vgMultMatrix(mat);

      vgDrawPath(m_aGlyphs[glyph], VG_FILL_PATH);

      xxPos += size * m_pGlyphAdvances[glyph] / 65536.0f;
   }

   vgLoadMatrix(mm);
}

///////////////////////////////////////////////////////////////////////////////
void FontEntry::RenderTextCentered
///////////////////////////////////////////////////////////////////////////////
(
   VGfloat      xPos,
   VGfloat      yPos,
   const char * sText,
   int          ptSize
)
///////////////////////////////////////////////////////////////////////////////
{
   VGfloat tw = TextWidth(sText, ptSize);

   RenderText( xPos - (tw / 2.0),
               yPos,
               sText,
               ptSize);
}

///////////////////////////////////////////////////////////////////////////////
void FontEntry::RenderTextRight
///////////////////////////////////////////////////////////////////////////////
(
   VGfloat      xPos,
   VGfloat      yPos,
   const char * sText,
   int          ptSize
)
///////////////////////////////////////////////////////////////////////////////
{
   VGfloat tw = TextWidth(sText, ptSize);

   RenderText( xPos - tw,
               yPos,
               sText,
               ptSize);
}

///////////////////////////////////////////////////////////////////////////////
// TextWidth returns the width of a text string at the specified font and size.
///////////////////////////////////////////////////////////////////////////////
VGfloat FontEntry::TextWidth
///////////////////////////////////////////////////////////////////////////////
(
   const char * sText,
   int          ptSize
)
///////////////////////////////////////////////////////////////////////////////
{
   
   VGfloat tw   = 0.0;
   VGfloat size = (VGfloat) ptSize;

   for (int i = 0; i < (int)strlen(sText); i++)
   {
      unsigned int character = (unsigned int) sText[i];

      int glyph = m_pCharacterMap[character];
      
      if (glyph == -1)
      {
         //
         // glyph is undefined
         //
         continue;            
      }
      
      tw += size * m_pGlyphAdvances[glyph] / 65536.0f;
   }
   return tw;
}

///////////////////////////////////////////////////////////////////////////////
//
// loadfont loads font path data
// derived from http://web.archive.org/web/20070808195131/http://developer.hybrid.fi/font2openvg/renderFont.cpp.txt
//
///////////////////////////////////////////////////////////////////////////////
bool FontEntry::Load
///////////////////////////////////////////////////////////////////////////////
(
   const int           * Points,
   const int           * PointIndices,
   const unsigned char * Instructions,
   const int           * InstructionIndices,
   const int           * InstructionCounts,
   const int           * adv,
   const short         * cmap,
   int                   nGlyph
)
///////////////////////////////////////////////////////////////////////////////
{
   bool bResult = true;

   if( nGlyph > g_nMaxFontPath )
   {
      bResult = false;
   }
   
   if(bResult)
   {
      memset(&m_aGlyphs[0], 0, sizeof(m_aGlyphs));
   }

   for(  int i = 0;
         bResult && (i < nGlyph);
         ++i)
   {
      const int *p = &Points[PointIndices[i] * 2];

      const unsigned char *instructions = &Instructions[InstructionIndices[i]];

      int ic = InstructionCounts[i];

      VGPath path = vgCreatePath(   VG_PATH_FORMAT_STANDARD,
                                    VG_PATH_DATATYPE_S_32,
                                    1.0f / 65536.0f,  
                                    0.0f,
                                    0,
                                    0,
                                    VG_PATH_CAPABILITY_ALL);
      m_aGlyphs[i] = path;

      if (ic)
      {
         vgAppendPathData( path,
                           ic,
                           instructions,
                           p);
      }
   }

   if(bResult)
   {
      m_pCharacterMap  = cmap;
      m_pGlyphAdvances = adv;
      m_nCount         = nGlyph;
   }

   return bResult;
}

///////////////////////////////////////////////////////////////////////////////
void FontEntry::Unload()
///////////////////////////////////////////////////////////////////////////////
{
   for(int i=0; i < m_nCount; ++i)
   {
      vgDestroyPath(m_aGlyphs[i]);
   }
}
