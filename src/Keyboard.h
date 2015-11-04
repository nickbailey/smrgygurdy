#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "Thread.h"
#include "Lock.h"
#include "Controller.h"
#include <alsa/asoundlib.h>

/**
 * Provides interface for the ALSA Sequencer API.
 * The sequencer client subscribes to a USB keyboard client with a specified id
 * and polls it for incoming MIDI events. It passes the appropriate commands
 * to an instance of Controller based on what it receives.
 */

class Keyboard : public Thread {
  
	Controller* controller;
	snd_seq_t* sequencer;
	int myId;
	int myPort;
	int keyboardId;
	int keyboardPort;
	int announce;

	/**
	 * Creates a new ALSA sequencer client
	 */		
	void createSequencer();


	/**
	 * Handle a MIDI event
	 *
	 * Read a MIDI event and takes an appropriate action. Possible events:
	 * CONTROLLER, PITCHBEND, NOTEON, NOTEOFF
	 *
	 * @param seq_handle pointer to the sequencer handle
	 */
	void midiAction();

	void connectKeyboard();

	public:
  
		/**
		 * Default constructor
		 * @param controller An instance of Controller class
		 * @param keyId ALSA sequencer client id of the keyboard
		 * @param keyPort Port number on the given client
		 * @param verbosity Print note on/off announcement if > 0
		 */
		Keyboard(Controller* controller, int keyId, int keyPort,
		         int verbosity = 1);
		
		/**
		 * Poll the MIDI keyboard for key events.
		 */
		virtual void run();

};

#endif /* KEYBOARD_H */
