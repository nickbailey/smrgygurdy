#include <cmath>
#include "ViolinFingering.h"

const ViolinFingering::TuningSet ViolinFingering::tuningSets[] = {
  // For each tuning, supply MIDI number associated with A440,
  //    divisions of the octave, and MIDI numbers for each string here.
  { 69, 12, {36, 43, 50, 57}, "12ET" },				// 12ET
  { 69, 19, {17, 28, 39, 50}, "19ET, A4 sounds at pitch" },	// 19ET, A4 sounds A4
  { 88, 19, {36, 47, 58, 69}, "19ET, A4 sounds A3" }		// 19ET, A4 sounds A3
};

// Unless told otherwise, use standard temperament.
ViolinFingering::Temperament ViolinFingering::theTemperament(et12);

ViolinFingering::Temperament ViolinFingering::setTemperament(Temperament t) {
  if (t >= 0 && t < END) theTemperament = t;
  return getTemperament();
}

int ViolinFingering::getStringHint(int midi) const throw (const char *) {
  
  // Using the current temperament...
  const TuningSet &t = tuningSets[theTemperament];
  
  // Below instrument's range?
  if (midi < t.stgmidi[0])
    throw "MIDI note below instrument's range";
  
  int i = t.stgmidi.size();
  while (--i)
    if (midi > t.stgmidi[i])
      break;
  
  return i;
}

double ViolinFingering::getStopPosn(int midi, int stg) const {
  // Use the current temperament (a lot)...
  const TuningSet &t = tuningSets[theTemperament];

  // Find note number associated with an open string
  int openmidi = t.stgmidi[stg];
  
  // The distance down the string is 1.0 - 2^(n/12) (for 12ET)
  // where n is the number of midi notes the requested note is
  // above the open string.
  //
  // In 12ET, 12 semitones above the open string is an octave,
  // i.e. 2^(-12/12) = half way down.
  return 1.0 - pow(2.0, static_cast<double>(openmidi-midi)/t.divnper8ve);
}