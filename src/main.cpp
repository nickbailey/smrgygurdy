#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <limits>
#include <libconfig.h++>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>

#include <config.h>

#include "SoundModelPoly.h"
#include "PlayoutThread.h"
#include "Pedal.h"
#include "OutputMixer.h"
#include "ALSAAdaptor.h"
#ifdef SUPPORT_JACKD
#	include "JACKAdaptor.h"
#endif
#ifdef SUPPORT_MINILAB1008
#	include "MiniLAB1008.h"
#endif
#ifdef SUPPORT_COMEDI
#	include "Comedi.h"
#endif
#ifdef SUPPORT_SERIALPEDAL
#	include "SerialPedal.h"
#endif
#include "ModulationPedal.h"
#include "DummyPedal.h"
#include "Controller.h"
#include "Keyboard.h"


using namespace std;
using namespace libconfig;

int usage(void);

int main(int argc, char ** argv) {
	// Check to see whether configuration file exists
	// Places to look for the config file.
	// Changes get written to the user config file;
	// this *must* be the first one.
	array<string, 3> sys_cfg_fqn = {
		string(getenv("HOME")) + "/.smrgygurdy",
		"/usr/local/etc/smrgygurdy.conf",
		"/etc/smrgygurdy.conf"
	};
	const string* user_cfg_fqn { &sys_cfg_fqn[0] }; // always write changes here.
	const char* cfg_fqn { nullptr };
	
	struct stat cfg_stat;
	for (const string& fqn : sys_cfg_fqn) {
		if (stat(fqn.c_str(), &cfg_stat) == -1) {
			cerr << "Couldn't stat " << fqn << endl;
		} else {
			cfg_fqn = fqn.c_str();
			break;
		}
	}
	cerr << "Reading configuration from " << cfg_fqn << endl;

	if (!(cfg_stat.st_mode & S_IRUSR)) {
		cerr << "Cannot read from " << cfg_fqn
		     << ": insufficient permission" << endl;
		exit(-1);
	}

	Config cfg;
	cfg.readFile(cfg_fqn);

	string pcm;         cfg.lookupValue("pcm", pcm);
	int bsize;          cfg.lookupValue("buffer_size", bsize);
	int rate;           cfg.lookupValue("sample_rate", rate);
	string pedal_dev;   cfg.lookupValue("pedal.kind", pedal_dev);
	string serial_dev;  cfg.lookupValue("pedal.serial_dev", serial_dev);
	int poly;           cfg.lookupValue("polyphony", poly);
	int noThreads;      cfg.lookupValue("threads", noThreads);
	int keyboard_id;    cfg.lookupValue("midi.controller_id", keyboard_id);
	int keyboard_port;  cfg.lookupValue("midi.controller_port", keyboard_port);
	int temperament;    cfg.lookupValue("midi.temperament", temperament);
	double output_gain; cfg.lookupValue("model.output_gain", output_gain);
	double autofade;    cfg.lookupValue("model.autofade_point", autofade);
	double v_bow_max;   cfg.lookupValue("model.bow_speed_max", v_bow_max);
	double v_bow_min;   cfg.lookupValue("model.bow_speed_min", v_bow_min);
	int verbosity;      cfg.lookupValue("verbosity", verbosity);
#ifdef SUPPORT_JACKD
	bool use_jack;      cfg.lookupValue("jack.use", use_jack);
	string jack_name;   cfg.lookupValue("jack.client_name", jack_name);
#endif

	// Parse command line, perhaps changing configured values
	static struct option long_options[] =  {
	  {"alsa-device",  required_argument, 0, 'd'},
	  {"autofade",     required_argument, 0, 'a'},
	  {"pedal-device", required_argument, 0, 'P'},
	  {"buffer-size",  required_argument, 0, 'b'},
	  {"sample-rate",  required_argument, 0, 'r'},
	  {"polyphony",    required_argument, 0, 'p'},
	  {"threads",      required_argument, 0, 't'},
	  {"midi-id",      required_argument, 0, 'M'},
	  {"midi-port",    required_argument, 0, 'm'},
	  {"temperament",  required_argument, 0, 'T'},
	  {"output-gain",  required_argument, 0, 'g'},
	  {"bow-speed-max",required_argument, 0, 'S'},
	  {"bow-speed-min",required_argument, 0, 's'},
	  {"list-midi",    no_argument,       0, 'l'},
	  {"verbose",      optional_argument, 0, 'v'},
	  {"serial-device",required_argument, 0, 'C'},
#ifdef SUPPORT_JACKD
	  {"jack",         optional_argument, 0, 'j'},
	  //{"jack-hookup",  required_argument, 0, 'J'},
#endif
	  {"help",         no_argument,       0, 'h'},
	  {0, 0, 0, 0}
	};
	
	{
		int c, option_index;
		do {
			c = getopt_long (argc, argv,
					"d:a:P:b:r:p:t:M:m:T:g:S:s:lv::C:j::h",
					long_options, &option_index);
			switch (c) {
			case 'd':
				pcm = optarg;
				cfg.lookup("pcm") = pcm;
				cout << "Using alsa device \"" << pcm << "\"" << endl;
				break;
			case 'a':
				autofade = atof(optarg);
				cfg.lookup("model.autofade") = autofade;
				cout << "Fade out below pedal = " << autofade << endl;
				break;
			case 'P':
				pedal_dev = optarg;
				cfg.lookup("pedal.kind") = pedal_dev;
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
			case 'T':
				temperament = 
				ViolinFingering::setTemperament(ViolinFingering::Temperament(atoi(optarg)));
				cfg.lookup("temperament") = temperament;
				cout << "Temperament is " << ViolinFingering::tuningSets[temperament].longdesc;
				break;
			case 'g':
				output_gain = atof(optarg);
				cfg.lookup("model.output_gain") = output_gain;
				cout << "Output gain set to " << output_gain << endl;
				break;
			case 'S':
				v_bow_max = atof(optarg);
				cfg.lookup("model.bow_speed_max") = v_bow_max;
				cout << "Bow speed (full pedal) set to " << v_bow_max <<endl;
				break;
			case 's':
				v_bow_min = atof(optarg);
				cfg.lookup("model.bow_speed_min") = v_bow_min;
				cout << "Bow speed (no pedal) set to " << v_bow_min << endl;
				break;
			case 'l':
				exit (system("aconnect -li"));
			case 'C':
				serial_dev = optarg;
				cfg.lookup("pedal.serial_dev") = serial_dev;
				cout << "Pedal connected to " << serial_dev << endl;
				break;
			case 'h':
				exit (usage());
			case 'v':
				verbosity = 1;
				if (optarg > 0) verbosity = atoi(optarg);
				cfg.lookup("verbosity") = verbosity;
				cout << "Verbose output (level " << verbosity << ")\n";
				break;
#ifdef SUPPORT_JACKD
			case 'j':
				use_jack = true;
				jack_name = "SMRGyGurdy";
				if (optarg > 0) {
					jack_name = optarg;
					if (jack_name == "no") {
						use_jack = false;
					} else {
						cfg.lookup("jack.client_name") = jack_name;
					}
				}
				cfg.lookup("jack.use") = use_jack;
				cout << "JACK audio output on port " << jack_name
				     << ".\nServer parameters override "
				        "rate and buffer size SMRGyGurdy settings.\n";
				break;
#endif
			case '?':
				// getopt_long already printed an error message
				break;
			}

		} while (c != -1);
	}
	
	// Assign Temperament
	ViolinFingering::setTemperament(ViolinFingering::Temperament(temperament));

	// Create Models
	SoundModelPoly *mainModel;		/* SoundModel to be passed to controller */
	vector<SoundModel*> subModels;		/* SoundModels to be passed to various PlayoutThreads */
	PlayoutThread* pothreads[noThreads];	/* Threads which render sound data */
	OutputMixer*   mixer;			/* Mixes outputs from the sound rendering threads */
	OutputAdaptor* sink;			/* Polyphonic sink to be used by all threads */

	int sysBSize {bsize};
	int sysRate  {rate};
	
#ifdef SUPPORT_JACKD
	if (use_jack)
		sink = new JACKAdaptor(jack_name);
	else
		sink = new ALSAAdaptor(pcm, sysBSize, sysRate);
#else
	sink = new ALSAAdaptor(pcm, sysBSize, sysRate);
#endif

	if (sink->size()) {
		sysBSize = sink->size();
		if (sysBSize != bsize)
			cout << "Audio system overrides configured buffer size. Using "
			     << sysBSize << ".\n";
	}
	
	if (sink->rate()) {
		sysRate = sink->rate();
		if (sysRate != rate)
			cout << "Audio system overrides configured sample rate. Using "
			     << sysRate << "Hz.\n";
	}

	mixer = new OutputMixer(noThreads, sysBSize, sink); 

	/* This will assign SoundModelMono instances as equally as possible to each SoundModelPoly,
	   one of which will be given to each thread */
	int perThread = poly/noThreads;
	int extra = poly % noThreads;

	for(int i = 0; i < noThreads; i++) {
		int models = perThread;
		if (i < extra) models++;
		//cout << "Assigning " << models << " models to a thread" << endl;
		subModels.push_back(new SoundModelPoly(models, output_gain));
	}

	mainModel = new SoundModelPoly(subModels, output_gain);

	// Create Playout Threads
	for(int i = 0; i < noThreads; i++) {
		pothreads[i] = new PlayoutThread(mixer, subModels[i], sysBSize);
		pothreads[i]->start();
	}

	// Create Controller
	Controller controller(mainModel, v_bow_min, v_bow_max, autofade);
	controller.start();

	// Create Keyboard
	Keyboard keyboard(&controller, keyboard_id, keyboard_port, verbosity);
	keyboard.start();

	// Create Pedal
	Pedal *pedal = NULL;
#ifdef SUPPORT_MINILAB1008
	if (!pedal_dev.compare("minilab1080"))
		pedal = new MiniLAB1008(&controller);
#endif
#ifdef SUPPORT_COMEDI
	if (!pedal_dev.compare("comedi"))
		pedal = new Comedi(&controller, 0.01, verbosity);
#endif
#ifdef SUPPORT_SERIALPEDAL
	try {
		if (!pedal_dev.compare("serial"))
			pedal = new SerialPedal(&controller, serial_dev, 0.01, verbosity);
	} catch (const char *e) {
		cout << "Caught exception creating SerialPedal object:\n"
		     << e << endl;
		exit(-1);
	}
#endif
	if (!pedal_dev.compare("modwheel"))
		pedal = new ModulationPedal(&controller, 0.01, verbosity);

	if (!pedal_dev.compare("dummy"))
		pedal = new DummyPedal(&controller);

	if (!pedal) {
		cerr << "Fatal: No compiled-in support for pedal device \"" <<
		        pedal_dev << "\"." << endl;
		exit (-1);
	}

	pedal->start();

	// Everything set up: commence audio output.
	sink->start();

	// Console interaction
	cout << "Enter Q to quit" << endl;
	cout << "      C to report stats" << endl;
	cout << "      G to set output gain" << endl;
	cout << "      S/Shift-S to set bow speed (no/full pedal)" << endl;
	cout << "      T to change temperament" << endl;

	{
		char ki;
		do {
			cin >> ki;

			switch (ki) {
			case 'c':
				cout << "Bowing at " << controller.get_bow_speed()
				<< "m/s because pedal value is "
				<< pedal->get_value() << endl;
				cout << "Bow speed limits: " << controller.min_speed
				<< "..." << controller.max_speed << endl;
				cout << "Output gain: " << output_gain << endl;
				cout << "Temperament: "
				<< ViolinFingering::tuningSets[ViolinFingering::getTemperament()].longdesc
				<< endl;
				break;

			case 'g':
				cout << "Current output gain = " << output_gain << endl
				<< "New value? ";
				while(!(cin >> output_gain)){
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "Invalid.  Enter floating-point output gain (e.g. 1.5): ";
				}
				mainModel->setOutputGain(output_gain);
				cfg.lookup("model.output_gain") = output_gain;
				break;

			case 'S':
				cout << "Current bow speed (full pedal) = " << v_bow_max << endl
				<< "New value? ";
				while(!(cin >> v_bow_max)){
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "Invalid.  Enter max bow speed in m/s: ";
				}
				controller.max_speed = v_bow_max;
				controller.speedChange(pedal->get_value());
				cfg.lookup("model.bow_speed_max") = v_bow_max;
				break;

			case 's':
				cout << "Current bow speed (nol pedal) = " << v_bow_min << endl
				<< "New value? ";
				while(!(cin >> v_bow_min)){
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "Invalid.  Enter minimum bow speed in m/s: ";
				}
				controller.min_speed = v_bow_min;
				controller.speedChange(pedal->get_value());
				cfg.lookup("model.bow_speed_min") = v_bow_min;
				break;
				
			case 't':
				cout << "Current temperament = "
				<< ViolinFingering::tuningSets[ViolinFingering::getTemperament()].longdesc
				<< endl
				<< "Select from the following:" << endl;
				for (ViolinFingering::Temperament i = ViolinFingering::Temperament(0);
				i < ViolinFingering::END;
				i = ViolinFingering::Temperament(i+1) ) {
					cout << '\t' << i << ": "
					<< ViolinFingering::tuningSets[i].longdesc
					<< endl;
				}
				while(!(cin >> temperament)){
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "Invalid.  Enter a temperament number from the list: ";
				}
				temperament = 
				ViolinFingering::setTemperament(ViolinFingering::Temperament(temperament));
				cfg.lookup("midi.temperament") = temperament;
				break;
				
			case 'q':      break;
			case EOF:      break;

			case '\n':     break;
			case ' ':      break;

			default:
				cout << ki << "? eh?" << endl;

			}
		}  while (ki != 'q');

		cfg.writeFile(user_cfg_fqn->c_str());

		exit(0);
	}
}

