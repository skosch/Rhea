// compile with something like clang++ -I/usr/include/freetype2 -lfreetype main.ccp config.cpp FT.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <cstdlib> // for rand()
#include <ctime> // for time()

#include "rhea_config.h"
#include "config.h"
#include "FT.h"

using namespace std;

/******************** MAIN.CPP ********************/


int main(void) {
  // Say hello
  cout << "Hi. Welcome to the Rhea Rasterizer, version " << string(rhea_VERSION_MAJOR) << "." << rhea_VERSION_MINOR << "!" << endl;

  // Read in configuration
  ConfigReader* cR;
  cR = new ConfigReader();
  cR->readConfigFile("./rhea_config");

  // Create FreeType reader object
  FTengine* FTE;
  FTE = new FTengine();
  FTE->preparePango(cR->getFontDesc(), cR->getPPEM());

  string charset = cR->getCharset();

  // Open output file
  ofstream outdata;
  stringstream filename;
  filename << "./output/" << cR->getFontDesc() << ".csv";
  outdata.open(filename.str());
  if(!outdata) {
    cerr << "Error: Could not create output file.\n";
    exit(1);
  }

  for(int i = 0; i < charset.length(); i++) {      // 97 = a, 122 = z
    outdata << FTE->getLetter( (char) charset[i]);
  }

  string spacingLine;
  for(int i = 0; i < charset.length(); i++) {
    for(int j = 0; j < charset.length(); j++) {
      outdata << (char) charset[i] << (char) charset[j] << ",";
      outdata << FTE->getSpacing((char)charset[i], (char)charset[j]) << endl;
    }
  }
  outdata.close();

  cout << "Done. Goodbye." << endl;
  return 0;
}
