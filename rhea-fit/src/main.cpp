// compile with something like clang++ -I/usr/include/freetype2 -lfreetype main.ccp config.cpp FT.cpp


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <cstdlib> // for rand()
#include <ctime> // for time()
#include <map>
#include <math.h>
#include <tuple>

#include "pngwriter.h"
#include "optionparser.h"
#include "readargs.h"
#include "rhea_config.h"
#include "letter.h"
#include "pair.h"
#include "csvrow.h"
#include "fields.h"
#include "rasterizer.h"
#include "analyzer.h"
#include "fitter.h"

using namespace std;

enum optionIndex{ UNKNOWN, HELP, RASTERIZE, ANALYZE, FIT, 
  LETTERS, XHEIGHT, PPEM, FONT, RASTERIZED, PREFIT, BDEPTH, MAXDELTAX,
  PAIRS, COEFFS };
const option::Descriptor usage[] {
  { UNKNOWN, 0, "", "", option::Arg::None, "USAGE:\n"},
    { HELP, 0, "h", "help", option::Arg::None, "Print usage and exit." },
    { RASTERIZE, 0, "r", "rasterize", option::Arg::None, "rasterize"},
    { ANALYZE, 0, "a", "analyze", option::Arg::None, "analyze"},
    { FIT, 0, "f", "fit", option::Arg::None, "fit"},
    { LETTERS, 0, "", "letters", option::Arg::Optional, "letters for rasterize"},
    { XHEIGHT, 0, "", "xheight", option::Arg::Optional, "xheight"},
    { PPEM, 0, "", "ppem", option::Arg::Optional, "ppem (default: 150)"},
    { FONT, 0, "", "font", option::Arg::Optional, "font description for rasterize"},
    { RASTERIZED, 0, "", "rasterized", option::Arg::Optional, "rasterized csv"},
    { PREFIT, 0, "", "prefit", option::Arg::Optional, "prefitted distances csv"}, 
    { BDEPTH, 0, "", "bdepth", option::Arg::Optional, "number of b steps"},
    { MAXDELTAX, 0, "", "maxdeltax", option::Arg::Optional, "max xdelta from right edge of left letter to find field size"},
    { PAIRS, 0, "", "pairs", option::Arg::Optional, "pairs for analyze/fit"},
    { COEFFS, 0, "", "coeffs", option::Arg::Optional, "coeffs for fit"},
    { 0, 0, 0, 0, 0, 0} // garbage catch-all
};
// --rasterize --letters abcDEF --font "Crimson 30"
// --/nalyze --rasterized Crimson30.csv --pairs abcDEF
// --fit --rasterized Crimson30.csv --pairs abcDEF --coeffs coeffs.csv


void run_pair_force_matrix(vector<vector<float>> *matrix, vector<Pair*> *pairs, const int degree) {
  for(int i=0; i<matrix->size(); i++) {
    matrix->at(i).resize(pow((degree+1),4));
    //the following initialization not needed since we write directly
    //vector<int>(pow((degree+1),4),0).swap(matrix->at(i));
  }

  cout << "0% done." << flush;
  for(int i = 0; i < pairs->size(); i++) {
    pairs->at(i)->fillPairPolynomialForceMatrix(matrix, i, degree);
    //pairs->at(i)->fillPairEdgeForceMatrix(matrix, i, 10,10,10);
    //pairs->at(i)->fillPairForceMatrix(matrix, i);
    cout << "\r" << (int) (100*i/pairs->size()) << "% done (" << i << "/" << pairs->size() << ")" << flush;
  }
  cout << "\nDone. Saving matrix to output file ....\n";
} 


/******************** MAIN.CPP ********************/

std::tuple< map<char, Letter*>, int, int, int, int > getRasterized(string filename, float xheight) {
  map<char, Letter*> letterDict;

  int capheight = 0;
  int maxwidth = 0;

  // read in the letters from RASTERIZED
  ifstream csvfile(filename);
  CSVRow row;
  while(row << csvfile)
  {
    // go through rasterized.csv and read in letter objects
    vector<bool> content;
    for(int i=0; i<row[4].length(); i++) {
      content.push_back(row[4][i] == '1' ? true : false);
    }
    Letter* l = new Letter(row[0][0], content, atoi(row[1].c_str()),
        atoi(row[2].c_str()), atoi(row[3].c_str()), xheight, 0);
    letterDict[row[0][0]] = l;
    if(l->height > capheight) capheight = l->height;
    if(l->width > maxwidth) maxwidth = l->width;
  }
  csvfile.close();

  // find bdepth:
  int bdepth = 0;
  for(map<char, Letter*>::const_iterator lLi = letterDict.begin();
      lLi != letterDict.end();
      lLi++) {
    Letter* lL = lLi->second; 
    for(map<char, Letter*>::const_iterator rLi = letterDict.begin();
        rLi != letterDict.end();
        rLi++) {
      Letter* rL = rLi->second;
      if(lL->maxb + rL->maxb > bdepth)
        bdepth = lL->maxb + rL->maxb;
    }
  }
  return std::make_tuple(letterDict, xheight, capheight, maxwidth, bdepth);
}

/***** getPrefit ****/

