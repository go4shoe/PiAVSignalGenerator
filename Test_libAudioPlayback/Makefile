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
##       $File: $
##
##   $Revision: #6 $
##
##       $Date: 2009/04/06 $
##
###################################################################################################

###################################################################################################
# input files
###################################################################################################

OBJS=TestPlaySine.o

###################################################################################################
# output file
###################################################################################################

BIN=TestPlaySine.bin

###################################################################################################
# add dependency for static libraries
###################################################################################################

LIBS = ../libAudioPlayback ../libILClient ../libUtil

###################################################################################################
# build depending static libs
###################################################################################################

.PHONY: libs $(LIBS)

all: $(LIBS) $(BIN)

$(LIBS):
	$(MAKE) -C $@

###################################################################################################
# extra linker flags
###################################################################################################

LDFLAGS+= \
../libAudioPlayback/libAudioPlayback.a \
../libILClient/libILClient.a \
../libUtil/libUtil.a 

###################################################################################################
# extra include flags
###################################################################################################

INCLUDES+=-I../ -I../libAudioPlayback -I../libILClient

###################################################################################################
# include master make file
###################################################################################################

include ../Makefile.include


