#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <config.h>
#include "SoundModelPoly.h"
#include "PlayoutThread.h"
#include "Pedal.h"
#include "MiniLAB1008.h"
#include "Factory.h"
#include "Controller.h"
#include "Keyboard.h"

using namespace std;

int main(int argc, char ** argv) {

	if(argc > 1) {
		cerr << "Usage: " << argv[0] << endl;
		exit(1);
	}

	// Load data from file
	int i;

	fstream file("config");
	string n[10];
	string v[10];

	for (i = 0; i < 10; i++) {
	        file >> n[i] >> v[i];
	}
	
	file.close();

	string pcm = v[0];
	int bsize = atoi(v[1].c_str());
	int rate = atoi(v[2].c_str());
	int poly = atoi(v[3].c_str());
	int noThreads = atoi(v[4].c_str());
	int keyboard_id = atoi(v[5].c_str());
	int keyboard_port = atoi(v[6].c_str());

	// Create Models
	SoundModel *mainModel;			/* SoundModel to be passed to controller */
	vector<SoundModel*> subModels;		/* SoundModels to be passed to various PlayoutThreads */
	PlayoutThread *pothreads[noThreads];	/* Threads which render sound data */
	OutputSink *sink;			/* Polyphonic sink to be used by all threads */

	sink = makeOutputSink(noThreads, pcm, bsize, rate);

	/* This will assign SoundModelMono instances as equally as possible to SoundModelPoly's
	   which will then be used by threads */
	int perThread = poly/noThreads;
	int extra = poly % noThreads;
	cout << "extra = " << extra << endl;

	for(int i = 0; i < noThreads; i++) {

		int models = perThread;
		if(i < extra) models++;
		cout << "Assigning " << models << " models to a thread" << endl;
		subModels.push_back(makeSoundModelPoly(models));
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
	return 0;	
}

