#include "fitter.h"

Fitter::Fitter(
    std::tuple< std::map< char, Letter*>, int, int, int, int > rasterizedLetters,
    std::vector< float > coeffs,
    int degree) {
  
  std::map< char, Letter* > letterDict = std::get<0>(rasterizedLetters);
  this->xheight = std::get<1>(rasterizedLetters);
  this->capheight = std::get<2>(rasterizedLetters);
  this->maxwidth = std::get<3>(rasterizedLetters);
  this->bdepth = std::get<4>(rasterizedLetters);
  this->degree = degree;
  this->coeffs = coeffs;

}


