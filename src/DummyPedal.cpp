#include "DummyPedal.h"
#include "Controller.h"

#include <unistd.h>

DummyPedal::DummyPedal(Controller *c) :
    Pedal(c)
{
    value = 0.75;
}

void DummyPedal::run() {
    while (1) {              // Nothing to see here.
        sleep(10);
    }
}
