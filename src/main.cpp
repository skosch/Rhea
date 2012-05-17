// compile with something like clang++ -I/usr/include/freetype2 -lfreetype main.ccp config.cpp FT.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>

#include "rhea_config.h"
#include "config.h"
#include "FT.h"
#include "letter.h"
#include "pair.h"
#include "triplet.h"

using namespace std;

void run_dxdy_limited(vector<vector<int>> *matrix, vector<Pair*> *pairs) {
  for(int i=0; i<matrix->size(); i++) {
    matrix->at(i).resize(65*55);
    vector<int>(65*55,0).swap(matrix->at(i));
  }

  cout << "0% done." << flush;
  for(int i = 0; i < pairs->size(); i++) {
    pairs->at(i)->fillMatrix_dxdy_limited(matrix, 65, 55, i);
    cout << "\r" << (int) (100*i/pairs->size()) << "% done." << flush;
  }
  cout << "\nDone. Saving matrix to output file ....\n";
}

void run_ydxdy_limited(vector<vector<int>> *matrix,  vector<Pair*> *pairs) {
  for(int i=0; i<matrix->size(); i++) {
    matrix->at(i).resize(20*10*10);
    vector<int>(20*10*10,0).swap(matrix->at(i));
  }

  cout << "0% done." << flush;
  for(int i = 0; i < pairs->size(); i++) {
    pairs->at(i)->fillMatrix_ydxdy_limited(matrix, 20, 10, 10, i);
    cout << "\r" << (int) (100*i/pairs->size()) << "% done." << flush;
  }
  cout << "\nDone. Saving matrix to output file ....\n";
}

void run_pair_feature_analysis(vector<vector<int>> *matrix,  vector<Pair*> *pairs) {
  for(int i=0; i<matrix->size(); i++) {
    matrix->at(i).resize(3);
    vector<int>(3,0).swap(matrix->at(i));
  }

  cout << "0% done." << flush;
  for(int i = 0; i < pairs->size(); i++) {
    pairs->at(i)->findPairFeatures(matrix, i);
    cout << "\r" << (int) (100*i/pairs->size()) << "% done." << flush;
  }
  cout << "\nDone. Saving matrix to output file ....\n";
}

void run_pair_proxistem_analysis(vector<vector<int>> *matrix,  vector<Pair*> *pairs) {
  for(int i=0; i<matrix->size(); i++) {
    matrix->at(i).resize(8);
    vector<int>(8,0).swap(matrix->at(i));
  }

  cout << "0% done." << flush;
  for(int i = 0; i < pairs->size(); i++) {
    pairs->at(i)->proxiStemAnalysis(matrix, i);
    cout << "\r" << (int) (100*i/pairs->size()) << "% done." << flush;
  }
  cout << "\nDone. Saving matrix to output file ....\n";
}

void run_triplet_feature_analysis(vector<vector<int>> *matrix,  vector<Triplet*> *triplets) {
  for(int i=0; i<matrix->size(); i++) {
    matrix->at(i).resize(3);
    vector<int>(3,0).swap(matrix->at(i));
  }

  cout << "0% done." << flush;
  for(int i = 0; i < triplets->size(); i++) {
    triplets->at(i)->findTripletFeatures(matrix, i);
    cout << "\r" << (int) (100*i/triplets->size()) << "% done." << flush;
  }
  cout << "\nDone. Saving matrix to output file ....\n";
}

int main(void) {

  // Say hello
  cout << "Hi. Welcome to the Rhea Spacing Analyzer, version " << string(rhea_VERSION_MAJOR) << "." << rhea_VERSION_MINOR << "!" << endl;

  // Read in configuration
  ConfigReader* cR;
  cR = new ConfigReader();
  cR->readConfigFile("./rhea_config");

  // Create FreeType reader object
  FTengine* FTE;
  FTE = new FTengine();
  cout << "Loading font:" << FTE->preparePango(cR->getFontDesc(), cR->getPPEM()) << endl;
  
  string charset = cR->getCharset();

  cout << "Letters 0% analyzed.";
  vector<Letter*> letters;
  for(int i = 0; i < charset.length(); i++) {      // 97 = a, 122 = z
    cout << charset[i];
    letters.push_back(FTE->getLetter( (char) charset[i] ) );
    cout << "\rLetters " << 100*i/charset.length() << "% analyzed.";
  }
  cout << endl;
 

 // Create Pair Objects
  vector<Pair*> pairs;
  for(int i = 0; i < letters.size(); i++) {
    for(int j = 0; j < letters.size(); j++) {
      pairs.push_back(new Pair(letters[i], letters[j], FTE));
    }
  }

  /*
  vector<Triplet*> triplets;
  for(int i = 0; i < letters.size(); i++) {
    for(int j = 0; j < letters.size(); j++) {
      for(int k = 0; k < letters.size(); k++) {
	triplets.push_back(new Triplet(letters[i], letters[j], letters[k], FTE));
      }
    }
  }
  */
  cerr << "Now creating array" << endl;
  // dx, dy limited analysis
  std::vector<std::vector<int>> matrix;

  matrix.resize((charset.length()*charset.length()));
  
  /*
  run_ydxdy_limited(&matrix, &pairs);
  run_triplet_feature_analysis(&matrix, &triplets);

  run_pair_feature_analysis(&matrix, &pairs);
  */

  run_pair_proxistem_analysis(&matrix, &pairs);

  ofstream outdata;
  outdata.open("output.csv");
  if(!outdata) {
    cerr << "Error: Could not create output file.\n";
    exit(1);
  }
  for(int i = 0; i < matrix.size(); i++) {
    for(int j = 0; j < matrix.at(1).size(); j++) {
      outdata << matrix.at(i).at(j) << " ";
    }
    outdata << "\n";
  }

  return 0;
}
