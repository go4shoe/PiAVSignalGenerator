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
//       $File: //depot/pi/libFont/FontEntry.h $
//
//   $Revision: #4 $
//
//       $Date: 2017/09/25 $
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FONT_ENTRY_H__
#define __FONT_ENTRY_H__

#include "VG/openvg.h"

///////////////////////////////////////////////////////////////////////////////
class FontEntry
///////////////////////////////////////////////////////////////////////////////
{
public:

   ///
   ///
   ///
   static FontEntry* Get
   (
      const char * sName
   );

   ///
   ///
   ///
   static bool Init();

   ///
   ///
   ///
   static void Deinit();

   ///
   ///
   ///
   void RenderText
   (
      VGfloat      xPos,
      VGfloat      yPos,
      const char * sText,
      int          ptSize
   );

   ///
   ///
   ///
   void RenderTextCentered
   (
      VGfloat      xPos,
      VGfloat      yPos,
      const char * sText,
      int          ptSize
   );

   ///
   ///
   ///
   void RenderTextRight
   (
      VGfloat      xPos,
      VGfloat      yPos,
      const char * sText,
      int          ptSize
   );

   ///
   ///
   ///
   VGfloat TextWidth
   (
      const char * sText,
      int          ptSize
   );

private:
   ///
   ///
   ///
   bool Load
   (
      const int           * Points,
      const int           * PointIndices,
      const unsigned char * Instructions,
      const int           * InstructionIndices,
      const int           * InstructionCounts,
      const int           * adv,
      const short         * cmap,
      int                   nGlyph
   );

   ///
   ///
   ///
   void Unload();

private:
   const short * m_pCharacterMap;
   const int *   m_pGlyphAdvances;
   int           m_nCount;
   VGPath        m_aGlyphs[256];
};



#endif // __FONT_ENTRY_H__
