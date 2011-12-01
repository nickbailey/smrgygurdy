#include "StringAllocator.h"
#include "SoundModelMono.h"
#include "OutputSink.h"
#include "Lock.h"
#include <string>
#include <iostream>

#define RELEASE_INIT 40960
#define DEFAULT_PEDAL_SPEED 1;

SoundModelMono::SoundModelMono() {

	this->noteOn = false;
	this->currentNote = -1;
	this->release = 0;
	this->pedalSpeed = DEFAULT_PEDAL_SPEED; 

}

void SoundModelMono::getSamples(short samples[], int bufferSize) {

	lock.acquire();

	/* Allows us to render sound when in release or on state */
	if(noteOn || release > 0) {
		violin.getSamples(samples, bufferSize);
		release -= bufferSize;
		if(release < 0) release = 0;
	}  else {
		memset(samples, 0, bufferSize*sizeof(short));
		/* This is negligable in terms of time compared to rendering the sound */
	}


	lock.release();
}

bool SoundModelMono::isPlaying() {

	bool out;

	lock.acquire();

	out = noteOn;

	lock.release();

	return out;

}

void SoundModelMono::setNoteOn(int semitone) {

	lock.acquire();

	if(noteOn == false || currentNote != semitone) {

		try{
			violin.setSemitone(semitone);
			violin.setPedalSpeed(pedalSpeed);
			noteOn = true;
			currentNote = semitone;

		} catch (const char* e) {
			std::cout << "Semitone out of range!" << std::endl;
		}
	}

	lock.release();

}

void SoundModelMono::setNoteOff(int semitone) { 

	lock.acquire();

	/* Only turn of if we're actually playing that note */
	if(currentNote == semitone) { 
		noteOn = false;
		release = RELEASE_INIT;
		violin.silence();
	}

	lock.release();

}

void SoundModelMono::setPedalSpeed(double speed) {

	lock.acquire();

	if(noteOn) violin.setPedalSpeed(speed); /* This avoids the model playing sound */
	pedalSpeed = speed;

	lock.release();

}

