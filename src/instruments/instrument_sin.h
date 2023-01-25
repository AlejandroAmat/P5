#ifndef INSTRUMENT_SENO
#define INSTRUMENT_SENO

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc {
  class InstrumentSin: public upc::Instrument {
    EnvelopeADSR adsr;
    unsigned int index;
	float A;
    std::vector<float> tbl;
    float f0, increment,phas;
  public:
    InstrumentSin(const std::string &param = "");
    void command(long cmd, long note, long velocity=100); 
    const std::vector<float> & synthesize();
    bool is_active() const {return bActive;} 
  };
}

#endif