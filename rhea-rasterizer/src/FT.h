/* FreeType engine -- implements an interface to the FreeType library
 * which renders the letters as needed */

#ifndef FTENGINE_H
#define FTENGINE_H

#include <iostream>
#include <string>
#include <sstream>
#include <ft2build.h>
#include <freetype/ftbitmap.h>
#include FT_FREETYPE_H
#include <pango/pangoft2.h>
#include <fontconfig/fontconfig.h>

using namespace std;

class FTengine {

 public:
  void preparePango(string fontDescription, int ppem);
  string getLetter(char letterChar);
  int getSpacing(char letterChar1, char letterChar2);

  int xheight;

 private:
  FT_Library library;
  FT_Face face;
  int error;
  int ppem;

  int capheight;

  PangoLayout *layout;
  PangoFontMap* PFM;
  PangoFont* pfont;
  PangoContext* context;
  PangoFontDescription* desc;

};


#endif
