// compile with something like clang++ -I/usr/include/freetype2 -lfreetype main.ccp config.cpp FT.cpp

#include <iostream>
#include <string>
#include <vector>

#include "rhea_config.h"
#include "config.h"
#include "FT.h"
#include "letter.h"
#include "pair.h"


using namespace std;

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
  cout << "Spacing To:" << FTE->getSpacing('T', 'o') << endl;
  cout << "Spacing VA:" << FTE->getSpacing('V', 'A') << endl;
  cout << "Spacing KA:" << FTE->getSpacing('K', 'l') << endl;
  cout << "Spacing NN:" << FTE->getSpacing('N', 'N') << endl;
  cout << "Spacing vo:" << FTE->getSpacing('v', 'o') << endl;
  cout << "Spacing LT:" << FTE->getSpacing('L', 'T') << endl;

  // Create Letter Objects
  vector<Letter*> letters;
  for(int i = 97; i < 98; i++) {      // 97 = a, 122 = z
    //letters.push_back(FTE->getLetter( (char) i ) );
  }
  cout << "Letters done" << endl;
  // Create Pair Objects
  vector<Pair*> pairs;
  for(int i = 0; i < letters.size(); i++) {
    for(int j = 0; j < letters.size(); j++) {
      //pairs.push_back(new Pair(letters[i], letters[j], FTE));
    }
  }


  return 0;
}
