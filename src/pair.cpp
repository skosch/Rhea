
#include "pair.h"

using namespace std;

/** Constructor
 ** Creates a new pair using references to two existing Letter objects.
 **/
Pair::Pair(Letter* lLetter, Letter* rLetter, FTengine* FTE) {
  lL = lLetter;
  rL = rLetter;
  kerning = FTE->getSpacing(lL->getLetterChar(), rL->getLetterChar());
  //  spacing = lL->getRSB() + kerning + rL->getLSB();
  if(kerning > 0) cout << "Pair loaded: " << lLetter->getLetterChar() << rLetter->getLetterChar() << ", Kerning: " << kerning << endl;
}
