
#include "triplet.h"

using namespace std;

/** Constructor
 ** Creates a new pair using references to two existing Letter objects.
 **/
Triplet::Triplet(Letter* lLetter, Letter* cLetter, Letter* rLetter, FTengine* FTE) {
  lL = lLetter;
  cL = cLetter;
  rL = rLetter;
  lSpacing = FTE->getSpacing(lL->getLetterChar(), cL->getLetterChar());
  rSpacing = FTE->getSpacing(cL->getLetterChar(), rL->getLetterChar());
  //cout << "Created pair " << lL->getLetterChar() << rL->getLetterChar() << " with total spacing " << spacing << endl;
  lWidth = lL->getWidth();
  cWidth = cL->getWidth();
  rWidth = rL->getWidth();
}



void Triplet::findTripletFeatures(vector<vector<int>> *matrix, int combno) {

  // find mathematical center
  
  int math_center = (lWidth + cWidth + rWidth + lSpacing + rSpacing)/2;

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
  int height = min(lL->getHeight(), min(rL->getHeight(), cL->getHeight()));

  for(int x0 = 0; x0 < (lWidth + cWidth + rWidth + lSpacing + rSpacing); x0++) {
    // calculate total moment
    for(int li = 0; li < lL->blackpixels.size(); li++) {
      factor = 1+abs(3-(3.0/(height/2))*lL->blackpixels[li].y);
      curvy1_moment_temp += (int) factor*abs(pow(x0-lL->blackpixels[li].x,2));
    }
    for(int ci = 0; ci < lL->blackpixels.size(); ci++) {
      factor = 1+abs(3-(3.0/(height/2))*cL->blackpixels[ci].y);
      curvy1_moment_temp += (int) factor*abs(pow(x0- (lWidth + lSpacing + cL->blackpixels[ci].x),2));
    }
    for(int ri = 0; ri < rL->blackpixels.size(); ri++) {
      factor = 1+abs(3-(3.0/(height/2))*rL->blackpixels[ri].y);
      curvy1_moment_temp += (int) factor*abs(pow(x0 - (lWidth + lSpacing + cWidth + rSpacing + rL->blackpixels[ri].x),2));
    }
    if(curvy1_moment_temp <= curvy1_moment){
      curvy1_moment = curvy1_moment_temp;
      curvy1_moment_temp = 0;
      curvy1_center = x0;
    } else {
      //      break;
    }
  }

  matrix->at(combno)[0] = math_center;
  matrix->at(combno)[1] = second_center;
  matrix->at(combno)[2] = curvy1_center;
  // find in-between-letter area
  
  // find 

}

