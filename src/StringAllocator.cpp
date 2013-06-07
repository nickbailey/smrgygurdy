#include "StringAllocator.h"
#include <artifastring/artifastring_instrument.h>
//#include <iostream>

StringAllocator::StringAllocator() : bowRatioFromBridge(0.07),
                                     bowForce(2.0),
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

void StringAllocator::setSemitone(int midinote) {
	/* Stop playing current string */
	vc.reset();
	stringNo = fingering.getStringHint(midinote);
	double fingerPos = fingering.getStopPosn(midinote, stringNo);
	
	/* TODO: Find a way to enable/disable this */
	//std::cout << "Playing midinote " << midinote << " on string " << stringNo << " at position " << fingerPos << std::endl;

	vc.finger(stringNo, fingerPos);
	vc.bow (stringNo, bowRatioFromBridge, bowForce, bowSpeed);
}


void StringAllocator::setPedalSpeed(double speed) {
	bowSpeed = speed;
	vc.bow (stringNo, bowRatioFromBridge, bowForce, bowSpeed);
}
