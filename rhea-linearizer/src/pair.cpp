
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


