#ifndef PAIR_H
#define PAIR_H

#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <limits.h>

#include "letter.h"

class Pair {

 public:
  Pair(Letter* lLetter, Letter* rLetter, int spacing);

  void fillPairPolynomialForceMatrix(vector<vector<int>> *matrix, int combno);
  void fillPairForceMatrix(vector<vector<int>> *matrix, int combno);
  void fillPairEdgeForceMatrix(vector<vector<int>> *matrix, int combno, int
dxsteps, int dysteps, int yavgsteps);

  Letter* getlLetter() {return lL;}
  Letter* getrLetter() {return rL;}

 private:
  Letter* lL;
  Letter* rL;

  int lWidth;
  int rWidth;

  int spacing; // rsb + kerning + lsb

};

#endif
