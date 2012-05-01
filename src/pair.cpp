
#include "pair.h"

using namespace std;

/** Constructor
 ** Creates a new pair using references to two existing Letter objects.
 **/
Pair::Pair(Letter* lLetter, Letter* rLetter, FTengine* FTE) {
  lL = lLetter;
  rL = rLetter;
  spacing = FTE->getSpacing(lL->getLetterChar(), rL->getLetterChar());
  //cout << "Created pair " << lL->getLetterChar() << rL->getLetterChar() << " with total spacing " << spacing << endl;
  
}
