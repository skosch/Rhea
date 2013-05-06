#include "letter.h"

#define PI 3.14159276

using namespace cv;
using namespace std;


Letter::Letter(char letterChar, FT_Bitmap* rawArray, int height, int y_baseline, int width, int xheight, int capheight) {

  // set variables
  this->letterChar = letterChar;
  
  
  // this puts the full ppem^2 square into cMat
  cMat = cv::Mat(rawArray->rows, rawArray->width, CV_8UC1, rawArray->buffer, rawArray->pitch);
  
  // crop cMat to the relevant size
  cMat = cMat(Rect(0, 0, width, height));
  
  // create the GIL matrix (gMat) if needed here ...
  
  

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

  this->pixelpitch = rawArray->pitch;
  this->height = height;
  this->y_top = y_baseline;
  this->y_bottom = height - y_baseline;
  this->width = width;
  this->xheight = xheight;
  this->capheight = capheight;
  this->houghstem_maxy = (int) xheight*0.8;
  this->houghstem_miny = (int) xheight*0.15;

  rBubbleExtent.resize(xheight);
  std::fill(rBubbleExtent.begin(), rBubbleExtent.end(), 0);
  lBubbleExtent.resize(xheight);
  std::fill(lBubbleExtent.begin(), lBubbleExtent.end(), 0);

  rEdgeOffset.resize(xheight);
  std::fill(rEdgeOffset.begin(), rEdgeOffset.end(), width);
  lEdgeOffset.resize(xheight);
  std::fill(lEdgeOffset.begin(), lEdgeOffset.end(), width);
  f_rlEdges();

  fill_b_on_angle();
  //printLetterInfo();
  //f_houghstems();
  //analyze();
}

// calculate blackpixel-at-angle matrix
void Letter::fill_b_on_angle() {
  
  b_on_angle.resize(blackpixels.size());
  int x0, x1, y0, y1,b, dx, dy;
  int error, e2, sx, sy;
  
  for(int i=0; i<b_on_angle.size(); i++) {
    b_on_angle[i].resize(360);

    // calculate number of black pixels in that direction.
    for(int a=0; a<360; a++) {
      b = 0;
      x0 = blackpixels[i].x;
      y0 = blackpixels[i].y;

      float aNE = atan((height-y0)*1.0/(width-x0))*180.0/PI;
      float aNW = 180 - atan((height-y0)*1.0/x0)*180.0/PI;
      float aSW = 180 + atan((y0*1.0)/x0)*180.0/PI;
      float aSE = 360 - atan(y0*1.0/(width-x0))*180.0/PI;

      if(a > aSE or a <= aNE) {
        x1 = width - 1;
        y1 = (int) (y0 + tan(a*PI/180.0)*(width-x0));
      } else if(a <= aNW) {
        x1 = (int) (x0 + tan(M_PI_2 - a*PI/180.0)*(height-y0));
        y1 = height - 1;
      } else if(a <= aSW) {
        x1 = 0;
        y1 = (int) (y0 - tan(a*PI/180.0)*(x0));
      } else if(a <= aSE) {
        x1 = (int) (x0 - tan(M_PI_2 - a*PI/180.0)*(y0));
        y1 = 0;
      }

      dx = x1 - x0;
      dy = y1 - y0;
      b = 0;

      // compute b using Bresenham's algorithm
      sx = (x0 < x1) ? 1 : -1;
      sy = (y0 < y1) ? 1 : -1;
      error = abs(dx)-abs(dy);
      do {
        b += getPixelFromBaseline(x0, y0);
        e2 = 2*error; // move on
        if (e2 > -dy) {
          error -= dy;
          x0 += sx;
        }
        if (e2 < dx) { 
          error += dx;
          y0 += sy;
        }
      } while((x0<(x1*sx)) and (y0<(y1*sy)));
      b_on_angle[i][a] = b;
    } // end for through all angles
  } // end for through all blackpixels
}

int Letter::getPixel(int y, int x) {
  y = this->height - y;
  return (pixels[y*pixelpitch + x] > 0 ? 1 : 0);
}

