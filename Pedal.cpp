#include "Pedal.h"
#include "Controller.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

#include <libhid/pmd.h>
#include <libhid/minilab-1008.h>

Pedal::Pedal(Controller *c) :
    controller(c), value(0.0)
{ }

