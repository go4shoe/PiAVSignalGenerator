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
//       $File: //depot/pi/libUtil/ErrorCode.h $
//
//   $Revision: #1 $
//
//       $Date: 2017/09/25 $
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ERROR_CODE_H__
#define __ERROR_CODE_H__

///////////////////////////////////////////////////////////////////////////////
struct Error
///////////////////////////////////////////////////////////////////////////////
{
	static const int Fail = -1;

	static const int ArgumentOutOfRange = -2;

	static const int OutOfMemory = -3;

	static const int Cleanup = -4;
};

///////////////////////////////////////////////////////////////////////////////
struct Success
///////////////////////////////////////////////////////////////////////////////
{
	static const int Ok = 0;
};

#endif // __ERROR_CODE_H__
