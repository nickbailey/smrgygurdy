#include "OutputDirect.h"
#include <string>
#include <iostream>
#include <alsa/asoundlib.h>
#include <poll.h>

#define CHANNELS 1 

OutputDirect::OutputDirect(std::string pcmName, unsigned int periodSize, unsigned int rate)
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
    snd_pcm_hw_params_set_period_size(playbackHandle, hardwareParams, periodSize/2, 0);
    snd_pcm_hw_params(playbackHandle, hardwareParams);

    snd_pcm_sw_params_t *softwareParams;
    snd_pcm_sw_params_alloca(&softwareParams);
    snd_pcm_sw_params_current(playbackHandle, softwareParams);
    snd_pcm_sw_params_set_avail_min(playbackHandle, softwareParams, periodSize/2);
    snd_pcm_sw_params(playbackHandle, softwareParams);

    return(playbackHandle);

}

OutputDirect::~OutputDirect() {

	// Nothing to do here
}

void OutputDirect::writeSamples(short buffer[], int length) {

	/* There is something interesting going on here where it requires
	   the length to be divided by two. If this is removed, bad things
	   happen. It doesn't seem to be a channel issue or a data size
	   issue */

	//while((snd_pcm_writei(playbackHandle, buffer, length/2)) < length/2) {
	while((snd_pcm_writei(playbackHandle, buffer, length)) < length) {
				
		// Buffer XRUN occured here
		snd_pcm_prepare(playbackHandle); /* Needs to be called to recover */
		std::cerr << "OutputDirect: Buffer underrun occurred" << std::endl;

	}
}

void OutputDirect::close() {

	snd_pcm_drain (playbackHandle); // Block until all sound in ALSA buffer has been played
	snd_pcm_close (playbackHandle);
}
