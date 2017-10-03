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
//       $File: //depot/pi/SignalGenerator/SignalGenerator.h $
//
//   $Revision: #19 $
//
//       $Date: 2017/10/01 $
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SIGNAL_GENERATOR_H__
#define __SIGNAL_GENERATOR_H__

#include "libGraphics/Graphics.h"
#include "libGraphics/VideoCore.h"
#include "libGraphics/DisplayManager.h"
#include "libAudioPlayback/AudioPlayback.h"

#include "MovingAverage.h"

#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>

///////////////////////////////////////////////////////////////////////////////
class SignalGenerator : public IVideoCoreCallback
///////////////////////////////////////////////////////////////////////////////
{
private:

	//
	//	The audio thread
	//
	pthread_t pthAudio;

	//
	//	The video thread
	//
	pthread_t pthVideo;

	//
	// video rendering
	//
	DisplayManager displayManager;

	VideoCore videoCore;

	Graphics graphics;

	sem_t m_semTVSynced;

	enum VideoConnectionState
	{
		VideoConnectionState_HDMI_unknown,

		VideoConnectionState_HDMI_unplugged,

		VideoConnectionState_HDMI_connected
	};

	VideoConnectionState m_eVideoConnectionState;

	bool m_bUpdateResolution;

	static volatile sig_atomic_t m_bPlayAudioVideo;

	MovingAverage m_fpsAverage;

	uint32_t m_nFrameCounter;

	struct timespec m_tpNow;

	struct timespec m_tpPrevious;

	//
	// audio play back
	//
	AudioPlayback m_audioPlayback;

	int m_cbAudioBufferSize;

public:

	SignalGenerator();

	~SignalGenerator();

	void Init();

	void InitAudio();

	void InitVideo();

	void Deinit();

	void DeinitAudio();

	void DeinitVideo();

	void Run();

	void Paint();

	void PaintBackground();

	static void * PlayAudioStatic(void *pData);

	void * PlayAudio();

	static void * PlayVideoStatic(void *pData);

	void * PlayVideo();

	static void StopAudioVideo(int signal);

	//
	// start: IVideoCoreCallback
	//
	void ConnectionStatusChange
	(
		uint32_t reason,

		uint32_t param1,

		uint32_t param2,

		const char *sReasonString
	);

	//
	// end: IVideoCoreCallback
	//
private:

	void SetNativeResolution();
};

#endif // __SIGNAL_GENERATOR_H__
