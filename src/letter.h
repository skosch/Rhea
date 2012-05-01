
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
#include <string>
#include <vector>

typedef struct {
  int x;
  int y; // position upwards from baseline
} point;

class Letter {

 public:
  Letter();
  Letter(char letterChar, FT_Bitmap* rawArray, int height, int y_baseline, int width);
  ~Letter();
  char getLetterChar() {return this->letterChar;}
  int getHeight() {return this->height;}
  int getYtop() {return this->y_top;}
  int getYbottom() {return this->y_bottom;}
  int getWidth() {return this->width;}
  int getLSB() {return this->l_sb;}
  int getRSB() {return this->r_sb;}

  void printLetterInfo();
  void analyze();

  void f_houghTransform();
  void f_moments();
  
  unsigned char* pixels;
  std::vector<point> blackpixels;

 private:
  char letterChar;

  boost::gil::gray8c_view_t* gMat;
  cv::Mat cMat;

  // dimensions:
  int height;       // from bottom to top of glyph
  int y_top;        // y-coordinate of top of glyph, from baseline
  int y_bottom;     // y-coordinate of bottom of glyph, from baseline
  int width;        // between left to right extreme vertices
  int l_sb;         // left sidebearing
  int r_sb;         // right sidebearing

};

#endif
