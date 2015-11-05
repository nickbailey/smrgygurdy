#include "ModulationPedal.h"
#include "Controller.h"
#include "Pedal.h"
#include "ModulationEventListener.h"

#include <unistd.h>
#include <iostream>
#include <cmath>

ModulationPedal::ModulationPedal(Controller *c,
                                 double eps,
                                 int verbosity) :
    Pedal(c, eps), ModulationEventListener(c), last_value(0.0)
{
  value = 0;
  announce = (verbosity > 1);
}

/* Most "pedals" run a concurrent thread to poll an
   external piece of hardware. But we can simply
   accept modulation wheel events, scale them into
   the range 0.0 - 1.0, then effect a speed change
   immediately. */

void ModulationPedal::modulationEventCallback(int modulation)
{
  value = (double)modulation / 127.0;
  
  if (fabs(value - last_value) > epsilon) {
    controller->speedChange(value);
    last_value = value;
    if (announce)
      std::cout << "Modulation pedal value: " << value << std::endl;
  }
  
  if (previousModulationListener)
    previousModulationListener->modulationEventCallback(modulation);

}

void ModulationPedal::run() {
    while (1) {              // Nothing to see here.
        sleep(10);
    }
}
