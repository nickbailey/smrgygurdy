/**
@mainpage SMRGyGurdy

SMRGyGurdy is a music synthesiser that tries to emulate a medieval
folk instrument called hurdy gurdy. It uses a MIDI keyboard and a foot
pedal for input and generates sound output using the Artifastring
synthesis library.


@section install 1. Installation

@subsection prereq 1.1 Pre-requisites

The following packages are required to build the source:

- python
- swig
- libasound
- libconfig++
- libartifastring https://github.com/gperciva/artifastring

@subsection pedal 1.2 Installing a Foot Pedal driver

SMRGyGurdy was intended to use a foot pedal to control the
bow speed in the physical model.

Two hardware foot-pedals are supported: the MiniLAB1008, and the
Comedi (Control and Measurement Interface: http://www.comedi.org/)
for the diverse A/D/A products supported by the Linux kernel.
The current version of the drivers were tested with the USB-DUX
peripheral http://www.linux-usb-daq.co.uk/

In all these cases, a pedal is required to operate a potential
divider, supplying one of the A-D inputs of the peripheral device
with a voltage varying typically between 0 and the analogue
VRef.

@subsubsection minilab1008 1.2.1 The minilab1008.

To install the driver for the minilab1008 ADC you need to have the
following packages:

- udev
- libusb-devel
- libusb-static
- libusb

Once these packages are installed, follow the instructions in README
located in MCCLIBHID.1.40.tgz to make and install libhid and the
associated minilab1008 headers.

Finally connect the the PC +5V ADC supply to the IN1 socket of the
pedal and CH0 IN to the OUT2 socket (and the associated GND
connectors), with the supplied jack plugs.

Note that this software has been rewritten using a more recent libusb
library and we welcome patches from user of these products.

@subsubsection Comedi 1.2.1 Comedi peripherals (e.g. the USB-DUX).

The Comedi foot pedal makes use of a convenience library which is
distributed under the BogIOLib project. This can be compiled 
and installed from the git repostiory at https://github.com/nickbailey/bogiolib

@subsection compile 1.3 Compile

In order to compile the code, the GNU C++ compiler and GNU make are
required.

I recommend building the program in a subdirectory (say, `build`)

@code
./autogen.sh
mkdir build
cd build
../configure
make
@endcode

By default, Comedi support is compiled in and minilab1008 support
is not. This can be changed by passing flags to the `configure`
command, respectively `--without-comedi` and `--with-MiniLAB1008`.
As usual, `--help` displays the full story.

@subsection run 1.4 Run

Run the compiled program by typing

@code
./src/smrgurdy
@endcode

@subsection config 1.5 Configuration

SMRGyGurdy remembers its configuration files across runs,
storing the configuration in `$HOME/.smrgygurdy`.

Either edit the config file to change the following options,
or supply them all on the command line the first time the
program is run. `smrgygurdy --help` shows how.

A typcal configuration file (copied direct from the
maintainer's machine) looks like this:

@code
pcm = "default";
pedal = "modwheel";
buffer_size = 512;
sample_rate = 44100;
polyphony = 10;
threads = 4;
verbosity = 0;
midi : 
{
  controller_id = 129;
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

The options are:
@code
        --alsa-device=dev | -ddev:      Set audio output stream
        --pedal-device=class | -Pclass: Set pedal device class
                Supported classes: comedi; minilab1080
                                   modwheel (wheel on MIDI controller)
                                   dummy (fixed value 0.75)
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
        --help | -h:                    Print usage and exit
@endcode

You can find out the id and port of your keyboard by using aconnect

@code
aconnect -lio
@endcode

or by running the program with the `-l` option.

@section background 2. Background

SMRGyGurdy was written as a drone instrument for the MicrotonalISM
project http://www.n-ism.org/Projects/microtonalism.php

Before the scale was standardised with 12 Equal Divisions of the
Octave, instruments playing with 19 (usually unequal) divisions were
not uncommon. 19EDO is a tuning system which is isomorphic with
the commonly used modern system; you can try it out using the
javascript keyboard demonstration at
http://www.n-ism.org/Projects/microtonalism.php (source code:
http://www.n-ism.org/Software/using_clavier.php). The musical
consequences of adopting the system, along with a simple
theoretical intruoduction to the tuning of the scale and
performances of some short works written in it by Composer
Graham Hair can be seen in the documentary film,
"Making Music with 19 Tones" (see https://vimeo.com/80450876)

@subsection references 2.1 References

- Artifastring library created by Graham Percival http://percival-music.ca/
- Application currently maintained by Nick Bailey http://www.n-ism.org/People/nick.php

@subsection thx 2.3 Acknowledgements

This project would not have been possible without the supervision of
Dr Martin Macauley and Dr Nicholas Bailey, as well as guidance from
Prof. Graham Hair and Graham Percival related to music theory.

It is currently maintained by Nick Bailey at the Science and Music
Research Group, part of the School of Engineering at The University
of Glasgow.

*/

