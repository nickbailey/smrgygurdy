#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <libconfig.h++>
#include <sys/types.h>
#include <sys/stat.h>
#include <config.h>
#include "SoundModelPoly.h"
#include "PlayoutThread.h"
#include "Pedal.h"
#include "OutputMixer.h"
#ifdef SUPPORT_MINILAB1008
#include "MiniLAB1008.h"
#else
#include "Comedi.h"
#endif
#include "Controller.h"
#include "Keyboard.h"


using namespace std;
using namespace libconfig;

int main(int argc, char ** argv) {

	if(argc > 1) {
		cerr << "Usage: " << argv[0] << endl;
		exit(1);
	}

	// Check to see whether configuration file exists
	string cfg_path(getenv("HOME"));
        cfg_path += "/.smrgygurdy";
	struct stat cfg_stat;
	if (stat(cfg_path.c_str(), &cfg_stat) == -1) {
		cerr << "Couldn't stat config file" << endl;
		exit(-1); // Should maybe copy a standard one here?
	}
	if (!(cfg_stat.st_mode & S_IRUSR)) {
		cerr << "Cannot read from " << cfg_path <<
		        ": insufficient permission" << endl;
		exit(-1);
	}

	Config cfg;
	cfg.readFile(cfg_path.c_str());

	string pcm;        cfg.lookupValue("pcm", pcm);
	int bsize;         cfg.lookupValue("buffer_size", bsize);
	int rate;          cfg.lookupValue("sample_rate", rate);
	int poly;          cfg.lookupValue("max_voices", poly);
	int noThreads;     cfg.lookupValue("threads", noThreads);
	int keyboard_id;   cfg.lookupValue("midi.controller_id", keyboard_id);
	int keyboard_port; cfg.lookupValue("midi.controller_port", keyboard_port);

	// Create Models
	SoundModel *mainModel;			/* SoundModel to be passed to controller */
	vector<SoundModel*> subModels;		/* SoundModels to be passed to various PlayoutThreads */
	PlayoutThread *pothreads[noThreads];	/* Threads which render sound data */
	OutputSink *sink;			/* Polyphonic sink to be used by all threads */

	sink = new OutputMixer(noThreads, pcm, bsize, rate);

	/* This will assign SoundModelMono instances as equally as possible to SoundModelPoly's
	   which will then be used by threads */
	int perThread = poly/noThreads;
	int extra = poly % noThreads;
	cout << "extra = " << extra << endl;

	for(int i = 0; i < noThreads; i++) {

		int models = perThread;
		if(i < extra) models++;
		cout << "Assigning " << models << " models to a thread" << endl;
		subModels.push_back(new SoundModelPoly(models));
	}

	mainModel = (SoundModel*)(new SoundModelPoly(subModels));

	// Create Playout Threads
	for(int i = 0; i < noThreads; i++) {
		pothreads[i] = new PlayoutThread(sink, subModels[i], bsize);
		pothreads[i]->start();
	}

	// Create Controller
	Controller controller(mainModel);
	controller.start();

	// Create Keyboard
	Keyboard keyboard(&controller, keyboard_id, keyboard_port);
	keyboard.start();

	// Create Pedal
#ifdef SUPPORT_MINILAB1008
	MiniLAB1008 pedal(&controller);
#else
	Comedi pedal(&controller);
#endif
	pedal.start();


	// Console interaction

	cout << "Enter Q to quit" << endl;
	cout << "      C to report stats" << endl;
	int c;
	while( (c = getchar()) != 'q') {
	    switch (c) {
	      case 'c':
	        cout << "Bowing at " << controller.get_bow_speed() <<
                        "m/s because pedal value is " <<
	                pedal.get_value() << endl; 
	        break;


	      case EOF:      break;
	      case '\n':     break;
	      case ' ':      break;

	      default:
	        cout << c << "? eh?" << endl;

	    }
	}
	cfg.writeFile(cfg_path.c_str());
	return 0;	
}

