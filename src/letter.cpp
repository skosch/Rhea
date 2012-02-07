#include "letter.h"

using namespace cv;
using namespace std;

Letter::Letter(char letterChar, cv::Mat* rawArray, int height, int y_top, 
	       int y_bottom, int width, int l_sb, int r_sb) {

  // set variables
  this->letterChar = letterChar;
  lMat = rawArray;

  this->height = height;
  this->y_top = y_top;
  this->y_bottom = y_bottom;
  this->width = width;
  this->l_sb = l_sb;
  this->r_sb = r_sb;

  //  printLetterInfo();
  analyze();

}

void Letter::analyze() {
  //f_houghTransform();
  f_moments();
}

void Letter::printLetterInfo() {
  cout << "Info for Letter " << letterChar << ":" << endl;
  cout << "Height:\t" << height << endl;
  cout << "Ytop:\t" << y_top << endl;
  cout << "Ybottom:" << y_bottom << endl;
  cout << "Width:\t" << width << endl;
  cout << "LSB:\t" << l_sb << endl;
  cout << "RSB:\t" << r_sb << endl;

  // create a window
  cv::namedWindow("Display", CV_WINDOW_AUTOSIZE); 
  cv::imshow("Display", *lMat);
  cv::waitKey(0);
}


void Letter::f_moments() {
  Moments moms;
  moms = moments( *lMat, true );
  
}



void Letter::f_houghTransform() {
  vector<cv::Vec4i> lines;
 cv::Mat dst, color_dst;
 
 cv::Canny(*lMat, dst, 50, 200, 3);
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


