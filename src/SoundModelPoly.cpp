#include "SoundModelPoly.h"
#include "SoundModelMono.h"
#include <cstring>
#include <vector>
//#include <iostream>

SoundModelPoly::SoundModelPoly(std::vector<SoundModel*> soundModelList,
                               double output_gain) {

	this->soundModelList = soundModelList;

	if(soundModelList.size() == 0)
		throw new NoSoundModelException();

	setOutputGain(output_gain);

}

SoundModelPoly::SoundModelPoly(int poly, double output_gain) {
	for (int i = 0; i < poly; i++) {
		soundModelList.push_back(new SoundModelMono);
	}
	setOutputGain(output_gain);
}

void SoundModelPoly::setOutputGain(double output_gain) {
	gain = output_gain;

	// tell all agregated SoundModelPoly's the gain's changed
	// (SoundModelMonos should ingore this virtual method invocation)
	for(std::vector<SoundModel*>::iterator sndModIterator =
	        soundModelList.begin();
	    sndModIterator != soundModelList.end();
	    sndModIterator++) {
		(*sndModIterator)->setOutputGain(gain);
	}
}

void SoundModelPoly::setNoteOn(int midinote) {

	for(unsigned int i = 0; i < soundModelList.size(); i++)
		if(!(soundModelList[i]->isPlaying())) {
			soundModelList[i]->setNoteOn(midinote);
			lastSoundModel = i;
			return;
		}

	/* Remember the last sound model to better distribute the keypresses */
	/* Only gets invoked if all contained models are playing */
	(++lastSoundModel) %= soundModelList.size();
	soundModelList[lastSoundModel]->setNoteOn(midinote);
}

void SoundModelPoly::setNoteOff(int midinote) {

	for(std::vector<SoundModel*>::iterator sndModIterator =
	        soundModelList.begin();
	    sndModIterator != soundModelList.end();
	    sndModIterator++) {
		(*sndModIterator)->setNoteOff(midinote);
	}
}

void SoundModelPoly::setPedalSpeed(double speed) {

	/* Need to send to all sub-soundModels */
	for(std::vector<SoundModel*>::iterator sndModIterator =
	        soundModelList.begin();
	    sndModIterator != soundModelList.end();
	    sndModIterator++) {
		(*sndModIterator)->setPedalSpeed(speed);
	}
}

bool SoundModelPoly::isPlaying() {

	for(std::vector<SoundModel*>::iterator sndModIterator =
	        soundModelList.begin();
	    sndModIterator != soundModelList.end();
	    sndModIterator++) {
		if((*sndModIterator)->isPlaying()) return true;
	}

	return false;
}

void SoundModelPoly::getSamples(short samples[], const int bufferSize) {

	short temp[bufferSize];
	int accumulator[bufferSize];
	memset(accumulator, 0, bufferSize*sizeof(int));

	int i;

	for(std::vector<SoundModel*>::iterator sndModIterator =
	        soundModelList.begin();
	    sndModIterator != soundModelList.end();
	    sndModIterator++) {
		(*sndModIterator)->getSamples(temp, bufferSize);
		for (i = 0; i < bufferSize; i++) {
			accumulator[i] += temp[i];
		}
	}

	for(i = 0; i < bufferSize; i++)
		samples[i] = accumulator[i] * gain;
}

