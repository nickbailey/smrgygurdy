# Making a Serial-Port Pedal

It's quite easy to build a serial port pedal
starting with an analogue one. In this photograph,
we use a volume pedal from an inexpensive
digital keyboard. This is the inside of the pedal
along with the matrix board, Arduino Nano, and
a 220&Omega; 0.25W resistor.

![The inside of the pedal prior to modification](https://github.com/nickbailey/smrgygurdy/blob/master/Arduino/images/inside_pedal.jpg)

In the bottom right you can see the hole drilled for
for the LED with the clip inserted.

Remove the existing wiring and connect the earthy side of the
potentiomenter to a ground pin on the Arduino Nano. Connect
the other end of the potentiometer to the Arduino V<sub>Ref</sub>
and connect the slider to an analogue input pin (A0 is the
default but you can configure which one you use in the software).

If you want to use an external LED, take a yellow one and connect
it between a PWM-capable data pin and earth with 220&Omega; in series.

With everything fixed down (except the USB cable), the inside
of the pedal will look something like this:

![The inside of the after modification](https://github.com/nickbailey/smrgygurdy/blob/master/Arduino/images/installed.jpg)

Note that in normal use, the firmware lights the LED very dimly
to indicate that power is applied, and briefly increases its
brightness while and shortly after the pedal is moved. For
this reason, it must be connected to a digital output pin capable
of PWM, which not all of the DIO pins on the Nano are. We used D3.

## Configuring the software

Open the Arduino project in their IDE application and edit
the configuration if required. The following parameters may
be adjusted (quoted from the top of the file):

```
//// User-configurable parameters
//   Analogue input pin for pedal potentiometer
constexpr int potPin { A0 };

//   The number of reads averaged to produce the
//   output value (noise reduction measure)
constexpr unsigned int readBufSize { 1 };

//   External LED pin
constexpr unsigned int LEDPin { 3 };

//   External LED brightess when nothing happens
constexpr unsigned int dimLED { 5 };

//   External LED brightess when (and shortly after) pedal change
constexpr unsigned int brightLED { 255 };

//   How many times around the main loop before LED goes dim
constexpr unsigned int flashLEDDelay { 800 };
```

LED brightness (PWM) is in the range 0-255. Pay attention that PWM
functionality is available on pins 3, 5, 6, 9, 10, and 11 on the
Nano.

Follow the normal procedure for building and downloading the
code.

Test by observing the console window in the Arduino IDE which should
display 4-significant-figure floating point numbers as the pedal
is moved and nothing when it isn't. It there is too much jitter,
try altering the parameters to the declaration of the SampleProcessor
object on line 67. The first refers to the number of analogue reads
which are averaged in an attempt to reduce noise; the second
is the minimum interesting change in platform-specific units
(on the Nano, ADC inputs range from 0 to 1023). The default
(4, 3) seems to work very well even on a very inexpensive pedal.

## Run SMRGyGurdy with the appropriate start-up parameters.

Close the IDE, and start SMRGyGurdy with the following command
(adjust the serial port if it is inapprpriate for your machine)

```
smrgygurdy --pedal=serial --serial-device=/dev/ttyUSB0
```

The parameters get saved in your per-user configuration, so there's no
need to repeat them next time.

## A note on the potentiometer value

The Arduino's manufacturers say that the minimum source impedence
for the analogue input pins to meet their specification is in the
region of 10K. This would suggest any potentiometer above 40K would
be unsuitable (not 10K... think about it!).

Fortuantely, it turns out that the analogue
input pins are internally multiplexed
on to a sample-and-hold capacitor, so that as long as successive reads
are always from the same pin rather than switching between pins at
possibly different voltages, the limitation is one of slew rate rather than
of accuracy. The analogue input presents a non-resistive load to the
voltage source under these circumstances, and the potentiometer
fitted to this pedal (100K) worked just fine.
