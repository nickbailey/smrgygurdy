#ifndef ALSA_ADAPTOR_H
#define ALSA_ADAPTOR_H

#include <alsa/asoundlib.h>
#include <string>
#include <poll.h>
#include "OutputAdaptor.h"

/**
 * Contains the connection to alsa, with a method to write directly to the PCM.
 *
 * This is essentailly an adaptor for alsa's snd_pcm_t type, and hence
 * inherits much of its behaviour.
 *
 * A name, period size and rate are required to open the PCM. The
 * sound data is assumed to contain a single channel, at a bit depth of 16.
 *
 * Sound is written to the output by writeSamples method, which must
 * be called periodically thereafter to create a continuous stream of sound to be played.
 */
class ALSAAdaptor : public OutputAdaptor {

	snd_pcm_t *playbackHandle;	///< Handle for alsa PCM
	int underrunning;		///< State variable for underrun reporting
	unsigned int bufsize;		///< Size of audio buffer
	
public:

	/**
	 * Create new ALSAAdaptor and prepare it for incoming data
	 *
	 * This may fail if there is another application using the hardware.
	 *	
	 * @param pcmName name of pcm to open (e.g. "hw:0")
	 * @param periodSize Size in samples of period data that will be written
	 * @param sampleRate Rate in hertz at which sound is sampled (Eg: 44100)
	 * @throws OutputAdaptorException Thrown if PCM cannot be opened for some reason (E.g it is in use)
	 */
	ALSAAdaptor(std::string pcmName, unsigned int periodSize, unsigned int sampleRate);

	/**
	 * Destructor
	 */
	virtual ~ALSAAdaptor();

	/**
	 * Write some samples to the output.
	 * 
	 * Following the first write, this should be called periodically
	 * to create a continuous stream of sound to be played. Failure to
	 * do so may result in sound artifacts and an XRUN message will be
	 * printed to stderr.  @param buffer Buffer containing the sample
	 * data 
	 *
	 * @param length Number of samples in the buffer
	 */
	virtual void writeSamples(short buffer[]);

private:
	/**
	 * Open a pcm, returns playback handle for pcm
	 *
	 * @param pcmName name of pcm to open (e.g. "hw:0")
	 * @param periodSize Size of period data that will be written
	 * @param sampleRate Rate in hertz at which sound is sampled (Eg: 44100)
	 * @throws OutputAdaptorException Thrown if PCM cannot be opened for whatever reason
	 */
	snd_pcm_t* openPcm(std::string pcmName, unsigned int periodSize, unsigned int rate);

	/**
	 * Report buffer underruns, but only once
	 *
	 * The system won't ever recover from an underrun if it's
	 * spending extra CPU effort telling the user about them,
	 * so print the error message once when the first occurs,
	 * then keep quiet unless a non-underrunning write has
	 * succeeded.
	 */
	void reportUnderrun(void);
};

#endif // AUDIO_OUTPUT_ADAPTOR_H
