
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

void Triplet::fillTripletForceMatrix(vector<vector<int>> *matrix, int combno) {

/* for every pixel-pixel combo between first and second letter,
 * add one to the corresponding matrix cell. Then do the same for second/third.
 */

 // cout << "\r Doing:" << lL->getLetterChar() << cL->getLetterChar() <<
 //rL->getLetterChar() << " " << combno << endl;
int dx, dy, yavg;
  for(int pl = 0; pl < lL->blackpixels.size(); pl++) {
    for(int pc = 0; pc < cL->blackpixels.size(); pc++) {
      if(cL->blackpixels[pc].y > cL->xheight or cL->blackpixels[pc].y < 0
          or lL->blackpixels[pl].y > lL->xheight or lL->blackpixels[pl].y < 0) 
              continue;
      dx = cL->blackpixels[pc].x - lL->blackpixels[pl].x + lWidth + lSpacing;
      dy = cL->blackpixels[pc].y - lL->blackpixels[pl].y;
      yavg = 0.5 * (cL->blackpixels[pc].y + lL->blackpixels[pl].y);
      dx /= 2;
      dy /= 5;
      dy = abs(dy);
      yavg /= 5;
      if(dx < 20 && dy < 10 && yavg < 10) {
        matrix->at(combno).at(100*dx + 10*dy + yavg) += 1;
      }
    }
  }
  
  //cout << "\r Still doing:" << lL->getLetterChar() << cL->getLetterChar() <<
  //rL->getLetterChar() <<   endl;
  for(int pc = 0; pc < cL->blackpixels.size(); pc++) {
    for(int pr = 0; pr < rL->blackpixels.size(); pr++) {
      if(cL->blackpixels[pc].y > cL->xheight or cL->blackpixels[pc].y < 0
          or rL->blackpixels[pr].y > rL->xheight or rL->blackpixels[pr].y < 0) 
              continue;

      dx = rL->blackpixels[pr].x - cL->blackpixels[pc].x + cWidth + rSpacing;
      dy = rL->blackpixels[pr].y - cL->blackpixels[pc].y;
      yavg = 0.5 * (rL->blackpixels[pr].y + cL->blackpixels[pc].y);
      dx /= 2;
      dy /= 5;
      dy = abs(dy);
      yavg /= 5;
      if(dx < 20 && dy < 10 && yavg < 10) {
        matrix->at(combno).at(100*dx + 10*dy + yavg) -= 1;
      }
    }
  }
 // cout << "\r Did:" << lL->getLetterChar() << cL->getLetterChar() <<
//  rL->getLetterChar() << endl;
}

void Triplet::fillTripletYWeightMatrix(vector<vector<int>> *matrix, int combno) {
  // for this triplet, find math center
  double math_center = (lWidth + cWidth + rWidth + lSpacing + rSpacing)/2;
  double a, b, c, d, third_center1, third_center2, third_center3;

 
  // then find the third moment center using the cubic equation
  //cout << "Now analyzing triplet " << combno << endl;
  for(int y = 0; y < lL->xheight; y++) {
    for(int x = 0; x < lWidth; x++) {
      a += lL->getPixel(y, x) * pow(x,3); // sum(x*a^3);
      b += 3*(lL->getPixel(y,x)*pow(x,2));// 3*sum(x*a^2);
      c += 3*(lL->getPixel(y,x)*x); // 3*sum(x*a);
      d += lL->getPixel(y,x); // sum(x);
    }
    for(int x = 0; x < cWidth; x++) {
      a += cL->getPixel(y, x) * pow((x+lWidth+lSpacing),3); // sum(x*a^3);
      b += 3*(cL->getPixel(y,x)*pow((x+lWidth+lSpacing),2));// 3*sum(x*a^2);
      c += 3*(cL->getPixel(y,x)*(x+lWidth+lSpacing)); // 3*sum(x*a);
      d += cL->getPixel(y,x); // sum(x);
    }
    for(int x = 0; x < rWidth; x++) {
      a += rL->getPixel(y, x) * pow((x+lWidth+lSpacing+cWidth+rSpacing),3);
      b += 3*(rL->getPixel(y,x)*pow((x+lWidth+lSpacing+cWidth+rSpacing),2));
      c += 3*(rL->getPixel(y,x)*(x+lWidth+lSpacing+cWidth+rSpacing));
      d += rL->getPixel(y,x); // sum(x);
    }
    
    gsl_poly_solve_cubic (c/d, b/d, a/d, &third_center1, &third_center2, &third_center3);
    /*
    if (combno == 43 && y == 0) {
          cout << "y,a,b,c,d,c/d,b/d,a/d,tc1, tc2, tc3, mc" << endl;
    }


    if (combno == 43) {

      cout << y << "," << a << "," << b << "," << c << "," << d << "," << c/d << "," << b/d << "," << a/d << "," << third_center1 << "," << third_center2 << "," << third_center3 << "," << math_center << endl;
    }
    //    cout << "Found third center at " << third_center << endl;

    double mindist = math_center - third_center1;
    if(abs(math_center - third_center2) < abs(mindist)) {
      mindist = math_center - third_center2;
    }
    if(abs(math_center - third_center3) < abs(mindist)) {
      mindist = math_center - third_center3;
    }

    if (abs(mindist) >= math_center) {
      mindist = 0;
    }
    */
    matrix->at(combno)[y] = (-third_center1)-math_center;
  }
}

