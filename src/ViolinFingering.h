/**
 * A class which determines a fingering for a given MIDI note,
 * based on the globally-defined temperament.
 * 
 * Currently, the highest possible string is used, which is to
 * say that where possible, the string is fingered in first position
 * 
 * @author Nick Bailey, SMRG Glasgow
 * @date   07 June 2013
 * 
 **/

#ifndef VIOLIN_FINGERING_H
#define VIOLIN_FINGERING_H

#include <vector>
#include <string>

class ViolinFingering {
  public:
    /**
     * Temperaments supported by the fingering class
     */
    enum Temperament {et12=0,		///< "Standard" 12ET tuning
                      et19,		///< 19ET tuning with MIDI A4 sounding A4
		      et19ott_bassa,	///< 19ET tuning with MIDI A4 sounding A3
		      END		///< C++ hack to determine no of supported temperaments
		                        ///< All additional ones must be inserted before this.
    };
  
    typedef struct {
      const int a4;			///< Midi code which will correspond to A4
      const int divnper8ve;		///< Divisions per octave
      const std::vector<int> stgmidi;	///< Lookup table associating MIDI note with open strings
      std::string longdesc;
    } TuningSet;
    
    ViolinFingering() { }
    
    /**
     * Change the current temperament to a different MIDI key mapping.
     * If the requested change is out-of-range, leave the current one unchanged.
     * 
     * @param t One of the supported temperaments.
     * @return The temperament actually set
     */
    static Temperament setTemperament(Temperament t);
    
    /**
     * Get the current temperament by which fingerings are calculated
     * 
     * @return the current temperament
     */
    static Temperament getTemperament(void) { return theTemperament; }
    
    /**
     * Array of tuning information (one for each supported temperament).
     * Use one of the Temperament enums (except END) to index it.
     */
    static const TuningSet tuningSets[];
    
    /**
     * Select a string to perform a given MIDI note
     * 
     * @param midi MIDI note to perform
     * @return String recommended for performance of the note
     * @throw char* Note is below the instrument's range
     */
    int getStringHint(int midi) const;

    /**
     * Find where a string should be stopped to generate the given midi note
     * 1.0 means open; 0.0 means stop at the bridge (won't really work!).
     * The returned position can be used as a parameter for the artifastring
     * physical model.
     * 
     * @param midi MIDI note to perform
     * @param stg String on which note is to be performed
     * @return position at which string should be stopped
     */
    double getStopPosn(int midi, int stg) const;
    
  protected:
    static Temperament theTemperament;
};

#endif
