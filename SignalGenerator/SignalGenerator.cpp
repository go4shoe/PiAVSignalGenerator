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
//       $File: //depot/pi/SignalGenerator/SignalGenerator.cpp $
//
//   $Revision: #25 $
//
//       $Date: 2017/10/01 $
//
///////////////////////////////////////////////////////////////////////////////

#include "SignalGenerator.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <locale>

#include "libAudioPlayback/SineWave.h"

///////////////////////////////////////////////////////////////////////////////
// screen margin
///////////////////////////////////////////////////////////////////////////////
static const uint32_t g_pxMarginText   = 12;

static const uint32_t g_pxMarginCircle =  1;

static const char * g_sDefaultFont = "MonoTypeface";

////////////////////////////////////////////////////////////////////////////////
static const char * g_aAudioDestination[] =
////////////////////////////////////////////////////////////////////////////////
{
	"local",

	"hdmi"
};

////////////////////////////////////////////////////////////////////////////////
// 0=headphones, 1=hdmi
////////////////////////////////////////////////////////////////////////////////
static const uint32_t g_eDefaultAudioDestination = 0;

////////////////////////////////////////////////////////////////////////////////
// audio sample rate in Hz
////////////////////////////////////////////////////////////////////////////////
static const uint32_t g_hzDefaultSampleRate = 48000;

////////////////////////////////////////////////////////////////////////////////
// audio test tone frequency in Hz
////////////////////////////////////////////////////////////////////////////////
static const uint32_t g_hzTestToneFrequency = 1000;

static const int32_t g_usTestToneLength = 50 * 1000;

////////////////////////////////////////////////////////////////////////////////
// numnber of audio channels
////////////////////////////////////////////////////////////////////////////////
static const uint32_t g_nChannels = 2;

////////////////////////////////////////////////////////////////////////////////
// number of bits per sample
////////////////////////////////////////////////////////////////////////////////
static const uint32_t g_nDefaultBitDepth = 16;

////////////////////////////////////////////////////////////////////////////////
// play state
////////////////////////////////////////////////////////////////////////////////
volatile sig_atomic_t SignalGenerator::m_bPlayAudioVideo = 0;

///////////////////////////////////////////////////////////////////////////////
SignalGenerator::SignalGenerator() :
///////////////////////////////////////////////////////////////////////////////
	m_eVideoConnectionState(VideoConnectionState_HDMI_unknown),

	m_bUpdateResolution(false),

	m_fpsAverage(600),

	m_nFrameCounter(0),

	m_cbAudioBufferSize(0)
///////////////////////////////////////////////////////////////////////////////
{
}

///////////////////////////////////////////////////////////////////////////////
SignalGenerator::~SignalGenerator()
///////////////////////////////////////////////////////////////////////////////
{

}

///////////////////////////////////////////////////////////////////////////////
void SignalGenerator::Init()
///////////////////////////////////////////////////////////////////////////////
{
	//
	// init bcm_host
	//
	bcm_host_init();

	//
	// Register signal
	//
	signal(SIGINT, StopAudioVideo);
}

///////////////////////////////////////////////////////////////////////////////
void SignalGenerator::Deinit()
///////////////////////////////////////////////////////////////////////////////
{
	//
	// deinit bcm_host
	//
	bcm_host_deinit();
}

///////////////////////////////////////////////////////////////////////////////
void SignalGenerator::InitAudio()
///////////////////////////////////////////////////////////////////////////////
{
	int32_t ret = 0;

	m_cbAudioBufferSize = AudioPlayback::GetBufferSize( g_nDefaultBitDepth, g_nChannels);

	m_audioPlayback.Init
	(
		g_hzDefaultSampleRate,

		g_nChannels,

		g_nDefaultBitDepth,

		10,

		m_cbAudioBufferSize
	);

	//
	// set output destination to HDMI embedded audio or analog out
	//
	ret = m_audioPlayback.SetDestination(g_aAudioDestination[g_eDefaultAudioDestination]);

	assert(ret == 0);
}

