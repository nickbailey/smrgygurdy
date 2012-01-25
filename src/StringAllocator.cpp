#include <cmath>
#include "StringAllocator.h"
#include <artifastring/violin_instrument.h>
#include <iostream>

StringAllocator::StringAllocator(): 

	bowRatioFromBridge(0.07),
	bowForce(2.0),
	vGString(36), /* TODO: Get the real value for this - semitone value of lowest string on violin */
	stringSpace(7),
	violin(5) { // 5 means 'cello (?!?)

	this->bowSpeed = 0;
	this->stringNo = 0;
	silence();
	
}

void StringAllocator::getSamples(short buffer[], int bufferSize) {

	violin.wait_samples (buffer, bufferSize);
}

void StringAllocator::silence() {

	violin.bow (stringNo, bowRatioFromBridge, 0, 0);
}

void StringAllocator::setSemitone(int semitone) {
	
	int stringDelta;
	double fingerPos;

	/* Stop playing current string */
	violin.reset();

	/* Currently cannot support semitones values lower than vGString */
	if(semitone < vGString) {
		throw "SemitoneOutOfRangeException";
	}

	stringNo = ( (semitone - vGString) / stringSpace );

	if ( stringNo <= 3 )
		stringDelta =  ( (semitone - vGString) % stringSpace );
	
	else { 
		stringNo = 3;
		stringDelta = ( (semitone - ( vGString + stringSpace*3 )) );
	}

	fingerPos = ( 1.0 - pow(2.0, -(double)stringDelta / 12.0) );	

	/* TODO: Find a way to enable/disable this */
	//std::cout << "Playing semitone " << semitone << " on string " << stringNo << " with delta " << stringDelta << " at position " << fingerPos << std::endl;

	violin.finger(stringNo, fingerPos);
	violin.bow (stringNo, bowRatioFromBridge, bowForce, bowSpeed);
}


void StringAllocator::setPedalSpeed(double speed) {
	
	bowSpeed = speed;
	//std:: cout << "StringAllocator: Changing pedal speed to " << speed << std::endl;
	violin.bow (stringNo, bowRatioFromBridge, bowForce, bowSpeed);
}
