
#include "pair.h"

using namespace std;

/** Constructor
 ** Creates a new pair using references to two existing Letter objects.
 **/
Pair::Pair(Letter* lLetter, Letter* rLetter, FTengine* FTE) {
  lL = lLetter;
  rL = rLetter;
  spacing = FTE->getSpacing(lL->getLetterChar(), rL->getLetterChar());
  //cout << "Created pair " << lL->getLetterChar() << rL->getLetterChar() << " with total spacing " << spacing << endl;
  lWidth = lL->getWidth();
  rWidth = rL->getWidth();
}


void Pair::fillMatrix_dxdy_limited(vector<vector<int>> *matrix, int maxdx, int maxdy, int combno) {

  // iterate through left letter's blackpixels
  for(int li = 0; li < lL->blackpixels.size(); li++) {
    for(int ri = 0; ri < rL->blackpixels.size(); ri++) {
      int dx = abs(spacing + lWidth - lL->blackpixels[li].x - rL->blackpixels[ri].x);
      int dy = abs(lL->blackpixels[li].y - rL->blackpixels[ri].y);
      if(dx < maxdx && dx > 0 && dy < maxdy && dy > 0) {
	matrix->at(combno).at(maxdy*dx+dy) += 1;
      }
    }
  }

}

void Pair::fillMatrix_ydxdy_limited(vector<vector<int>> *matrix, int maxdx, int maxdy, int maxy, int combno) {

  // iterate through left letter's blackpixels
  for(int li = 0; li < lL->blackpixels.size(); li++) {
    for(int ri = 0; ri < rL->blackpixels.size(); ri++) {
      int dx = abs(spacing + lWidth - lL->blackpixels[li].x - rL->blackpixels[ri].x);
      int dy = abs(lL->blackpixels[li].y - rL->blackpixels[ri].y);
      int avgy = (lL->blackpixels[li].y + rL->blackpixels[ri].y)/2/3;
      if(dx < maxdx && dx > 0 && dy < maxdy && dy > 0 && avgy < maxy && avgy > 0) {
	matrix->at(combno).at( (maxy/3)*maxdy*dx + (maxy/3)*dy + avgy) += 1;
      }
    }
  }

}

void Pair::proxiStemAnalysis(vector<vector<int>> *matrix, int combno) {
    
    // for every pair, find distance between stems
    matrix->at(combno)[0] = lL->rStemOffset + spacing + rL->lStemOffset;
// also, note stem intensities and offsets
matrix->at(combno)[1] = 100*lL->rStemIntensity;
matrix->at(combno)[2] = 100*rL->lStemIntensity;
matrix->at(combno)[3] = lL->rStemOffset;
matrix->at(combno)[4] = rL->lStemOffset;

// find closest, farthest, mean, variance, median, and avg(75%-90% dist)
int closest = lL->getWidth() + rL->getWidth();
int farthest = 0;
int mean = 0;
int topavg = 0;

for(int i = 0; i < lL->xheight; i++) {
  if(lL->rEdgeOffset[i] + spacing + rL->lEdgeOffset[i] < closest) {
    closest = lL->rEdgeOffset[i] + spacing + rL->lEdgeOffset[i];
  }
  if(lL->rEdgeOffset[i] + spacing + rL->lEdgeOffset[i] > farthest) {
    farthest = lL->rEdgeOffset[i] + spacing + rL->lEdgeOffset[i];
  }
 }
matrix->at(combno)[5] = closest;
matrix->at(combno)[6] = farthest;
matrix->at(combno)[7] = spacing;

    // for every pair, find proximity as 
    //  - closest distance (x, direct)
    //  - farthest distance 

    // How to quantify proximity?
  }


void Pair::findPairFeatures(vector<vector<int>> *matrix, int combno) {

  // find mathematical center
  
  int math_center = (lWidth + rWidth + spacing)/2;

  // find second moment center iteratively
  int second_center = 0;
  /*
  int second_moment = INT_MAX;
  int second_moment_temp = 0;
  for(int x0 = 0; x0 < (lWidth + rWidth + spacing); x0++) {
    // calculate total moment
    for(int li = 0; li < lL->blackpixels.size(); li++) {
      second_moment_temp += pow(x0-lL->blackpixels[li].x,2);
    }
    for(int ri = 0; ri < rL->blackpixels.size(); ri++) {
      second_moment_temp += pow(x0 - (lWidth + spacing + rL->blackpixels[ri].x),2);
    }
    if(second_moment_temp < second_moment){
      second_moment = second_moment_temp;
      second_moment_temp = 0;
      second_center = x0;
    } else if (second_moment_temp > second_moment) {
      break;
    }
  }

  // find third moment center
  int third_center;
  int third_moment = INT_MAX;
  int third_moment_temp = 0;
  for(int x0 = 0; x0 < (lWidth + rWidth + spacing); x0++) {
    // calculate total moment
    for(int li = 0; li < lL->blackpixels.size(); li++) {
      third_moment_temp += abs(pow(x0-lL->blackpixels[li].x,3));
    }
    for(int ri = 0; ri < rL->blackpixels.size(); ri++) {
      third_moment_temp += abs(pow(x0 - (lWidth + spacing + rL->blackpixels[ri].x),3));
    }
    if(third_moment_temp < third_moment){
      third_moment = third_moment_temp;
      third_moment_temp = 0;
      third_center = x0;
    } else if (third_moment_temp > third_moment) {
      break;
    }
  }
  */
 // find curvey second moment center
  int curvy1_center;
  int curvy1_moment = INT_MAX;
  int curvy1_moment_temp = 0;
  int factor = 1;
  int height = min(lL->getHeight(), rL->getHeight());
  for(int x0 = 0; x0 < (lWidth + rWidth + spacing); x0++) {
    // calculate total moment
    for(int li = 0; li < lL->blackpixels.size(); li++) {
      factor = 1+abs(3-(3.0/(height/2))*lL->blackpixels[li].y);
      curvy1_moment_temp += factor*abs(pow(x0-lL->blackpixels[li].x,3));
    }
    for(int ri = 0; ri < rL->blackpixels.size(); ri++) {
      factor = 1+abs(3-(3.0/(height/2))*rL->blackpixels[ri].y);
      curvy1_moment_temp += factor*abs(pow(x0 - (lWidth + spacing + rL->blackpixels[ri].x),3));
    }
    if(curvy1_moment_temp < curvy1_moment){
      curvy1_moment = curvy1_moment_temp;
      curvy1_moment_temp = 0;
      curvy1_center = x0;
    }
  }

  matrix->at(combno)[0] = math_center;
  matrix->at(combno)[1] = second_center;
  matrix->at(combno)[2] = curvy1_center;
  // find in-between-letter area
  
  // find 

}

