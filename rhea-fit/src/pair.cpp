
#include "pair.h"

#define PI 3.14159276
using namespace std;

/** Constructor
 ** Creates a new pair using references to two existing Letter objects.
 **/
Pair::Pair(Letter* lLetter, Letter* rLetter, int spacing) {
  lL = lLetter;
  rL = rLetter;
  this->spacing = spacing;
  lWidth = lL->getWidth();
  rWidth = rL->getWidth();
}

void Pair::fillPairPolynomialForceMatrix(vector<vector<float>> *matrix, int combno, const int degree) {
  /* for every pixel-pixel combo between first and second letter,
   * add one to the corresponding matrix cell.
   */

  // first, prepare the host matrix.
  int totalwidth = lL->width + spacing + rL->width;
  int totalheight = max(lL->y_top, rL->y_top);
  char *pairpixelsH; // host data
  int nPairpixels = totalwidth * totalheight;
  int nBytes = nPairpixels * sizeof(char);
  pairpixelsH = (char*)calloc(nBytes, sizeof(char)); //initialize to zero


  // fill the host matrix with stuff
  for(int pl = 0; pl < lL->blackpixels.size(); pl++) {
    if(lL->blackpixels[pl].y < 0) continue;
    pairpixelsH[lL->blackpixels[pl].y*totalwidth + lL->blackpixels[pl].x] = 1;
  }
  for(int pr = 0; pr < rL->blackpixels.size(); pr++) {
    if(rL->blackpixels[pr].y < 0) continue;
    pairpixelsH[rL->blackpixels[pr].y*totalwidth + lL->width + rL->blackpixels[pr].x + spacing] = 1;
  }

  // create pixel pairs
  int nPairs = lL->blackpixels.size() * rL->blackpixels.size();

  unsigned char* pairsH;
  pairsH = (unsigned char*)calloc(6*nPairs*sizeof(unsigned char), sizeof(unsigned char));
  
  for(int pl = 0; pl < lL->blackpixels.size(); pl++) {
    for(int pr = 0; pr < rL->blackpixels.size(); pr++) {
      unsigned char* pairPtr = &pairsH[6*(pl * rL->blackpixels.size() + pr)];
      pairPtr[0] = (lL->blackpixels[pl].x >> 8) & 0xff;
      pairPtr[1] = lL->blackpixels[pl].x & 0xff;
      pairPtr[2] = ((rL->blackpixels[pr].x + lL->width + spacing) >> 8) & 0xff;
      pairPtr[3] = (rL->blackpixels[pr].x + lL->width + spacing) & 0xff;
      pairPtr[4] = lL->blackpixels[pl].y & 0xff;
      pairPtr[5] = rL->blackpixels[pr].y & 0xff;
    }
  }
  
  // create black info for pixel pairs
  unsigned char* pairsbH = (unsigned char*)calloc(nPairs*sizeof(unsigned char), sizeof(unsigned char));
  for(int pl = 0; pl < lL->blackpixels.size(); pl++) {
    for(int pr = 0; pr < rL->blackpixels.size(); pr++) {
      pairsbH[pl * rL->blackpixels.size() + pr] = (lL->brs[pl] + rL->bls[pr]) & 0xff;
    }
  }

  // create the output polynomial
  float *outmH;
  outmH = (float*)calloc(pow((degree+1),4)*sizeof(float), sizeof(float));

 // kernel_wrapper(pairpixelsH, totalwidth, totalheight, pairsH, nPairs, outmH, degree, pow((degree+1),4), lL->xheight, combno<=10?true:false , pairsbH);

  // add results to matrix
  for(int i=0; i < pow((degree+1),4); i++) {
    matrix->at(combno)[i] = outmH[i];
  }

  // free host stuff
  free(pairpixelsH);
  free(pairsH);
  free(outmH);
  // return
  return;


}
void Pair::fillPairForceMatrix(vector<vector<int>> *matrix, int combno) {
  /* for every pixel-pixel combo between first and second letter,
   * add one to the corresponding matrix cell.
   */

  // cout << "\r Doing:" << lL->getLetterChar() << cL->getLetterChar() <<
  //rL->getLetterChar() << " " << combno << endl;
  int dx, dy, yavg;
  for(int pl = 0; pl < lL->blackpixels.size(); pl++) {
    for(int pr = 0; pr < rL->blackpixels.size(); pr++) {
      if(rL->blackpixels[pr].y > rL->xheight or rL->blackpixels[pr].y < 0
          or lL->blackpixels[pl].y > lL->xheight or lL->blackpixels[pl].y < 0) 
        continue;
      dx = rL->blackpixels[pr].x - lL->blackpixels[pl].x + lWidth + spacing;
      dy = rL->blackpixels[pr].y - lL->blackpixels[pl].y;
      yavg = 0.5 * (rL->blackpixels[pr].y + lL->blackpixels[pl].y);
      dx /= 2;
      dy /= 5;
      dy = abs(dy);
      yavg /= 5;
      if(dx < 20 && dy < 10 && yavg < 10 && dx > 0 && dy > 0 && yavg > 0) {
        matrix->at(combno).at(100*dx + 10*dy + yavg) += 1;
      }
    }
  }

}

void Pair::fillPairEdgeForceMatrix(vector<vector<int>> *matrix, int combno, int
    dxsteps, int dysteps, int yavgsteps) {
  /* for every pixel-pixel combo between first and second letter,
   * add one to the corresponding matrix cell.
   */

  // cout << "\r Doing:" << lL->getLetterChar() << cL->getLetterChar() <<
  //rL->getLetterChar() << " " << combno << endl;
  int dx, dy, yavg;
  int xheight = lL->xheight;
  for(int pl = 0; pl < lL->xheight; pl++) {
    for(int pr = 0; pr < rL->xheight; pr++) {

      dx = rL->lEdgeOffset[pr] - lL->rEdgeOffset[pl] + spacing;
      if (dx < 0) continue;
      if (dx >= xheight) dx = xheight-1;
      dx = sqrt(dx) * dxsteps * 1.0/sqrt(xheight);

      dy = sqrt(abs(pl-pr)) * dysteps * 1.0 / sqrt(xheight);

      yavg = 0.5 * (pl+pr) * yavgsteps / xheight;
      matrix->at(combno).at(dysteps*yavgsteps*dx + yavgsteps*dy + yavg) += 1;
    }
  }

}