std::vector< Pair* > getPrefit(string filename, map< char, Letter* > letterDict) {
    std::vector< Pair* > pairs;
    ifstream csvfile(filename);
    CSVRow row;
    while(row << csvfile)
    {
      // read in pairs from prefit.csv
      // format: xy,15
      pairs.push_back(new Pair(letterDict[(char)row[0][0]],
            letterDict[(char)row[0][1]], atoi(row[1].c_str())));
    }
    csvfile.close();
  return pairs;
}

/**** getCoeffs ****/

std::vector< float > getCoeffs(string filename, int nMonomials) {
    // read in coefficients for fitting from the coeffs.csv file
    ifstream csvfile(filename);
    CSVRow row;
    std::vector< float > coeffs(nMonomials);
    int i = 0;
    while(row << csvfile)
    {
      // read in pairs from coeffs.csv
      // format: 14\n3\n425.1623\n-0.005123 
      if(i >= nMonomials) break;
      coeffs[i] = atof(row[0].c_str()); // fix the atof
      i++;
    }
    csvfile.close();
    return coeffs;
}

/*************** MAIN *******************/
/*************** MAIN *******************/
/*************** MAIN *******************/
/*************** MAIN *******************/
/*************** MAIN *******************/

int main(int argc, char* argv[]) {


  // Parse command line arguments.
  // The following would work:
  // rasterize --letters abcDEF --font "Crimson 30"
  // analyze --rasterized Crimson30.csv --pairs abcDEF
  // fit --rasterized Crimson30.csv --pairs abcDEF --coeffs coeffs.csv

  argc -= (argc > 0);
  argv += (argc > 0);
  option::Stats  stats(usage, argc, argv);
  option::Option* options = new option::Option[stats.options_max];
  option::Option* buffer  = new option::Option[stats.buffer_max];
  option::Parser parse(usage, argc, argv, options, buffer);

  if(parse.error()) {
    cout << "Couldn't parse arguments. Exiting." << endl;
    return 1;
  }

  if(options[HELP] || argc == 0) {
    option::printUsage(std::cout, usage);
    return 0;
  }

  const float xheight = (float)intFromArg(options[XHEIGHT].arg, "xheight");
  const int maxdeltax = intFromArg(options[MAXDELTAX].arg, "maxdeltax", 100, options[FIT]);
  const int degree = 2;

  if(options[RASTERIZE]) {
    cout << "rasterize!" << endl;
    return 0;
  }

  /* We're not rasterizing, so we're going to be doing something with existing
   * letters. Get'em rasterized letters into letter and pair objects,
   * and start preparing the OpenCL stuff.
   */

  /**** CREATE LETTER OBJECTS ****/
  vector<Letter*> letters;
  vector<Pair*> pairs;

  // open rasterizer file
  if(options[RASTERIZED].arg == NULL) {
    cout << "No rasterized CSV file given. Exiting." << endl;
    return 1;
  }



  /**** RUN ANALYZE ****/

  if(options[ANALYZE]) {
    // Say hello
    cout << "Hi. Welcome to the Rhea Analysis Tool, version " << string(rhea_VERSION_MAJOR) << "." << rhea_VERSION_MINOR << "!" << endl;

    auto rasterizedLetters = getRasterized(options[RASTERIZED].arg, xheight);
    std::vector< Pair* > prefitPairs = getPrefit(options[PREFIT].arg, std::get<0>(rasterizedLetters));

    // this tools looks at rasterized fonts with metrics and derives coeffs from
    // them.

   Analyzer analyzer(rasterizedLetters, prefitPairs, degree);
    analyzer.getMatrix();
    //Analyzer analyzer("/home/sebastian/autokern/rhea/rhea-fit/bin/outmatrix.txt", degree);

    //analyzer.findAndStoreCoeffs("coeffs.csv");
    return 0;
  }

  /********************* RUN FIT *********************/

  if(options[FIT]) {
    // Say hello
    cout << "Hi. Welcome to the Rhea Letterfitting Tool, version " << string(rhea_VERSION_MAJOR) << "." << rhea_VERSION_MINOR << "!" << endl;

    // create Fitter object

    auto rasterizedLetters = getRasterized(options[RASTERIZED].arg, xheight);
    std::vector< float > coeffs;
    int nMonomials = pow(degree + 1, 4);
    coeffs = getCoeffs(options[COEFFS].arg, nMonomials);
    Fitter fitter(rasterizedLetters, coeffs, degree);

   
  } // end of if [FIT]

  /**************** delete stuff ******************/
  delete[] options;
  delete[] buffer;

  /* Deallocate resources */

  return 0;
}
/*

   cerr << "Now creating array" << endl;
   std::vector<std::vector<float>> matrix;

   matrix.resize(pairs.size());
   cout << "Matrix length: " << matrix.size() << endl;
   run_pair_force_matrix(&matrix, &pairs, 3);

   cout << "Now putting into files" << endl;
   ofstream outdata;
   outdata.open("./output/polynomial_matrix");
   if(!outdata) {
   cerr << "Error: Could not create output file.\n";
   exit(1);
   }
   for(int i = 0; i < matrix.size(); i++) {
   for(int j = 0; j < matrix.at(1).size(); j++) {
   outdata << matrix[i][j] << ",";
   }
   outdata << endl;
   }
   outdata.close();
   cout << "Done. Goodbye." << endl;
   return 0;
   }*/
