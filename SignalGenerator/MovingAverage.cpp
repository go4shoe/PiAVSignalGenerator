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
//       $File: //depot/pi/SignalGenerator/MovingAverage.cpp $
//
//   $Revision: #2 $
//
//       $Date: 2017/09/25 $
//
///////////////////////////////////////////////////////////////////////////////


#include "MovingAverage.h"

MovingAverage::MovingAverage(uint32_t nDepth)
{
   m_nEntries = 0;
   m_nDepth   = nDepth;
}

void MovingAverage::Add(float dValue)
{
   if( m_nEntries >= m_nDepth )
   {
      m_dSum -= m_dSum / m_nEntries;
   }
   else
   {
      ++m_nEntries;
   }

   m_dSum += dValue;

}

float MovingAverage::Get() const
{
   return m_dSum / m_nEntries;
}

