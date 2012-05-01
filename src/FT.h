/* FreeType engine -- implements an interface to the FreeType library
 * which renders the letters as needed */

#ifndef FTENGINE_H
#define FTENGINE_H

#include <iostream>
#include <string>
#include <ft2build.h>
#include <freetype/ftbitmap.h>
#include FT_FREETYPE_H
#include <vector>
#include "core/core.hpp"
#include <pango/pangoft2.h>
#include <fontconfig/fontconfig.h>
#include "letter.h"

using namespace std;

class FTengine {

 public:
  FTengine();
  ~FTengine();

  int preparePango(string fontDescription, int ppem);
  Letter* getLetter(char letterChar);
  int getSpacing(char letterChar1, char letterChar2);

 private:
  FT_Library library;
  FT_Face face;
  int error;
  int ppem;

  PangoLayout *layout;
  PangoFontMap* PFM;
  PangoFont* pfont;
  PangoContext* context;
  PangoFontDescription* desc;

};


#endif
