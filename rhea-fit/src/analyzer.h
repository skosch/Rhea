#ifndef ANALYZER_H
#define ANALYZER_H

#include <tuple>
#include <vector>
#include <map>
#include <math.h>
#include <cstdlib>

#include "Eigen/Dense"
#include "Eigen/SVD"

#include "csvrow.h"
#include "letter.h"
#include "pair.h"
#include "openclfuncs.h"

class Analyzer {
  public:
    Analyzer();

    // standard constructor -- takes in rasterized letters plus known spacings
    Analyzer(std::tuple< std::map< char, Letter* >, int, int, int, int > rasterizedLetters,
        std::vector< Pair* > prefitPairs, int degree);

    // debugging constructor -- takes in a matrix in CSV format
    Analyzer(string matrixFilename, int degree);

    void getMatrix();
    void findAndStoreCoeffs(string coeffsFilename);

  private:
  int xheight;
  int capheight;
  int maxwidth;
  int bdepth;
  int degree;
  int nMonomials;
  std::vector< Pair* > prefitPairs;
  std::vector< std::vector< float > > matrix;
  Eigen::MatrixXf eigenMatrix;

  Eigen::MatrixXf pinv(Eigen::MatrixXf& m, double epsilon);
};

#endif
