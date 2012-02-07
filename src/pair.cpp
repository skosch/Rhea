
#include "pair.h"

using namespace std;

/** Constructor
 ** Creates a new pair using references to two existing Letter objects.
 **/
Pair::Pair(Letter* lLetter, Letter* rLetter, FTengine* FTE) {
  lL = lLetter;
  rL = rLetter;
  kerning = FTE->getKerning(lL->getLetterChar(), rL->getLetterChar());
  //  spacing = lL->getRSB() + kerning + rL->getLSB();
}
