
#include "FT.h"
using namespace std;

FTengine::FTengine() {

}


int FTengine::preparePango(string fontDescription = "Arial 20", int ppem = 100) {
  this->ppem = ppem;
  PFM = pango_ft2_font_map_new();
  context = pango_font_map_create_context(PANGO_FONT_MAP(PFM));
  layout = pango_layout_new(context);
  
  desc = pango_font_description_from_string(fontDescription.c_str());
  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);

  pango_ft2_font_map_set_resolution(PANGO_FT2_FONT_MAP(PFM), ppem, ppem);
  pfont = pango_font_map_load_font(PFM, context, desc);

  cout << "PANGO_SCALE: " << PANGO_SCALE << endl;
  return 0;
}


Letter* FTengine::getLetter(char letterChar) {
  char text[2] = {letterChar, '\0'};
  pango_layout_set_text(layout, text, -1);
  pango_layout_context_changed(layout);

  FT_Bitmap *bm;
  PangoRectangle inkrect;

  bm = g_slice_new(FT_Bitmap);
  bm->rows = ppem;
  bm->width = ppem;
  bm->pitch = ppem;
  bm->num_grays = 256;
  bm->pixel_mode = FT_PIXEL_MODE_GRAY;

  bm->buffer = (unsigned char*) g_malloc(bm->pitch * bm->rows);
  memset(bm->buffer, 0x00, bm->pitch * bm->rows);
  pango_layout_get_pixel_extents(layout, &inkrect, NULL);
  
  int y_baseline = (int)((float) pango_layout_get_baseline(layout))/PANGO_SCALE - inkrect.y;
  pango_ft2_render_layout(bm, layout, -inkrect.x, -inkrect.y);

  for(int i = 0; i < bm->rows; i++) {
    for(int j = 0; j < bm->pitch; j++) {
      bm->buffer[i*bm->pitch + j] &= 128; 
      bm->buffer[i*bm->pitch + j] *= 255; // this line can be removed for performance
    }
  }
  
  // create a letter object here, then return it
  Letter *result = new Letter(letterChar, bm, inkrect.height, y_baseline, inkrect.width);


  return result;
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


