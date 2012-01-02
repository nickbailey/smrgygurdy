#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <libconfig.h++>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <config.h>
#include "SoundModelPoly.h"
#include "PlayoutThread.h"
#include "Pedal.h"
#include "OutputMixer.h"

// Include header files for pedal hardware
#ifdef SUPPORT_MINILAB1008
#include "MiniLAB1008.h"
#endif
#ifdef SUPPORT_COMEDI
#include "Comedi.h"
#endif

#include "Controller.h"
#include "Keyboard.h"


using namespace std;
using namespace libconfig;

int usage(const char*);

int main(int argc, char ** argv) {

	// Check to see whether configuration file exists
	string cfg_path(getenv("HOME"));
        cfg_path += "/.smrgygurdy";
	string user_cfg_path(cfg_path); // always write changes here.
	struct stat cfg_stat;
	if (stat(cfg_path.c_str(), &cfg_stat) == -1) {
		cerr << "Couldn't stat " << cfg_path <<
		        " - looking in /etc" << endl;
		cfg_path = "/etc/smrgygurdy.conf";
	}
	if (stat(cfg_path.c_str(), &cfg_stat) == -1) {
		cerr << "Couldn't stat config file" << endl;
		exit(-1);
	}
	if (!(cfg_stat.st_mode & S_IRUSR)) {
		cerr << "Cannot read from " << cfg_path <<
		        ": insufficient permission" << endl;
		exit(-1);
	}

	Config cfg;
	cfg.readFile(cfg_path.c_str());

	string pcm;         cfg.lookupValue("pcm", pcm);
	string pedal_dev;   cfg.lookupValue("pedal", pedal_dev);
	int bsize;          cfg.lookupValue("buffer_size", bsize);
	int rate;           cfg.lookupValue("sample_rate", rate);
	int poly;           cfg.lookupValue("polyphony", poly);
	int noThreads;      cfg.lookupValue("threads", noThreads);
	int keyboard_id;    cfg.lookupValue("midi.controller_id", keyboard_id);
	int keyboard_port;  cfg.lookupValue("midi.controller_port", keyboard_port);
	int verbosity;      cfg.lookupValue("verbosity", verbosity);

	// Parse command line, perhaps changing configured values
	static struct option long_options[] =  {
	  {"alsa-device",  required_argument, 0, 'd'},
	  {"pedal-device", required_argument, 0, 'P'},
	  {"buffer-size",  required_argument, 0, 'b'},
	  {"sample-rate",  required_argument, 0, 'r'},
	  {"polyphony",    required_argument, 0, 'p'},
	  {"threads",      required_argument, 0, 't'},
	  {"midi-id",      required_argument, 0, 'M'},
	  {"midi-port",    required_argument, 0, 'm'},
	  {"verbose",      optional_argument, 0, 'v'},
	  {"list-midi",    no_argument,       0, 'l'},
	  {"help",         no_argument,       0, 'h'},
	  {0, 0, 0, 0}
	};
	int c, option_index;
	do {
		c = getopt_long (argc, argv,
				 "d:P:b:r:p:t:M:m:v::lh",
		                 long_options, &option_index);
		switch (c) {
		    case 'd':
			pcm = optarg;
			cfg.lookup("pcm") = pcm;
			cout << "Using alsa device \"" << pcm << "\"" << endl;
			break;
		    case 'P':
			pedal_dev = optarg;
			cfg.lookup("pedal") = pedal_dev;
			cout << "Using pedal device \"" << pedal_dev << "\"" << endl;
			break;
		    case 'b':
			bsize = atoi(optarg);
			cfg.lookup("buffer_size") = bsize;
			cout << "Audio buffer size = " << bsize << endl;
			break;
		    case 'r':
			rate = atoi(optarg);
			cfg.lookup("sample_rate") = rate;
			cout << "Audio sample rate = " << rate << endl;
			break;
		    case 'p':
			poly = atoi(optarg);
			cfg.lookup("polyphony") = poly;
			cout << "Polyphony: " << poly << " voices" << endl;
			break;
		    case 't':
			noThreads = atoi(optarg);
			cfg.lookup("threads") = noThreads;
			cout << noThreads << " concurrent threads" << endl;
			break;
		    case 'M':
			keyboard_id = atoi(optarg);
			cfg.lookup("midi.controller_id") = keyboard_id;
			cout << "Midi controller has id " << keyboard_id << endl;
			break;
		    case 'm':
			keyboard_port = atoi(optarg);
			cfg.lookup("midi.controller_port") = keyboard_port;
			cout << "Midi controller uses port " << keyboard_port << endl;
			break;
		    case 'h':
			exit (usage(argv[0]));
		    case 'l':
			exit (system("aconnect -i"));
		    case 'v':
			verbosity = 1;
			if (optarg > 0) verbosity = atoi(optarg);
			cfg.lookup("verbosity") = verbosity;
		    case '?':
			// getopt_long already printed an error message
			break;
		}

	} while (c != -1);

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
	Keyboard keyboard(&controller, keyboard_id, keyboard_port, verbosity);
	keyboard.start();

	// Create Pedal
	Pedal *pedal = NULL;
#ifdef SUPPORT_MINILAB1008
	if (pedal_dev == "minilab1080")
		pedal = new MiniLAB1008(&controller);
#endif
#ifdef SUPPORT_COMEDI
	if (pedal_dev.compare("comedi") == 0)
		pedal = new Comedi(&controller, 0.01, verbosity);
#endif
	if (pedal) {
		pedal->start();
	} else {
		cerr << "Fatal: Requested pedal hardware is unsupported"
		     << endl;
		exit(-1);
	}


	// Console interaction

	cout << "Enter Q to quit" << endl;
	cout << "      C to report stats" << endl;

	while( (c = getchar()) != 'q') {
	    switch (c) {
	      case 'c':
	        cout << "Bowing at " << controller.get_bow_speed() <<
                        "m/s because pedal value is " <<
	                pedal->get_value() << endl; 
	        break;


	      case EOF:      break;
	      case '\n':     break;
	      case ' ':      break;

	      default:
	        cout << c << "? eh?" << endl;

	    }
	}
	cfg.writeFile(user_cfg_path.c_str());
	delete pedal;
	return 0;	
}

int usage(const char* cmd)
{
	cout << "Usage: " << cmd << " [options]" << endl;
	cout << "Options:" << endl;
	cout << "\t--alsa-device=dev | -ddev:      Set audio output stream" << endl;
	cout << "\t--pedal-device=class | -Pclass: Set pedal device class" << endl;
	cout << "\t\tSupported classes: comedi; minilab1080" << endl;
	cout << "\t--buffer-size=size | -bsize:    Set audio buffer length to size" << endl;
	cout << "\t--sample-rate=rate | -rrate:    Set audio sample rate in Hz" << endl;
	cout << "\t--polyphony=voices | -pvoices:  Set max number of sounding voices" << endl;
	cout << "\t--threads=thrds | -ttrhds:      Use thrds threads for synthesis" << endl;
	cout << "\t--midi-id=id | -Mid:            Set midi controller to id" << endl;
	cout << "\t--midi-port=port | -mport:      Set midi controller port" << endl;
	cout << "\t--verbose[=level] | -v[level]:  Set verbosity" << endl;
	cout << "\t\t0 = miniumum errors only;" << endl;
	cout << "\t\t1 = report midi note on/off;" << endl;
	cout << "\t\t2 = also report changes of pedal value" << endl;
	cout << "\t--list-midi | -l:               List midi input devices and exit" << endl;
	cout << "\t--help | -h:                    Print usage and exit" << endl;
	return 0;
}

