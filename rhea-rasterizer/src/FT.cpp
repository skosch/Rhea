
#include "FT.h"
using namespace std;

void FTengine::preparePango(string fontDescription = "Arial 20", int ppem = 100) {
  this->ppem = ppem;
  PFM = pango_ft2_font_map_new();
  context = pango_font_map_create_context(PANGO_FONT_MAP(PFM));
  layout = pango_layout_new(context);
  
  desc = pango_font_description_from_string(fontDescription.c_str());
  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);

  pango_ft2_font_map_set_resolution(PANGO_FT2_FONT_MAP(PFM), ppem, ppem);
  pfont = pango_font_map_load_font(PFM, context, desc);

  // get xheight, cause letters need this stuff all the time
  PangoRectangle inkrect;
  pango_layout_set_text(layout, "x", -1);
  pango_layout_context_changed(layout);
  pango_layout_get_pixel_extents(layout, &inkrect, NULL);
  xheight = (inkrect.height);
  // get capheight, cause letters need this stuff all the time
  pango_layout_set_text(layout, "Y", -1);
  pango_layout_context_changed(layout);
  pango_layout_get_pixel_extents(layout, &inkrect, NULL);
  capheight = (inkrect.height);

  cout << "Loaded font: " << fontDescription << endl;
  cout << "x-height (x): " << xheight << endl;
  cout << "Cap height (Y): " << capheight << endl;
}


string FTengine::getLetter(char letterChar) {
  char text[2] = {letterChar, '\0'};
  pango_layout_set_text(layout, text, -1);
  pango_layout_context_changed(layout);

  FT_Bitmap *bm;
  PangoRectangle inkrect;

  bm = g_slice_new(FT_Bitmap);
  bm->rows = 2*ppem;
  bm->width = ppem;
  bm->pitch = ppem;


  bm->num_grays = 256;
  bm->pixel_mode = FT_PIXEL_MODE_GRAY;

  bm->buffer = (unsigned char*) g_malloc(bm->pitch * bm->rows);
  memset(bm->buffer, 0x00, bm->pitch * bm->rows);
  pango_layout_get_pixel_extents(layout, &inkrect, NULL);
  
  int y_baseline = (int)((float) pango_layout_get_baseline(layout))/PANGO_SCALE - inkrect.y;
  pango_ft2_render_layout(bm, layout, -inkrect.x, -inkrect.y);

  stringstream letterContent;

  for(int i = 0; i < inkrect.height; i++) {
    for(int j = 0; j < inkrect.width; j++) {
      letterContent << ( (bm->buffer[i*bm->pitch+j] & 128)>0 ? "1" : "0" );
    }
  }

  stringstream letterLine;
  letterLine << letterChar << "," << inkrect.height << "," << y_baseline << ",";
  letterLine << inkrect.width << ",";
  letterLine << letterContent.str() << endl;
  return letterLine.str();
}


/************************************** 
 * getSpacing() gets the width of each glyph, and the 
 * width of both glyphs set together (incl. kerning) to
 * get the total horizontal spacing between the two glyphs.
 **************************************/
int FTengine::getSpacing(char letterChar1, char letterChar2) {
  PangoRectangle l_inkrect;
  PangoRectangle r_inkrect;
  PangoRectangle inkrect;

  char text[3] = {letterChar1, letterChar2, '\0'};
  pango_layout_set_text(layout, text, -1);
  pango_layout_context_changed(layout);

  pango_layout_get_pixel_extents(layout, &inkrect, NULL);
  int totalwidth = inkrect.width;

  pango_layout_set_text(layout, text, 1);
  pango_layout_context_changed(layout);

  pango_layout_get_pixel_extents(layout, &l_inkrect, NULL);
  int leftwidth = l_inkrect.width;

  pango_layout_set_text(layout, text+1, 1);
  pango_layout_context_changed(layout);

  pango_layout_get_pixel_extents(layout, &r_inkrect, NULL);
  int rightwidth = r_inkrect.width;

  return totalwidth - leftwidth - rightwidth;
}