int usage(void)
{
        cout << "\t--alsa-device=dev | -ddev:      Set audio output stream" << endl;
        cout << "\t--pedal-device=class | -Pclass: Set pedal device class" << endl;
        cout << "\t        Supported classes: comedi; minilab1080" << endl;
        cout << "\t                           modwheel (wheel on MIDI controller)" << endl;
	cout << "\t                           serial (read from serial port:" << endl;
	cout << "\t                                   see --serial-device)" << endl;
	cout << "\t                           dummy (fixed value 0.75)" << endl;
#ifdef SUPPORT_SERIALPEDAL
	cout << "\t--serial-device=dev | -Cdev:    Specify device node for a serial pedal" << endl;
#endif
	cout << "\t--buffer-size=size | -bsize:    Set audio buffer length to size" << endl;
        cout << "\t--sample-rate=rate | -rrate:    Set audio sample rate in Hz" << endl;
        cout << "\t--polyphony=voices | -pvoices:  Set max number of sounding voices" << endl;
        cout << "\t--threads=thrds | -ttrhds:      Use thrds threads for synthesis" << endl;
        cout << "\t--midi-id=id | -Mid:            Set midi controller to id" << endl;
        cout << "\t--midi-port=port | -mport:      Set midi controller port" << endl;
        cout << "\t--bow-speed-max=spd | -Sspd:    Set bow speed in m/s for full pedal" << endl;
        cout << "\t--bow-speed-min=spd | -sspd:    Set bow speed for pedal fully up" << endl;
        cout << "\t--output-gain=gain | -ggain:    Set output gain (floating point)" << endl;
        cout << "\t--verbose[=level] | -v[level]:  Set verbosity" << endl;
        cout << "\t        1 = report midi note on/off;" << endl;
        cout << "\t        2 = also report changes of pedal value" << endl;
        cout << "\t--list-midi | -l:               List midi input devices and exit" << endl;
	cout << "\t--serial-dev=dev | -Cdev:       Read serial pedal data from dev" << endl;
#ifdef SUPPORT_JACKD
	cout << "\t--jack[=name] | -j[name]:       Use JACK; optionally set port name" << endl;
	cout << "\t--jack=no | -jno:               Use ALSA" << endl;
#endif
        cout << "\t--help | -h:                    Print usage and exit" << endl;

	return 0;
}

