#include "letter.h"

using namespace cv;
using namespace std;

Letter::Letter(char letterChar, FT_Bitmap* rawArray, int height, int y_baseline, int width) {

  // set variables
  this->letterChar = letterChar;
  
  /*
  // this puts the full ppem^2 square into cMat
  cMat = cv::Mat(rawArray->rows, rawArray->width, CV_8UC1, rawArray->buffer, rawArray->pitch);
  
  // crop cMat to the relevant size
  cMat = cMat(Rect(0, 0, width, height));
  
  // create the GIL matrix (gMat) if needed here ...
  
  */

  pixels = rawArray->buffer; // this points to a bitmap still ppem^2 big

  for(int row = 0; row < height; row++) {
    for(int x = 0; x < width; x++) {
      if(pixels[row*rawArray->pitch+x] > 0) {
	point blp;
	blp.x = x;
	blp.y = y_baseline-row;
	blackpixels.push_back(blp);
      }
    }
  }

  this->height = height;
  this->y_top = y_baseline;
  this->y_bottom = height - y_baseline;
  this->width = width;

  //printLetterInfo();
  //analyze();
}

void Letter::analyze() {
  //f_houghTransform();
  //f_moments();
}

void Letter::printLetterInfo() {
  cout << "Info for Letter " << letterChar << ":" << endl;
  cout << "Height:\t" << height << endl;
  cout << "Ytop:\t" << y_top << endl;
  cout << "Ybottom:" << y_bottom << endl;
  cout << "Width:\t" << width << endl;

  // create a window
  cv::namedWindow("Display", CV_WINDOW_AUTOSIZE); 
  cv::imshow("Display", cMat);
  cv::waitKey(0);
}


void Letter::f_moments() {
  Moments moms;
  moms = moments( cMat, true );
  
}



void Letter::f_houghTransform() {
  vector<cv::Vec4i> lines;
 cv::Mat dst, color_dst;
 
 cv::Canny(cMat, dst, 50, 200, 3);
 cv::cvtColor(dst, color_dst, CV_GRAY2BGR);
 cv::HoughLinesP( dst, lines, 1, CV_PI/180, 80, 30, 10 );
 for( size_t i = 0; i < lines.size(); i++ )
   {
     cv::line( color_dst, cv::Point(lines[i][0], lines[i][1]),
	       cv::Point(lines[i][2], lines[i][3]), cv::Scalar(0,0,255), 3, 8 );
   }
 /* cv::namedWindow( "Source", 1 );
 cv::imshow( "Source", *lMat );

 cv::namedWindow( "Detected Lines", 1 );
 cv::imshow( "Detected Lines", color_dst );

 cv::waitKey(0);*/
}


