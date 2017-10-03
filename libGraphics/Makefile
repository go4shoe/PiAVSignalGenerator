###################################################################################################
##
## This file is part of "Pi AV Signal Generator".
##
## "Pi AV Signal Generator" is free software : you can redistribute it and / or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## "Pi AV Signal Generator" is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with "Pi AV Signal Generator". If not, see <http://www.gnu.org/licenses/>.
##
###################################################################################################
##
## Copyright (C) 2009-2017 Markus P Schumann (go4shoe@hotmail.com)
##
###################################################################################################
##
##      mailto:go4shoe@hotmail.com
##
##     $Author: Markus $
##
##       $File: //depot/pi/libGraphics/Makefile $
##
##   $Revision: #7 $
##
##       $Date: 2017/09/30 $
##
###################################################################################################

###################################################################################################
# Input source files
###################################################################################################

SOURCE_FILES = Graphics.cpp VideoCore.cpp DisplayManager.cpp

###################################################################################################
# Output static library filename
###################################################################################################

OUTPUT_LIBRARY = libGraphics.a

###################################################################################################
# Object files (derived from source file names)
###################################################################################################

OBJECT_FILES = $(SOURCE_FILES:.cpp=.o)

###################################################################################################
# Inlucde files (derived from source file names)
###################################################################################################

INCLUDE_FILES = $(SOURCE_FILES:.cpp=.h)

###################################################################################################
# extra linker flags
###################################################################################################
# LDFLAGS+=

###################################################################################################
# Extra include flags
###################################################################################################

INCLUDES+=-I../

###################################################################################################
# Include master make file
###################################################################################################

include ../Makefile.include

