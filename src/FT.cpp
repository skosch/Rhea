
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
}


Letter* FTengine::getLetter(char letterChar) {
  pango_layout_set_text(layout, &letterChar, -1);
  pango_layout_context_changed(layout);

  FT_Bitmap *bm;

  bm = g_slice_new(FT_Bitmap);
  bm->rows = ppem;
  bm->width = ppem;
  bm->pitch = ppem;
  bm->num_grays = 256;
  bm->pixel_mode = FT_PIXEL_MODE_GRAY;

  bm->buffer = (unsigned char*) g_malloc(bm->pitch * bm->rows);
  memset(bm->buffer, 0x00, bm->pitch * bm->rows);

  pango_ft2_render_layout(bm, layout, 0, 0);
}


int FTengine::getSpacing(char letterChar1, char letterChar2) {

  // first, layout both characters to get the kerned length
  char text[3] = {letterChar1, letterChar2, '\0'};
  pango_layout_set_text(layout, text, -1);
  pango_layout_context_changed(layout);

  PangoRectangle inkrect;
  pango_layout_get_pixel_extents(layout, &inkrect, NULL);
  int totalwidth = inkrect.width;
  
  PangoItem* item = (PangoItem*) pango_itemize(context, text, 0, strlen(text), pango_layout_get_attributes(layout), NULL)->data;
  PangoGlyphString* glyphstr = pango_glyph_string_new();
  pango_shape(text+item->offset, item->length, &item->analysis, glyphstr);

  PangoRectangle l_inkrect;
  PangoRectangle r_inkrect;
  pango_font_get_glyph_extents(pfont, glyphstr->glyphs[0].glyph, &l_inkrect, NULL);
  pango_font_get_glyph_extents(pfont, glyphstr->glyphs[1].glyph, &r_inkrect, NULL);
  int leftwidth = ((float) l_inkrect.width)/PANGO_SCALE;
  int rightwidth = ((float) r_inkrect.width)/PANGO_SCALE;
  pango_item_free(item);
  pango_glyph_string_free(glyphstr);

  return totalwidth - leftwidth - rightwidth;
}

