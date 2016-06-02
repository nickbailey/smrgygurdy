/**
@mainpage SMRGyGurdy

SMRGyGurdy is a music synthesiser that tries to emulate a medieval
folk instrument called a hurdy-gurdy. It uses a MIDI keyboard and a foot
pedal for input and generates sound output using the Artifastring
synthesis library. It can be downloaded from https://github.com/nickbailey/smrgygurdy


@section install 1. Installation

@subsection prereq 1.1 Pre-requisites

The following packages are required to build the source:

- libasound
- libconfig++
- libartifastring https://github.com/gperciva/artifastring

If you like to use Python to test out the string parameters
passed to the artifastring library, you will also need

- python
- swig

There's a short example of how to do that in the file `parameters.py`
in the root directory. This isn't required for building and
day-to-day use of the SMRGyGurdy.

@subsection pedal 1.2 Installing a Foot Pedal driver

SMRGyGurdy was intended to use a foot pedal to control the
bow speed in the physical model.

As well as the "dummy" foot-pedal, which can be used when no
physical device is available,
four foot-pedals are supported:

- `MiniLAB1008`
- `Comedi` (Control and Measurement Interface: http://www.comedi.org:8000/)
  for the diverse A/D/A products supported by the Linux kernel
- `modwheel` which reads modulation wheels on the attached MIDI controller
- `serial` which reads ASCII strings representing floating point numbers
  in the range 0-1.0 from the nomiated serial port.

The current version of the ADC-based drivers were tested with the USB-DUX
peripheral http://www.linux-usb-daq.co.uk/

In the case of the ADC solutions,
a pedal is required to operate a potential
divider, supplying one of the ADC inputs of the peripheral device
with a voltage varying typically between 0 and the analogue
V<sub>Ref</sub>.

@subsubsection minilab1008 1.2.1 The minilab1008

We no longer support the MCD DAQ miniLAB-1008. The product is actively
supported for linux (see http://www.mccdaq.com/daq-software/Linux-Support.aspx)
but we prefer the similarly-priced USB-DUX solutions which use the
native Linux kernel support for data acquisition devices.

If you have a set-up and working miniLAB1008, to use it with a conventional
volume pedal make up a cable connecting the PC +5V ADC supply to the
pedal's input socket and CH0 IN to the output socket (and the associated GND
connectors).

Note that this software has been rewritten using a more recent libusb
library and we welcome patches from user of these products.

@subsubsection Comedi 1.2.1 Comedi peripherals (e.g. the USB-DUX).

The Comedi foot pedal makes use of a convenience library which is
distributed under the BogIOLib project. This can be compiled 
and installed from the git repostiory at https://github.com/nickbailey/bogiolib

@subsection deployment 1.3 Deployment

@subsubsection compilation 1.3.1 Compilation

In order to compile the code, the GNU C++ compiler, autotools and GNU make are
required.

I recommend building the program in a subdirectory (say, `build`)

@code
./autogen.sh
mkdir build
cd build
../configure
make
@endcode

By default, support for Comedi and SerialPedals is
compiled in and minilab1008 support is not.
This can be changed by passing flags to the `configure`
command, respectively `--without-comedi`, `--without-serialpedal`
and `--with-MiniLAB1008`. As usual, `--help` displays the full story.

@subsubsection installation 1.3.2 Installation

@code
sudo make install
@endcode

This causes the smrgygurdy binary to be installed (by default)
in /usr/local/bin, and its system-wide configuaration file
in /usr/local/etc.

You can change the location of isntallation by configuring with,
for example, `--prefix=/` and determine where the configuration
file is installed with, e.g., `--sysconfdir=/etc`.

@subsubsection run 1.3.3 Invocation

Run the compiled program by typing

@code
src/smrgurdy [options]
@endcode

The options are:
@code
        --alsa-device=dev | -ddev:      Set audio output stream
        --pedal-device=class | -Pclass: Set pedal device class
                Supported classes: comedi; minilab1080
                                   modwheel (wheel on MIDI controller)
                                   serial (read from serial port:
                                           see --serial-device)
                                   dummy (fixed value 0.75)
        --serial-device=dev | -Cdev:    Specify device node for a serial pedal
        --buffer-size=size | -bsize:    Set audio buffer length to size
        --sample-rate=rate | -rrate:    Set audio sample rate in Hz
        --polyphony=voices | -pvoices:  Set max number of sounding voices
        --threads=thrds | -ttrhds:      Use thrds threads for synthesis
        --midi-id=id | -Mid:            Set midi controller to id
        --midi-port=port | -mport:      Set midi controller port
        --bow-speed-max=spd | -Sspd:    Set bow speed in m/s for full pedal
        --bow-speed-min=spd | -sspd:    Set bow speed for pedal fully up
        --output-gain=gain | -ggain:    Set output gain (floating point)
        --verbose[=level] | -v[level]:  Set verbosity
                1 = report midi note on/off;
                2 = also report changes of pedal value
        --list-midi | -l:               List midi input devices and exit
        --serial-dev=dev | -Cdev:       Read serial pedal data from dev
        --help | -h:                    Print usage and exit
@endcode

You can find out the id and port of your keyboard by using aconnect

@code
aconnect -lio
@endcode

or by running the program with the `-l` option.


@subsubsection config 1.3.4 Configuration

SMRGyGurdy searches for configuration information in the
following places, using the first file which it finds:

- `~/.smrgygurdy` for per-user configuration changes;
- `${prefix}/smrgygurdy.conf` (usually `/usr/local/etc/smrgygurdy.conf`)
  for system-wide local configuration;
- `/etc/smrgurdy.conf` for package-installed configuration.

SMRGyGurdy remembers its configuration files across runs,
storing the configuration in `$HOME/.smrgygurdy` on exit.

Either edit the config file to change the following options,
or supply them all on the command line the first time the
program is run. `smrgygurdy --help` shows how.

A typcal configuration file (copied direct from the
maintainer's machine) looks like this:

@code
pcm = "default";
pedal : 
{
  kind = "serial";
  serial_dev = "/dev/ttyUSB0";
};
buffer_size = 512;
sample_rate = 44100;
polyphony = 10;
threads = 2;
verbosity = 0;
midi : 
{
  controller_id = 20;
  controller_port = 0;
  temperament = 0;
};
model : 
{
  output_gain = 1.5;
  autofade_point = 0.2;
  bow_speed_min = 0.4;
  bow_speed_max = 1.5;
};
@endcode


@section background 2. Background

SMRGyGurdy was written as a drone instrument for the MicrotonalISM
project http://www.n-ism.org/Projects/microtonalism.php

Before the scale was standardised with 12 Equal Divisions of the
Octave, instruments playing with 19 (usually unequal) divisions were
not uncommon. 19EDO is a tuning system which is isomorphic with
the commonly used modern system; see
http://www.n-ism.org/Projects/microtonalism.php 
You can try it out using the
javascript keyboard demonstration:
http://www.n-ism.org/Projects/Microtonalism/keyboard_demo.php (source code:
http://www.n-ism.org/Software/using_clavier.php). The musical
consequences of adopting the system, along with a simple
theoretical intruoduction to the tuning of the scale and
performances of some short works written in it by Composer
Graham Hair can be seen in the documentary film,
"Making Music with 19 Tones" (see https://vimeo.com/80450876)

@section thx 3 Acknowledgements

- Artifastring library created by Graham Percival http://percival-music.ca/
- Application currently maintained by Nick Bailey http://www.n-ism.org/People/nick.php

This project would not have been possible without the supervision of
Dr Martin Macauley and Dr Nicholas Bailey, as well as guidance from
Prof. Graham Hair and Graham Percival related to music theory.

It is currently maintained by Nick Bailey at the Science and Music
Research Group, part of the School of Engineering at The University
of Glasgow.

*/

