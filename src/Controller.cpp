#include <queue>
#include "Controller.h"
#include "SoundModel.h"
#include "Lock.h"
#include "Thread.h"
#include <string>
#include <iostream>

Controller::Controller(SoundModel *playout,
                       double speed_min,
                       double speed_max) :
	isPlaying(true), pedalTriggered(false),
	noteQueue(),
	speed(speed_max), max_speed(speed_max), min_speed(speed_min)
{
	playout->setPedalSpeed(speed);
	this->playout = playout;
}

Controller::~Controller(){
}


void Controller::run(){
	while(isPlaying){
		//Wait until a signal is received.
		outputLock.acquire();
		outputLock.wait();
		outputLock.release();

		//If notes are waiting to be passed pass them.
		queueLock.acquire();
		if(!noteQueue.empty()){
			//Acquire lock on noteQueue before reading and popping all notes from it.
		
			while(!noteQueue.empty()){
				if(noteQueue.front().noteOff)
					playout->setNoteOff(noteQueue.front().note);
				else
					playout->setNoteOn(noteQueue.front().note);
				noteQueue.pop();
			}
		
			
		}
		queueLock.release();

		//If pedal speed has changed pass new value to playout.
		pedalLock.acquire();
		if(pedalTriggered){
			outputLock.acquire();
			playout->setPedalSpeed(speed);
			outputLock.release();
			pedalTriggered = false;
		}
		pedalLock.release();
			
	}
}

void Controller::keyEvent(bool type, int note){
	//Store incoming note at end of buffer.
	queueLock.acquire();
	noteQueue.push(QueueItem(type, note));
	queueLock.release();
	
	//Broadcast change in status.
	outputLock.acquire();
	outputLock.broadcast();
	outputLock.release();
}

void Controller::speedChange(double spd){
	pedalLock.acquire();
	speed = min_speed + spd*(max_speed - min_speed);
	pedalTriggered = true;
	pedalLock.release();
	outputLock.acquire();
	outputLock.broadcast();
	outputLock.release();
}
