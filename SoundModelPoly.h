#include "SoundModel.h"
#include <string>
#include <vector>

#ifndef SOUND_MODEL_POLY_H
#define SOUND_MODEL_POLY_H

/**
 * Will play multiple notes at once. 
 *
 * This works by dividing the work amongst an array of SoundModel's which are passed in through the constructor.
 */
class SoundModelPoly : public SoundModel {

	std::vector<SoundModel*> soundModelList;
	int	lastSoundModel;
	int	soundModelNo;
	
	public:

	
		/**
		 * Create a new instance of SoundModelPoly
		 *
		 * This takes in a vector of sound models. These will usually
		 * be SoundModelMono instances, to give the polyphonic
		 * effect. However, they could also be SoundModelPoly
		 * instances, which could be useful for allowing each instance
		 * to be rendered by a single thread, but having note and
		 * pedal data controlled from a central instance.
		 * 
		 * @param soundModelList Vector containing list of models to be contained within the instance
		 */
		SoundModelPoly(std::vector<SoundModel*> soundModelList);

		/**
		 * Start a note playing. 
		 *
		 * This will try to prioritise SoundModels which are not
		 * playing over ones which are, to ensure that maximum
		 * polyphony is achieved.
		 *
		 * @param semitone Semitone value of note to turn on
		 */
		virtual void setNoteOn(int semitone);

		/**
		 * Stop a note playing
		 *
		 * This simply sends the same message to all contained
		 * SoundModels. Hence multiple contained models are playing
		 * the same note, the note will be disabled on all models.
		 *
		 * @param semitone Semitone alue of note to turn off
		 */
		virtual void setNoteOff(int semitone);

		/**
		 * Set the pedal speed of all contained sound models
		 *
		 * @param speed Pedal speed in m/s
		 */
		virtual void setPedalSpeed(double speed);

		/**
		 * @return True if a note is currently being played by and of
		 * the contained models.
		 */
		virtual bool isPlaying();

		/**
		 * Render a frame of samples of all contained models into a
		 * buffer, and advanced time by the length of the buffer.
		 *
		 * @param samples Buffer where samples should be stored. Must have enough space for requested data.
		 * @param bufferSize Size in samples of the buffer
		 */
		virtual void getSamples(short samples[], int bufferSize);

		/** Thrown when empty SoundModel vector passed into constructor */
		class NoSoundModelException {};

};

#endif /* SOUND_MODEL_POLY_H */
