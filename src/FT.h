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

  int prepareFT(string fontPath, int ppem);
  int preparePango(string fontPath, int ppem);
  Letter* getLetter(char letterChar);
  int getKerning(char letterChar1, char letterChar2);

 private:
  FT_Library library;
  FT_Face face;
  int error;
  
};


#endif
