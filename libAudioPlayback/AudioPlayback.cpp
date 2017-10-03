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
//       $File: //depot/pi/libAudioPlayback/AudioPlayback.cpp $
//
//   $Revision: #9 $
//
//       $Date: 2017/09/26 $
//
///////////////////////////////////////////////////////////////////////////////

#include "AudioPlayback.h"

#include "libUtil/ErrorCode.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <semaphore.h>


#include "bcm_host.h"

extern "C"
{
#include "ilclient.h"
}

///////////////////////////////////////////////////////////////////////////////
// sleep and latency times
///////////////////////////////////////////////////////////////////////////////
const uint32_t g_msCTTWSleep  = 10;
const uint32_t g_msMinLatency = 20;

///////////////////////////////////////////////////////////////////////////////
// samples per buffer
///////////////////////////////////////////////////////////////////////////////
static const int g_cbSamplesPerBuffer = 1024;


///////////////////////////////////////////////////////////////////////////////
//
// The AudioPlaybackState is represents the audio engine state.
//
///////////////////////////////////////////////////////////////////////////////
struct AudioPlaybackState
///////////////////////////////////////////////////////////////////////////////
{
	sem_t m_semaphore;

	ILCLIENT_T  *m_pILClient;

	COMPONENT_T *m_pAudioRender;

	COMPONENT_T *m_aAudioRenderList[2];

	//
	// buffers owned by the client
	//
	OMX_BUFFERHEADERTYPE *m_pUserBufferList;
};

///////////////////////////////////////////////////////////////////////////////
static void InputBufferCallback
///////////////////////////////////////////////////////////////////////////////
(
	void *data,

	COMPONENT_T *comp
)
///////////////////////////////////////////////////////////////////////////////
{
	// NOP - could add code indicating more buffers are available.
}

///////////////////////////////////////////////////////////////////////////////
AudioPlayback::AudioPlayback()
///////////////////////////////////////////////////////////////////////////////
{
	this->audio_playback_state = NULL;
}

///////////////////////////////////////////////////////////////////////////////
AudioPlayback::~AudioPlayback()
///////////////////////////////////////////////////////////////////////////////
{
}

