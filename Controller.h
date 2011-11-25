#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Thread.h"
#include "Lock.h"
#include "SoundModel.h"
#include <queue>
#include <string>

 /**
 *Controls interaction between input and output stages of the hurdy gurdy.
 *
 */

class Controller: public Thread{

	/**
	 *An item on the noteQueue.
	 */
	class QueueItem{
	public:
		//True if the
		bool noteOff;
		int note;

		/**
		 *Creates a new item and initialises its variables.
		 */
		QueueItem(bool noteOff, int note){
			this->noteOff = noteOff;
			this->note = note;
		}

	};
	

  //Locks for signalling of changes in keyboard or pedal input.
  Lock outputLock;

  //SoundModel itself.
  SoundModel *playout;

  //Variable for storing the current status of playout.
  bool isPlaying;

  //Stores whether or not the pedal has triggered a broadcast.
  bool pedalTriggered;

  //Buffers notes sent by keyboard.  Notes are stored in instantiations of type QueueItem.
  std::queue<QueueItem> noteQueue;

  //Lock for accessing the noteQueue.
  Lock queueLock;

  //Lock for accessing the pedal variables.
  Lock pedalLock;

  //Stores last pedal speed sent by pedal.
  double speed;



 public:

  /**
   *Creates a new controller object.
   *
   *@param playout Soundmodel to which controller passes signals.
   */
  Controller(SoundModel *playout);

  /**
   *Called when a controller object is destroyed.
   */
  ~Controller();


   /**
   *Called by keyboard interface whenever an event happens.
   *
   *Stores notes in the noteQueue while waiting for processor time.
   *
   *@param noteOff is a boolean which is true if the event is turning a note off.
   *@param note indicates which note to turn off.
   */
  void keyEvent(bool noteOff, int note);

  /**
   *Starts the running of playout
   */
  virtual void run();

  /**
   *Called by pedal interface whenever speed changes significantly.
   *
   *Calls setPedalSpeed method in playout to pass on new value.
   *
   *@param speed Speed at which bow should rotate.
   */
  void speedChange(double speed);
};

#endif /* CONTROLLER_H */
