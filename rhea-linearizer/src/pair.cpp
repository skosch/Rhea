
#include "pair.h"

#define PI 3.14159276
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

void Pair::fillPairPolynomialForceMatrix(vector<vector<int>> *matrix, int combno) {
/* for every pixel-pixel combo between first and second letter,
 * add one to the corresponding matrix cell.
 */

 // cout << "\r Doing:" << lL->getLetterChar() << cL->getLetterChar() <<
 //rL->getLetterChar() << " " << combno << endl;
int dx, dy, yavg, b;
int x0, x1, y0, y1;

  cout << lL->getLetterChar() << rL->getLetterChar() << " now coming up"<< endl;
  cout << "0% done";
  for(int pl = 0; pl < lL->blackpixels.size(); pl++) {
    for(int pr = 0; pr < rL->blackpixels.size(); pr++) {
      if(rL->blackpixels[pr].y < 0 or lL->blackpixels[pl].y < 0) {
              cout << "skipping" << endl;
              continue;
      }
      x0 =  lL->blackpixels[pl].x;
      x1 = lWidth + spacing + rL->blackpixels[pr].x;
      dx = x1 - x0;
      
      y0 = lL->blackpixels[pl].y;
      y1 = rL->blackpixels[pr].y;
      dy = y1 - y0;
      
      // calculate angles and total b
      int angleleft;
      int angleright;

      if(y0 <= y1 and x0 < x1) {
        angleleft = atan(dy*1.0/dx)*180.0/PI;
        angleright = angleleft + 180;
      } else if(y0 <= y1 and x0 > x1) {
        angleleft = 90 + atan(-dy*1.0/dx)*180.0/PI;
        angleright = angleleft + 180;
      } else if(y0 > y1 and x0 < x1) {
        angleleft = 360 - atan(-dy*1.0/dx)*180.0/PI;
        angleright = angleleft - 180;
      } else if(y0 > y1 and x0 >= x1) {
        angleleft = 180 + atan(dy*1.0/dx)*180.0/PI;
        angleright = angleleft - 180;
      }

      b = lL->b_on_angle[pl][(int)(angleleft)]
        + rL->b_on_angle[pr][(int)(angleright)];

      yavg = 0.5 * (y0 + y1);
    
      

      // loop through matrix row and add polynomial values
      for(int ex=0; ex < 4; ex++) {
        for(int ey=0; ey < 4; ey++) {
          for(int ez=0; ez < 4; ez++) {
            for(int eb=0; eb < 4; eb++) {
              double result = pow(dx*1.0/lL->xheight, ex) + pow(dy*1.0/lL->xheight, ey) + pow(yavg*1.0/lL->xheight, ez) + pow(b*1.0/lL->xheight, eb);
              matrix->at(combno).at(eb + 4*ez + 16*ey + 64*ex) 
                += result;
            }
          }
        }
      } 
      
    }
  }
  
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


void Pair::fillMatrix_contourvar(vector<vector<int>> *matrix, int combno) {
  // stddev l/r/b, avgdist, mindist, maxdist;
  float avgdist = 0;
  float sd_dist = 0;
  float mindist = 100000;
  float maxdist = 0;
  float ddist = 0;
  for(int y = 0; y < lL->xheight; y++) {
    avgdist += (lL->rEdgeOffset[y] + spacing + rL->lEdgeOffset[y]);
  }
  avgdist /= lL->xheight;

  for(int y = 0; y < lL->xheight; y++) {
    ddist = (lL->rEdgeOffset[y] + rL->lEdgeOffset[y]);
    sd_dist += pow(ddist -avgdist , 2);
    if (ddist < mindist) mindist = ddist;
    if (ddist > maxdist) maxdist = ddist;
  }
  sd_dist = sqrt(sd_dist);
  matrix->at(combno)[0] = avgdist;
  matrix->at(combno)[1] = sd_dist;
  matrix->at(combno)[2] = mindist;
  matrix->at(combno)[3] = maxdist;
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

void Pair::fillBubbleMatrix(vector<vector<vector<int>>> *bubblematrix, vector<vector<int>> *distancematrix, int combno) {

    int lLno = ((int)lL->getLetterChar())-97;
    int rLno = ((int)rL->getLetterChar())-97; // find alphabet # from ascii
    
    for(int y = 0; y < lL->xheight; y++) {
      bubblematrix->at(y)[combno][lLno*2+1] = 1; // lL but right bubble edge
      bubblematrix->at(y)[combno][rLno*2] = 1;
      distancematrix->at(y)[combno] = lL->rEdgeOffset[y] + spacing + rL->lEdgeOffset[y];
    }

    // go through all y-coordinates
    // for each, add up edgepixels + spacing to get total distance
    // bubblematrix->at(y)[combno][rLno] = 1
    // bubblematrix->at(y)[combno][rRno] = 1
    // distancematrix->at(y)[combno] = totaldistance


  }
