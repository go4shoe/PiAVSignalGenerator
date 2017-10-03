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
//       $File: //depot/pi/libUtil/BaseTypes.h $
//
//   $Revision: #1 $
//
//       $Date: 2017/09/25 $
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __BASE_TYPES_H__
#define __BASE_TYPES_H__

#ifndef ArrayCount
#define ArrayCount(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

///////////////////////////////////////////////////////////////////////////////
// typedefs
///////////////////////////////////////////////////////////////////////////////
typedef int           int32_t;
typedef unsigned      uint32_t;
typedef unsigned char uint8_t;


///////////////////////////////////////////////////////////////////////////////
static int Align16(int n)
///////////////////////////////////////////////////////////////////////////////
{
	return (n + 15) & ~15;
}

#endif // __BASE_TYPES_H__