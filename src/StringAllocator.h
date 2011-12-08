#include "artifastring/violin_instrument.h"
#ifndef STRING_ALLOCATOR_H
#define STRING_ALLOCATOR_H

/**
 * Will create violin strings and decide which string should be used for playing the various notes
 */
class StringAllocator {

	const double bowRatioFromBridge, bowForce;
	const int vGString;
	double bowSpeed;
	int stringSpace;
	ViolinInstrument violin;
	int stringNo;
	

	public:
		/**
		 * Default constructor
		 */
		StringAllocator();

		/**
	  	 * Return sample buffer containing sound samples
		 * @param buffer Array to which samples should be written                                                                                            
		 * @param bufferSize length of array
		 */
		void getSamples(short buffer[], int bufferSize);

		/**
		 * Select a given semitone on the violin and begin playing
		 *
		 * If the violin has been silenced, this will cause it to begin playing again.
		 *
		 * @param semitone Semitone value to be played (0-127)
		 */
		void setSemitone(int semitone);

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
