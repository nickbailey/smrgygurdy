/**
@mainpage HurdyGurdy

HurdyGurdy is a music synthesiser that tries to emulate a medieval
folk instrument called hurdy gurdy. It uses a MIDI keyboard and a foot
pedal for input and generates sound output using the Artifastring
synthesis library.


@section install 1. Installation

@subsection prereq 1.1 Pre-requisites

The following packages are required to build the source:

- python
- swig
- libasound

@subsection pedal 1.2 Installing Foot Pedal driver

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

@subsection compile 1.3 Compile

In order to compile the code, the GNU C++ compiler and GNU make are
required.

@code
make
@endcode

@subsection run 1.4 Run

Run the compiled program by typing

@code
./hurdygurdy
@endcode

@subsection config 1.5 Configuration

Edit the config file to change the following options. Note: each setting in the config file must be in the order shown below:

@code
pcm hw:0
bsize 256
rate 44100
poly 6
threads 2
keyboard_id 20
keyboard_port 0
@endcode

The options are:
- pcm: output device
- bsize: buffer size
- rate: audio sampling rate
- poly: note polyphony
- threads: number of playout threads
- keyboard_id: ALSA sequencer client id of the MIDI keyboard
- keyboard_port: ALSA sequencer client port of the MIDI keyboard

You can find out the id and port of your keyboard by using aconnect

@code
aconnect -lio
@endcode


@section background 2. Background

@subsection references 2.1 References

Artifastring library created by Graham Percival http://percival-music.ca/

@subsection thx 2.3 Acknowledgements

This project would not have been possible without the supervision of
Dr Martin Macauley and Dr Nicholas Bailey, as well as guidance from
Prof. Graham Hair and Graham Percival related to music theory.

*/

