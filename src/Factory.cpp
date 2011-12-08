/* This file contains the definitions for factory methods to create the output section of the code */
#include "Factory.h"
#include "SoundModel.h"
#include "SoundModelPoly.h"
#include "SoundModelMono.h"
#include "OutputSink.h"
#include "OutputDirect.h"
#include "OutputMixer.h"

#include <string>
#include <iostream>

using namespace std;

/* Create a polyphonic SoundModel for a given polyphony */
SoundModel *makeSoundModelPoly(int poly) {

	vector<SoundModel*> monos;

	for(int i=0; i < poly; i++) {
		monos.push_back(new SoundModelMono());
		cout << "Creating a mono" << endl;
	}

	return (SoundModel*)(new SoundModelPoly(monos));
}

OutputSink *makeOutputSink(int sources, std::string pcm, int bufferSize, int rate) {

	OutputDirect *direct = new OutputDirect(pcm, bufferSize, rate);
	
	return (OutputSink*)(new OutputMixer(sources, direct, bufferSize));
}

