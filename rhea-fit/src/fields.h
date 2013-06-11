#ifndef FIELDS_H
#define FIELDS_H

#include <iostream>

void getFixedField(int ffheight,
    int ffwidth,
    int ffycenter,
    int ffxcenter,
    int bdepth,
    int degree,
    float xheight,
    float* coeffs,
    float* output) {


  const int offs1 = degree + 1;
  const int offs2 = offs1 * offs1;
  const int offs3 = offs2 * offs1;

  cout << "Calculating fixed field ..." << flush;

  // go through the pixels of the fixed field.
  for(int fieldb = 0; fieldb < bdepth; fieldb++) {
    for(int fieldy = 0; fieldy < ffheight; fieldy++) {
      for(int fieldx = 0; fieldx < ffwidth; fieldx++) {
        // calculate field value
        float vdx = (fieldx - ffxcenter)/(float)xheight;
        float vdy = abs(fieldy - ffycenter)/(float)xheight;
        float vyy =  0.5*(fieldy + ffycenter)/(float)xheight;
        float vbl = fieldb/(float)xheight;
        //precompute powers
        float *vdxp, *vdyp, *vyyp, *vblp;
        vdxp = (float*)malloc(sizeof(float)*(degree + 1));
        vdyp = (float*)malloc(sizeof(float)*(degree + 1));
        vyyp = (float*)malloc(sizeof(float)*(degree + 1));
        vblp = (float*)malloc(sizeof(float)*(degree + 1));

        for(int power=0; power < degree+1; power++) {
          vdxp[power] = pow(vdx, power); // can probably be replaced with hardcoded powers
          vdyp[power] = pow(vdy, power);
          vyyp[power] = pow(vyy, power);
          vblp[power] = pow(vbl, power);
        }

        float fresult = 0.0;
        for(int edx = 0; edx < degree+1; edx++) {
          for(int edy = 0; edy < degree+1; edy++) {
            for(int eyy = 0; eyy < degree+1; eyy++) {
              for(int eb = 0; eb < degree+1; eb++) {
                fresult += coeffs[eb + offs1*eyy +
                  offs2*edy + offs3*edx]
                  * vdxp[edx]
                  * vdyp[edy]
                  * vyyp[eyy]
                  * vblp[eb];
              }
            }
          }
        }
        output[ffwidth*bdepth*fieldy + bdepth*fieldx + fieldb] = fresult;
      }
    }
    cout << "\rFixed field " << (int)(100.0*fieldb/(float)bdepth) << "% calculated." << flush;
  }
  cout << "\rFixed field calculated" << endl;
}


void getLetterField(Letter* L,
    int fieldWidth,
    int fieldHeight,
    int rLmaxb,
    int ffwidth,
    int ffycenter,
    int ffxcenter,
    int bdepth,
    float* fixedField,
    float* letterField) {

  // loop through all pixels
  cout << "Now calculating letterField" << flush;
  for(int br = 0; br < rLmaxb; br++) {
    for(int fieldy = 0; fieldy < fieldHeight; fieldy++) {
      for(int fieldx = 0; fieldx < fieldWidth; fieldx++) {
        // calculate field at that point.
        // the total field at that point is the field of all letter pixels added
        // up.
        
        for(int pl=0; pl<L->numPixels; pl++) {
          letterField[fieldWidth*rLmaxb*fieldy + rLmaxb*fieldx + br] +=
            fixedField[ffwidth*bdepth*(fieldy - L->blackpixels[pl].y + ffycenter)
                       + bdepth*(fieldx - L->blackpixels[pl].x + ffxcenter)
                       + (br + L->brs[pl])];
        }
      }
    }
    cout << "\rletterField " << (int)(100.0*br/(float)rLmaxb) << "% calculated." << flush ;
  }
  cout << "\r" << flush;
}


#endif
