
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

  return error;
}

Letter* FTengine::getLetter(char letterChar) {
  //  Letter* nL = new Letter();

  // load the letter into FreeType

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

  for(int y = 0; y < face->glyph->bitmap.rows; y++) {
    uchar* rowPtr = byteArray->ptr(y);
    for(int x = 0; x < face->glyph->bitmap.width; x++) {
      rowPtr[x] = (byteMap.buffer[y*face->glyph->bitmap.width + x]==0?0:255);
    }
  }

  FT_Bitmap_Done(library, &byteMap);
  
  // create a window
  cv::namedWindow("Display", CV_WINDOW_AUTOSIZE); 
  cv::imshow("Display", *byteArray);
  cv::waitKey(0);

  // return nL;
}