///////////////////////////////////////////////////////////////////////////////
int32_t AudioPlayback::Init
///////////////////////////////////////////////////////////////////////////////
(
	uint32_t hzSampleRate,

	uint32_t nChannels,

	uint32_t nBitDepth,

	uint32_t nBuffers,

	uint32_t cbBufferSize
)
///////////////////////////////////////////////////////////////////////////////
{
	if (hzSampleRate < 8000) return Error::ArgumentOutOfRange;

	if (hzSampleRate > 192000) return Error::ArgumentOutOfRange;

	if (nChannels < 1) return Error::ArgumentOutOfRange;

	if (nChannels > 8) return Error::ArgumentOutOfRange;

	if (nBuffers <= 0) return Error::ArgumentOutOfRange;

	uint32_t cbBytesPerSample = (nBitDepth * this->GetOutputChannels(nChannels)) / 8;

	if (cbBufferSize < cbBytesPerSample) return Error::ArgumentOutOfRange;

	if ((nBitDepth != 16) && (nBitDepth != 32)) return Error::ArgumentOutOfRange;

	this->m_hzSampleRate = hzSampleRate;

	this->m_cbBytesPerSample = cbBytesPerSample;

	this->m_nBuffers = nBuffers;


	//
	// buffer lengths must be 16 byte aligned for VCHI
	//
	int cbBufferSizeAligned = Align16(cbBufferSize);

	//
	// buffer offsets must also be 16 byte aligned for VCHI
	//

	audio_playback_state = new AudioPlaybackState;

	if (this->audio_playback_state == NULL) return Error::OutOfMemory;

	OMX_ERRORTYPE error = OMX_ErrorNone;

	//
	// create semaphore
	//
	if (sem_init(&this->audio_playback_state->m_semaphore, 0, 1) != 0) return Error::OutOfMemory;

	this->audio_playback_state->m_pILClient = ilclient_init();

	if (this->audio_playback_state->m_pILClient == NULL) return Error::OutOfMemory;

	//
	// set callback if buffer was emptied; TODO check return code
	//
	ilclient_set_empty_buffer_done_callback(this->audio_playback_state->m_pILClient, InputBufferCallback, this->audio_playback_state);

	//
	// initialize OMX
	//
	if (OMX_Init() != OMX_ErrorNone) return Error::OutOfMemory;

//
	// TODO check return error ecode
	//
	ilclient_create_component
	(
		this->audio_playback_state->m_pILClient,

		&this->audio_playback_state->m_pAudioRender,

		"audio_render",

		ILCLIENT_ENABLE_INPUT_BUFFERS | ILCLIENT_DISABLE_ALL_PORTS
	);

	if (this->audio_playback_state->m_pAudioRender == NULL) return Error::OutOfMemory;

	this->audio_playback_state->m_aAudioRenderList[0] = this->audio_playback_state->m_pAudioRender;

	//
	// set up the number/size of buffers
	//
	OMX_PARAM_PORTDEFINITIONTYPE port_definition;

	memset(&port_definition, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

	port_definition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);

	port_definition.nVersion.nVersion = OMX_VERSION;

	port_definition.nPortIndex = 100;

	if (OMX_GetParameter(ILC_GET_HANDLE(this->audio_playback_state->m_pAudioRender), OMX_IndexParamPortDefinition, &port_definition) != OMX_ErrorNone)
	{
		return Error::Fail;
	}

	port_definition.nBufferSize = cbBufferSizeAligned;

	port_definition.nBufferCountActual = nBuffers;

	if (OMX_SetParameter(ILC_GET_HANDLE(this->audio_playback_state->m_pAudioRender), OMX_IndexParamPortDefinition, &port_definition) != OMX_ErrorNone)
	{
		return Error::Fail;
	}
	//
	// set the pcm parameters
	//
	OMX_AUDIO_PARAM_PCMMODETYPE pcm_mode_type;

	memset(&pcm_mode_type, 0, sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));

	pcm_mode_type.nSize = sizeof(OMX_AUDIO_PARAM_PCMMODETYPE);

	pcm_mode_type.nVersion.nVersion = OMX_VERSION;

	pcm_mode_type.nPortIndex = 100;

	pcm_mode_type.nChannels = GetOutputChannels(nChannels);

	pcm_mode_type.eNumData = OMX_NumericalDataSigned;

	pcm_mode_type.eEndian = OMX_EndianLittle;

	pcm_mode_type.nSamplingRate = hzSampleRate;

	pcm_mode_type.bInterleaved = OMX_TRUE;

	pcm_mode_type.nBitPerSample = nBitDepth;

	pcm_mode_type.ePCMMode = OMX_AUDIO_PCMModeLinear;

	switch (nChannels)
	{

	case 1:
		pcm_mode_type.eChannelMapping[0] = OMX_AUDIO_ChannelCF;

		break;

	case 3:
		pcm_mode_type.eChannelMapping[2] = OMX_AUDIO_ChannelCF;

		pcm_mode_type.eChannelMapping[1] = OMX_AUDIO_ChannelRF;

		pcm_mode_type.eChannelMapping[0] = OMX_AUDIO_ChannelLF;

		break;

		//
		// falling through the case statements is intentional
		//
	case 8:
		pcm_mode_type.eChannelMapping[7] = OMX_AUDIO_ChannelRS;

	case 7:
		pcm_mode_type.eChannelMapping[6] = OMX_AUDIO_ChannelLS;

	case 6:
		pcm_mode_type.eChannelMapping[5] = OMX_AUDIO_ChannelRR;

	case 5:
		pcm_mode_type.eChannelMapping[4] = OMX_AUDIO_ChannelLR;

	case 4:
		pcm_mode_type.eChannelMapping[3] = OMX_AUDIO_ChannelLFE;

		pcm_mode_type.eChannelMapping[2] = OMX_AUDIO_ChannelCF;

	case 2:
		pcm_mode_type.eChannelMapping[1] = OMX_AUDIO_ChannelRF;

		pcm_mode_type.eChannelMapping[0] = OMX_AUDIO_ChannelLF;

		break;

	default:

		return Error::ArgumentOutOfRange;

	}

	if (OMX_SetParameter(ILC_GET_HANDLE(this->audio_playback_state->m_pAudioRender), OMX_IndexParamAudioPcm, &pcm_mode_type) != OMX_ErrorNone)
	{
		return Error::ArgumentOutOfRange;
	}

	ilclient_change_component_state( this->audio_playback_state->m_pAudioRender, OMX_StateIdle);


	if(ilclient_enable_port_buffers(this->audio_playback_state->m_pAudioRender, 100, NULL, NULL, NULL) >= 0)
	{
		ilclient_change_component_state(audio_playback_state->m_pAudioRender, OMX_StateExecuting);

		return Success::Ok;
	}

	//
	// error cleanup
	//
	ilclient_change_component_state( audio_playback_state->m_pAudioRender, OMX_StateLoaded);

	ilclient_cleanup_components(audio_playback_state->m_aAudioRenderList);

	error = OMX_Deinit();

	assert(error == OMX_ErrorNone);

	ilclient_destroy(audio_playback_state->m_pILClient);

	sem_destroy(&audio_playback_state->m_semaphore);

	if(audio_playback_state)
	{
		delete audio_playback_state;

		audio_playback_state = NULL;
	}

	return Error::OutOfMemory;
}

