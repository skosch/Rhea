#ifndef PAIR_H
#define PAIR_H

#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <limits.h>
#include <ft2build.h>
#include "core/core.hpp"
#include "lpsolve/lp_lib.h"

#include "letter.h"
#include "FT.h"

class Pair {

 public:
  Pair(Letter* lLetter, Letter* rLetter, FTengine* FTE);

  void fillMatrix_contourvar(vector<vector<int>> *matrix, int combno);
  void fillMatrix_dxdy_limited(vector<vector<int>> *matrix, int maxdx, int maxdy, int combno);
  void fillMatrix_ydxdy_limited(vector<vector<int>> *matrix, int maxdx, int maxdy, int maxy, int combno);
  void findPairFeatures(vector<vector<int>> *matrix, int combno);
  void fillPairPolynomialForceMatrix(vector<vector<int>> *matrix, int combno);
  void fillPairForceMatrix(vector<vector<int>> *matrix, int combno);
  void fillPairEdgeForceMatrix(vector<vector<int>> *matrix, int combno, int
dxsteps, int dysteps, int yavgsteps);
  void proxiStemAnalysis(vector<vector<int>> *matrix, int combno);
  void fillBubbleMatrix(vector<vector<vector<int>>> *bubblematrix, vector<vector<int>> *distancematrix, int combno);

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
