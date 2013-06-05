#include <cmath>
#include "StringAllocator.h"
#include <artifastring/artifastring_instrument.h>
//#include <iostream>

StringAllocator::StringAllocator(): 

	bowRatioFromBridge(0.07),
	bowForce(2.0),
	vCString(36), /* 'Cello C is MIDI note 36 */
	stringSpace(7),
	vc(Cello) {

	this->bowSpeed = 0;
	this->stringNo = 0;
	silence();
	
}

void StringAllocator::getSamples(short buffer[], int bufferSize) {

	vc.wait_samples (buffer, bufferSize);
}

void StringAllocator::silence() {

	vc.bow (stringNo, bowRatioFromBridge, 0, 0);
}

void StringAllocator::setSemitone(int semitone) {
	
	int stringDelta;
	double fingerPos;

	/* Stop playing current string */
	vc.reset();

	/* Currently cannot support semitones values lower than vGString */
	if(semitone < vCString) {
		throw "SemitoneOutOfRangeException";
	}

	stringNo = ( (semitone - vCString) / stringSpace );

	if ( stringNo <= 3 ) {
		stringDelta =  ( (semitone - vCString) % stringSpace );
	} else { 
		stringNo = 3;
		stringDelta = ( (semitone - ( vCString + stringSpace*3 )) );
	}

	fingerPos = ( 1.0 - pow(2.0, -(double)stringDelta / 12.0) );	

	/* TODO: Find a way to enable/disable this */
	//std::cout << "Playing semitone " << semitone << " on string " << stringNo << " with delta " << stringDelta << " at position " << fingerPos << std::endl;

	vc.finger(stringNo, fingerPos);
	vc.bow (stringNo, bowRatioFromBridge, bowForce, bowSpeed);
}


void StringAllocator::setPedalSpeed(double speed) {
	
	bowSpeed = speed;
	//std:: cout << "StringAllocator: Changing pedal speed to " << speed << std::endl;
	vc.bow (stringNo, bowRatioFromBridge, bowForce, bowSpeed);
}