///////////////////////////////////////////////////////////////////////////////
int32_t AudioPlayback::Deinit()
///////////////////////////////////////////////////////////////////////////////
{
	if (this->audio_playback_state == NULL) return Success::Ok;

	ilclient_change_component_state(this->audio_playback_state->m_pAudioRender, OMX_StateIdle);

	if (OMX_SendCommand(ILC_GET_HANDLE(this->audio_playback_state->m_pAudioRender), OMX_CommandStateSet, OMX_StateLoaded, NULL) != OMX_ErrorNone)
	{
		return Error::Cleanup;
	}


	ilclient_disable_port_buffers
	(
		this->audio_playback_state->m_pAudioRender,

		100,

		this->audio_playback_state->m_pUserBufferList,

		NULL,

		NULL
	);

	ilclient_change_component_state(this->audio_playback_state->m_pAudioRender, OMX_StateLoaded);

	ilclient_cleanup_components(this->audio_playback_state->m_aAudioRenderList);

	if (OMX_Deinit() != OMX_ErrorNone)
	{
		return Error::Cleanup;
	}

	ilclient_destroy(this->audio_playback_state->m_pILClient);

	sem_destroy(&this->audio_playback_state->m_semaphore);

	if (this->audio_playback_state)
	{
		delete this->audio_playback_state;

		this->audio_playback_state = NULL;
	}

	return Success::Ok;
}

///////////////////////////////////////////////////////////////////////////////
uint8_t *AudioPlayback::GetBuffer()
///////////////////////////////////////////////////////////////////////////////
{
	OMX_BUFFERHEADERTYPE *pHeader = ilclient_get_input_buffer ( this->audio_playback_state->m_pAudioRender, 100, 0);

	if (pHeader == NULL) return NULL;

	sem_wait (&this->audio_playback_state->m_semaphore);

	pHeader->pAppPrivate = this->audio_playback_state->m_pUserBufferList;

	this->audio_playback_state->m_pUserBufferList = pHeader;

	sem_post (&this->audio_playback_state->m_semaphore);

	return pHeader->pBuffer;
}

///////////////////////////////////////////////////////////////////////////////
int32_t AudioPlayback::PlayBuffer
///////////////////////////////////////////////////////////////////////////////
(
	uint8_t *buffer,

	uint32_t length
)
///////////////////////////////////////////////////////////////////////////////
{
	if( length % this->m_cbBytesPerSample ) return Error::ArgumentOutOfRange;

	sem_wait (&this->audio_playback_state->m_semaphore);

	//
	// search through user list for the right buffer header
	//
	OMX_BUFFERHEADERTYPE *pHeader   = this->audio_playback_state->m_pUserBufferList;

	OMX_BUFFERHEADERTYPE *pPrevious = NULL;

	while (pHeader != NULL	&& pHeader->pBuffer != buffer && pHeader->nAllocLen < length)
	{
		pPrevious = pHeader;

		pHeader = (OMX_BUFFERHEADERTYPE*) pHeader->pAppPrivate;
	}

	if(pHeader) 
	{
		//
		// we found it => remove from list linked list
		//
		if(pPrevious)
		{
			pPrevious->pAppPrivate = pHeader->pAppPrivate;
		}
		else
		{
			this->audio_playback_state->m_pUserBufferList = (OMX_BUFFERHEADERTYPE*) pHeader->pAppPrivate;
		}
	}
	
	sem_post (&this->audio_playback_state->m_semaphore);

	if(pHeader)
	{
		pHeader->pAppPrivate = NULL;

		pHeader->nOffset     = 0;

		pHeader->nFilledLen  = length;

		if ( OMX_EmptyThisBuffer (ILC_GET_HANDLE(this->audio_playback_state->m_pAudioRender), pHeader) != OMX_ErrorNone )
		{
			return Error::Fail;
		}
	}

	return Success::Ok;
}