void Triplet::findTripletFeatures(vector<vector<int>> *matrix, int combno) {

  // find mathematical center
  
  int math_center = (lWidth + cWidth + rWidth + lSpacing + rSpacing)/2;

  // find second moment center iteratively
  int second_center = 0;
  /*
  unsigned long long second_moment = 18446744073709551615;
  unsigned long long second_moment_temp = 0;
  for(int x0 = 0; x0 < (lWidth + cWidth + rWidth + lSpacing + rSpacing); x0++) {
    // calculate total moment
    for(int li = 0; li < lL->blackpixels.size(); li++) {
      second_moment_temp += abs(pow(x0-lL->blackpixels[li].x,3));
    }
    for(int ci = 0; ci < cL->blackpixels.size(); ci++) {
      second_moment_temp += abs(pow(x0-(lWidth + lSpacing + cL->blackpixels[ci].x),3));
    }
    for(int ri = 0; ri < rL->blackpixels.size(); ri++) {
      second_moment_temp += abs(pow(x0 - (lWidth + lSpacing + cWidth + rSpacing + rL->blackpixels[ri].x),3));
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
  for(int x0 = 0; x0 < (lWidth + cWidth + rWidth + lSpacing + rSpacing); x0++) {
    // calculate total moment
    for(int li = 0; li < lL->blackpixels.size(); li++) {
      third_moment_temp += pow(x0-lL->blackpixels[li].x,3);
    }
    for(int ci = 0; ci < cL->blackpixels.size(); ci++) {
      third_moment_temp += pow(x0-(lWidth + lSpacing + cL->blackpixels[ci].x),3);
    }
    for(int ri = 0; ri < rL->blackpixels.size(); ri++) {
      third_moment_temp += pow(x0 - (lWidth + lSpacing + cWidth + rSpacing + rL->blackpixels[ri].x),3);
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
  int height = lL->xheight;

  for(int x0 = 0; x0 < (lWidth + cWidth + rWidth + lSpacing + rSpacing); x0++) {
    // calculate total moment
    for(int li = 0; li < lL->blackpixels.size(); li++) {
      factor = 2+(4*lL->blackpixels[li].y/height)-(4*pow(lL->blackpixels[li].y,2)/(height*height));
      curvy1_moment_temp += (int) factor*abs(pow(x0-lL->blackpixels[li].x,2));
    }
    for(int ci = 0; ci < lL->blackpixels.size(); ci++) {
      factor = 2+(4*cL->blackpixels[ci].y/height)-(4*pow(cL->blackpixels[ci].y,2)/(height*height));
      curvy1_moment_temp += (int) factor*abs(pow(x0- (lWidth + lSpacing + cL->blackpixels[ci].x),2));
    }
    for(int ri = 0; ri < rL->blackpixels.size(); ri++) {
      factor = 2+(4*rL->blackpixels[ri].y/height)-(4*pow(rL->blackpixels[ri].y,2)/(height*height));

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
  //matrix->at(combno)[1] = second_center;
  //  matrix->at(combno)[2] = third_center;
  matrix->at(combno)[3] = curvy1_center;
  // find in-between-letter area
  
  // find 

}

