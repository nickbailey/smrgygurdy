#ifndef SOUNDMODEL_H 
#define SOUNDMODEL_H

/**
 * Interface for classes providing the ability to play notes.
 */
class SoundModel {

	public:

		/**
		 * Set that a note should begin playing, may turn off currently playing notes if maximum polyphony is exceeded
		 *
		 * @param semitone The semitone value of the activated key (0-127)
		 */
		virtual void setNoteOn(int semitone) = 0;

		/**
		 * Set that a note should stop playing
		 *
		 * This will only have an effect if the current note is playing.
		 *
		 * @param semitone The semitone value of the deactivated key (0-127)
		 */
		virtual void setNoteOff(int semitone) = 0;

		/**
		 * Update the current pedal speed.
		 *
		 * @param speed Speed value of the pedal, measured in metres per second
		 */
		virtual void setPedalSpeed(double speed) = 0;

		/**
		 * Return true if there is currently at least one note playing
		 */
		virtual bool isPlaying() = 0;

		/**
         * Render a period of sound data and store in samples. 
         *
         * @param samples Array to which samples should be written
         * @param bufferSize length of array
         */
         virtual void getSamples(short samples[], int bufferSize) = 0;


};

#endif /* SOUNDMODEL_H */
