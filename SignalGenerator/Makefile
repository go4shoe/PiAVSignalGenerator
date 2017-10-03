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
##       $File: //depot/pi/SignalGenerator/Makefile $
##
##   $Revision: #8 $
##
##       $Date: 2017/10/01 $
##
###################################################################################################

###################################################################################################
# Input source files
###################################################################################################

SOURCE_FILES = MovingAverage.cpp SignalGenerator.cpp main.cpp

###################################################################################################
# Output binary executable filename
###################################################################################################

OUTPUT_BINARY = SignalGenerator.bin

###################################################################################################
# Object files (derived from source file names)
###################################################################################################

OBJECT_FILES = $(SOURCE_FILES:.cpp=.o)

###################################################################################################
# Add dependency for static libraries
###################################################################################################

DEPENDS_ON_LIBRARIES = \
../libGraphics \
../libFont \
../libILClient \
../libAudioPlayback

###################################################################################################
# Phony target
###################################################################################################
#
# In short: '.PHONY' is a fake (phony) target ensuring that all of DEPENDS_ON_LIBRARIES gets
# rebuild - if required.
#
# The file '.PHONY' doesn't exists and we don't create '.PHONY'.
# But we are forcing Make to verify each sub directory listed in DEPENDS_ON_LIBRARIES.
# Make thinks that creating '.PHONY' happens by executing Make in all of DEPENDS_ON_LIBRARIES
#
###################################################################################################

.PHONY: depends_on_libraries $(DEPENDS_ON_LIBRARIES)

###################################################################################################
# All targets
###################################################################################################

all: $(DEPENDS_ON_LIBRARIES) $(OUTPUT_BINARY)

$(DEPENDS_ON_LIBRARIES):
	$(MAKE) -C $@

###################################################################################################
# Extra linker flags (list of static libraries needed to built 'SignalGenerator.bin'
###################################################################################################
LDFLAGS += \
../libGraphics/libGraphics.a \
../libFont/libFont.a \
../libILClient/libILClient.a \
../libAudioPlayback/libAudioPlayback.a 

###################################################################################################
# Extra include flags (just the parent directory)
###################################################################################################
INCLUDES += -I../

###################################################################################################
# include master make file
###################################################################################################
include ../Makefile.include

