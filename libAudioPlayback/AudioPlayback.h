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
//       $File: //depot/pi/libAudioPlayback/AudioPlayback.h $
//
//   $Revision: #8 $
//
//       $Date: 2017/09/26 $
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AUDIO_PLAYBACK_H__
#define __AUDIO_PLAYBACK_H__

#include "libUtil/BaseTypes.h"

///////////////////////////////////////////////////////////////////////////////
// forwards
///////////////////////////////////////////////////////////////////////////////
struct AudioPlaybackState;

///////////////////////////////////////////////////////////////////////////////
// Enables client code to render PCM audio
///////////////////////////////////////////////////////////////////////////////
class AudioPlayback
///////////////////////////////////////////////////////////////////////////////
{
private:
	AudioPlaybackState *audio_playback_state;

	uint32_t m_nBuffers;

	uint32_t m_cbBytesPerSample;

	uint32_t m_hzSampleRate;

public:

	//
	//	Constructor
	//
	AudioPlayback();

	//
	//	Destructor
	//
	~AudioPlayback();

	//
	//	Initializes the playback engine.
	//	Returns return 0 on success, -1 on failure
	//
	int32_t Init
	(
		//	The sample rate, in samples per second, for the PCM data.
		//	Range: 8000 to 96000
		uint32_t hzSampleRate,

		//	The number of channels for the PCM data.Must be 1, 2, 4, or 8.
		//	Channels must be sent interleaved.
		uint32_t nChannels,

		//	The bitdepth per channel per sample.  Must be 16 or 32.
		uint32_t nBitDepth,

		//	The number of buffers that will be created to write PCM samples into.
		uint32_t nBuffers,

		//	The size in bytes of each buffer that will be used to write PCM samples into.
		//	Note that small buffers of less than a few Kb in size may be faster than
		//	larger buffers.
		uint32_t cbBufferSize
	);

	//
	//	Deinitialize
	//
	int32_t Deinit();


	//
	//	Gets an empty buffer. If no buffer is available returns NULL
	//
	uint8_t *GetBuffer();


	//
	// Sends a buffer containing raw samples to be rendered.
	// Returns return 0 on success, -1 on failure
	//
	int32_t PlayBuffer
	(
		// Must be a pointer previously returned by GetBuffer(). 
		// After calling this function the buffer pointer must not be referenced until returned.
		// again by another call to audioplay_get_buffer().
		uint8_t *buffer,

		// buffer length
		uint32_t length
	);

	//
	//	Returns the current audio playout buffer size in samples or
	//	the latency until the next sample is rendered.
	//
	uint32_t GetLatency();

	//
	// Sets the output destination
	//
	// returns 0 on success, -1 on failure
	//
	int32_t SetDestination
	(
		const char *name
	);

	//
	//
	//
	void Sleep();

	//
	//
	//
	static int GetSamplesPerBuffer();

	//
	//
	//
	static int GetBufferSize
	(
		int nBitDepth,

		int nChannels
	);

	//
	//
	//
	static int GetOutputChannels
	(
		int nChannels
	);

};

#endif // __AUDIO_PLAYBACK_H__
