/* This file contains the definitions for factory methods to create the output section of the code */

#include "SoundModel.h"
#include "OutputSink.h"
#include <string>

using namespace std;

/* Create a polyphonic SoundModel for a given polyphony */
SoundModel *makeSoundModelPoly(int poly);

/* Create an OutputSink with a number of sources for a given PCM, bufferSize and rate*/
OutputSink *makeOutputSink(int sources, std::string pcm, int bufferSize, int rate);


