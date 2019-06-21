#ifndef STRING_ALLOCATOR_H
#define STRING_ALLOCATOR_H
#include "artifastring/artifastring_instrument.h"
#include "artifastring/artifastring_defines.h"
#include "ViolinFingering.h"

/**
 * Will create violin strings and decide which string should be used for playing the various notes
 */
class StringAllocator {

	ViolinFingering fingering;
	const double bowRatioFromBridge, bowForce;
	double bowSpeed;
	int stringSpace;
	ArtifastringInstrument vc;
	int stringNo;

	public:
		/**
		 * Default constructor
		 * 
		 * @param sr operating sample rate
		 */
		StringAllocator(const int sr);

		/**
	  	 * Return sample buffer containing sound samples
		 * @param buffer Array to which samples should be written                                                                                            
		 * @param bufferSize length of array
		 */
		void getSamples(short buffer[], int bufferSize);

		/**
		 * Select a fingering position corresponding to the given MIDI note number on the
		 * violin and begin playing
		 *
		 * If the violin has been silenced, this will cause it to begin playing again.
		 *
		 * @param midinote Semitone value to be played (0-127)
		 */
		void setSemitone(int midinote);

		/**
		 * Set speed at which the string is being bowed.
		 *
		 * If the violin has been silenced, this will cause it to begin playing again.
		 *
		 * @param speed Speed at which sound is being played (0-TODO:decide)
		 */
		void setPedalSpeed(double speed);

		/**
		 * Silence the playback of the violin
		 */
		void silence();

};

#endif /* STRING_ALLOCATOR_H */
