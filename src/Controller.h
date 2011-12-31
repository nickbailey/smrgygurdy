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
   *@param speed_min Bow speed minimum pedal value (0)
   *@param speed_max Bow speed for max pedal value (1.0)
   */
  Controller(SoundModel *playout,
             double speed_min = 0.15, double speed_max = 1.6);

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
   * Called by pedal interface whenever speed changes significantly.
   * The value supplied is a double in the range 0.0-1.0.
   * This is mapped into a bow speed in the range given when the
   * object is constructed (defaults are supplied).
   *
   * Calls setPedalSpeed method in playout to pass on new value.
   *
   * @param speed Speed at which bow should rotate.
   */
  void speedChange(double spd);

  /**
   * Access the current physical bow speed
   */
  double get_bow_speed() const { return speed; }
 private:
  double max_speed; ///< maximum bow speed (pedal = 1.0)
  double min_speed; ///< minumum bow speed (pedal = 0.0)
};

#endif /* CONTROLLER_H */
