
#include "FT.h"
using namespace std;

FTengine::FTengine() {

}

int FTengine::prepareFT(string fontPath, int ppem) {
  error = FT_Init_FreeType(&library);
  if (error) {
    cout << "An error occured during FreeType2 library initialization.\n";
    exit(1);
    }

  error = FT_New_Face( library, fontPath.c_str(), 0, &face );
  if(error == FT_Err_Unknown_File_Format) {
    cout << "FT Couldn't read file format.\n";
    exit(1);
  } else if (error) {
    cout << "FT Couldn't read file.\n";
    exit(1);
  }
  error = FT_Set_Pixel_Sizes(face, 0, ppem);
  preparePango(fontPath, ppem);
  return error;
}

int FTengine::preparePango(string fontPath, int ppem) {
  PangoLayout *layout;
  PangoFontMap* PFM;
  PangoContext* context;
  PangoFontDescription* desc;

  PFM = pango_ft2_font_map_new();
  context = pango_font_map_create_context(PANGO_FONT_MAP(PFM));
  layout = pango_layout_new(context);
  
  desc = pango_font_description_from_string("Sans, 20");
  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);

  pango_layout_set_text(layout, "VATO!", -1);
  ppem = 100;
  pango_ft2_font_map_set_resolution(PANGO_FT2_FONT_MAP(PFM), ppem, ppem);

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

  int i=0;
  for(int y=0;y<bm->rows;y++)    
    {      
      for(int x=0;x<bm->width;x++)	
	{	  
	  if
	    (bm->buffer[i++]==0) cout << " ";
	  else
	    cout << "*";
	}
      cout << y << endl;
    }
 
  // http://www.mail-archive.com/gtk-i18n-list@gnome.org/msg01645.html
 
}


Letter* FTengine::getLetter(char letterChar) {

  FT_UInt glyph_index = FT_Get_Char_Index(face, (FT_ULong) letterChar);
  error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
  if(error) {
    cout << "Could not load glyph (Glyph Code: " << (FT_ULong)letterChar << ")\n";
  }

  error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);

  FT_Bitmap byteMap;
  FT_Bitmap_New(&byteMap);
  FT_Bitmap_Convert(library, &face->glyph->bitmap, &byteMap, 1);

  // get the raw array
  cv::Mat* byteArray = new cv::Mat(face->glyph->bitmap.rows, face->glyph->bitmap.width, CV_8UC1);

  for(int y = 0; y < byteMap.rows; y++) {
    uchar* rowPtr = byteArray->ptr(y);
    for(int x = 0; x < byteMap.width; x++) {
      rowPtr[x] = (byteMap.buffer[y*byteMap.width + x]==0?0:255);
    }
  }


  
  float scalefactor = (face->glyph->metrics.width)/(1.0*byteMap.width);

  Letter* nL = new Letter(letterChar,
			  byteArray,
			  byteMap.rows,
			  (int)face->glyph->metrics.horiBearingY/scalefactor,
			  (int)face->glyph->metrics.horiBearingY/scalefactor-byteMap.rows,byteMap.width,
			  (int)face->glyph->metrics.horiBearingX/scalefactor,
			  (int)((face->glyph->metrics.horiAdvance - face->glyph->metrics.horiBearingX - (face->glyph->metrics.width))/scalefactor));

  FT_Bitmap_Done(library, &byteMap);

  return nL;
}

int FTengine::getKerning(char letterChar1, char letterChar2) {
  FT_Vector delta;
  error = FT_Get_Kerning(face, (FT_ULong) letterChar1, (FT_ULong) letterChar2, FT_KERNING_UNSCALED, &delta);
  if(error) {
    cout << "Error reading kerning data: " << error << endl;
  }
  cout << FT_HAS_KERNING(face);
  cout << "Getting kerning for " << (FT_ULong) letterChar1 << "." << (FT_ULong) letterChar2 << " face:" << face->family_name << " " << delta.x << endl;
  return delta.x;
}
