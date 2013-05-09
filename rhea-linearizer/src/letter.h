
#ifndef LETTER_H
#define LETTER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>

using namespace std;
typedef struct {
  int x;
  int y; // position upwards from baseline
} point;

class Letter {

 public:
  Letter();
  Letter(char letterChar, vector<bool> rawArray, int height, int y_baseline, int width, int xheight, int capheight);
  ~Letter();
  char getLetterChar() {return this->letterChar;}
  int getHeight() {return this->height;}
  int getYtop() {return this->y_top;}
  int getYbottom() {return this->y_bottom;}
  int getWidth() {return this->width;}
 
  void printLetterInfo();

  void f_rlEdges();
  
  int getPixel(int y, int x);
  int getPixelFromBaseline(int y, int x);

  vector<bool> pixels;
  std::vector<point> blackpixels;
  std::vector<int> rEdgeOffset;
  std::vector<int> lEdgeOffset;
  
  std::vector<std::vector<int>> b_on_angle; // blackness counts from pixels
  void fill_b_on_angle();
  
  int lStemOffset = 0; // number of pixels from edge to stem center
  int rStemOffset = 0;

  int height;       // from bottom to top of glyph
  int xheight;
  int pixelpitch;

 private:
  char letterChar;

  // dimensions:
 
  int y_top;        // y-coordinate of top of glyph, from baseline
  int y_bottom;     // y-coordinate of bottom of glyph, from baseline
  int width;        // between left to right extreme vertices
  int capheight;
  int houghstem_miny = 0; // everything below is ignored in houghstem
  int houghstem_maxy = 0; 
};

#endif
