#include "SoundModelPoly.h"
#include <cstring>
#include <vector>

/* All samples will be multiplied by this constant to avoid clipping */
#define MIX_MULTIPLIER 0.5 /* TODO: This needs to be centralised better */

SoundModelPoly::SoundModelPoly(std::vector<SoundModel*> soundModelList) {

	this->soundModelList = soundModelList;

	if(soundModelList.size() == 0)
		throw new NoSoundModelException();

}

void SoundModelPoly::setNoteOn(int semitone) {

	for(unsigned int i = 0; i < soundModelList.size(); i++)
		if(!(soundModelList[i]->isPlaying())) {
			soundModelList[i]->setNoteOn(semitone);
			lastSoundModel = i;
			return;
		}

	/* Remember the last sound model to better distribute the keypresses */
	/* Only gets invoked if all contained models are playing */
	(++lastSoundModel) %= soundModelList.size();
	soundModelList[lastSoundModel]->setNoteOn(semitone);


}

void SoundModelPoly::setNoteOff(int semitone) {

	for(unsigned int i = 0; i < soundModelList.size(); i++)
		soundModelList[i]->setNoteOff(semitone);
}

void SoundModelPoly::setPedalSpeed(double speed) {

	/* Need to send to all sub-soundModels */
	for(unsigned int i = 0; i < soundModelList.size(); i++) {

		soundModelList[i]->setPedalSpeed(speed);
	}
}

bool SoundModelPoly::isPlaying() {


	for(unsigned int i = 0; i < soundModelList.size(); i++)
		if(soundModelList[i]->isPlaying()) return true;

	return false;
}

void SoundModelPoly::getSamples(short samples[], int bufferSize) {

	short *temp = new short[bufferSize];
	memset(samples, 0, bufferSize*sizeof(short));

	for(unsigned int i = 0; i < soundModelList.size(); i++) {
		soundModelList[i]->getSamples(temp, bufferSize);
		for(int j = 0; j < bufferSize; j++)
			samples[j] += temp[j] * MIX_MULTIPLIER;
	}

	delete[] temp;
}

