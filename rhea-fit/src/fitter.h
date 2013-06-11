#ifndef FITTER_H
#define FITTER_H

#include "letter.h"

#include <tuple>
#include <map>
#include <vector>

class Fitter {
  public:
    Fitter(std::tuple< std::map< char, Letter*>, int, int, int, int > rasterizedLetters, std::vector< float > coeffs, int degree);

    void fitLetters() {
      /*
        // create fresh pair objects from the letters
    for(map<char, Letter*>::const_iterator lL = letterDict.begin();
        lL != letterDict.end();
        lL++) {
      for(map<char, Letter*>::const_iterator rL = letterDict.begin();
          rL != letterDict.end();
          rL++) {
        pairs.push_back(new Pair(lL->second, rL->second, 0));
      }
    }




    // first, calculate the fixed field itself.
    int ffheight = 2 * capheight - 1; // height of fixed field
    int ffycenter = capheight - 1;    // index of pixel
    int ffwidth = 2 * maxwidth - 1 + maxdeltax;
    int ffxcenter = maxwidth - 1;

    float* fixedField = (float*)calloc(sizeof(float) * ffheight * ffwidth * bdepth, sizeof(float));
    getFixedField(ffheight, ffwidth, ffycenter, ffxcenter, bdepth,
            degree, xheight, coeffs, fixedField);


    // now loop through the letters
    Letter* L;
    int field_width, field_size;
    for(map<char, Letter*>::const_iterator lL = letterDict.begin();
        lL != letterDict.end();
        lL++) {

      L = lL->second; 
      cout << "\rNow calculating the field: " << L->getLetterChar() << endl;


      // get the (L + neighbour)'s maxb
      int rLmaxb = 0;
      for(map<char, Letter*>::const_iterator rL = letterDict.begin();
         rL != letterDict.end();
         rL++) {
        if(rL->second->maxb > rLmaxb)
          rLmaxb = rL->second->maxb;
      }
      // get the Letter's field
      float* letterField;
      field_width = L->width + maxdeltax;
      field_size = field_width * capheight * rLmaxb;
      letterField = (float*)calloc(sizeof(float)*field_size, sizeof(float));

      getLetterField(L, field_width, capheight, rLmaxb, ffwidth, ffycenter, ffxcenter, bdepth, fixedField, letterField);

      for(int bb = 0; bb < rLmaxb; bb++) { 
        / now print the field, b=0, to a pixbuf file. /
        stringstream outfilename;
        outfilename << "./outfield_" << L->getLetterChar() << "_" << bb;
        ofstream outdata;
        outdata.open(outfilename.str());
        if(!outdata) {
          cerr << "Error: Could not create output file.\n";
          exit(1);
        }

        for(int pl = 0; pl < L->numPixels; pl++) {
          letterField[field_width*rLmaxb*L->blackpixels[pl].y +
            rLmaxb*L->blackpixels[pl].x + bb] = -1000;
        }

        for(int fieldy = 0; fieldy < capheight; fieldy++) {
          for(int fieldx = 0; fieldx < field_width; fieldx++) {
            outdata << letterField[field_width*rLmaxb*fieldy + rLmaxb*fieldx + bb] << ",";
          }
          outdata << endl;
        }
        outdata.close();
      }

      // now loop through all letter pairs and adjust them.
      float theta = 1.0;
      int epsilon = 5;

      for(map<char, Letter*>::const_iterator rLi = letterDict.begin();
          rLi != letterDict.end();
          rLi++) {
        Letter* rL = rLi->second; 
        
        // calculate initial force
        float oldforce = 1000000000;
        int bestdistance = 100000; 
        for(int distance = (int)xheight; distance >= -(int)xheight; distance--) {
          float newforce = 0;
            for(int pr = 0; pr < rL->numPixels; pr++) {
            newforce += letterField[field_width*rLmaxb*rL->blackpixels[pr].y
                                   + rLmaxb*(L->width + distance + rL->blackpixels[pr].x)
                                   + rL->bls[pr]];
          }
          if(abs(newforce) < oldforce) {
            oldforce = newforce;
            bestdistance = distance;
          }
        }

        cout << "Distance between " << L->getLetterChar() << " and " << 
          rL->getLetterChar() << ": " << bestdistance << " (at " <<
          oldforce << ")" << endl;
      }
    
      free(letterField);

      cout << "Memory released." << endl << endl;

    } // end of loop through all letters L
    free(fixedField);
    free(coeffs);

         */
    };

  private:
  int xheight;
  int capheight;
  int maxwidth;
  int bdepth;
  int degree;
  std::vector< float > coeffs;
};

#endif
