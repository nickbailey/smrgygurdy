#include "OutputDirect.h"
#include <string>
#include <alsa/asoundlib.h>
#include <poll.h>
#include <iostream>

#define CHANNELS 1 

OutputDirect::OutputDirect(std::string pcmName, unsigned int periodSize, unsigned int rate) :
    underrunning(false)
{
	playbackHandle = openPcm(pcmName, periodSize, rate);
}

snd_pcm_t * OutputDirect::openPcm(std::string pcmName, unsigned int periodSize, unsigned int rate) {

    snd_pcm_t *playbackHandle;

    if (snd_pcm_open (&playbackHandle, pcmName.c_str(), SND_PCM_STREAM_PLAYBACK, 0) < 0) {
		std::cerr << "OutputDirect: Cannot open audio device " << pcmName << std::endl;
		throw new UnableToOpenPCMException;
    }

    snd_pcm_hw_params_t *hardwareParams;
    snd_pcm_hw_params_alloca(&hardwareParams);
    snd_pcm_hw_params_any(playbackHandle, hardwareParams);
    snd_pcm_hw_params_set_access(playbackHandle, hardwareParams, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(playbackHandle, hardwareParams, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_rate_near(playbackHandle, hardwareParams, &rate, 0);
    snd_pcm_hw_params_set_channels(playbackHandle, hardwareParams, CHANNELS);
    snd_pcm_hw_params_set_periods(playbackHandle, hardwareParams, 1, 0);
    snd_pcm_hw_params_set_period_size(playbackHandle, hardwareParams, periodSize, 0);
    snd_pcm_uframes_t bufsize = periodSize * 2;
    snd_pcm_hw_params_set_buffer_size_near(playbackHandle, hardwareParams, &bufsize);
    snd_pcm_hw_params(playbackHandle, hardwareParams);

    snd_pcm_sw_params_t *softwareParams;
    snd_pcm_sw_params_alloca(&softwareParams);
    snd_pcm_sw_params_current(playbackHandle, softwareParams);
    snd_pcm_sw_params_set_avail_min(playbackHandle, softwareParams, periodSize);
    snd_pcm_sw_params(playbackHandle, softwareParams);

    return(playbackHandle);

}

OutputDirect::~OutputDirect() {
	// Nothing to do here
}

void OutputDirect::writeSamples(short buffer[], int length) {
	while((snd_pcm_writei(playbackHandle, buffer, length)) < length) {
				
		// Buffer XRUN occured here
		snd_pcm_prepare(playbackHandle); /* Needs to be called to recover */
		reportUnderrun();
	}

	if (underrunning > 0) underrunning--;
}

void OutputDirect::reportUnderrun(void) {
	if (!underrunning) {
		std::cerr << "OutputDirect: Buffer underrun occurred" <<
		             std::endl;
	}
	underrunning = 20;
}
		

void OutputDirect::close() {

	snd_pcm_drain (playbackHandle); // Block until all sound in ALSA buffer has been played
	snd_pcm_close (playbackHandle);
}