///////////////////////////////////////////////////////////////////////////////
uint32_t AudioPlayback::GetLatency()
///////////////////////////////////////////////////////////////////////////////
{
	OMX_PARAM_U32TYPE param;

	memset(&param, 0, sizeof(OMX_PARAM_U32TYPE));

	param.nSize = sizeof(OMX_PARAM_U32TYPE);

	param.nVersion.nVersion = OMX_VERSION;

	param.nPortIndex = 100;

	if ( OMX_GetConfig (ILC_GET_HANDLE (this->audio_playback_state->m_pAudioRender), OMX_IndexConfigAudioRenderingLatency, &param) != OMX_ErrorNone )
	{
		return Error::Fail;
	}

	return param.nU32;
}

///////////////////////////////////////////////////////////////////////////////
int32_t AudioPlayback::SetDestination
///////////////////////////////////////////////////////////////////////////////
(
	const char *name
)
///////////////////////////////////////////////////////////////////////////////
{
	OMX_CONFIG_BRCMAUDIODESTINATIONTYPE audio_destination_type;
	
	memset (&audio_destination_type, 0, sizeof(audio_destination_type));
	
	audio_destination_type.nSize = sizeof(OMX_CONFIG_BRCMAUDIODESTINATIONTYPE);
	
	audio_destination_type.nVersion.nVersion = OMX_VERSION;

	if ( NULL != name && strlen(name) < sizeof(audio_destination_type.sName) )
	{
		strcpy ( (char *)audio_destination_type.sName, name);
		
		if (OMX_SetConfig( ILC_GET_HANDLE(this->audio_playback_state->m_pAudioRender), OMX_IndexConfigBrcmAudioDestination, &audio_destination_type) != OMX_ErrorNone)
		{
			return Error::Fail;
		}
	}

	return Success::Ok;
}

///////////////////////////////////////////////////////////////////////////////
void AudioPlayback::Sleep()
///////////////////////////////////////////////////////////////////////////////
{
	//
	// try and wait for a minimum latency time (in ms) before
	// sending the next packet
	//

	uint32_t msMinLatency = (this->m_hzSampleRate * (g_msMinLatency + g_msCTTWSleep) / 1000);

	while( GetLatency() > msMinLatency )
	{
		usleep (g_msCTTWSleep * 1000);
	}
}

///////////////////////////////////////////////////////////////////////////////
// static
///////////////////////////////////////////////////////////////////////////////
int AudioPlayback::GetSamplesPerBuffer()
///////////////////////////////////////////////////////////////////////////////
{
	return g_cbSamplesPerBuffer;
}

///////////////////////////////////////////////////////////////////////////////
// static
///////////////////////////////////////////////////////////////////////////////
int AudioPlayback::GetBufferSize
///////////////////////////////////////////////////////////////////////////////
(
	int nBitDepth,

	int nChannels
)
///////////////////////////////////////////////////////////////////////////////
{
	return (g_cbSamplesPerBuffer * nBitDepth * GetOutputChannels(nChannels)) / 8;
}

///////////////////////////////////////////////////////////////////////////////
// static
///////////////////////////////////////////////////////////////////////////////
int AudioPlayback::GetOutputChannels
///////////////////////////////////////////////////////////////////////////////
(
	int nChannels
)
///////////////////////////////////////////////////////////////////////////////
{
	int nOutputChannels = 0;

	switch(nChannels)
	{
	case 1:
	case 2:
		nOutputChannels = nChannels;
		break;

	case 3:
	case 4:
		nOutputChannels = 4;
		break;

	case 5:
	case 6:
	case 7:
	case 8:
		nOutputChannels = 8;
		break;

	default:
		nOutputChannels = 0;
	}

	return nOutputChannels;
}
