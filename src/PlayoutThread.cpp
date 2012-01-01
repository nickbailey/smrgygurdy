#include "SoundModelMono.h"
#include "OutputSink.h"
#include "Lock.h"
#include "PlayoutThread.h"
#include <vector>
#include <cstring>
#include <iostream>

PlayoutThread::PlayoutThread(OutputSink * outputHandle, SoundModel *model, int bufferSize) {

	this->outputHandle = outputHandle;
	this->model = model;
	this->bufferSize = bufferSize;	
	this->playing = true;
}

void PlayoutThread::run() {
	
	short dataBuf[bufferSize];
	while(isPlaying()) {
		model->getSamples(dataBuf, bufferSize);
		outputHandle->writeSamples(dataBuf, bufferSize);
	}

}

void PlayoutThread::stop() {

	playLock.acquire();
	playing = false;
	playLock.release();
}
	
bool PlayoutThread::isPlaying() {

	bool out;

	playLock.acquire();
	out = playing;
	playLock.release();

	return out;

}