///////////////////////////////////////////////////////////////////////////////
void SignalGenerator::InitVideo()
///////////////////////////////////////////////////////////////////////////////
{
	this->videoCore.Init();

	this->videoCore.SetCallback(this);

	sem_init(&m_semTVSynced, 0, 0);

	if( this->videoCore.UpdateResolution() )
	{
		sem_wait(&m_semTVSynced);
	}

	sem_destroy(&m_semTVSynced);

	this->displayManager.Init();

	this->graphics.Init(&this->displayManager);
}

///////////////////////////////////////////////////////////////////////////////
void SignalGenerator::DeinitAudio()
///////////////////////////////////////////////////////////////////////////////
{
	std::cerr << "SignalGenerator::DeinitAudio" << std::endl;

	m_audioPlayback.Deinit();
}

///////////////////////////////////////////////////////////////////////////////
void SignalGenerator::DeinitVideo()
///////////////////////////////////////////////////////////////////////////////
{
	std::cerr << "SignalGenerator::DeinitVideo" << std::endl;

	this->graphics.Deinit();

	this->displayManager.Deinit();

	this->videoCore.Deinit();
}

///////////////////////////////////////////////////////////////////////////////
void SignalGenerator::Run()
///////////////////////////////////////////////////////////////////////////////
{
	//
	// set play state
	//
	m_bPlayAudioVideo = 1;

	//
	// create video thread
	//
	pthread_create
	(
		& pthVideo,

		NULL,

		SignalGenerator::PlayVideoStatic,

		(void *) this
	);

	//
	// create audio thread
	//
	pthread_create
	(
		& pthAudio,

		NULL,

		SignalGenerator::PlayAudioStatic,

		(void *) this
	);

	pthread_join(pthVideo, NULL);

	pthread_join(pthAudio, NULL);

	std::cerr << "SignalGenerator::Run: audio and video thread ended." << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
void SignalGenerator::PaintBackground()
///////////////////////////////////////////////////////////////////////////////
{
	uint32_t nWidth  = this->graphics.GetWidth();

	uint32_t nHeight = this->graphics.GetHeight();

	//
	// SMPTE color bars (top)
	//
	uint8_t aSmpteTopColorBars[] =
	{
		//
		// r    g    b
		//
		204, 204, 204,    // grey

		255, 255,   0,    // yellow

		  0, 255, 255,    // turkoise

		  0, 255,   0,    // green

		255,   0, 255,    // cyan

		255,   0,   0,    // red

		  0,   0, 255,    // blue
	};

	static const char * aColorNames[] =
	{
		"grey",

		"yellow",

		"turkoise",

		"green",

		"cyan",

		"red",

		"blue"
	};

	uint32_t nSmpteTopColorBars = sizeof(aSmpteTopColorBars) / sizeof(aSmpteTopColorBars[0]) / 3;

	float fSmpteTopColorBarWidth  = float(nWidth) / nSmpteTopColorBars;

	float fSmpteTopColorBarHeight = float(nHeight) * 2 / 3;

	for(uint32_t i=0; i < nSmpteTopColorBars; ++i)
	{
		float x = (float) i * fSmpteTopColorBarWidth;

		float y = float(nHeight) - fSmpteTopColorBarHeight;

		float w = fSmpteTopColorBarWidth;

		float h = fSmpteTopColorBarHeight;

		float sw = 1;

		float fill[4] =
		{
			(float) aSmpteTopColorBars[i*3 + 0] / 255.0f,

			(float) aSmpteTopColorBars[i*3 + 1] / 255.0f,

			(float) aSmpteTopColorBars[i*3 + 2] / 255.0f,

			1.0f
		};

		float stroke[4] =
		{
			(float) aSmpteTopColorBars[i*3 + 0] / 255.0f,

			(float) aSmpteTopColorBars[i*3 + 1] / 255.0f,

			(float) aSmpteTopColorBars[i*3 + 2] / 255.0f,

			1.0f
		};

		this->graphics.Rect (x, y , w, h, sw, fill, stroke);

		float fillText[4] =
		{
			0.0f,

			0.0f,

			0.0f,

			1.0f
		};

		this->graphics.RenderTextCentered
		(
			x + (fSmpteTopColorBarWidth / 2),

			y + fSmpteTopColorBarHeight * 3 / 4 - 12, aColorNames[i],

			nWidth / 48,

			g_sDefaultFont,

			fillText
		);
	}

	//
	// gray bars (bottom)
	//
	uint32_t nBottomGrayBars = 8;

	float fBottomGrayBarWidth  = float(nWidth) / nBottomGrayBars;

	float fBottomGrayBarHeight = float(nHeight) / 3;

	for(uint32_t i=0; i < nBottomGrayBars; ++i)
	{
		float x = (float) i * fBottomGrayBarWidth;

		float y = 0;

		float w = fBottomGrayBarWidth;

		float h = fBottomGrayBarHeight;

		float sw = 1;

		float fGrayValue = (float) i / (float) (nBottomGrayBars-1);
      
		float fill[4] =
		{
			fGrayValue,

			fGrayValue,

			fGrayValue,

			1.0f
		};

		float stroke[4] =
		{
			fGrayValue,

			fGrayValue,

			fGrayValue,

			1.0f
		};

		this->graphics.Rect (x, y , w, h, sw, fill, stroke);

#ifdef SHOW_GRAY_VALUES

		float fillText[4] =
		{
			1.0f,
			0.0f,
			0.0f,
			1.0f
		};

		std::stringstream ss;

		ss << int(fGrayValue * 255.0);

		this->graphics.RenderTextCentered
		(
			x + (fBottomGrayBarWidth / 2),

			float(nHeight) / 6 - 12,

			ss.str().c_str(),

			nWidth / 48,

			g_sDefaultFont,

			fillText
		);

#endif

	}

	//
	// circle to check aspect ratio
	//
	{
		float x  = float(nWidth  - g_pxMarginCircle) / 2.0f + 0.5f;

		float y  = float(nHeight - g_pxMarginCircle) / 2.0f + 0.5f;

		float w  = float(nHeight - 2 * g_pxMarginCircle) - 0.5f;

		float h  = float(nHeight - 2 * g_pxMarginCircle) - 0.5f;

		float sw = 3;

		float fill[4] =
		{
			0.0f,

			0.0f,

			0.0f,

			0.0f
		};

		float stroke[4] =
		{
			0.0f,

			0.0f,

			0.0f,

			1.0f
		};

		this->graphics.Ellipse
		(
			x,
			y ,
			w,
			h,
			sw,
			fill,
			stroke
		);
	}
}

///////////////////////////////////////////////////////////////////////////////
// static
///////////////////////////////////////////////////////////////////////////////
void *SignalGenerator::PlayAudioStatic
///////////////////////////////////////////////////////////////////////////////
(
	void *pData
)
///////////////////////////////////////////////////////////////////////////////
{
	void *pResult = NULL;

	if(pData)
	{
		SignalGenerator *pSignalGenerator = (SignalGenerator*) pData;
		pResult = pSignalGenerator->PlayAudio();
	}

	return pResult;
}

///////////////////////////////////////////////////////////////////////////////
void *SignalGenerator::PlayAudio()
///////////////////////////////////////////////////////////////////////////////
{
	void *pResult = NULL;

	InitAudio();

	int ixPhase = 0;

	int nPhaseIncrement = GetSinResolution() * g_hzTestToneFrequency / g_hzDefaultSampleRate;

	while(m_bPlayAudioVideo)
	{
		uint8_t * pBuffer = NULL;

		while ((pBuffer = m_audioPlayback.GetBuffer()) == NULL)
		{
			usleep(10*1000);
		}

		int16_t * pBufferS16 = (int16_t *) pBuffer;

		struct timespec tpNow;
		clock_gettime(CLOCK_REALTIME, &tpNow);
      
		int usNow = tpNow.tv_nsec / 1000;

		int usLatency  = 1000 * 1000 * m_audioPlayback.GetLatency() / g_hzDefaultSampleRate;

			usLatency += 1000 * 1000 * AudioPlayback::GetSamplesPerBuffer() / g_hzDefaultSampleRate;

		usNow += usLatency;

		//
		// fill the buffer
		//
		for (int i=0; i < AudioPlayback::GetSamplesPerBuffer(); i++)
		{
			int16_t pcmValue = GetSin(ixPhase);

			ixPhase += nPhaseIncrement;
          
			int usTone = usNow + i * 1000 * 1000 / g_hzDefaultSampleRate;

			if( usTone > 1000 * 1000 )
			{
				usTone -= 1000 * 1000; 
			}

			for(int j=0; j< AudioPlayback::GetOutputChannels(g_nChannels); j++)
			{
				if (g_nDefaultBitDepth == 32)
				{
					*pBufferS16++ = 0;
				}

				if( (usTone >= 0) && (usTone < g_usTestToneLength ) )
				{
					*pBufferS16++ = pcmValue;
				}
				else
				{
					*pBufferS16++ = 0;
				}
			}
		}

		//
		// try and wait for a minimum latency time (in ms) before
		// sending the next packet
		//
		m_audioPlayback.Sleep();

		int ret = m_audioPlayback.PlayBuffer(pBuffer, m_cbAudioBufferSize);

		assert(ret == 0);
	}

	DeinitAudio();

	return pResult;
}

///////////////////////////////////////////////////////////////////////////////
// static
///////////////////////////////////////////////////////////////////////////////
void *SignalGenerator::PlayVideoStatic
///////////////////////////////////////////////////////////////////////////////
(
	void *pData
)
///////////////////////////////////////////////////////////////////////////////
{
	void * pResult = NULL;

	if(pData)
	{
		SignalGenerator * pSignalGenerator = (SignalGenerator*) pData;

		pResult = pSignalGenerator->PlayVideo();
	}

	return pResult;
}

///////////////////////////////////////////////////////////////////////////////
void *SignalGenerator::PlayVideo()
///////////////////////////////////////////////////////////////////////////////
{
	void *pResult = NULL;

	InitVideo();

	clock_gettime(CLOCK_REALTIME, &m_tpNow);

	//
	// get current display settings state
	//
	TV_DISPLAY_STATE_T tvDisplayState;

	memset(&tvDisplayState, 0, sizeof(TV_DISPLAY_STATE_T));

	vc_tv_get_display_state(&tvDisplayState);

	if ((tvDisplayState.state & ( VC_HDMI_HDMI | VC_HDMI_DVI )) != 0)
	{
		m_eVideoConnectionState = VideoConnectionState_HDMI_connected;
	}
	else
	{
		m_eVideoConnectionState = VideoConnectionState_HDMI_unplugged;
	}

	while(m_bPlayAudioVideo)
	{
		if(m_bUpdateResolution)
		{
			SetNativeResolution();

			m_bUpdateResolution = false;
		}

		switch(m_eVideoConnectionState)
		{
		
		case VideoConnectionState_HDMI_connected:
			Paint();
			break;

		case VideoConnectionState_HDMI_unplugged:
			break;

			default:
			;
		}
	}

	DeinitVideo();

	return pResult;
}

///////////////////////////////////////////////////////////////////////////////
// static
///////////////////////////////////////////////////////////////////////////////
void SignalGenerator::StopAudioVideo(int signal)
///////////////////////////////////////////////////////////////////////////////
{
	std::cerr << "SignalGenerator::StopAudioVideo signal : " << signal << std::endl;
	
	m_bPlayAudioVideo = 0;
}

///////////////////////////////////////////////////////////////////////////////
void SignalGenerator::Paint()
///////////////////////////////////////////////////////////////////////////////
{
	this->graphics.Start();

	PaintBackground();

	//
	// some text
	//
	{
		float fill[4] =
		{
			1.0f,
			0.0f,
			0.0f,
			1.0f
		};

		std::stringstream ss;

		std::locale loc("");

		ss.imbue(loc);

		ss << std::fixed << m_nFrameCounter++ << " frames";

		uint32_t nWidth  = this->graphics.GetWidth();

		this->graphics.RenderText
		(
			g_pxMarginText * nWidth / 1920,

			g_pxMarginText * nWidth / 1920,

			ss.str().c_str(),

			nWidth / 48,

			g_sDefaultFont,

			fill
		);
	}

	m_tpPrevious = m_tpNow;

	clock_gettime(CLOCK_REALTIME, &m_tpNow);

	//
	// show frame rate
	//
	{
		float frameDuration = m_tpNow.tv_nsec - m_tpPrevious.tv_nsec;

		if( frameDuration < 0.0 )
		{
			frameDuration += (1000 * 1000 * 1000);
		}

		frameDuration /= (1000 * 1000 * 1000);

		frameDuration += m_tpNow.tv_sec - m_tpPrevious.tv_sec;

		float frameRate = (frameDuration > 0.0 ) ? (1.0 / frameDuration) : 0.0;

		float fill[4] =
		{
			1.0f,
			0.0f,
			0.0f,
			1.0f
		};

		uint32_t nWidth  = this->graphics.GetWidth();

		uint32_t nHeight = this->graphics.GetHeight();

		std::stringstream ss;

		m_fpsAverage.Add(frameRate);

		ss	<< nWidth  << " x "
			<< nHeight << " @ "
			<< std::setprecision(1)
			<< std::fixed
			<< m_fpsAverage.Get()
			<< " fps";

		this->graphics.RenderTextRight
		(
			nWidth - g_pxMarginText * nWidth / 1920,

			g_pxMarginText * nWidth / 1920,

			ss.str().c_str(),

			nWidth / 48,

			g_sDefaultFont,

			fill
		);
	}

	//
	// draw pie
	//
	{
		float fill[4] =
		{
			1.0f,
			0.0f,
			0.0f,
			1.0f
		};

		uint32_t nWidth  = this->graphics.GetWidth();

		uint32_t nHeight = this->graphics.GetHeight();

		float extentAngle = m_tpNow.tv_nsec;

		extentAngle *= 360;

		extentAngle /= (1000 * 1000 * 1000);

		this->graphics.Arc
		(
			nWidth / 2,

			nHeight / 2,

			nHeight / 2,

			nHeight / 2,

			90,

			-extentAngle,

			1,

			fill,

			fill
		);
	}

	this->graphics.End();
}

///////////////////////////////////////////////////////////////////////////////
// start: IVideoCoreCallback
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
void SignalGenerator::ConnectionStatusChange
///////////////////////////////////////////////////////////////////////////////
(
   uint32_t     reason,
   uint32_t     param1,
   uint32_t     param2,
   const char * sReasonString
)
///////////////////////////////////////////////////////////////////////////////
{
	std::cerr	<< "SignalGenerator::ConnectionStatusChange: "

				<< " reason: "	<< reason

				<< " param1: "	<< param1

				<< " param2: "	<< param2

				<< " sReasonString: "	<< sReasonString

				<< std::endl;

	switch(reason)
	{

	case VC_HDMI_CHANGING_MODE:

		m_bUpdateResolution = true;

		break;

	case VC_HDMI_UNPLUGGED:

		m_eVideoConnectionState = VideoConnectionState_HDMI_unplugged;

		break;

	case VC_HDMI_ATTACHED:

		if( m_eVideoConnectionState == VideoConnectionState_HDMI_unplugged )
		{
			m_bUpdateResolution = true;
		}
		
		m_eVideoConnectionState = VideoConnectionState_HDMI_connected;

		break;

	case VC_SDTV_NTSC:
	case VC_SDTV_PAL:
	case VC_HDMI_DVI:
	case VC_HDMI_HDMI:
		//
		// signal we are ready now
		//
		sem_post(&m_semTVSynced);

		break;

	default:
		;
	}
}

///////////////////////////////////////////////////////////////////////////////
// end: IVideoCoreCallback
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
void SignalGenerator::SetNativeResolution()
///////////////////////////////////////////////////////////////////////////////
{
	std::cerr << "Start: SignalGenerator::SetNativeResolution" << std::endl;

	this->graphics.Deinit();

	this->displayManager.Deinit();

	sem_init(&m_semTVSynced, 0, 0);

	if( this->videoCore.UpdateResolution() )
	{
		sem_wait(&m_semTVSynced);
	}

	sem_destroy(&m_semTVSynced);

	this->displayManager.Init();

	this->graphics.Init(&this->displayManager);

	std::cerr << "End: SignalGenerator::SetNativeResolution" << std::endl;
}
