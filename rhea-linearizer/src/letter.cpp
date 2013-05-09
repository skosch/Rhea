#include "letter.h"

#define PI 3.14159276

using namespace std;


Letter::Letter(char letterChar, vector<bool> rawArray, int height, int y_baseline, int width, int xheight, int capheight) {

  // set variables
  this->letterChar = letterChar;

  for(int row = 0; row < height; row++) {
    for(int x = 0; x < width; x++) {
      if(rawArray[row*width + x]) {
        point blp;
        blp.x = x;
        blp.y = y_baseline-row;
        blackpixels.push_back(blp);
      }
    }
  }

  this->height = height;
  this->y_top = y_baseline;
  this->y_bottom = height - y_baseline;
  this->width = width;
  this->xheight = xheight;
  this->capheight = capheight;

  rEdgeOffset.resize(xheight);
  std::fill(rEdgeOffset.begin(), rEdgeOffset.end(), width);
  lEdgeOffset.resize(xheight);
  std::fill(lEdgeOffset.begin(), lEdgeOffset.end(), width);
  f_rlEdges();

  fill_b_on_angle();
  //printLetterInfo();
  //f_houghstems();
  //analyze();
}

// calculate blackpixel-at-angle matrix
void Letter::fill_b_on_angle() {
 // TODO SOMETHING GOES WRONG HERE.
  b_on_angle.resize(blackpixels.size());
  int x0, x1, y0, y1,b, dx, dy;
  int error, e2, sx, sy;

  for(int i=0; i<b_on_angle.size(); i++) {
    b_on_angle[i].resize(360);

    // calculate number of black pixels in that direction.
    for(int a=0; a<360; a++) {
      b = 0;
      x0 = blackpixels[i].x;
      y0 = blackpixels[i].y;

      float aNE = atan((height-y0)*1.0/(width-x0))*180.0/PI;
      float aNW = 180 - atan((height-y0)*1.0/x0)*180.0/PI;
      float aSW = 180 + atan((y0*1.0)/x0)*180.0/PI;
      float aSE = 360 - atan(y0*1.0/(width-x0))*180.0/PI;

      if(a > aSE or a <= aNE) {
        x1 = width - 1;
        y1 = (int) (y0 + tan(a*PI/180.0)*(width-x0));
      } else if(a <= aNW) {
        x1 = (int) (x0 + tan(M_PI_2 - a*PI/180.0)*(height-y0));
        y1 = height - 1;
      } else if(a <= aSW) {
        x1 = 0;
        y1 = (int) (y0 - tan(a*PI/180.0)*(x0));
      } else if(a <= aSE) {
        x1 = (int) (x0 - tan(M_PI_2 - a*PI/180.0)*(y0));
        y1 = 0;
      }

      dx = x1 - x0;
      dy = y1 - y0;
      b = 0;

      // compute b using Bresenham's algorithm
      sx = (x0 < x1) ? 1 : -1;
      sy = (y0 < y1) ? 1 : -1;
      error = abs(dx)-abs(dy);
      do {
        b += getPixelFromBaseline(x0, y0);
        e2 = 2*error; // move on
        if (e2 > -dy) {
          error -= dy;
          x0 += sx;
        }
        if (e2 < dx) { 
          error += dx;
          y0 += sy;
        }
      } while((x0<(x1*sx)) and (y0<(y1*sy)));
      b_on_angle[i][a] = b;
    } // end for through all angles
  } // end for through all blackpixels
}

int Letter::getPixel(int y, int x) {
  y = this->height - y;
  return (pixels[y*pixelpitch + x] > 0 ? 1 : 0);
}

int Letter::getPixelFromBaseline(int y, int x) {
  y = this->y_top - y;
  return (pixels[y*pixelpitch + x] > 0 ? 1 : 0);
}

void Letter::printLetterInfo() {
  cout << "Info for Letter " << letterChar << ":" << endl;
  cout << "Height:\t" << height << endl;
  cout << "Ytop:\t" << y_top << endl;
  cout << "Ybottom:" << y_bottom << endl;
  cout << "Width:\t" << width << endl;


}

void Letter::f_rlEdges() {
  // this goes from 0 to xheight
  for(int i = 0; i < blackpixels.size(); i++) {
    if(blackpixels[i].y < 0 or blackpixels[i].y >= xheight) continue;

    if(blackpixels[i].x < lEdgeOffset[blackpixels[i].y]) {
      lEdgeOffset[blackpixels[i].y] = blackpixels[i].x;
    }
    if((width-blackpixels[i].x) < rEdgeOffset[blackpixels[i].y]) {
      rEdgeOffset[blackpixels[i].y] = width-blackpixels[i].x;
    }
  }
}

