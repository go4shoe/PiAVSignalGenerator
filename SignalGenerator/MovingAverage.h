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
//       $File: //depot/pi/SignalGenerator/MovingAverage.h $
//
//   $Revision: #2 $
//
//       $Date: 2017/09/25 $
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __MOVING_AVERAGE_H__
#define __MOVING_AVERAGE_H__

#include <stdint.h>

class MovingAverage
{
public:
   MovingAverage(uint32_t nDepth);
   void Add(float value); 
   float Get() const;

private:
   uint32_t m_nDepth;
   uint32_t m_nEntries;
   float    m_dSum;
   
};
#endif // __MOVING_AVERAGE_H__
