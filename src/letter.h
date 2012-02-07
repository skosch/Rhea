
#ifndef LETTER_H
#define LETTER_H

#include "core/core.hpp"
#include "highgui/highgui.hpp"
#include "imgproc/imgproc.hpp"
#include "objdetect/objdetect.hpp"

#include <iostream>
#include <string>
#include <vector>

class Letter {

 public:
  Letter(char letterChar, cv::Mat* rawArray, int height, int y_top,
	 int y_bottom, int width, int l_sb, int r_sb);
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

 private:
  char letterChar;
  cv::Mat* lMat;

  // dimensions:
  int height;       // from bottom to top of glyph
  int y_top;        // y-coordinate of top of glyph, from baseline
  int y_bottom;     // y-coordinate of bottom of glyph, from baseline
  int width;        // between left to right extreme vertices
  int l_sb;         // left sidebearing
  int r_sb;         // right sidebearing

};

#endif
