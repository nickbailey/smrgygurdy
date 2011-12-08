#include "SoundModelMono.h"
#include "OutputSink.h"
#include "Thread.h"
#include "Lock.h"

#include <vector>

#ifndef PLAYOUT_THREAD_H
#define PLAYOUT_THREAD_H

/**
 * Thread to render samples.
 *
 * This will continually render samples from a SoundModel and send it
 * to an OutputSink.
 */
class PlayoutThread : public Thread {

	OutputSink *outputHandle; /* Output where data should be written */
	SoundModel *model;	      /* Model where data is generated */
	bool playing;		      /* True while thread is playing */
	Lock playLock;		      /* Lock to be used when accessing playing */
	int bufferSize;		      /* Size of buffer to use when generating/writing */

	bool isPlaying();	      /* Synchronously reads the playing boolean */

	public:
		
	    /**
		 * Default constructor. 
		 *
		 * @param outputHandle OutputSink where data should go
		 * @param model	       SoundModel where data should come from
		 * @param bufferSize   Number of samples to use per period
		 */
		PlayoutThread(OutputSink *outputHandle, SoundModel *model, int bufferSize);
	
		/** Begin running the playout thread */
		virtual void run();

		/** Stop the playout thread */
		void stop();
	
};

#endif
