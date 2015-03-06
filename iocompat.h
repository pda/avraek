#pragma once

#include <avr/io.h>

#ifndef TIFR0
  #ifdef TIFR
    // e.g. ATtiny85
    #define TIFR0 TIFR
  #else
    #error "TIFR0 or TIFR required"
  #endif
#endif
