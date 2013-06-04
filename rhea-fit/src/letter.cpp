#include "letter.h"

#define PI 3.14159276

using namespace std;


Letter::~Letter() {
  if(field != NULL) {
    free(field);
  }
}

Letter::Letter(char letterChar, vector<bool> rawArray, int height, int y_baseline, int width, const int xheight, int capheight) {

  // set variables
  this->letterChar = letterChar;

  int br, bl;
  for(int row = 0; row < y_baseline; row++) {
    for(int x = 0; x < width; x++) {
      if(rawArray[row*width + x]) {
        point blp;
        blp.x = x;
        blp.y = y_baseline - row - 1;
        blackpixels.push_back(blp);

        br = 0;
        bl = 0;
        // now see how many black pixels there are to the right
        for(int xr = x + 1; xr < width; xr++) {
          if(rawArray[row*width + xr]) br++;
        }
        // now see how many black pixels there are to the left
        for(int xl = x - 1; xl >= 0; xl--) {
          if(rawArray[row*width + xl]) bl++;
        }
        brs.push_back(br);
        bls.push_back(bl);
      }
    }
  }

  this->numPixels = blackpixels.size();  

  this->pixels = rawArray;
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
  
}

void Letter::createFieldPairs(int nPairs, unsigned char* pairsH, int maxdeltax, int height, int bdepth) {
  pairsH = (unsigned char*)calloc(8*nPairs*sizeof(unsigned char), sizeof(unsigned char));
  
  for(int pl = 0; pl < blackpixels.size(); pl++) {
    for(int y1 = 0; y1 < height; y1++) {
      for(int x1 = 0; x1 < width+maxdeltax; x1++) {
        for(int b_right = 0; b_right < bdepth; b_right++) {
          unsigned char* pairPtr = &pairsH[8*(b_right + x1*bdepth + y1*(width+maxdeltax) + pl*((width+maxdeltax)*height*bdepth))];
          pairPtr[0] = (blackpixels[pl].x >> 8) & 0xff;
          pairPtr[1] = blackpixels[pl].x & 0xff;
          pairPtr[2] = (x1 >> 8) & 0xff;
          pairPtr[3] = x1 & 0xff;
          pairPtr[4] = blackpixels[pl].y & 0xff;
          pairPtr[5] = y1 & 0xff;
          pairPtr[6] = (brs[pl] + b_right) & 0xff;
          pairPtr[7] = b_right & 0xff;
        }
      }
    }
  }
}

void Letter::createLetterPixels(unsigned char* letterPixels) {
    
  for(int pl = 0; pl < blackpixels.size(); pl++) {
          unsigned char* pairPtr = &(letterPixels[4*pl]);
          pairPtr[0] = (blackpixels[pl].x >> 8) & 0xff;
          pairPtr[1] = blackpixels[pl].x & 0xff;
          pairPtr[2] = blackpixels[pl].y & 0xff;
          pairPtr[3] = brs[pl];
  }
}


int Letter::getPixel(int y, int x) {
  y = this->height - y;
  return (pixels[y*width + x] ? 1 : 0);
}

int Letter::getPixelFromBaseline(int y, int x) {
  y = this->y_top - y;
  return (pixels[y*width + x] ? 1 : 0);
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

void Letter::storeField(int* field, int field_width, int field_height, int field_depth) {
  this->field = field;
  this->field_width = field_width;
  this->field_height = field_height;
  this->field_depth = field_depth;
}