int Letter::getPixelFromBaseline(int y, int x) {
  y = this->y_top - y;
  return (pixels[y*pixelpitch + x] > 0 ? 1 : 0);
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

void Letter::f_rlEdges() {
  // this goes from 0 to xheight
  for(int i = 0; i < blackpixels.size(); i++) {
    if(blackpixels[i].y < 0 or blackpixels[i].y >= xheight) continue;

    if(blackpixels[i].x < lEdgeOffset[blackpixels[i].y]) {
      lEdgeOffset[blackpixels[i].y] = blackpixels[i].x;
    }
    if((width-blackpixels[i].x) < rEdgeOffset[blackpixels[i].y]) {
      rEdgeOffset[blackpixels[i].y] = width-blackpixels[i].x;
    }
  }
}

void Letter::f_houghstems() {
  vector<int> stemIntensity;
  stemIntensity.resize(width);
  std::fill(stemIntensity.begin(), stemIntensity.end(), 0);

  // iterate through letter columns and aggregate down

  for(int i = 0; i < blackpixels.size(); i++) {
    if(blackpixels[i].y > houghstem_miny && blackpixels[i].y < houghstem_maxy) {
      stemIntensity[blackpixels[i].x]++;
    }
  }

  // smooth stem intensity
    for(int i = 3; i < stemIntensity.size()-3; i++) {
      stemIntensity[i] = (int) ((float)(stemIntensity[i-3]+stemIntensity[i-2]+stemIntensity[i-1]+stemIntensity[i]+stemIntensity[i+1]+stemIntensity[i+2]+stemIntensity[i+3]))/7;
    }

  // find max stems from left and right edge in left and right halves
  int maxstemheight = houghstem_maxy - houghstem_miny;
  for(int i = 0; i < min(2*width/3.0, xheight/2.0); i++) {
    if( ((float) stemIntensity[i])/maxstemheight > lStemIntensity) {
      lStemIntensity = ((float) stemIntensity[i]/maxstemheight);
      lStemOffset = i;
    }
  }

  for(int i = width - 1; i >  width-min(2*width/3.0, xheight/2.0); i--) {
    if( ((float) stemIntensity[i])/maxstemheight > rStemIntensity) {
      rStemIntensity = ((float) stemIntensity[i]/maxstemheight);
      rStemOffset = width - i;
    }
  }

  // find extents of surrounding plateaus (90% of max)
  int lStemlExtent = 0;
  int lStemrExtent = 0;
  for( lStemlExtent = 0; stemIntensity[lStemOffset-lStemlExtent]>0.9*maxstemheight*lStemIntensity; lStemlExtent++) {}
  for( lStemrExtent = 0; stemIntensity[lStemOffset+lStemrExtent]>0.9*maxstemheight*lStemIntensity; lStemrExtent++) {}
  lStemOffset += (lStemrExtent-lStemlExtent)/2.0;
  lStemIntensity = ((float) stemIntensity[lStemOffset]/maxstemheight);

  int rStemlExtent = 0;
  int rStemrExtent = 0;
  for( rStemlExtent = 0; stemIntensity[width-rStemOffset-rStemlExtent]>0.9*maxstemheight*rStemIntensity; rStemlExtent++) {}
  for( rStemrExtent = 0; stemIntensity[width-rStemOffset+rStemrExtent]>0.9*maxstemheight*rStemIntensity; rStemrExtent++) {}
  rStemOffset -= (rStemrExtent-rStemlExtent)/2.0;
  rStemIntensity = ((float) stemIntensity[width-rStemOffset]/maxstemheight);
  /*  
  cout << "Letter " << letterChar << " ***********" << endl;
  cout << "LStemOffset: " <<  lStemOffset << endl;
  cout << "LStemIntensity: " << lStemIntensity << endl;
  cout << "RStemOffset: " << rStemOffset << endl;
  cout << "RStemIntensity: " << rStemIntensity << endl;
  cout << "RStemRExtent: " << rStemrExtent << endl;
  cout << "RStemLExtent: " << rStemlExtent << endl;


  // save to file
  ofstream outdata;
  outdata.open("letter_houghs.csv", ios_base::app);
  if(!outdata) {
    cerr << "Error: Could not create output file.\n";
    exit(1);
  }
  outdata << letterChar;
  for(int i = 0; i < stemIntensity.size(); i++) {
    outdata << stemIntensity[i] << " ";
  }
  outdata << "\n";  

  // display the result
  cv::Mat hMat(cMat); // copy original letter matrix
  hMat.convertTo(hMat, CV_8UC3); // convert to RGB

  Point3_<uchar>* p = hMat.ptr<Point3_<uchar>>(height-1,0);
  for(int i = 0; i < width/2; i++) {
    if(i != lStemOffset) {
      p = hMat.ptr<Point3_<uchar>>((int) (height - stemIntensity[i])-1,i);
    p->x = 255;}
  }
  for(int i = width -1; i > width/2; i--) {
    if(i != width - rStemOffset) {
      p = hMat.ptr<Point3_<uchar>>((int) (height - stemIntensity[i])-1,i);
    p->x = 255;}
  }
  
  cv::namedWindow("Display", CV_WINDOW_AUTOSIZE); 
  cv::imshow("Display", hMat);
  cv::waitKey(0);
    */
  
}

void Letter::setBubble(int y, int extent, bool leftside) {
  if(leftside) {
    lBubbleExtent[y] = extent;
  } else {
    rBubbleExtent[y] = extent;
  }
}

void Letter::showBubbledLetter() {

  // calculate max bubble extent on either side, add that to cMat
  int maxlbubble = *max_element(lBubbleExtent.begin(), lBubbleExtent.end());
  int maxrbubble = *max_element(rBubbleExtent.begin(), rBubbleExtent.end());
  
  cv::Mat hMat = cv::Mat::zeros(xheight,width+maxlbubble+maxrbubble, CV_8UC3);

  cv::Point origin(maxlbubble,0);
  cMat = cv::Mat(cMat, cv::Rect(0,max(0,y_top-xheight),width,xheight));
  
  cv::Mat roi = cv::Mat(hMat, cv::Rect(origin, cMat.size()));
  cvtColor(cMat, cMat, CV_GRAY2RGB);

  cMat.convertTo(roi, CV_8UC3, 3);
  cMat.copyTo(roi);
  cvtColor(cMat, cMat, CV_RGB2HSV);  
  uchar* px = NULL;
  for(int y = 0; y < xheight; y++) {
    for(int x = max(0,(maxlbubble+lEdgeOffset[xheight-y]-lBubbleExtent[xheight-y]));	x < min(width+maxlbubble+maxrbubble,(maxlbubble+width-rEdgeOffset[xheight-y]+rBubbleExtent[xheight-y])); x++) {
      px = hMat.ptr<uchar>(y,x);      
      if(px[0]+px[1]+px[2] == 0) {
	px[0] = (int)(((int)letterChar-97)*9.5);
	px[1] = 180;
	px[2] = 230;
      }
    }
  }
  
  //  cv::namedWindow("Display", CV_WINDOW_AUTOSIZE); 
  //  cv::imshow("Display", hMat);
  std::stringstream filename_builder;
  filename_builder << "bubble_" << letterChar << ".png";
  cout << letterChar << ": " << width << endl;
  cv::imwrite(filename_builder.str(), hMat);
  //  cv::waitKey(0);
  
}


void Letter::findLetterFeatures(std::vector<std::vector<int>> *matrix, int combno) {

  // find mathematical center
  int math_center = width/2;

  // find second moment center iteratively
  int second_center = 0;
  int second_moment = INT_MAX;
  int second_moment_temp = 0;
  for(int x0 = 0; x0 < width; x0++) {
    // calculate total moment
    for(int i = 0; i < blackpixels.size(); i++) {
      second_moment_temp += pow(x0-blackpixels[i].x,2);
    }
    if(second_moment_temp < second_moment){
      second_moment = second_moment_temp;
      second_moment_temp = 0;
      second_center = x0;
    } else if (second_moment_temp > second_moment) {
      break;
    }
  }
  // find third moment center iteratively
  int third_center = 0;
  unsigned long long third_moment =  18446744073709551615;
  unsigned long long third_moment_temp = 0;
  for(int x0 = 0; x0 < width; x0++) {
    // calculate total moment
    for(int i = 0; i < blackpixels.size(); i++) {
      third_moment_temp += abs(pow(x0-blackpixels[i].x,3));
    }
    if(third_moment_temp < third_moment){
      third_moment = third_moment_temp;
      third_moment_temp = 0;
      third_center = x0;
    } else if (third_moment_temp > third_moment) {
      //      break;
    }
  }

  matrix->at(combno)[0] = width;
  matrix->at(combno)[1] = second_center;
  matrix->at(combno)[2] = third_center;
  matrix->at(combno)[3] = math_center;

  // find in-between-letter area
  
  // find 

}

