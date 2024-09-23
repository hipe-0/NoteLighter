// Potentiometers
// by Hippolyte Mounier
// 2024-06-19
//
// Goal : handles a number of potentiometers, 

#define POT_UNDEFINED    -1
#define FILTER_WEIGHT    0.3
#define VALUE_STABILIZER 4.0     // filters chaotic values

#include "iferror.h"

static inline int min_(int x, int y) { return (x>y) ? y : x ; }
static inline int max_(int x, int y) { return (x>y) ? x : y ; }

class Potentiometers
{
  private:
  IntegrityChecker a;

  long                     lastUpdate_;
  static const int         NB_POTS_MAX = 20;

  int   pins_              [NB_POTS_MAX];
  int   potMin_            [NB_POTS_MAX];
  int   potMid_            [NB_POTS_MAX];
  int   potMax_            [NB_POTS_MAX];
  float calibMin_          [NB_POTS_MAX];
  float calibMid_          [NB_POTS_MAX];  
  float calibMax_          [NB_POTS_MAX];
  float precision_         [NB_POTS_MAX];

  // dynamic values
	int   rawPotValues_          [NB_POTS_MAX];
  float filteredPotValues_     [NB_POTS_MAX];
  float lastConsideredPotValues_ [NB_POTS_MAX];

  float currentPotValuesMaped_ [NB_POTS_MAX];
  float lastPotValuesMaped_     [NB_POTS_MAX];

  IntegrityChecker b;

  public:
  Potentiometers(): a("MuxPotentiometersBegin"), b("MuxPotentiometersEnd")
  { 
    for(int i = 0; i<NB_POTS_MAX ; i++)
      pins_ [i] = POT_UNDEFINED;
  }


  void definePot(int id, int pinNr, int min, int mid, int max, float minvalue, float midvalue, float maxvalue, float precision = 0.01)
  {
    IFERROR ( id < 0 || id >= NB_POTS_MAX, "definePot has wrong id" )

    pins_[id]       = pinNr;
    potMin_  [id]   = min;
    potMid_  [id]   = mid;
    potMax_  [id]   = max;
    calibMin_[id]   = minvalue;
    calibMid_[id]   = midvalue;
    calibMax_[id]   = maxvalue;
    precision_[id]  = precision;

    rawPotValues_[id] = analogRead(pins_[id]);
    filteredPotValues_[id] = rawPotValues_[id];
    currentPotValuesMaped_[id] = mapPotentiometer_(filteredPotValues_[id], potMin_[id], potMid_[id], potMax_[id], calibMin_[id], calibMid_[id], calibMax_[id]);
  }


  void readPotentiometers()
	{
	  for(int i = 0; i < NB_POTS_MAX ; i++)
      if(pins_[i] != POT_UNDEFINED)
      {
        lastPotValuesMaped_[i]     = currentPotValuesMaped_[i] ;

        int raw = analogRead(pins_[i]);
        rawPotValues_[i] = raw;

        filteredPotValues_[i] =  filteredPotValues_[i] * (1.0 - FILTER_WEIGHT) +  ((float) raw) * FILTER_WEIGHT ;

        if ( abs( filteredPotValues_[i] - lastConsideredPotValues_[i] ) >= VALUE_STABILIZER )
        {
          lastConsideredPotValues_[i] = filteredPotValues_[i];
          float maped = mapPotentiometer_(filteredPotValues_[i], potMin_[i], potMid_[i], potMax_[i], calibMin_[i], calibMid_[i], calibMax_[i]);
          if ( precision_[i] > 0.01 ) 
            maped = round(maped / precision_[i]) *  precision_[i];

          currentPotValuesMaped_[i] = maped;
        }

      }

    a.failIfCorrupt();
    b.failIfCorrupt();
  }


  float getPotValueMaped(int number)
  {
    if ( pins_[number] == POT_UNDEFINED)       return 0.0; // error code

    IFERROR ( number >= NB_POTS_MAX || number < 0, "getPotValueMaped has wrong input number" )

    return currentPotValuesMaped_[number] ;
  }


  bool hasChanged(int number)
  {
    if ( pins_[number] == POT_UNDEFINED)       return 0;

    IFERROR ( number >= NB_POTS_MAX || number < 0, "hasChanged has wrong input number" )

    return (currentPotValuesMaped_[number] != lastPotValuesMaped_[number]);
  }

  int getRawPotValue(int number)
  {
    IFERROR ( number >= NB_POTS_MAX || number < 0, "getRawPotValue has wrong input number" )

    return rawPotValues_[number];
  }

  private :

  float mapPotentiometer_(int potValue, int potMin, int potMid, int potMax, float calibMin, float calibMid, float calibMax) 
  {
    // Ensure potValue is within the valid range
    potValue = min_(max_(potValue, 0), 1023);

    // Map the potentiometer value to the calibrated range
    float mappedValue;
    if (potValue <= potMid) 
      // Map to the lower half
      mappedValue = calibMid - (calibMid - calibMin) * (( potMid - static_cast<float>(potValue)) / (potMid - potMin));
     else 
      // Map to the upper half
      mappedValue = calibMid + (calibMax - calibMid) * ((static_cast<float>(potValue) - potMid) / (potMax - potMid));

    if (mappedValue > calibMax) mappedValue = calibMax;
    if (mappedValue < calibMin) mappedValue = calibMin;
    return mappedValue;
  }

};

