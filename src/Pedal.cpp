#include "Pedal.h"
#include "Controller.h"

Pedal::Pedal(Controller *c, double eps) :
    controller(c), value(0.0), epsilon(eps)
{ }

