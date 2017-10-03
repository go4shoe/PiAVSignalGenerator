////////////////////////////////////////////////////////////////////////////////
// Audio output demo using OpenMAX IL though the ilcient helper library
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <semaphore.h>

#include "bcm_host.h"

#include "AudioPlayback.h"
#include "SineWave.h"


#define PI (1<<16>>1)

#define SIN(x) g_aSineWaveValues[((x)>>6) & (g_nSineWaveEntries-1)]

#define COS(x) SIN((x)+(PI>>1))



////////////////////////////////////////////////////////////////////////////////
static const char * g_aAudioDestination[] =
////////////////////////////////////////////////////////////////////////////////
{
   "local",
   "hdmi"
};

////////////////////////////////////////////////////////////////////////////////
void TestPlayTone
////////////////////////////////////////////////////////////////////////////////
(
   int hzSampleRate,
   int nBitDepth,
   int nChannels,
   int eOutputDestination
)
////////////////////////////////////////////////////////////////////////////////
{
   AudioPlayback * pAudioPlayback = NULL;

   int32_t ret = 0;
   int phase   = 0;
   int inc     = 256<<16;
   int dinc    = 0;
   
   int cbBufferSize = AudioPlayback::GetBufferSize( nBitDepth,
                                                    nChannels);

   assert(eOutputDestination == 0 || eOutputDestination == 1);

   //
   // create playback object
   // 
   pAudioPlayback = new AudioPlayback;

   pAudioPlayback->Init(   hzSampleRate,
                           nChannels,
                           nBitDepth,
                           10,
                           cbBufferSize);
   //
   // set output destination to HDMI embedded audio or analog out
   //
   ret = pAudioPlayback->SetDestination(g_aAudioDestination[eOutputDestination]);

   assert(ret == 0);

   // iterate for 5 seconds worth of packets

   for (int n=0; n < ((hzSampleRate * 1000) / AudioPlayback::GetSamplesPerBuffer()); n++)
   {
      uint8_t * pBuffer = NULL;

      while((pBuffer = pAudioPlayback->GetBuffer()) == NULL)
      {
         usleep(10*1000);
      }

      int16_t * pBufferS16 = (int16_t *) pBuffer;

      //
      // fill the buffer
      //
      for (int i=0; i < AudioPlayback::GetSamplesPerBuffer(); i++)
      {
         int16_t val = SIN(phase);

         phase += inc >> 16;
         inc   += dinc;

         if (inc>>16 < 512)
         {
            dinc++;
         }
         else
         {
            dinc--;
         }

         for(int j=0; j< AudioPlayback::GetOutputChannels(nChannels); j++)
         {
            if (nBitDepth == 32)
            {
               *pBufferS16++ = 0;
            }
            *pBufferS16++ = val;
         }
      }

      //
      // try and wait for a minimum latency time (in ms) before
      // sending the next packet
      //
      pAudioPlayback->Sleep();


      ret = pAudioPlayback->PlayBuffer(pBuffer, cbBufferSize);

      assert(ret == 0);
   }

   delete pAudioPlayback;
}

////////////////////////////////////////////////////////////////////////////////
int main
////////////////////////////////////////////////////////////////////////////////
(
   int     argc,
   char ** argv
)
////////////////////////////////////////////////////////////////////////////////
{
   // 0=headphones, 1=hdmi
   int audio_dest = 1;

   // audio sample rate in Hz
   int samplerate = 48000;

   // numnber of audio channels
   int channels = 2;

   // number of bits per sample
   int bitdepth = 16;

   bcm_host_init();

   if (argc > 1)
      audio_dest = atoi(argv[1]);
   if (argc > 2)
      channels = atoi(argv[2]);
   if (argc > 3)
      samplerate = atoi(argv[3]);

   printf("Outputting audio to %s\n", audio_dest==0 ? "analogue":"hdmi");

   TestPlayTone(samplerate, bitdepth, channels, audio_dest);

   return 0;
}

