
#ifndef LETTER_H
#define LETTER_H

#include "core/core.hpp"
#include "highgui/highgui.hpp"
#include "imgproc/imgproc.hpp"
#include "objdetect/objdetect.hpp"
#include "boost/gil/gil_all.hpp"
#include <ft2build.h>
#include <freetype/ftbitmap.h>
#include FT_FREETYPE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

typedef struct {
  int x;
  int y; // position upwards from baseline
} point;

class Letter {

 public:
  Letter();
  Letter(char letterChar, FT_Bitmap* rawArray, int height, int y_baseline, int width, int xheight, int capheight);
  ~Letter();
  char getLetterChar() {return this->letterChar;}
  int getHeight() {return this->height;}
  int getYtop() {return this->y_top;}
  int getYbottom() {return this->y_bottom;}
  int getWidth() {return this->width;}
 
  void setBubble(int y, int extent, bool leftside);

  void showBubbledLetter();

  void printLetterInfo();
  void analyze();

  void f_rlEdges();
  void f_houghstems();
  void f_moments();
  
  unsigned char* pixels;
  std::vector<point> blackpixels;
  std::vector<int> rEdgeOffset;
  std::vector<int> lEdgeOffset;
  std::vector<int> lBubbleExtent;
  std::vector<int> rBubbleExtent;

  int lStemOffset = 0; // number of pixels from edge to stem center
  int rStemOffset = 0;
  float lStemIntensity = 0; // percentage of validty of the max
  float rStemIntensity = 0; 

  int height;       // from bottom to top of glyph
  int xheight;

 private:
  char letterChar;

  boost::gil::gray8c_view_t* gMat;
  cv::Mat cMat;

  // dimensions:
 
  int y_top;        // y-coordinate of top of glyph, from baseline
  int y_bottom;     // y-coordinate of bottom of glyph, from baseline
  int width;        // between left to right extreme vertices
  int capheight;
  int houghstem_miny = 0; // everything below is ignored in houghstem
  int houghstem_maxy = 0; 
};

#endif
