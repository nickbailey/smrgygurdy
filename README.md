SMRGyGurdy
==========

A physical-model-based, polyphonic hurdygurdy synthesiser
based on Graham Percival's artifastring library.

## Credits

Author: EE+Computing Students Team E
Maintained by: Nick Bailey
Date: 21 March 2011

## Documentation
To view the documentation run doxygen in the root
level directory, then visit doc/index.html

To build for the first time from the top-level directory, say:
```
	./autogen.sh
```

Say `./configure --help` to list configuration options.

The following libraries need to be installed first:

```
	artifastring
        comedilib (if configured to use comedi devices - default)
        bogiolib (https://github.com/nickbailey/bogiolib -
                  a wrapper to simplify access to comedilib)
```
A version of this library with some maintenance updates is available here:
https://github.com/nickbailey/artifastring

We recommend you build in a local build dirctory and install thus:

```
	mkdir build
	cd build
	../configure <driver options>
	make
	sudo make install
```

You'll need a valid configuration file to run this program.
`make install` wil copy a suitable one into an appropriate
place on your system. `smrgygurdy` writes
the current configuration to the latter on exiting via the
q command.

There is information modifying an analogue volume pedal to
use as a serial port pedal in the `Arduino` directory.

Nick Bailey, June 2nd, 2016.

