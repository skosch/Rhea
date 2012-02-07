#ifndef PAIR_H
#define PAIR_H

#include <string>
#include <iostream>
#include <vector>
#include <ft2build.h>
#include "core/core.hpp"

#include "letter.h"
#include "FT.h"

class Pair {

 public:
  Pair(Letter* lLetter, Letter* rLetter, FTengine* FTE);


 private:
  Letter* lL;
  Letter* rL;

  int spacing; // rsb + kerning + lsb
  int kerning; // kerning value for this pair


};

#endif
